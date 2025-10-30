# BlankNote plugin system documentation

*Written by a human btw*

Developing plugins is damn simple (even vibecoder could do it)

Lemme explain it fast

- Plugins are stored in `$HOME/.config/blanknote/plugins`

For example plugin name can be `uppercase` with this code in it
```
#!/bin/bash
tr "[:lower:]" "[:upper:]"
```
As you can see its bash code that just turns lower case to upper case. 

- When developing plugins you shouldnt care about how to get text to edit and where to send it back

BlankNote plugin system is designed to automatically do it. Look, here is the example of what happens inside BlankNote (simplified to understand, not the actual code fragment): 
```c
// 1. BlankNote gets selected text (or full buffer if nothing selected)
input_text = get_selected_or_all_text();

// 2. Runs your plugin as a subprocess
proc = spawn([plugin_path], stdin=PIPE, stdout=PIPE)

// 3. Sends text to plugin's STDIN
write(proc.stdin, input_text)
close(proc.stdin)

// 4. Reads result from plugin's STDOUT
output = read_all(proc.stdout)

// 5. Replaces selection (or full text) with output
replace_text_with(output)
```

So, to make your plugin work it just needs to 

 - Read from STDIN → get the text to process
 - Write to STDOUT → return the modified text

- You can create plugin in ANY language that can be executed on your machine!

Oh, look, thats the plugin on JavaScript (run using bun.js)
```js
#!/usr/bin/env bun

let input = '';

process.stdin.setEncoding('utf8');

// Read all from STDIN
process.stdin.on('readable', () => {
  const chunk = process.stdin.read();
  if (chunk !== null) {
    input += chunk;
  }
});

process.stdin.on('end', () => {
  // Reverse and write to STDOUT
  const reversed = input.split('').reverse().join('');
  process.stdout.write(reversed);
});
```

To install it, just put it in `$HOME/.config/blanknote/plugins/js-reverse` for example

And you can create a plugin on `python`, `lua`, `ruby`, `go` (compiled binary), `c` (also compiled bin), EVEN `php`!

Just dont forget to add a sheband or compile it to use

Look in `example-plugins` folder to see some weirdest plugins :)

- To create great plugins its better to use the BlankNote format for plugins, its simple.

 - Name: 1 word or more connected with dash (`-`)
 - Path: All plugins should be like this `$HOME/.config/blanknote/plugins/<plugin name>`
 - Extention: Not needed, but you can add it if you want (no sense)

If you built a nice plugin using this guidelines, you can send a PR with it to [BlankNote repo](https://github.com/fynjirby/blanknote) 

PRs are **always** welcome

## Tips

This may help you to build a nice plugin

 - Use shebang → no need to specify interpreter
 - Test in terminal: `echo "test" | ./your-plugin`
 - Keep it fast — no long loops or network (unless intentional)
 - No GUI — plugins run in background (but you can send notifications for example)
