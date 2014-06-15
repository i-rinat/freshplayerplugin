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

Early alpha. Some video players work, with both video and audio.

Install
=======

Ubuntu-Install https://github.com/andykimpe/freshplayerplugin/blob/master/ubuntu-install

Project is using cmake (>=2.8.8) build system. Make build subdirectory, go
there, call `cmake ..`, then `make`. Put generated
`libfreshwrapper.so` into broswer plugins directory. Note, path to
PepperFlash is hardcoded, it should be at
`/opt/google/chrome/PepperFlash/libpepflashplayer.so`. Having Chrome
installed should be enough.

License
=======

The MIT License. See LICENSE.MIT for full text.
