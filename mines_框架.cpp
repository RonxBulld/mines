/*
   我将会在这个文件详细的说明，怎么
开始一个mines框架，以及怎样正确的使用
它，可能包含有实现上的说明，但不必深究
*/
/* 宏开关 如果是 安卓上使用c4 编译，
请打开 __gles 宏，mines 目前没有写
自己的编译参数，所以借用SDL2的，请确
定你安装了SDL，并能正确编译SDL2
*/
//#define __glut 1
#define __gles  1
/*
   你必须包含 glue 头文件告诉c4 
使用SDL参数来编译你的代码，以正确链接
到 gles 库，通常我在注释里写这一句 
*/
//#include <android_native_app_glue.h>

// mines 头文件，目前有四个
// core 核心主要是搭建 mines 环境
// 目前 安卓上使用 gles ，PC上暂时使用的glut，如果你想在PC上使用mines 请确定安装了 glut 库
#include "mines_core_gles.h"
#include "mines_core_glut.h"
// aux 主要是提供下层绘图功能
#include "mines_aux.h"
// test 自己写的基于aux扩展功能
#include "mines_test.h"
// mines 函数命名规则
// coreXXX   auxXXX   testXXX
// 我们用到的其他头文件
//#include <stdio.h>
// 以上是头文件部分
// end_include<ALL>

/*
mines 的由来，因为只能使用手机的原因
(所以，也不要吐槽我的代码排版问题，可
能你看着很乱，但在我的4.5寸手机屏幕上
看着还不错，ps，我开了自动换行哦),我
打算用SDL去写一些小代码，来实现自己无
聊时生出的无脑想法，通常写了半小时发现
自己搞不定就放弃，然后因为自己代码风格
太烂，导致SDL各种问题，甚至经常挂掉，
想要放弃SDL，在某易子的怂恿下，开始试
试GL ES，最后野心膨胀就有了mines (
mine es的缩写，被某易子吐槽为 扫雷)
在过去几天，以每天修改一次的速度在一周
内修改了七次，包括 添加 ext.h 支持 
Surface(image)和纹理，添加 BMP
 loader 和 Circle，添加 test.h，
在PC上移植到glut(仅仅更换了core)，
重新更正了函数命名，以及修复了一些已知
的bug,,真的非常非常累人，比如，在移植
到PC时，我本来想使用Win32，后来在小
易子来自gcc的反对下，暂时使用了glut,
第一次在我的VC上顺利运行，但是和手机
的图片位置有一些不同，折腾了很久也没
明白哪里错了，直到我注意到使用的纹理尺
寸不同，尽管图片内容一样....(何其作死
在这里表示一下怨气好了，以下是正文)
*/

/* 
   在开始代码之前，你有必要明白mines
的想法,它是对SDL不满意而出现的，基于
手机编写代码困难而实现的，所以mines的实现是回调式的，主体框架由mines接管。
   mines把程序看作由一个个场景组成，
你只需要编写场景，然后告诉mines如何切
换它们，尽管main函数是由mines接管，
我还是提供了一个main以符合某些易子的 
变态审美，你可以把它理解为初始化函数，
OK，让我们看一下场景如何编写
*/

// 一个场景就是一个继承自 SenceBase
// 的类，可以重载的函数的函数有两个，
// OnFrame 帧绘图函数 
// OnEvent 事件处理函数...
// 如果狠一点，还可以把它们变成一个


// 一个简易场景，绘制一个矩形填充屏幕
class MyScene:
   public SceneBase{
public:
  // 绘图函数
	int OnFrame(){
		auxClear(); // 清理上一帧数据
		testDrawRect(); // 绘制矩形
    return 1;   
    // 返回 1   表示立即更新
    // 返回 0   表示放弃更新
    // 当然我们不会放弃更新咯~
}

	void OnEvent(int event,float x, float y, float z){
		// 我们暂时不需要处理消息
	}
};

// 此处的main并非通常意义上的main(
// 因为main已经托管给 mines 了嘛)
// 其实就是个用户初始化函数啦 
// 此时窗口尚未创建哦
int main(){
	// 最重要的是设置用户场景
	coreSetScene(new MyScene);
	// 还可以设置窗口大小
	// 当然只对 glut 有效，gles 无法设置大小
	// coreSetWidth (400);
	// coreSetHeight(400);
	// 在这里写自己的初始化代码
	// 返回 1 表示一切OK
	// 返回 0 初始化失败 退出
	return 1;  
}