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


## Black screen with sound only

3d is enabled by default at the moment, and requiring working OpenGL ES 2.0. With
some video adapters 3d doesn't work as intended for some reason. If you have the
issue, disable 3d by adding `enable_3d = 0` line to `~/.config/freshwrapper.conf`.
If 3d generally works, but fails only for some instances, try to disable 3d
for transparent movies by adding `enable_3d_transparent = 0` to
`~/.config/freshwrapper.conf`.

## DRM doesn't work

Desktop plugin doesn't use PPB_Flash_DRM interface, but plugin from ChromeOS does.
To get it, download latest version of
[linux_recovery.sh](https://dl.google.com/dl/edgedl/chromeos/recovery/linux_recovery.sh).
That file contains a lot of links to ChromeOS recovery images for various Chromebooks.
I tested one with "zako" in the name, it's for x86_64 hardware. Other recovery images
may contain x86 and/or arm versions too. After download, find Flash player in
`/opt/google/chrome/pepper/` directory on partition 3. Partition have to be mounted
with "ro" flag, otherwise it won't work.

After you extract libpepflashplayer.so (which is the only file you need from that entire
image), add or update `pepperflash_path` parameter in configuration file
`~/.config/freshwrapper.conf` to point to where you put libpepflashplayer.so.

## Random network downloads fail

Some routers forcefully lowercase DNS queries, which interferes with dns0x20 security
trick used by libevent2's DNS client. Configuration parameter `randomize_dns_case` is
used to control whenever that trick is used. If you've enabled it, and have network
issues, turn it off again.
(See [dns0x20](https://tools.ietf.org/html/draft-vixie-dnsext-dns0x20-00) for details).

## Where is `mms.cfg`?

Try `~/.config/freshwrapper-data/Shockwave Flash/System/mms.cfg`
