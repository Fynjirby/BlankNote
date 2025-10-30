#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

static int default_font_size = 16;
static int font_size = 16;
static char bg_color[64] = "#1e1e1e";
static char font_name[128] = "JetBrains Mono, FiraCode Nerd Font, Fira Code, "
                             "Ubuntu Mono, DejaVu Sans Mono, monospace";
static int margin_vertical = 5;
static int margin_horizontal = 5;

static char current_file[512] = "";
static GtkCssProvider *css_provider = NULL;

static void load_config(void) {
  char path[256];
  snprintf(path, sizeof(path), "%s/.config/blanknote/config", g_get_home_dir());

  FILE *f = fopen(path, "r");
  if (!f)
    return;

  char line[256];
  while (fgets(line, sizeof(line), f)) {
    g_strstrip(line);
    if (line[0] == '#' || line[0] == '\0')
      continue;

    char key[64], value[128];
    if (sscanf(line, "%63[^=] = \"%127[^\"]\"", key, value) == 2 ||
        sscanf(line, "%63[^=] = %127s", key, value) == 2) {
      g_strstrip(key);
      g_strstrip(value);

      if (strcmp(key, "bg") == 0) {
        g_strlcpy(bg_color, value, sizeof(bg_color));
      } else if (strcmp(key, "font") == 0) {
        g_strlcpy(font_name, value, sizeof(font_name));
      } else if (strcmp(key, "font-size") == 0) {
        int m = atoi(value);
        default_font_size = font_size = m;
      } else if (strcmp(key, "margin") == 0) {
        int m = atoi(value);
        margin_vertical = margin_horizontal = m;
      } else if (strcmp(key, "margin-x") == 0) {
        int m = atoi(value);
        margin_horizontal = m;
      } else if (strcmp(key, "margin-y") == 0) {
        int m = atoi(value);
        margin_vertical = m;
      }
    }
  }
  fclose(f);
}

static void update_style(GtkWidget *text_view) {
  if (!css_provider)
    css_provider = gtk_css_provider_new();

  char css[512];
  g_snprintf(css, sizeof(css),
             "textview, textview text {\n"
             "  font-family: '%s';\n"
             "  font-size: %dpt;\n"
             "  background: %s;\n"
             "  color: white;\n"
             "  margin: %dpx %dpx;\n"
             "}\n"
             "scrolledwindow {\n"
             "  background: %s;\n"
             "}\n",
             font_name, font_size, bg_color, margin_vertical, margin_horizontal,
             bg_color);

  gtk_css_provider_load_from_string(css_provider, css);

  GdkDisplay *display = gtk_widget_get_display(text_view);
  gtk_style_context_add_provider_for_display(display,
                                             GTK_STYLE_PROVIDER(css_provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);
}

static void open_file_response(GObject *source, GAsyncResult *res,
                               gpointer user_data) {
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);
  GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
  GFile *file = gtk_file_dialog_open_finish(dialog, res, NULL);
  if (!file)
    return;

  gchar *contents = NULL;
  gsize len;
  if (g_file_load_contents(file, NULL, &contents, &len, NULL, NULL)) {
    gtk_text_buffer_set_text(buffer, contents, len);
    g_strlcpy(current_file, g_file_peek_path(file), sizeof(current_file));
    g_free(contents);
  }
  g_object_unref(file);
}

static void open_file(GtkWindow *window, GtkTextBuffer *buffer) {
  GtkFileDialog *dialog = gtk_file_dialog_new();
  gtk_file_dialog_open(dialog, window, NULL, open_file_response, buffer);
}

static void save_file_response(GObject *source, GAsyncResult *res,
                               gpointer user_data) {
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);
  GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
  GFile *file = gtk_file_dialog_save_finish(dialog, res, NULL);
  if (!file)
    return;

  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);
  gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
  g_file_replace_contents(file, text, strlen(text), NULL, FALSE, 0, NULL, NULL,
                          NULL);
  g_strlcpy(current_file, g_file_peek_path(file), sizeof(current_file));
  g_object_unref(file);
  g_free(text);
}

