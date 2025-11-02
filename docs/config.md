# BlankNote config docs

*Written by a human btw*

In BlankNote the config aligns in `$HOME/.config/blanknote/config` file. It has `ini`-like syntax with `key = value` system

You can set values with brackets or without them, it doesnt matter since config parser just removes them

# Available config options

| Option         | Type    | Default     | Description |
|----------------|---------|-------------|-------------|
| **bg**         | string  | `#1e1e1e`   | Background color of the editor |
| **font**       | string  | `"JetBrains Mono, FiraCode Nerd Font..."` | Font family list (first available font is used). |
| **font-size**  | int     | `16`        | Font size in points |
| **margin**     | int     | `5`         | Sets both horizontal and vertical margins (in pixels) |
| **margin-x**   | int     | `5`         | Sets horizontal margin only |
| **margin-y**   | int     | `5`         | Sets vertical margin only |
| **sel-bg**     | string  | `#264F78`   | Selection background color |
| **sel-color**  | string  | `#FFFFFF`   | Selection text color |

# Example config

From [Egor's Dotfiles](https://github.com/fynjirby/dotfiles)

[BlankNote Config](https://github.com/Fynjirby/dotfiles/blob/arch/.config/blanknote/config)
```ini
font="Fira Code Nerd Font"
bg="#0F0F0F"
margin=8
sel-bg="#355E3B"
```
