#include "files.h"
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

char current_file[512] = "";

void open_file_response(GObject *source, GAsyncResult *res,
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

void open_file(GtkWindow *window, GtkTextBuffer *buffer) {
  GtkFileDialog *dialog = gtk_file_dialog_new();
  gtk_file_dialog_open(dialog, window, NULL, open_file_response, buffer);
}

void save_file_response(GObject *source, GAsyncResult *res,
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

void save_file(GtkWindow *window, GtkTextBuffer *buffer) {
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

void open_file_from_arg(GApplication *app, GFile **files, gint n_files,
                        const char *hint, gpointer user_data) {
  if (n_files > 0) {
    g_strlcpy(current_file, g_file_get_path(files[0]), sizeof(current_file));
  }
  g_application_activate(app);
}
