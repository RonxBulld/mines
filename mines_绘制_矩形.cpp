// 在安卓上我们使用 gles
#define __gles  1
// 在c4一定要记得加上下面这一句哦
//#include <android_native_app_glue.h>
// mines头文件
#include "mines_core_gles.h"
#include "mines_core_glut.h"
#include "mines_aux.h" 
#include "mines_test.h"
//
// 
// 我们的场景，在这个例子以及接下来的
// 例子,我们将只关心绘制，OnFrame
// 很难用几句话说清mines的全部绘图
// 规则，我尽量用简单的代码告诉你如何
// 正确的绘制你想需要的东西
class MyScene:
   public SceneBase{
public:
  // 绘图函数
	int OnFrame(){
		auxClear(); // 清理上一帧数据
	// 全程打开自动复位功能，接下来我们
	// 使用的是绝对坐标，事实上mines
	// 使用的是相对坐标
	// clear 之后，坐标归零
    auxSetState( STATE_BACKLOCATE);
  // 设置使用的颜色为绿色，默认白色
    auxSetColor(0,255,0); 
    // 以后的绘制都是绿色的哦
    // 将要绘制的矩形，不同于 SDL_Rect,mines使用的矩形由左上角坐标及右下角坐标构成，因为我是学VC的嘛，深受 Windows API 毒害
    // 位于 200,200,大小200,200
    Rect rc={200,200,400,400};
    // 绘制一个矩形
    testDrawRect(&rc);
    // 设置边框是紫色的
    auxSetColor(255,0,255); 
    // 绘制边框,参数0表示不填充 (默认参数为1,表示填充)
    testDrawRect(&rc,0);
    //由于开启了自动复位，每次绘制完 成后，参考坐标被恢复到零，下面我们使用 相对坐标来尝试绘制
    // 设置绘制颜色 亮蓝色
    auxSetColor(128,128,255);
    // 直接定位到之前绘制矩形的中心
    auxLocate(300,300);
    // 接下来绘制之前，使用的坐标都 是相对于之前定位点的坐标
    // 使用相对坐标的好处就是我们只 需要关心物体的形状，而不用关心其位置
    Rect rd = {-25,-25,25,25};
    testDrawRect(&rd);
    // 绘制完成，坐标被自动归零
    // OK，下面开始绘制更多
    // 获取屏幕中点～
    int cx = coreGetWidth()/2;
    int cy =coreGetHeight()/2;
    int n = 12;  // 绘制12个
    // 矩形大小，不关心位置哦
    Rect re = {-10,-30,10,30};
          /* 当我们绘制一个由几个个体组
成的整体时，个体之间使用相对坐标时，自
动复位可能会对我们的绘制造成困扰，那么
可以暂时关闭自动复位
*/    	    	      
  // 关闭自动复位(自动复位原理，开启 自动复位，记录当时位置，每次绘制后恢复，关闭自动复位时，也会恢复一次啦
      auxSetState( STATE_BACKLOCATE,0);
    for(int i = 0;i<n;i++){
    	// 绕中心旋转角度～
      testRotate(i*360/n, cx,cy);
    	// 定位到离中心半径200的地方
    	auxLocate(cx,cy-200);
    	// 设置颜色
    	auxSetColor(255,0,0);
    	// 绘制矩形
    	testDrawRect(&re);
    	// 绘制过后,由于关闭了自动复 位，位置并未归零，可以在原地继续绘制
    	// 设置颜色
    	auxSetColor(0,255,255);
    	// 绘制边框
    	testDrawRect(&re,0);
    	// 绘制完成后我们手动复位
      auxInitLocate();
    }
    return 1;   
    // 还记得 return 1 么
}
  // 我们暂时不需要处理消息
};
int main(){
	// 把我们的场景塞给 minies
	coreSetScene(new MyScene);
	return 1; // 一切 OK
}

//写在最后，自动复位功能是一个非常有意思的东西，感谢与我讨论mines的小易子