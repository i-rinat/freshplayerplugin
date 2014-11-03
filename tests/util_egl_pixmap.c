#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

int
main(void)
{
    Display *dpy = XOpenDisplay(NULL);
    Window root_wnd = DefaultRootWindow(dpy);
    Window wnd = XCreateSimpleWindow(dpy, root_wnd, 0, 0, 400+20, 300+20, 0, 0, 0xa0a0f0);

    printf("root_wnd = %p\n", (void *)root_wnd);

    XMapWindow(dpy, wnd);
    XSync(dpy, False);
    printf("wnd = %p\n", (void *)wnd);

    EGLDisplay egl_dpy = eglGetDisplay(dpy);
    eglInitialize(egl_dpy, NULL, NULL);

    int nconfigs;
    EGLConfig egl_config;
    EGLint cfg_attrs[] = { EGL_ALPHA_SIZE, 0,
                           EGL_BLUE_SIZE, 8,
                           EGL_GREEN_SIZE, 8,
                           EGL_RED_SIZE, 8,
                           EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                           EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
                           EGL_NONE };
    EGLBoolean ret = eglChooseConfig(egl_dpy, cfg_attrs, &egl_config, 1, &nconfigs);

    printf("nconfigs = %d\n", nconfigs);
    printf("egl_config = %p\n", egl_config);

    EGLint ctx_attrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext glc = eglCreateContext(egl_dpy, egl_config, EGL_NO_CONTEXT, ctx_attrs);
    if (glc == EGL_NO_CONTEXT) {
        printf("eglCreateContext returned EGL_NO_CONTEXT\n");
        exit(1);
    }
    printf("context ok\n");

    Pixmap pixmap = XCreatePixmap(dpy, wnd, 400, 300, DefaultDepth(dpy, 0));
    EGLSurface egl_surf = eglCreatePixmapSurface(egl_dpy, egl_config, pixmap, NULL);
    if (egl_surf == EGL_NO_SURFACE) {
        printf("eglCreatePixmapSurface returned EGL_NO_SURFACE\n");
        exit(1);
    }
    printf("pixmap and surface ok\n");

    ret = eglMakeCurrent(egl_dpy, egl_surf, egl_surf, glc);
    if (!ret) {
        printf("eglMakeCurrent failed\n");
        exit(1);
    }
    printf("eglMakeCurrent ok\n");

    glClearColor(1, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFinish();
    eglWaitGL();

    XCopyArea(dpy, pixmap, wnd, DefaultGC(dpy, 0),
              0, 0, 400, 300, 10, 10);
    XSync(dpy, False);

    sleep(10);
    eglTerminate(egl_dpy);
    XFreePixmap(dpy, pixmap);
    XDestroyWindow(dpy, wnd);
    XCloseDisplay(dpy);
    return 0;
}
