About
=====

PPAPI-host NPAPI-plugin adapter.

As you know, Adobe have suspended further development of Flash player
plugin for GNU/Linux. Latest available as an NPAPI plugin version 11.2
will get security updates for five years (since its release on May
4th, 2012), but further development have been ceased. Fortunately or
not, newer versions are still available for Linux as a part of Chrome
browser, where Flash comes bundled in a form of PPAPI plugin. PPAPI or
Pepper Plugin API is an interface promoted by Chromium/Chrome team for
browser plugins. It's NPAPI-inspired yet significantly different API
which have every concievable function plugin may want. Two-dimensional
graphics, OpenGL ES, font rendering, network access, audio, and so
on. It's huge, there are 107 groups of functions, called interfaces
which todays Chromium browser offers to plugins. And specs are not
final yet. Interfaces are changing, newer versions are arising, older
ones are getting deleted.

For various reasons Firefox developers are not interested now in
implementing PPAPI in Firefox.  However that does not mean it cannot
be done.

The main goal of this project is to get PPAPI (Pepper) Flash player
working in Firefox. This can be done in two ways. First one is to
implement full PPAPI interface in Firefox itself. Other one is to
implement a wrapper, some kind of adapter which will look like browser
to PPAPI plugin and look like NPAPI plugin for browser.

First approach requires strong knowledge of Firefox internals, and
moreover additional effort to get the code into
mainstream. Maintaining a set of patches doesn't look like a good
idea. Second approch allows to concentrace on two APIs only. Yes one
of them is big, but still graspable. Second way will be used for the
project. It will benifit other browsers too, not only Firefox.


Status
======

Eternal alpha. I use it on day-to-day basis though.

Security note
=============

All available Pepper Plugin API documentation usually accompanied with
assertions of enhanced security due to active sandboxing usage. It's
worth to note, that API itself doesn't make any sandboxing, it's only
allows sandboxed implementations. This particular implementation
**doesn't implement any sandbox**. That means if any malicious code breaks
through plugin security, there is no additional barriers.

Install
=======

Project is using cmake (>=2.8.8) build system.

* Install prerequisites.
```
    $ sudo apt-get install cmake pkg-config libasound2-dev liburiparser-dev \
           libglib2.0-dev libconfig-dev libpango1.0-dev libxinerama-dev     \
           libegl1-mesa-dev libevent-dev libgtk+2.0-dev libgles2-mesa-dev
```

* Make `build` subdirectory, go there, call
```
    $ cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    $ make
```

* Put generated `libfreshwrapper-pepperflash.so` into broswer plugins directory (`~/.mozilla/plugins`)


When loaded by browser it will search for `libpepflashplayer.so` in a directories
where it can be: in Chrome (stable/beta/unstable) directory, and in
`/usr/lib/pepperflashplugin-nonfree/` (pepperflashplugin-nonfree puts it there).
It should be enough to get it running, but if it doesn't, specify full path in
`~/.config/freshwrapper.conf`. You may find sample configuration file in `/data`.
It's better to have `manifest.json` alongside with `libpepflashplayer.so`,
actual Flash version will be taken from that manifest.

You can also build from source using this script by copying and pasting it into an sh file anywhere then running it:
https://github.com/zeuslightning125/UtilScripts/blob/2e4163af6a5c3b73f52fb4ffac5af0ea1b8cf829/Unix/Linux/Install-Build/installpepperflashfirefox.sh

All dependencies as well as things required to build are automatically installed, and the files are copied into the mozilla plugins.
Be sure to turn off firefox before running this script, and restart firefox once complete.

License
=======

The MIT License. See LICENSE.MIT for full text.