static void save_file(GtkWindow *window, GtkTextBuffer *buffer) {
  if (current_file[0] != '\0') {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    GFile *file = g_file_new_for_path(current_file);
    g_file_replace_contents(file, text, strlen(text), NULL, FALSE, 0, NULL,
                            NULL, NULL);
    g_object_unref(file);
    g_free(text);

    GApplication *app = g_application_get_default();
    if (app) {
      GNotification *notify = g_notification_new("BlankNote");
      gchar *body = g_strdup_printf("File %s saved!", current_file);
      g_notification_set_body(notify, body);
      g_free(body);
      g_application_send_notification(app, NULL, notify);
      g_object_unref(notify);
    }
    return;
  }

  GtkFileDialog *dialog = gtk_file_dialog_new();
  gtk_file_dialog_save(dialog, window, NULL, save_file_response, buffer);
}

static void open_file_from_arg(GApplication *app, GFile **files, gint n_files,
                               const char *hint, gpointer user_data) {
  if (n_files > 0) {
    g_strlcpy(current_file, g_file_get_path(files[0]), sizeof(current_file));
  }
  g_application_activate(app);
}

static void run_plugin(const char *plugin_path, GtkTextView *text_view) {
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
  GtkTextIter start, end;
  char *input_text = NULL;

  if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
    input_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
  } else {
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    input_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
  }

  const char *cmd[] = {plugin_path, NULL};
  GError *error = NULL;
  GSubprocess *proc = g_subprocess_newv(
      cmd, G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
      &error);
  if (!proc) {
    g_warning("Failed to start plugin %s: %s", plugin_path, error->message);
    g_error_free(error);
    g_free(input_text);
    return;
  }

  GOutputStream *in = g_subprocess_get_stdin_pipe(proc);
  GInputStream *out = g_subprocess_get_stdout_pipe(proc);

  g_output_stream_write(in, input_text, strlen(input_text), NULL, NULL);
  g_output_stream_close(in, NULL, NULL);
  g_free(input_text);

  GString *output_data = g_string_new(NULL);
  char io_buf[4096];
  gssize bytes_read;
  while ((bytes_read = g_input_stream_read(out, io_buf, sizeof(io_buf), NULL,
                                           NULL)) > 0) {
    g_string_append_len(output_data, io_buf, bytes_read);
  }

  g_subprocess_wait_check(proc, NULL, NULL);

  char *result = g_strdup(output_data->str);
  g_string_free(output_data, TRUE);

  if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
    gtk_text_buffer_delete(buffer, &start, &end);
    gtk_text_buffer_insert(buffer, &start, result, -1);
  } else {
    gtk_text_buffer_set_text(buffer, result, -1);
  }

  g_free(result);
  g_object_unref(proc);
}

static void launch_fuzzel_plugins(GtkTextView *text_view) {
  char *plugins_dir = g_build_filename(g_get_home_dir(), ".config", "blanknote",
                                       "plugins", NULL);
  GDir *dir = g_dir_open(plugins_dir, 0, NULL);
  if (!dir) {
    g_free(plugins_dir);
    return;
  }

  GString *list = g_string_new(NULL);
  const char *name;
  while ((name = g_dir_read_name(dir))) {
    char *path = g_build_filename(plugins_dir, name, NULL);
    if (g_file_test(path, G_FILE_TEST_IS_EXECUTABLE)) {
      g_string_append_printf(list, "%s\n", name);
    }
    g_free(path);
  }
  g_dir_close(dir);

  if (list->len == 0) {
    g_string_free(list, TRUE);
    g_free(plugins_dir);
    return;
  }

  const char *fuzzel_cmd[] = {"fuzzel", "-d", "--lines=5", NULL};
  GError *error = NULL;
  GSubprocess *fuzzel = g_subprocess_newv(
      fuzzel_cmd,
      G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error);
  if (!fuzzel) {
    g_warning("fuzzel not available: %s", error->message);
    g_error_free(error);
    g_string_free(list, TRUE);
    g_free(plugins_dir);
    return;
  }

  GOutputStream *in = g_subprocess_get_stdin_pipe(fuzzel);
  g_output_stream_write(in, list->str, list->len, NULL, NULL);
  g_output_stream_close(in, NULL, NULL);
  g_string_free(list, TRUE);

  GInputStream *out = g_subprocess_get_stdout_pipe(fuzzel);
  char buffer[256] = {0};
  gsize bytes_read = 0;
  GBytes *bytes = g_input_stream_read_bytes(out, 256, NULL, &error);
  if (bytes) {
    const char *data = g_bytes_get_data(bytes, &bytes_read);
    if (bytes_read > 0 && data[bytes_read - 1] == '\n')
      bytes_read--;
    memcpy(buffer, data, bytes_read < 255 ? bytes_read : 255);
    g_bytes_unref(bytes);
  }
  g_object_unref(fuzzel);

  if (buffer[0] != '\0') {
    char *plugin_path = g_build_filename(plugins_dir, buffer, NULL);
    if (g_file_test(plugin_path, G_FILE_TEST_IS_EXECUTABLE)) {
      run_plugin(plugin_path, text_view);
    }
    g_free(plugin_path);
  }

  g_free(plugins_dir);
}

