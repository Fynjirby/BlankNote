#include "config.h"
#include "files.h"
#include "plugins.h"
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>

#ifndef VERSION
#define VERSION "unknown"
#endif

static GtkCssProvider *css_provider = NULL;

static void update_style(GtkWidget *text_view) {
  if (!css_provider)
    css_provider = gtk_css_provider_new();

  char css[1024];
  g_snprintf(css, sizeof(css),
             "textview text selection {\n"
             "  background-color: %s;\n"
             "  color: %s;\n"
             "}\n"
             "textview, textview text {\n"
             "  font-family: '%s';\n"
             "  font-size: %dpt;\n"
             "  background-color: %s;\n"
             "  color: white;\n"
             "  margin: %dpx %dpx;\n"
             "}\n"
             "scrolledwindow {\n"
             "  background: %s;\n"
             "}\n",
             config.sel_bg, config.sel_color, config.font_family,
             config.font_size, config.bg_color, config.margin_vertical,
             config.margin_horizontal, config.bg_color);

  gtk_css_provider_load_from_string(css_provider, css);

  GdkDisplay *display = gtk_widget_get_display(text_view);
  gtk_style_context_add_provider_for_display(display,
                                             GTK_STYLE_PROVIDER(css_provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);
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
      g_strstrip(config.launcher);
      if (config.launcher[0] == '\0') {
        g_warning("Launcher not set");
        GtkAlertDialog *dialog = gtk_alert_dialog_new(
            "In order to launch plugins, set launcher in config. For more "
            "info, see \"man blanknote\"");
        gtk_alert_dialog_show(dialog, window);
        return TRUE;
      }

      launch_plugins(GTK_TEXT_VIEW(text_view), config.launcher,
                     config.plugins_path);
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
      config.font_size += 2;
      break;

    case GDK_KEY_minus:
    case GDK_KEY_KP_Subtract:
      config.font_size -= 2;
      break;

    case GDK_KEY_0:
    case GDK_KEY_KP_0:
      config.font_size = config.default_font_size;
      break;

    default:
      return FALSE;
    }

    if (config.font_size < 6)
      config.font_size = 6;
    if (config.font_size > 72)
      config.font_size = 72;

    update_style(text_view);
    return TRUE;
  }

  return FALSE;
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "BlankNote");
  gtk_window_set_default_size(GTK_WINDOW(window), 700, 500);

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
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
      printf("BlankNote %s\n", VERSION);
      return 0;
    }
  }

  load_config();

  char *config_dir =
      g_build_filename(g_get_user_config_dir(), "blanknote", NULL);
  char *config_path = g_build_filename(config_dir, "config.ini", NULL);
  if (!g_file_test(config_path, G_FILE_TEST_EXISTS)) {
    if (!g_file_test(config_dir, G_FILE_TEST_IS_DIR)) {
      g_mkdir_with_parents(config_dir, 0755);
    }
    g_file_set_contents(config_path, "", 0, NULL);
  }

  const char *plugins_dir = config.plugins_path;
  if (!g_file_test(plugins_dir, G_FILE_TEST_IS_DIR)) {
    g_mkdir_with_parents(plugins_dir, 0755);
  }

  g_free(config_dir);

  GtkApplication *app =
      gtk_application_new("blanknote", G_APPLICATION_HANDLES_OPEN);
  g_signal_connect(app, "open", G_CALLBACK(open_file_from_arg), NULL);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
