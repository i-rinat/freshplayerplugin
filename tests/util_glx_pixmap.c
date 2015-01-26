#undef NDEBUG
#include <assert.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glx.h>


Window      wnd;
Window      root_wnd;
Display    *dpy;
Pixmap      pixmap;
GLXContext  glc;
GLXPixmap   glx_pixmap;


PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;

void
handle_graphics_expose(XEvent *ev)
{
    int ret = glXMakeCurrent(dpy, glx_pixmap, glc);
    assert(ret);
    printf("glXMakeCurrent ok\n");

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glFinish();

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

    glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)
        glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

    printf("root_wnd = %p\n", (void *)root_wnd);

    XSelectInput(dpy, wnd, ExposureMask);
    XMapWindow(dpy, wnd);
    XSync(dpy, False);
    printf("wnd = %p\n", (void *)wnd);

    int nconfigs = -1;
    GLXFBConfig fb_config;
    int cfg_attrs[] = { GLX_ALPHA_SIZE, 0,
                        GLX_BLUE_SIZE, 8,
                        GLX_GREEN_SIZE, 8,
                        GLX_RED_SIZE, 8,
                        GLX_X_RENDERABLE,   True,
                        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT | GLX_PIXMAP_BIT,
                        None };

    GLXFBConfig *fb_cfgs = glXChooseFBConfig(dpy, 0, cfg_attrs, &nconfigs);
    fb_config = fb_cfgs[0];
    XFree(fb_cfgs);

    printf("nconfigs = %d\n", nconfigs);
    printf("fb_config = %p\n", fb_config);

    const int ctx_attrs[] = {
        GLX_RENDER_TYPE,                GLX_RGBA_TYPE,
        GLX_CONTEXT_MAJOR_VERSION_ARB,  2,
        GLX_CONTEXT_MINOR_VERSION_ARB,  0,
        GLX_CONTEXT_PROFILE_MASK_ARB,   GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
        None,
    };
    glc = glXCreateContextAttribsARB(dpy, fb_config, NULL, True, ctx_attrs);
    assert(glc);
    printf("context ok\n");

    pixmap = XCreatePixmap(dpy, wnd, 400, 300, DefaultDepth(dpy, 0));
    glx_pixmap = glXCreatePixmap(dpy, fb_config, pixmap, NULL);
    assert(glx_pixmap);
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

    glXDestroyPixmap(dpy, glx_pixmap);
    XFreePixmap(dpy, pixmap);
    XDestroyWindow(dpy, wnd);
    XCloseDisplay(dpy);
    return 0;
}