static gboolean handle_keys(GtkEventControllerKey *controller, guint keyval,
                            guint keycode, GdkModifierType state,
                            gpointer user_data) {
  GtkWidget *text_view = GTK_WIDGET(user_data);
  GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(text_view));
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

  if (state & GDK_CONTROL_MASK) {
    switch (keyval) {
    case GDK_KEY_b:
      launch_fuzzel_plugins(GTK_TEXT_VIEW(text_view));
      return TRUE;

    case GDK_KEY_s:
    case GDK_KEY_S:
      save_file(window, buffer);
      return TRUE;

    case GDK_KEY_o:
    case GDK_KEY_O:
      open_file(window, buffer);
      return TRUE;

    case GDK_KEY_plus:
    case GDK_KEY_KP_Add:
    case GDK_KEY_equal:
      font_size += 2;
      break;

    case GDK_KEY_minus:
    case GDK_KEY_KP_Subtract:
      font_size -= 2;
      break;

    case GDK_KEY_0:
    case GDK_KEY_KP_0:
      font_size = default_font_size;
      break;

    default:
      return FALSE;
    }

    if (font_size < 6)
      font_size = 6;
    if (font_size > 72)
      font_size = 72;

    update_style(text_view);
    return TRUE;
  }

  return FALSE;
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "BlankNote");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

  GtkWidget *text_view = gtk_text_view_new();
  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), text_view);
  gtk_window_set_child(GTK_WINDOW(window), scrolled);

  if (current_file[0] != '\0') {
    GFile *file = g_file_new_for_path(current_file);
    gchar *contents = NULL;
    gsize len;
    if (g_file_load_contents(file, NULL, &contents, &len, NULL, NULL)) {
      gtk_text_buffer_set_text(
          gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)), contents, len);
      g_free(contents);
    }
    g_object_unref(file);
  }
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
  update_style(text_view);

  GtkEventController *key = gtk_event_controller_key_new();
  g_signal_connect(key, "key-pressed", G_CALLBACK(handle_keys), text_view);
  gtk_widget_add_controller(text_view, key);

  gtk_widget_set_visible(window, TRUE);
}

int main(int argc, char **argv) {
  load_config();

  char *config_dir =
      g_build_filename(g_get_home_dir(), ".config", "blanknote", NULL);
  char *config_path = g_build_filename(config_dir, "config", NULL);
  if (!g_file_test(config_path, G_FILE_TEST_EXISTS)) {
    if (!g_file_test(config_dir, G_FILE_TEST_IS_DIR)) {
      g_mkdir_with_parents(config_dir, 0755);
    }
    g_file_set_contents(config_path, "", 0, NULL);
  }

  char *plugins_dir = g_build_filename(config_dir, "plugins", NULL);
  if (!g_file_test(plugins_dir, G_FILE_TEST_IS_DIR)) {
    g_mkdir_with_parents(plugins_dir, 0755);
  }

  g_free(config_dir);
  g_free(plugins_dir);

  GtkApplication *app =
      gtk_application_new("dev.fynjirby.blanknote", G_APPLICATION_HANDLES_OPEN);
  g_signal_connect(app, "open", G_CALLBACK(open_file_from_arg), NULL);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
