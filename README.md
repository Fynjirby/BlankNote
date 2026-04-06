# BlankNote - a damn simple note taking app 

BlankNote is written in C with GTK4. It has no UI elements except the text view, controlled with keybindings.

# Building
Dependencies: `gtk4`

Clone repo, run `make`, done! Now run with `make run`

To install system-wide, run `sudo make install`
To remove it, use `sudo make uninstall`

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
