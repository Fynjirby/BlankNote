# BlankNote config

In BlankNote the config aligns in `$HOME/.config/blanknote/config.ini` file.

# Available config options

| Option          | Type    | Default     | Description |
|-----------------|---------|-------------|-------------|
| [font] **font-family** | string  | `JetBrains Mono, Hack, FiraCode, ...` | Font family list (first available font is used). |
| [font] **font-size**   | int     | `16`        | Font size in points (pt) |
| [colors] **bg-color**    | string  | `#1e1e1e`   | Background color of the editor |
| [colors] **sel-bg**      | string  | `#264F78`   | Selection background color |
| [colors] **sel-color**   | string  | `#FFFFFF`   | Selection text color |
| [ui] **margin**      | int     | `5`         | Sets both horizontal and vertical margins (in pixels) |
| [ui] **margin-x**    | int     | `5`         | Sets horizontal margin only |
| [ui] **margin-y**    | int     | `5`         | Sets vertical margin only |
| [plugins] **launcher**    | string  | -           | Launcher command (e.g. - "rofi -dmenu" or "fuzzel -d") |
| [plugins] **path**        | string  | `/usr/share/blanknote/plugins` | Path where to find plugins |

# Example config

```ini
[font]
font-family=Hack
font-size=12

[colors]
bg-color=#0D0D0D
sel-bg=#755215
sel-color=#F1F1F1

[ui]
margin-x=8
margin-y=4

[plugins]
launcher=rofi -dmenu
path=/home/me/.blanknote/plugins
```

This this right config format, when you edit it, be careful with syntax
