#ifndef PLUGINS_H
#define PLUGINS_H

#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>

typedef struct {
  GtkTextView *text_view;
  char *plugins_path;
} PluginContext;

void run_plugin(const char *plugin_path, GtkTextView *text_view);
void launch_plugins(GtkTextView *text_view, const char *launcher,
                    const char *path);

#endif
