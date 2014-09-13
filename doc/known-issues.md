# Known issues


## Failed to load libpepflashplayer.so

You see fine example of
[programmer art](http://en.wikipedia.org/wiki/Programmer_art)
on a screen instead of a Flash movie:
![missing libpepflashplayer.so](img-missing-pepflash.png)

That means you either have no `libpepflashplayer.so` or its location
is unknown to freshwrapper. This project is a shim, adapter. It has no
implementation of Flash technology itself, but allows using
PepperFlash bundled with Chrome browser.

Usually installing Chrome (stable, beta, or unstable) is enough, those
paths are searched among the others. If you do have
`libpepflashplayer.so` on your machine in some non-standard location,
you have to add a line to `~/.config/freshwrapper.conf` with an actual
path:
```
pepperflash_path = "/usr/lib64/PepperFlash/libpepflashplayer.so"
```

## Flash version is incorrect

For example, 13.1.2.3. PepperFlash have its version separated from
binary, it's stored in a file named `manifest.json`. Make sure you
have up-to-date `manifest.json` near `libpepflashplayer.so`. It's
worth to note this version is used only for plugin listing in
browser. Various version detection scripts on the web will acquire
correct version from Flash internals.


## Web pages cannot find Flash at all

Firefox caches plugin verion information to avoid calling all of them
at startup. Sometimes it marks Flash as non-functional and doesn't try
to reload it at all. Can be fixed by removing `pluginreg.dat` file
from firefox profile directory. That file will be recreated on next
Firefox startup.
[Here](https://support.mozilla.org/en-US/kb/profiles-where-firefox-stores-user-data)
is how to find where profile directory is.

## Flash have crashed or hung

It's a bug. [Create an issue](https://github.com/i-rinat/freshplayerplugin/issues)
about this. I need a scenario which can be used to reproduce an issue. It
would be nice if you search through open issues to avoid duplicates.
