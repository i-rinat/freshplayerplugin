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
which have every conceivable function plugin may want. Two-dimensional
graphics, OpenGL ES, font rendering, network access, audio, and so
on. It's huge, there are 111 groups of functions, called interfaces
which todays Chromium browser offers to plugins. Although specs are not
final yet, and new interface versions are arising, with some older
ones getting deleted; rate of change have significantly slowed down.

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
idea. Second approach allows to concentrate on two APIs only. Yes one
of them is big, but still graspable. Second way will be used for the
project. It will benefit other browsers too, not only Firefox.


Status
======

Mostly works. Looks like all essential APIs are implemented.

Known issues
============

described [here](doc/known-issues.md).

Security note
=============

All available Pepper Plugin API documentation usually accompanied with
assertions of enhanced security due to active sandboxing usage. It's
worth to note, that API itself doesn't make any sandboxing, it's only
allows sandboxed implementations. This particular implementation
**doesn't implement any sandbox**. That means if any malicious code breaks
through plugin security, there is no additional barriers. This is the
same level of security as NPAPI Flash have.

Install
=======

Project is using cmake (>=2.8.8) build system.

* Install prerequisites.
```
    Debian/Ubuntu:
    $ sudo apt-get install cmake gcc g++ pkg-config ragel libasound2-dev \
           libssl-dev libglib2.0-dev libpango1.0-dev libgl1-mesa-dev     \
           libevent-dev libgtk2.0-dev libxrandr-dev libxrender-dev       \
           libxcursor-dev libv4l-dev libgles2-mesa-dev libavcodec-dev    \
           libva-dev libvdpau-dev libdrm-dev libicu-dev
    Fedora:
    $ sudo dnf install cmake gcc gcc-c++ pkgconfig ragel alsa-lib-devel openssl-devel \
           glib2-devel pango-devel mesa-libGL-devel libevent-devel gtk2-devel         \
           libXrandr-devel libXrender-devel libXcursor-devel libv4l-devel             \
           mesa-libGLES-devel  ffmpeg-devel libva-devel libvdpau-devel libdrm-devel   \
           pulseaudio-libs-devel libicu-devel

```
* (optional) To enable PulseAudio support, install `libpulse-dev`.
* (optional) To enable JACK support, install `libjack-jackd2-dev` and `libsoxr-dev`

* Create a `build` subdirectory in the root directory, from that folder, call
```
    $ cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    $ make
```

* Put generated `libfreshwrapper-flashplayer.so` into browser plugins directory (`~/.mozilla/plugins`)
  or install system-wide by calling:
```
    # make install
```

By default `make install` will put plugin(s) to `${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/mozilla/plugins`. The
path could be changed either by changing CMake parameter `CMAKE_INSTALL_PREFIX`, or by setting
`MOZPLUGIN_INSTALL_DIR`.

When loaded by browser it will search for `libpepflashplayer.so` in a directories
where it can be: in Chrome (stable/beta/unstable) directory, and in
`/usr/lib/pepperflashplugin-nonfree/` (pepperflashplugin-nonfree puts it there).
It should be enough to get it running, but if it doesn't, specify full path in
`~/.config/freshwrapper.conf`. You may find sample configuration file in `/data`.
It's better to have `manifest.json` alongside with `libpepflashplayer.so`,
actual Flash version will be taken from that manifest.

Hardware-accelerated video decoding code requires relatively new version of libavcodec
(January 2013, version 54.39.0). If you don't have it yet, code won't compile unless
you disable hwdec by `WITH_HWDEC=0`. To do so, change configuration step command to:
```
    $ cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWITH_HWDEC=0 ..
```

Note, even if built, hardware-accelerated video decoding is still disabled by default.
You need to explicitly enable it in configuration file.

Install / OpenGL|ES 2 support
-----------------------------

Although it's better to use system implementation of OpenGL|ES 2, it's still possible
to leverage [ANGLE](https://code.google.com/p/angleproject/) GLES2 emulation by adding
`WITH_GLES2=0` to cmake command line at configuration time (`-DWITH_GLES2=0` to be exact).
Note, that requires C++11-capable compiler.


License
=======

The MIT License. See `LICENSE` file for full text.

Directory `3rdparty/` contains source code from 3rd parties which can be
distributed under other license terms. See source code for details.
