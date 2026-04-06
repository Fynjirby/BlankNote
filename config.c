#include "config.h"
#include "glib.h"

Config config = {.default_font_size = 16,
                 .font_size = 16,
                 .font_family =
                     "JetBrains Mono, Hack, FiraCode, Ubuntu Mono, DejaVu Sans "
                     "Mono, monospace",
                 .bg_color = "#1e1e1e",
                 .sel_bg = "#264F78",
                 .sel_color = "#FFFFFF",
                 .margin_vertical = 5,
                 .margin_horizontal = 5,
                 .launcher = "",
                 .plugins_path = ""};

void load_config(void) {
  char *path = g_build_filename(g_get_user_config_dir(), "blanknote",
                                "config.ini", NULL);

  GKeyFile *kf = g_key_file_new();
  GError *err = NULL;

  if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, &err)) {
    if (err)
      g_error_free(err);
    g_key_file_unref(kf);
    g_free(path);
    return;
  }

  gchar *tmp;

  tmp = g_key_file_get_string(kf, "colors", "bg-color", NULL);
  if (tmp) {
    g_strlcpy(config.bg_color, tmp, sizeof(config.bg_color));
    g_free(tmp);
  }

  tmp = g_key_file_get_string(kf, "colors", "sel-bg", NULL);
  if (tmp) {
    g_strlcpy(config.sel_bg, tmp, sizeof(config.sel_bg));
    g_free(tmp);
  }

  tmp = g_key_file_get_string(kf, "colors", "sel-color", NULL);
  if (tmp) {
    g_strlcpy(config.sel_color, tmp, sizeof(config.sel_color));
    g_free(tmp);
  }

  tmp = g_key_file_get_string(kf, "font", "font-family", NULL);
  if (tmp) {
    g_strlcpy(config.font_family, tmp, sizeof(config.font_family));
    g_free(tmp);
  }

  if (g_key_file_has_key(kf, "font", "font-size", NULL)) {
    int size = g_key_file_get_integer(kf, "font", "font-size", NULL);
    config.default_font_size = config.font_size = size;
  }

  if (g_key_file_has_key(kf, "ui", "margin", NULL)) {
    int m = g_key_file_get_integer(kf, "ui", "margin", NULL);
    config.margin_horizontal = config.margin_vertical = m;
  }

  if (g_key_file_has_key(kf, "ui", "margin-x", NULL)) {
    config.margin_horizontal =
        g_key_file_get_integer(kf, "ui", "margin-x", NULL);
  }

  if (g_key_file_has_key(kf, "ui", "margin-y", NULL)) {
    config.margin_vertical = g_key_file_get_integer(kf, "ui", "margin-y", NULL);
  }

  tmp = g_key_file_get_string(kf, "plugins", "launcher", NULL);
  if (tmp) {
    g_strlcpy(config.launcher, tmp, sizeof(config.launcher));
    g_free(tmp);
  }

  tmp = g_key_file_get_string(kf, "plugins", "path", NULL);
  if (tmp) {
    g_strlcpy(config.plugins_path, tmp, sizeof(config.plugins_path));
    g_free(tmp);
  }

  if (config.plugins_path[0] == '\0') {
    char *p = g_build_filename(g_get_home_dir(), ".local", "share", "blanknote",
                               "plugins", NULL);
    g_strlcpy(config.plugins_path, p, sizeof(config.plugins_path));
    g_free(p);
  }

  g_key_file_unref(kf);
  g_free(path);
}
