# Source guide

This is a introductory document to work-in-progress project. Due to its changing nature, some parts
of this page may become outdated. I put here some high-level concepts used in the code. Comments
about particular parts are in the code. If there are no comments, well, that part is not documented.

## Project name

Despite project name is FreshPlayerPlugin, I use term freshwrapper in this document and through
the code, because (i) it shorter, and (ii) most of the code is target plugin agnostic.

## Directory structure

`/src` &mdash; sources <br>
`/tests` &mdash; tests <br>
`/3rdparty` &mdash; source code from third parties <br>
`/3rdparty/ppapi` &mdash; PPAPI headers from Chromium source / PPAPI SDK <br>
`/3rdparty/npapi` &mdash; NPAPI headers from Mozilla Firefox source / Plugin SDK <br>
`/3rdparty/parson` &mdash; JSON library called "parson" <br>

In /src, each unit which name starts with `ppb_` implement particular PPB interface.  Plain,
trusted and private versions go to the same file. `np_functions.c` contains NPAPI plugin part
functions. Some PPAPI interfaces, notably graphics and URL fetching have their code splitted
between `ppb_*.c` and `np_functions.c`.

## Interfaces

Read available documentation for [NPAPI](https://developer.mozilla.org/docs/Plugins) and
[PPAPI](https://developer.chrome.com/native-client/pepper_stable/c).

## Threads

NPAPI plugins are originally was running inside browser process. They were integrated into browsers
main loop, so intensive computation or IO waiting was slowing browser down.  Now all major browsers
run plugins out-of-process, but plugin host process still immitates browser. PPAPI was designed
to support out-of-process operation, but hopefully Flash still uses event-driven model. Despite
similarity, one of Pepper interfaces relies on access to the core message loop to operate. That's
why there are two main threads: one is "browser thread", and other is "plugin thread". When browser
creates plugin instance, freshwrapper creates "plugin thread" and runs message loop instance in it.

Having separate message loop allows reentring it, which in turn required for context menu support.
Having two threads require sophisticated inter-thread call solution. Both NPAPI and PPAPI have
callback support. First does this through `NPP_PluginThreadAsyncCall()`, second through
`PPP_Core.CallOnMainThread()`. Hence the name suffixes for callback functions: `_ptac` and `_comt`.
They have different prototypes, but difference in names makes it easier to discern them.

It's crutial to call Pepper plugin functions on plugin thread, and Xlib and browser (NPN_*)
functions in browser thread.

## Inter-thread calls

Some of the API are callback-based. Function of some PPAPI interface may return
PP_OK_COMPLETIONPENDING instead of result to tell caller that actual result will be delivered
by asynchronous callback. But there are still functions have synchronous operation. Just waiting
for a barrier can create a deadlock when two threads wait for each other. The solution is to run
nested message loops. Plugin thread always have running message loop. Message loop for browser
thread is created if there is no any. It's also crucial to pass correct depth of loop nesting
to exit loops in a correct order.

Some places (p2n_proxy_class.c) use chained callbacks to ensure message loop is running before
actual callback is started.
