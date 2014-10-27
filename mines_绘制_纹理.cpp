/* 这一次我们继续把精力放到OnFrame 上,你将学会使用mines进行纹理绘制
*/
// tell gcc we use gles
//#define __gles 1
#define __glut  1
// you must include glue to tell c4droid link to gles lib
//#include <android_native_app_glue.h>

// mines headers
#include "mines_core_gles.h"
#include "mines_core_glut.h"
#include "mines_aux.h"
#include "mines_test.h"

// 设置图片路径，你可以修改为自己的
// 到了这里已经不想写注释了，so
#ifdef   __glut 
#define  __bmp__ "sdcard/1.bmp"
#endif
#ifdef   __gles
#define  __bmp__ "/sdcard/mines/1.bmp"
#endif
// other headers
#include <stdio.h>
// My App Scene
class MyScene:
   public SceneBase{
public:
  // a clear scene 
	int OnFrame(){
 // RGBA,if u need alpha 
 // get the texture!
 static testTexture* tex = 0;
 if(tex == 0){
  testImage* img = testBmpLoad(__bmp__); 
   if(!img)log("Load Image Failed\n");
  else tex = new testTexture(img);
  delete img;
 }
 
 // before all should clean
 auxClear();
 auxSetState( STATE_ALL );
 int w = coreGetWidth();
 int h = coreGetHeight();
 int n = 4;
 auxSetColor(255,255,255,168);
 auxSetCurTex(tex->take());
 testDrawRect(0);
 for(int i= 0; i< n; i++){
	testRotate(i*360/n,w/2,h/2);
	auxLocate(w/2,200);
  testTexCopy(tex);
 }
 return 1;
}
   };

int main(){
	// 450 * 800 my cell's screen size
	coreSetWidth(450);
	coreSetHeight(800);
	// set user scene
	coreSetScene(new MyScene);
	//engine_window_run();
	// return 1 all ok
	// -n exit 
	return 1;  
}