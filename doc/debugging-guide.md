# Debugging guide

Assuming you have Firefox as a browser.

## Simplify deploy

First, put a symlink of built `libfreshwrapper-pepperflash.so` to `~/.mozilla/plugins`.
This step simplifies deploy step. You just rebuild, restart browser, and you are running
new version.

## Use `gdb` to get a backtrace

Open terminal emulator window, execute command
```
$ ulimit -c unlimited
```
to enable creating of core files. Core file is a memory dump of failed process.

Then launch browser from the same terminal (`$ firefox`), navigate to the page causing crash,
make it crash, close the browser. File named "core" should appear in a current directory. You then
launch gdb debugger with
```
$ gdb /usr/lib/firefox/plugin-container core
```
and in gdb shell type
```
(gdb) thread apply all bt
```
`bt` (or `backtrace`) command instructs gdb to print a backtrace, chain of calls which led to
crash. If you build Debug of RelWithDebInfo version, it will also print source code references
based on debug info embedded into the binary. `thread apply all bt` does that for every thread.

Path to plugin-container file (which is binary used by Firefox to run plugins out-of-process. It's
so called plugin host) may be different on your machine, you'll need to find it yourself.

## How to put large text block in a message on GitHub

GitHub uses Markdown as a format language. You start large text block with \`\`\` on a separate
line and end with another \`\`\` on a separate line.

## Running under Valgrind

Valgrind is a great tool which can help to find subtle bugs. Major drawback is tremendous slowdown,
programs typically run 10-20 slower under Valgrind. Hopefully one can check only `plugin-container`.
Trick is replacing `plugin-container` with a shell script while renaming `plugin-container` to
`plugin-container.bin`.

```sh
#!/bin/sh

PCBIN=/usr/lib/firefox/plugin-container.bin

OPTS=
OPTS="$OPTS --log-file=logv.%p"
OPTS="$OPTS --smc-check=all-non-file"
OPTS="$OPTS --error-limit=no"
OPTS="$OPTS --num-callers=20"
OPTS="$OPTS $VALGRIND_OPTS"

if [ -n "$USE_VALGRIND" ]; then
    exec valgrind $OPTS $PCBIN $@
else
    exec $PCBIN $@
fi
```

Then you can run firefox instrumented like that:
```
USE_VALGRIND=1 firefox
```
It's worth to rebuild valgrind-friendly version of firefox from source
([link](https://developer.mozilla.org/en-US/docs/Mozilla/Testing/Valgrind)).


## Trace PPAPI plugin calls

If you need to trace which calls original plugin plugin makes while running under Chrome, you can
use [ppapi-trace](https://github.com/i-rinat/ppapi-trace). It's not an easy to use tool, you'll
need to dig into code if you want to use it for anything other than just printing function names
to the console.

## Verbose tracing

Reconfiguring project with
```
$ cmake -DTRACE_ALL=1 ..
```
and subsequent rebuild will enable verbose tracing. There will be every API call in standard output,
not only unimplemented functions. Verbose tracing can be enabled on per-file basis. One need to add
`#define TRACE_ALL` line at the top of a source file.
