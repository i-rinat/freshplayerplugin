#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>


Window      wnd;
Window      root_wnd;
Display    *dpy;
Pixmap      pixmap;
EGLDisplay  egl_dpy;
EGLContext  glc;
EGLSurface  egl_surf;


void
handle_graphics_expose(XEvent *ev)
{
    EGLBoolean ret;

    ret = eglMakeCurrent(egl_dpy, egl_surf, egl_surf, glc);
    if (!ret) {
        printf("eglMakeCurrent failed\n");
        exit(1);
    }
    printf("eglMakeCurrent ok\n");

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glFinish();
    eglWaitGL();

    XCopyArea(dpy, pixmap, wnd, DefaultGC(dpy, 0),
              0, 0, 400, 300, 10, 10);
    XSync(dpy, False);
}

int
main(void)
{
    dpy = XOpenDisplay(NULL);
    root_wnd = DefaultRootWindow(dpy);
    wnd = XCreateSimpleWindow(dpy, root_wnd, 0, 0, 400+20, 300+20, 0, 0, 0xa0a0f0);

    printf("root_wnd = %p\n", (void *)root_wnd);

    XSelectInput(dpy, wnd, ExposureMask);
    XMapWindow(dpy, wnd);
    XSync(dpy, False);
    printf("wnd = %p\n", (void *)wnd);

    egl_dpy = eglGetDisplay(dpy);
    eglInitialize(egl_dpy, NULL, NULL);

    int nconfigs = -1;
    EGLConfig egl_config;
    EGLint cfg_attrs[] = { EGL_ALPHA_SIZE, 0,
                           EGL_BLUE_SIZE, 8,
                           EGL_GREEN_SIZE, 8,
                           EGL_RED_SIZE, 8,
                           EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                           EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
                           EGL_NONE };
    eglChooseConfig(egl_dpy, cfg_attrs, &egl_config, 1, &nconfigs);

    printf("nconfigs = %d\n", nconfigs);
    printf("egl_config = %p\n", egl_config);

    EGLint ctx_attrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    glc = eglCreateContext(egl_dpy, egl_config, EGL_NO_CONTEXT, ctx_attrs);
    if (glc == EGL_NO_CONTEXT) {
        printf("eglCreateContext returned EGL_NO_CONTEXT\n");
        exit(1);
    }
    printf("context ok\n");

    pixmap = XCreatePixmap(dpy, wnd, 400, 300, DefaultDepth(dpy, 0));
    egl_surf = eglCreatePixmapSurface(egl_dpy, egl_config, pixmap, NULL);
    if (egl_surf == EGL_NO_SURFACE) {
        printf("eglCreatePixmapSurface returned EGL_NO_SURFACE\n");
        exit(1);
    }
    printf("pixmap and surface ok\n");

    // message loop
    while (1) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        switch (ev.type) {
        case GraphicsExpose:
        case Expose:
            handle_graphics_expose(&ev);
            break;
        default:
            // do nothing
            break;
        }
    }

    eglTerminate(egl_dpy);
    XFreePixmap(dpy, pixmap);
    XDestroyWindow(dpy, wnd);
    XCloseDisplay(dpy);
    return 0;
}
