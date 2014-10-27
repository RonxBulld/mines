/*  有必要单独将颜色单独列出么，我认
为很有必要，mines使用了gl作为绘图支
持，最终生成点的颜色受很多因素影响，
  顶点颜色 (STATE_COLOR)$尚未测试
  纹理颜色 (STATE_TEXTURE)
  全局颜色 (auxSetColor)
  混合函数 (STATE_BLEND)
  // 其中顶点颜色尚未测试,固未打开
  生效顺序按照上面排列依次作用，尽
管受影响的因素如此之多，但是我们不需要
去具体关心它们是如何作用的，只需要设定
我们需要什么不需要什么就足够了，这里说
一下初始值(因为混合的存在，mines使用
RGBA颜色，如果没有特别说明，alpha值
默认为255)
  顶点颜色 白色(255,255,255,255)
  纹理颜色 取决于纹理，alpha默认255
  全局颜色 默认应该是白色吧
  混合函数 默认是依照源的alpha混合
/\席八席八
请不要疑心，为什么mines没有提供点和直
线的函数，在教程的最后，你会接触到最核
心的mines绘图功能，如果自己实现一个会
不会有一些小小的成就感呢...诶嘿嘿~~~
*/

// 告诉gcc我们使用 gles
#define __gles  1
// 下一句注释告诉c4链接到 gles 库
//#include <android_native_app_glue.h>

// mines 头文件
#include "mines_core_gles.h"
#include "mines_core_glut.h"
#include "mines_aux.h"
#include "mines_test.h"
// 其他头文件
#include <math.h>
//
// 一个宏
#define R(i)  (random()%(int)(i))
// 我需要 min,max ~ 请勿吐槽
#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
// 获取矩形交集,这个代码是从我的另一 个文件里面扣出来的
Rect* GetActiveArea(Rect& src,
Rect& dst){
	static Rect rc = {0};
	rc.l = max(src.l,dst.l);
	rc.t = max(src.t,dst.t);
	rc.r = min(src.r,dst.r);
	rc.b = min(src.b,dst.b);
	// is a rect ?
	if(rc.r < rc.l || rc.b < rc.t)  return 0;
	return &rc;
}

//
/* 我们进行一个小小的碰撞测试，哈哈哈
 物体都是矩形，我们需要定义它的位置和
 大小(同时作为它的质量)以及它的速度，
 最后我们需要它的颜色和透明度，还记得
 要加上重力(其实是y方向的加速度)咯～
 */
struct obj{
	float x,  y,  m;  //位置，大小
	float vx, vy, f;  //速度，重力
	uchar r,g,b,  a;  //颜色
	obj(){
		int w,h;   // 保存窗口尺寸
		w = coreGetWidth();
		h = coreGetHeight();
		x = 50 + R(w - 100);
	}
};
// 我们的场景
class MyScene:
   public SceneBase{
public:
   // 往场景里添加 物体
public:
   int OnFrame(){
   	// 开始绘制咯
   	auxClear(); 
   	// 开启自动复位
   	auxSetState( STATE_BACKLOCATE);
   	// 我们的矩形大小，还记得 mines矩形的定义么
   	// 不关心位置信息，事实上我喜 欢用 auxLocate 来定位，得益于自动 复位功能~(我不喜欢计算坐标)
   	Rect rc = {0,0,200,200};
     // 切记默认的alpha值是255
     
   }
};

// 已经不用解释下面的了吧，应该 ^_-
int main(){
	coreSetScene(new MyScene);
	return 1;  
}