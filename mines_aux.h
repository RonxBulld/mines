/*   aux.h    */
#ifndef MINES_AUX_H
#define MINES_AUX_H

typedef unsigned char uchar;
typedef unsigned int  uint;
enum { STATE_TEXTURE=1<<0, 
STATE_BLEND=1<<1, 
STATE_COLOR=0,           // untest,so invalid it
STATE_BACKLOCATE=1<<8};
#define STATE_ALL (STATE_TEXTURE+STATE_BLEND+STATE_COLOR+STATE_BACKLOCATE)
enum{ DRAW_POINTS=0 , DRAW_LINES,DRAW_STRIP, DRAW_FRAME,DRAW_TRIANGLES, DRAW_POLY};
struct Point{
	union { 
	struct { uchar r,g,b,a; };
	uint c; };
	float x,y,z,u,v; };
const char* auxGetErrorInfo();
inline void auxClear();
inline void auxSetColor(uchar r, uchar g, uchar b, uchar a = 255);
inline void auxSetCurTex(uint);
inline uint auxGetCurTex();
inline int auxTestState(uint);
void auxSetState(uint stat,int bOpen = 1);
inline void auxLocate(float x, float y,float z = 0.0f);
inline void auxRotate(float a, float x = 0,float y = 0,float z = 1);
inline void auxScale(float x, float y, float z = 1);
inline void auxInitLocate();
void auxDrawArray(int type,int n, Point pt[]);

// texture
uint auxGenTexture(int, int, uint*);
void auxDelTex(uint);
// u must add this to aux.h
//#define glEnable
//#define glEnableClientState
#endif

/*  aux.cpp   */
//#include "mines_aux.h"
//#define glPushMatrix()  0
//#define glPopMatrix()   0
#define __gZAdd()  
extern inline int coreGetMode();
static int __gDrawType[] = { GL_POINTS,GL_LINES, GL_LINE_STRIP,GL_LINE_LOOP,
GL_TRIANGLES,GL_TRIANGLE_FAN};
#define __gGetStat(T)  (__gUserState & (T))
#define __GetStat(S,T) ((S)&(T))
uint __gUserState = 0;
uint __gCurTexture = 0;
double __gZf = 0.0f;
inline int auxTestState(uint s){
	return __gGetStat(s);
}
void auxSetState(uint stat,int bOpen){
	if(bOpen){
	  if(__GetStat(stat, STATE_TEXTURE) && !__gGetStat(STATE_TEXTURE))
		glEnable(GL_TEXTURE_2D),glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	  if(__GetStat(stat, STATE_BLEND) && !__gGetStat(STATE_BLEND))
		glEnable(GL_BLEND);	
	  if(__GetStat(stat, STATE_COLOR) && !__gGetStat(STATE_COLOR))
	    glEnableClientState(GL_COLOR_ARRAY);
	  if(__GetStat(stat, STATE_BACKLOCATE) && !__gGetStat(STATE_BACKLOCATE))
	      glPushMatrix();
	  __gUserState |= stat;
	}
	else{ 	
		if(__GetStat(stat, STATE_TEXTURE) && __gGetStat(STATE_TEXTURE))
		glDisable(GL_TEXTURE_2D),glDisableClientState( GL_TEXTURE_COORD_ARRAY);
	  if(__GetStat(stat, STATE_BLEND) && __gGetStat(STATE_BLEND))
		glDisable(GL_BLEND);	
	  if(__GetStat(stat, STATE_COLOR) && __gGetStat(STATE_COLOR))
	    glDisableClientState( GL_COLOR_ARRAY);
	  	      if(__GetStat(stat, STATE_BACKLOCATE) && !__gGetStat(STATE_BACKLOCATE))
	      glPopMatrix();
	  __gUserState &= ~stat;
	}
}

inline void auxClear(){
  glLoadIdentity();
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //auxSetState(STATE_ALL,0);
  __gZf = 0.0f;
}

inline void auxSetColor(uchar r, uchar g, uchar b, uchar a){
	glColor4f(r/255.f,g/255.f,
	   b/255.f,a/255.f);
}
inline void auxSetCurTex(uint t)
{  
glBindTexture(GL_TEXTURE_2D,t);__gCurTexture = t;
}
inline uint auxGetCurTex(){
	return __gCurTexture;
}
inline void auxLocate(float x, float y, float z){
	glTranslatef(x,y,z);
}
inline void auxRotate(float a, float x,float y,float z){
	glRotatef(a,x,y,z);
}
inline void auxScale(float x, float y, float z){
	glScalef(x,y,z);
}
inline void auxInitLocate(){
	glLoadIdentity();
}
void auxDrawArray(int type,int n, Point pt[]){
	if(coreGetMode() == 0)  glTranslatef(0,0,__gZf+=.5);
	glVertexPointer(3, GL_FLOAT, sizeof(Point), &pt->x);	 
	if(__gGetStat(STATE_COLOR))
	  glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Point), &pt->c);  // wait to write
	if(__gGetStat(STATE_TEXTURE))
		 glTexCoordPointer(2, GL_FLOAT, sizeof(Point), &pt->u);
	glDrawArrays( __gDrawType[type],0,n);
	if(__gGetStat( STATE_BACKLOCATE)) glPopMatrix(),glPushMatrix();
}


/*
void imgUpdateTexture(uint tex,image* img, int x = 0, int y = 0){
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D,
0,x,y,img->w,img->h,GL_RGBA,GL_RGBA,img->data);
}
*/
uint auxGenTexture(int w, int h, uint* data){
	if(w <= 0 || h<= 0 || data == 0) return 0;
	uint tex = 0;
	glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
        // tell gl dont give MaSaiKe while i was stretch it
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

				// GL_RGBA, we use rgba
	// GL_RGBA , the pixel format
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,w, h, 0, GL_RGBA , GL_UNSIGNED_BYTE, data);
	return tex;
}

void auxDelTex(uint tex){
	glDeleteTextures(1,&tex);
}

//void* imgReadPixels(x,y,w,h){
//	glPixelStorei (GLenum pname, GLint param);}



const char* auxGetErrorInfo()
{
	switch (glGetError())
	{
	case GL_NO_ERROR:
		return NULL;
	case GL_INVALID_ENUM:
		return ("GL Invalid Enum");
	case GL_INVALID_VALUE:
		return ("GL Invalid Value");
	case GL_INVALID_OPERATION:
		return ("GL Invalid Operation");
	case GL_OUT_OF_MEMORY:
		return ("GL Out Of Memory");
		//case GL_INVALID_FRAMEBUFFER_OPERATION:
		//	return ("GL Invalid FrameBuffer Operation\n");
	case  GL_STACK_OVERFLOW:
		return ("GL Stack Overflow");
	case GL_STACK_UNDERFLOW:
		return ("GL Stack Underflow");
		//case GL_TABLE_TOO_LARGE:
		//	return ("GL Table Too Large\n");
	};
	return "GL Undefined Error";
}