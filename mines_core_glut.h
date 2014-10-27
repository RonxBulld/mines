#ifdef __glut

// i want write log
#include <stdio.h>
#include <stdarg.h>
void log(const char* fmt,...){
	static int f = 0;
	FILE* fp = fopen("jni.log",f?"a+":"w+");
	va_list args;
	va_start(args, fmt);
	vfprintf(fp, fmt, args);
	va_end(args);
	fclose(fp); f=1;
}

/*  core.h   */
#ifndef MINES_CORE_H
#define MINES_CORE_H
enum { WIN_INIT = 0,WIN_QUIT, WIN_FOCUS,WIN_LOST, WIN_TOUCH,WIN_SENSOR,
};
enum { WIN_2D = 0,WIN_3D};

class SceneBase{
	public: 
	virtual int  OnFrame(){ return 0; }
	virtual void OnEvent (int,float,float,float){}
};
void coreSetScene(SceneBase*);
inline int  coreGetWidth();
SceneBase*  coreGetScene();
void coreSetMode(int mode = 0);
inline int  coreGetMode();
inline int  coreGetWidth();
inline int  coreGetHeight();
inline void coreSetWidth(int);
inline void coreSetHeight(int);

//#define main __gUserMain
#endif


/*   core.cpp    */
//BEGIN_INCLUDE(all)
#include "glut.h"
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glaux.lib")
#pragma comment(lib,"glut32.lib")
extern int __gUserMain(); 
struct engine{
	// for gl
	float fovy , zNear , zFar;
	// for user
	int width , height ; 
	int mode;
	SceneBase* user;
};
engine* __gAppEngine;


static void engine_window_resize(int w , int h)
{
	struct engine* engine = __gAppEngine;
	glMatrixMode(GL_PROJECTION);             // project matrix
	glLoadIdentity();
	engine->width = w , engine->height = h; 	
	glViewport(0,0, engine->width, engine->height);                  // view port
	if(engine->mode) gluPerspective(engine->fovy , 1.0f * engine->width / engine->height, engine->zNear , engine->zFar);
	else glOrtho(0,engine->width, engine->height,0,-engine->zFar, engine->zFar);
	glMatrixMode(GL_MODELVIEW);              // modelview matrix
	glLoadIdentity();
}

static void engine_init_egl() {
    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);	         glDepthFunc(GL_LESS);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableClientState(GL_VERTEX_ARRAY);
	glShadeModel(GL_FLAT);
    struct engine* engine = __gAppEngine;
	engine->fovy = 60.0f;
    engine->zNear = 0.0f;
    engine->zFar = 10000.0f;
}

static void engine_draw_frame() {
    if (__gAppEngine->user->OnFrame())  glutSwapBuffers();
}

int main(int argc,char** argv) 
{
	engine en = {0};   __gAppEngine = &en;
	en.width = 800 , en.height = 600 , en.mode = WIN_2D;
	if(__gUserMain() <= 0) return -1;
	glutInit(&argc,argv); 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
	glutInitWindowSize(en.width,en.height); 
	glutInitWindowPosition(200,200); 
	glutCreateWindow("minies v0.1"); 	
    __gAppEngine->user->OnEvent(WIN_INIT,en.width,en.height,0);
	engine_init_egl();
	glutReshapeFunc(engine_window_resize); //glutDisplayFunc(display_cube); 
	glutDisplayFunc(engine_draw_frame); 
	glutIdleFunc(engine_draw_frame); 
	glutMainLoop(); 
	__gAppEngine->user->OnEvent(WIN_QUIT , 0 , 0 , 0);
	return 0;
} 



/* *jjjjjj*  */

void coreSetScene(SceneBase* in)
{
	if ( in != NULL) 
		__gAppEngine->user = in;
}
SceneBase* coreGetScene(){
	return __gAppEngine->user;
}
void coreSetMode(int mode){
	__gAppEngine->mode = mode;
	 engine_window_resize(__gAppEngine->width , __gAppEngine->height);
}
inline int coreGetWidth(){
	return __gAppEngine->width;
}
inline int coreGetHeight(){
	return __gAppEngine->height;
}

inline void coreSetWidth(int w){
    __gAppEngine->width = w;
}
inline void coreSetHeight(int h){
	__gAppEngine->height = h;
}

inline int coreGetMode(){
	return __gAppEngine->mode;
}

#define main __gUserMain

#endif