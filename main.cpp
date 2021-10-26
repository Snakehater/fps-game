#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <GL/glx.h>
#include <GL/gl.h>

#include <sys/time.h>
#include <unistd.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092
typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);


#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	800
#define FPS 30

uint8_t shutdown = 0;

#define SKIP_TICKS ( 1000 / FPS )

// macros
void Render( void );
void HandleEvents( XEvent ev );
void Resize( int w, int h );
void Shutdown( void );
void fill_triangle_buffer( void );

struct triangle { 
	float color[ 3 ];
	float p1[ 3 ];
	float p2[ 3 ];
	float p3[ 3 ];
}; 

struct triangle triangles[2];

static double GetMilliseconds() {
	static timeval s_tTimeVal;
	gettimeofday(&s_tTimeVal, NULL);
	double time = s_tTimeVal.tv_sec * 1000.0; // sec to ms
	time += s_tTimeVal.tv_usec / 1000.0; // us to ms
	return time;
}


int main (int argc, char ** argv){
	Display *dpy = XOpenDisplay(0);
	Window win;
	XEvent ev;

	int nelements;
	GLXFBConfig *fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), 0, &nelements);

	static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, None };
	XVisualInfo *vi = glXChooseVisual(dpy, DefaultScreen(dpy),attributeList);
	
	// Set Window attributes
	XSetWindowAttributes swa;
	swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);

	// Select window inputs to be triggered in the eventlistener
	XSelectInput( dpy, win, PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask );
	
	XMapWindow (dpy, win);

	//oldstyle context:
	//	GLXContext ctx = glXCreateContext(dpy, vi, 0, GL_TRUE);

	std::cout << "glXCreateContextAttribsARB " << (void*) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB") << std::endl;
	GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

	int attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		0};

	
	// Redirect Close
	Atom atomWmDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &atomWmDeleteWindow, 1);

	// Create GLX OpenGL context

	GLXContext ctx = glXCreateContextAttribsARB(dpy, *fbc, 0, true, attribs);

	glXMakeCurrent (dpy, win, ctx);
	glMatrixMode( GL_PROJECTION );
    	glOrtho( -1, 1, 1, -1, -1, 1 );

		glClearColor (0, 0.5, 1, 1);
		glClear (GL_COLOR_BUFFER_BIT);
		glXSwapBuffers (dpy, win);

	
	fill_triangle_buffer();

	// prepare gameloop
	
	double prevTime = GetMilliseconds();
	double currentTime = GetMilliseconds();
	double deltaTime = 0.0;

	timeval time;
	long sleepTime = 0;
	gettimeofday(&time, NULL);
	long nextGameTick = (time.tv_sec * 1000) + (time.tv_usec / 1000);	

	while ( shutdown != 1 ) {
		// Get events if there is any
		if (XPending(dpy) > 0) {
			XNextEvent(dpy, &ev);
			if (ev.type == Expose) {
				XWindowAttributes attribs;
				XGetWindowAttributes(dpy, win, &attribs);
				Resize(attribs.width, attribs.height);
			}
			if (ev.type == ClientMessage) {
				if (ev.xclient.data.l[0] == atomWmDeleteWindow) {
					break;
				}
			}
			else if (ev.type == DestroyNotify) { 
				break;
			}
		}
		
		// Framelimit calculations, before heavy load
		currentTime = GetMilliseconds();
		deltaTime = double( currentTime - prevTime ) * 0.001;
		prevTime = currentTime;
		
		// Do work
		HandleEvents( ev );
		Render();
		glXSwapBuffers( dpy, win );
		
		// Limit Framerate
		gettimeofday( &time, NULL );
		nextGameTick += SKIP_TICKS;
		sleepTime = nextGameTick - ( (time.tv_sec * 1000) + (time.tv_usec / 1000) );
		usleep((unsigned int)(sleepTime/1000));
	}

	ctx = glXGetCurrentContext(); 
	glXDestroyContext(dpy, ctx); 
}

void fill_triangle_buffer( void ){
	triangles[ 0 ] = {   
				{  1.0f,  0.0f, 0.0f },
				{ -0.5f, -0.5f, 0.0f },
				{  0.5f, -0.5f, 0.0f },
				{ -0.5f,  0.5f, 0.0f }  };
	
	triangles[ 1 ] = {   
				{  0.0f,  0.0f, 1.0f },
				{  0.5f, -0.5f, 0.0f },
				{  0.5f,  0.5f, 0.0f },
				{ -0.5f,  0.5f, 0.0f } };
}

void Resize(int w, int h) {
	glViewport(0, 0, w, h);
}

void Shutdown() {
	shutdown = 1;	
}


void Render( void ) {
	glClearColor ( 1.0f, 1.0f, 1.0f, 1 );
	glClear( GL_COLOR_BUFFER_BIT );	
	glBegin(GL_TRIANGLES);
		/*glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( test_cnt, 0.0f, 1.0f );
		glVertex3f( 1.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, -1.0f, 1.0f );
		*/

	int numTriangles = sizeof(triangles)/sizeof(triangles[0]);
	for ( int i = 0; i < numTriangles; i++ ) {
	glClear( GL_COLOR_BUFFER_BIT );	
	glBegin(GL_TRIANGLES);
		struct triangle tr = triangles[ i ];
		glColor3f( tr.color[ 0 ], tr.color[ 1 ], tr.color[ 2 ] );
		glVertex3f( tr.p1[ 0 ], tr.p1[ 1 ], tr.p1[ 1 ] );
		glVertex3f( tr.p2[ 0 ], tr.p2[ 1 ], tr.p2[ 1 ] );
		glVertex3f( tr.p3[ 0 ], tr.p3[ 1 ], tr.p3[ 1 ] );
	}


	glEnd();
	glFlush();

}

void HandleEvents( XEvent ev ) {
	int x, y;

	switch ( ev.type ) {
		case ButtonPress:
			if ( ev.xbutton.button == 1 ) {
				std::cout << "Left mouse down \n";
			}
			break;
		case ButtonRelease:
			if ( ev.xbutton.button == 1 ) {
				std::cout << "Left mouse up \n";
			}
			break;
		case KeyPress:
			if ( ev.xkey.keycode == 9 ) { // ESC
				Shutdown();
			}
			break;
		case MotionNotify:
			x = ev.xmotion.x;
			y = ev.xmotion.y;
			//std::cout << "Mouse X:" << x << ", Y: " << y << "\n";
			break;	
	}
}
