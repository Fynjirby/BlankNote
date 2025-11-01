#include "plugins.h"
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

void run_plugin(const char *plugin_path, GtkTextView *text_view) {
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

void fuzzel_read_done(GObject *source, GAsyncResult *res, gpointer user_data) {
  GInputStream *out = G_INPUT_STREAM(source);
  GError *error = NULL;
  GBytes *bytes = g_input_stream_read_bytes_finish(out, res, &error);
  if (!bytes)
    return;

  gsize bytes_read;
  const char *data = g_bytes_get_data(bytes, &bytes_read);

  gsize len = bytes_read;
  if (len > 0 && data[len - 1] == '\n')
    len--;

  gchar *buffer = g_malloc(len + 1);
  memcpy(buffer, data, len);
  buffer[len] = '\0';

  GtkTextView *text_view = GTK_TEXT_VIEW(user_data);

  char *plugins_dir = g_build_filename(g_get_home_dir(), ".config", "blanknote",
                                       "plugins", NULL);
  char *plugin_path = g_build_filename(plugins_dir, buffer, NULL);
  g_free(buffer);
  g_free(plugins_dir);

  if (g_file_test(plugin_path, G_FILE_TEST_IS_EXECUTABLE)) {
    run_plugin(plugin_path, text_view);
  }
  g_free(plugin_path);
}

void launch_fuzzel_plugins(GtkTextView *text_view) {
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
    g_warning("Fuzzel not available: %s", error->message);
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
  g_input_stream_read_bytes_async(out, 256, G_PRIORITY_DEFAULT, NULL,
                                  fuzzel_read_done, text_view);

  g_object_unref(fuzzel);
  g_free(plugins_dir);
}
