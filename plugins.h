#ifndef PLUGINS_H
#define PLUGINS_H

#include <gtk/gtk.h>

void run_plugin(const char *plugin_path, GtkTextView *text_view);
void launch_fuzzel_plugins(GtkTextView *text_view);

#endif
