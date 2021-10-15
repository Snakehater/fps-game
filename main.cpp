#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <sys/utsname.h>
 
int main(int argc, char** argv)
{
  Display* dpy = XOpenDisplay(NULL);
  if (dpy == NULL) 
  {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }
 
  int s = DefaultScreen(dpy);
  Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, s), 10, 10, 660, 200, 1,
                                   BlackPixel(dpy, s), WhitePixel(dpy, s));
  XSelectInput(dpy, win, ExposureMask | KeyPressMask);
  XMapWindow(dpy, win);
 
  XStoreName(dpy, win, "Richochet robots");
 
  Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False); 
  XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);  

  bool uname_ok = false;
  struct utsname sname;  
  int ret = uname(&sname);
  if (ret != -1)
  {
    uname_ok = true;
  }
 
  XEvent xevent;
  while (1) 
  {
    XNextEvent(dpy, &xevent);
    if (xevent.type == Expose) 
    {
      int y_offset = 20;
 
      const char* s1 = "X11 test app under Linux";
 
      const char* s2 = "(C)2012 Geeks3D.com"; 
      XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, s1, strlen(s1));
      y_offset += 20;
      XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, s2, strlen(s2));
      y_offset += 20;
 
      if (uname_ok)
      {
        char buf[256] = {0};
 
        sprintf(buf, "System information:");
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf, strlen(buf));
        y_offset += 15;
 
        sprintf(buf, "- System: %s", sname.sysname);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf, strlen(buf));
        y_offset += 15;
 
        sprintf(buf, "- Release: %s", sname.release);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf, strlen(buf));
        y_offset += 15;
 
        sprintf(buf, "- Version: %s", sname.version);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf, strlen(buf));
        y_offset += 15;
 
        sprintf(buf, "- Machine: %s", sname.machine);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf, strlen(buf));
        y_offset += 20;
      }
 
 
      XWindowAttributes  wa;
      XGetWindowAttributes(dpy, win, &wa);
      int width = wa.width;
      int height = wa.height;
      char buf[128]={0};
      sprintf(buf, "Current window size: %dx%d", width, height);
      XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf, strlen(buf));
      y_offset += 20;
    }
 
    if (xevent.type == KeyPress)
    {
      char buf[128] = {0};
      KeySym keysym;
      /* int len = */ XLookupString(&xevent.xkey, buf, sizeof buf, &keysym, NULL);
      if (keysym == XK_Escape)
        break;
    }
 
    if ((xevent.type == ClientMessage) && 
        (static_cast<unsigned int>(xevent.xclient.data.l[0]) == WM_DELETE_WINDOW))
    {
      break;
    }
  }
 
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  return 0;
}