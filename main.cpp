#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <GL/glx.h>
#include <GL/gl.h>
// #include <GL/glut.h>	
// #include <GL/glu.h>

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

struct triangle triangles[ 12 ];

static double GetMilliseconds() {
	static timeval s_tTimeVal;
	gettimeofday(&s_tTimeVal, NULL);
	double time = s_tTimeVal.tv_sec * 1000.0; // sec to ms
	time += s_tTimeVal.tv_usec / 1000.0; // us to ms
	return time;
}


GLuint loadBMP_custom( const char * imagepath ) {
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;
	
	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file) {
		printf("Image could not be opened\n");
		return 0;
	}

	if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
	    printf("Not a correct BMP file\n");
	    return false;
	}

	if ( header[0]!='B' || header[1]!='M' ){
	    printf("Not a correct BMP file\n");
	    return 0;
	}
	
	// Read ints from the byte array
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)
		imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)
		dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// return GLuint
	return textureID;
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

	glXMakeCurrent( dpy, win, ctx );
	glMatrixMode( GL_PROJECTION );
	glEnable( GL_CULL_FACE ); 
	glCullFace( GL_FRONT );
    	glFrustum( -1, 1, 1, -1, -1, 1 );
	
		glClearColor (0, 0.5, 1, 1);
		glClear (GL_COLOR_BUFFER_BIT);
		glXSwapBuffers (dpy, win);

	
	fill_triangle_buffer();

	// Load and bind texture
	glEnable(GL_TEXTURE_2D);
	loadBMP_custom("textures/texture.bmp");


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
		//Render();
		
glClear( GL_COLOR_BUFFER_BIT );	
glBegin(GL_TRIANGLES);
	glColor3f( 255, 255, 255 );
/*
glTexCoord2f( 0.0f, 1.0f );
	glVertex3f( -0.5f,  0.5f, 0.0f );
glTexCoord2f( 1.0f, 1.0f );
	glVertex3f(  0.5f,  0.5f, 0.0f );
glTexCoord2f( 0.0f, 0.0f );
	glVertex3f( -0.5f, -0.5f, 0.0f );
		
	glVertex3f(  0.5f,  0.5f, 0.0f );
glTexCoord2f( 1.0f, 0.0f );
	glVertex3f(  0.5f, -0.5f, 0.0f );
	glVertex3f( -0.5f, -0.5f, 0.0f );
*/

	 // first triangle, bottom left half
	glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -0.5f, -0.5f, 0 );
	glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -0.5f,  0.5f, 0 );
	glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  0.5f, -0.5f, 0 );

	// second triangle, top right half
	glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  0.5f, -0.5f, 0 );
	glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -0.5f,  0.5f, 0 );
	glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  0.5f,  0.5f, 0 );

glEnd();
glFlush();
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
				{  1.0f,  0.0f,  0.0f },
				{ -0.5f, -0.5f, -0.5f },
				{  0.5f,  0.5f, -0.5f },
				{  0.5f, -0.5f, -0.5f }  };
	triangles[ 1 ] = {   
				{  0.0f,  0.0f,  1.0f },
				{ -0.5f, -0.5f, -0.5f },
				{ -0.5f,  0.5f, -0.5f },
				{  0.5f,  0.5f, -0.5f } };
	triangles[ 2 ] = {   
				{  0.0f,  0.0f,  1.0f },
				{  0.5f,  0.5f, -0.5f },
				{  0.5f, -0.5f,  0.5f },
				{  0.5f, -0.5f, -0.5f }  };
	triangles[ 3 ] = {   
				{  1.0f,  1.0f,  0.0f },
				{  0.5f,  0.5f, -0.5f },
				{  0.5f,  0.5f,  0.5f },
				{  0.5f, -0.5f,  0.5f } };
	triangles[ 4 ] = {   
				{  1.0f,  0.0f,  0.0f },
				{ -0.5f, -0.5f,  0.5f },
				{  0.5f, -0.5f,  0.5f },
				{ -0.5f,  0.5f,  0.5f }  };
	triangles[ 5 ] = {   
				{  0.0f,  0.0f,  1.0f },
				{  0.5f, -0.5f,  0.5f },
				{  0.5f,  0.5f,  0.5f },
				{ -0.5f,  0.5f,  0.5f } };
	triangles[ 6 ] = {   
				{  0.0f,  0.0f,  1.0f },
				{ -0.5f,  0.5f, -0.5f },
				{ -0.5f, -0.5f,  0.5f },
				{ -0.5f,  0.5f,  0.5f }  };
	triangles[ 7 ] = {   
				{  1.0f,  1.0f,  0.0f },
				{ -0.5f,  0.5f, -0.5f },
				{ -0.5f, -0.5f, -0.5f },
				{ -0.5f, -0.5f,  0.5f } };
	triangles[ 8 ] = {   
				{  1.0f,  0.0f,  0.0f },
				{ -0.5f,  0.5f, -0.5f },
				{ -0.5f,  0.5f,  0.5f },
				{  0.5f,  0.5f,  0.5f }  };
	triangles[ 9 ] = {   
				{  0.0f,  0.0f,  1.0f },
				{ -0.5f,  0.5f, -0.5f },
				{  0.5f,  0.5f,  0.5f },
				{  0.5f,  0.5f, -0.5f } };
	triangles[ 10 ] = {   
				{  0.0f,  0.0f,  1.0f },
				{  0.5f, -0.5f, -0.5f },
				{  0.5f, -0.5f,  0.5f },
				{ -0.5f, -0.5f,  0.5f }  };
	triangles[ 11 ] = {   
				{  1.0f,  1.0f,  0.0f },
				{  0.5f, -0.5f, -0.5f },
				{ -0.5f, -0.5f,  0.5f },
				{ -0.5f, -0.5f, -0.5f } };
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
		glVertex3f( tr.p1[ 0 ], tr.p1[ 1 ], tr.p1[ 2 ] );
		glVertex3f( tr.p2[ 0 ], tr.p2[ 1 ], tr.p2[ 2 ] );
		glVertex3f( tr.p3[ 0 ], tr.p3[ 1 ], tr.p3[ 2 ] );
	}


	glEnd();
	glFlush();

}

float dx, dy;
float prevx, prevy;
uint8_t prev_defined = 0;
uint8_t key_down = 0;

void HandleEvents( XEvent ev ) {
	int x, y;

	switch ( ev.type ) {
		case ButtonPress:
			if ( ev.xbutton.button == 1 ) {
				std::cout << "Left mouse down \n";
    				// glRotatef( 0.01, 0.0, 0.0, 1.0 );
				if ( key_down == 0 )
					prev_defined = 0;
				key_down = 1;
			}
			break;
		case ButtonRelease:
			if ( ev.xbutton.button == 1 ) {
				std::cout << "Left mouse up \n";
				key_down = 0;
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
			if ( key_down == 0 )
				break;
			if ( !prev_defined ) {
				prevx = x;
				prevy = y;
				prev_defined = 1;
				break;
			}
			
			dx = x - prevx;
			dy = y - prevy;

			prevx = x;
			prevy = y;

			glRotatef( -dy/10, 1.0f, 0.0f, 0.0f );
			glRotatef( -dx/10, 0.0f, 1.0f, 0.0f );
			
			//std::cout << "Mouse X:" << x << ", Y: " << y << "\n";
			break;	
	}
}
