#include "config.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int default_font_size = 16;
int font_size = 16;
char bg_color[64] = "#1e1e1e";
char font_name[128] = "JetBrains Mono, FiraCode Nerd Font, Fira Code, "
                      "Ubuntu Mono, DejaVu Sans Mono, monospace";
int margin_vertical = 5;
int margin_horizontal = 5;

void load_config(void) {
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
