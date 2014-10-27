
/*   test.h    */

// these functions are just for test,based on aux.h ,so it may be undelicious
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#define RGBA(r,g,b,a) (uchar(r)| (uchar(g)<<8)| (uchar(b)<<16)|uchar(a)<<24))
#define RGB(r,g,b)  RGBA(r,g,b,255)

struct Rect {float l,t,r,b;};

class testImage { 	
public:
  int w , h ; uint *data;
  testImage(int,int);
  ~testImage();
  inline uint& Pixel(int,int);
  int Save(const char*);
  void Line(int x1, int y1, int x2, int y2, uint color = -1);
	void Circle(int cx , int cy , int r , uint c = -1);
	void FillRect(Rect* rc = 0 , uint c = -1);
};
testImage* testBmpLoad(const char*);


class testTexture {
public:
	uint tex;	int  w,h;
public:
  testTexture(testImage* img = NULL){
  	
  	if(img) w = img->w,h = img->h , tex = auxGenTexture(w,h,img->data);
  	else tex = 0, w = h = 0;
  }
  ~testTexture(){
 	if(Check())auxDelTex(tex); 
  	tex = 0; w = h = 0;
  }
  int Check(){
  	return tex && w>0 && h>0;
  }
  uint take(){
  	if(Check()) return tex;
  	else return log("Want to Get a Invalid Texture!\n"),0;
  }
};
#define P(i,_x,_y,_u,_v) pt[i].x = _x, pt[i].y = _y, pt[i].u = _u, pt[i].v = _v, pt[i].c = -1
void logArray(int n,Point* pt){
	for(int i=0; i<n; i++)
	  log("%d. %.2f  %.2f  %.2f  %.2f\n\n",i, pt[i].x,pt[i].y, pt[i].u,pt[i].v);
	log("\n\n\n");
}
inline void testDrawLine(float x2, float y2, float x1 = 0, float y1 = 0){	 
  Point pt[2]; 
  P(0,x1,y1,0,0);
  P(1,x2,y2,1,1);
  logArray(2,pt);
  auxDrawArray(DRAW_LINES, 2,pt);
}


void testDrawRect(Rect* dst=0, int bFill = 1){
  Rect r = {-1,-1, coreGetWidth() +1, coreGetHeight()+1};
	if(dst == 0) dst = &r;
	Point pt[4] = {0};
	P(0,dst->l,dst->t,0,0);
  P(1,dst->r,dst->t,1,0);
	P(2,dst->r,dst->b,1,1);
  P(3,dst->l,dst->b,0,1);	
  auxDrawArray(bFill ? DRAW_POLY:DRAW_FRAME,4,pt);
}
void testTexCopy(testTexture* tex, Rect* src = 0,Rect* dst = 0){
  // check the params 5050100100
  if(!tex || !tex->Check()) return;
  Rect rc = {0,0, tex->w,tex->h} , rt = rc;
	if(!src) src=&rc;
	if(!dst) dst=&rt, dst->r = src->r-src->l , dst->b = src->b-src->t;
	// store the state
  int  bs = auxTestState(STATE_TEXTURE);
  uint ct =auxGetCurTex();       
  if(!bs) auxSetState(STATE_TEXTURE);
  if(ct != tex->take())
    auxSetCurTex(tex->take());
	// prepare the vertexs
	float l = src->l/tex->w;
	float r = src->r/tex->w;
	float t = src->t/tex->h;
	float b = src->b/tex->h;
	Point pt[4] = {0};
	P(0,dst->l,dst->t,l,t);
  P(1,dst->r,dst->t,r,t);
	P(2,dst->r,dst->b, r,b);
  P(3,dst->l,dst->b, l,b);
  // draw rect !
  //logArray(4,pt);
  	auxDrawArray(DRAW_POLY,4,pt);
	// backup state
	if(ct != tex->take())
     auxSetCurTex(t);     
  if(!bs)auxSetState( STATE_TEXTURE,0);
}
void testRotate(float a , float x = 0, float y = 0){
	// must open backlocate
  auxLocate(x,y);
	auxRotate(a);
	auxLocate(-x,-y);
}





testImage::testImage(int _w,int _h){
	w=_w,h=_h;
	data=(uint*)malloc(w*h*4);
	memset(data,0,w*h*4);
}
testImage::~testImage(){
	w=h=0; free(data); data=0;
}

inline uint& testImage::Pixel(int x,int y){
	static uint i = 0;
  if(x<0||x>w-1) return i;
  if(y<0||y>h-1) return i;
	return data[y*w+x] ; 
}

	
void testImage::Line(int x1 , int y1 , int x2 , int y2, uint c) {
	int l = sqrt( (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) );
	if(l == 0) l = 1;
	for(int i=0;i <= l ; i++) {
		int x=(x1*i+x2*(l-i))/l;
		int y=(y1*i+y2*(l-i))/l;
		Pixel(x,y)=c; 
	}
}
void testImage::Circle(int cx , int cy , int r , uint c) {
    int x=0,y=r , d = 1 - r;
    while(x<=y)
		Pixel(cx+x,cy+y)=c,    Pixel(cx+y,cy+x)=c,
		Pixel(cx-x,cy+y)=c,    Pixel(cx-y,cy+x)=c, 
		Pixel(cx-x,cy-y)=c,    Pixel(cx-y,cy-x)=c, 
		Pixel(cx+x,cy-y)=c,    Pixel(cx+y,cy-x)=c,
		d = d + (d<0?2*x+3: 2*(x-y--)+5),x++;
}
		
	
int testImage::Save(const char* bmp){
		uchar*q;  uint head[] = {*(uint*)"BM"<<16,w*h*4+54,0,54,40,w,h,(32<<16)+1, 0 , 0 , 3780 , 3780 , 0 , 0 , 0};
	FILE* fp = fopen(bmp,"w+b");
		if(fp==NULL)return 0;
		uint buf[10240];	
      fwrite((char*)head+2,1,54,fp);
		for(int y=0;y<h;y++){
	      for(int x=0;x<w;x++)
	  q=(uchar*)&data[x+(h-1-y)*w], buf[x]=q[2]|(q[1]<<8)|(q[0]<<16);
 fwrite(buf,1,w*4,fp);
		}
     fclose(fp); return 1;
}



testImage* testBmpLoad(const char* bmp){
uchar t[54],buf[40960],*q;
int a,s,w,h;
testImage* img=0;
FILE*fp=fopen(bmp,"rb"); 
if(fp==NULL) return NULL;
 fread(t,1,54,fp);
img = new testImage(w=*(int*)
(t+18),h=*(int*)(t+22)), a=*(uchar*)
(t+28)/8,s=(w*a+3)&~3; //p=(uchar*)img->p;
	if(a==3)for(int y=0; y<h;y++){
	//	fseek(fp , 54 + y*s , SEEK_SET);   
		int r = fread(q=buf,1,s,fp);	
		for(int x=0;x<w;x++)
  img->data[x+(h-1-y)*w]=q[2]| (q[1]<<8)|(q[0]<<16)|(255<<24),q+=a;
	}
	else for(int y=0; y<h;y++){
	//	fseek(fp , 54 + y*s , SEEK_SET);
		fread(q=buf,1,s,fp);
		for(int x=0;x<w;x++)
  img->data[x+(h-1-y)*w]=q[2]| (q[1]<<8)|(q[0]<<16)|(q[3]<<24),q+=a;
	}
fclose(fp);  return img;
}