#ifndef FILES_H
#define FILES_H

#include <gtk/gtk.h>

extern char current_file[512];

void open_file_response(GObject *source, GAsyncResult *res, gpointer user_data);
void open_file(GtkWindow *window, GtkTextBuffer *buffer);
void save_file_response(GObject *source, GAsyncResult *res, gpointer user_data);
void save_file(GtkWindow *window, GtkTextBuffer *buffer);
void open_file_from_arg(GApplication *app, GFile **files, gint n_files, const char *hint, gpointer user_data);

#endif
