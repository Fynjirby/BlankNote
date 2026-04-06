#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_config(void);

typedef struct {
  int default_font_size;
  int font_size;
  char font_family[128];
  char bg_color[64];
  char sel_bg[64];
  char sel_color[64];
  int margin_vertical;
  int margin_horizontal;
  char launcher[128];
  char plugins_path[256];
} Config;

extern Config config;

#endif
