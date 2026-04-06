# BlankNote - a damn simple note taking app 

BlankNote is written in C with GTK4. It has no UI elements except the text view, controlled with keybindings.

# Building
Dependencies: `gtk4`
1. Clone repo
2. Build with `make`
3. Install with `sudo make install`

To uninstall, use `sudo make uninstall`

For developing, run `make run`

# Configuration
Check `docs/config.md` or use `man 5 blanknote` for configuration documentation

# Plugins
To develop plugins please check the plugin guidelines in `docs/plugins.md`

# Usage
Run with `blanknote [file]`

## Keybindings
`Ctrl-plus` - Zoom text in

`Ctrl-minus` - Zoom text out

`Ctrl-0` - Reset zooming level to default

`Ctrl-O` - Open file picker

`Ctrl-S` - Save file

`Ctrl-B` - Open plugins picker
