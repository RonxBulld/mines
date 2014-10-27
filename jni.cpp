
//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>
#include <math.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android_native_app_glue.h>


#include <stdlib.h>

extern int main();
class UserBase{
	public: 
	virtual int  OnFrame()=0;
	virtual void OnEvent (int,float,float,float)=0;
};
struct engine {
	// 
    struct android_app* app;            
    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;
    // EGL
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
  
    int32_t width;
    int32_t height;
	// GLES
	float fovy , zNear , zFar ;
    // user 
	int animating;
	int mode;
	// 
	UserBase* user;
};



//GLUES
static void __gluMakeIdentityf(GLfloat m[16])
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

#define __glPi 3.14159265358979323846

void gluPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    GLfloat m[4][4];
    GLfloat sine, cotangent, deltaZ;
    GLfloat radians=(GLfloat)(fovy/2.0f*__glPi/180.0f);


    deltaZ=zFar-zNear;
    sine=(GLfloat)sin(radians);
    if ((deltaZ==0.0f) || (sine==0.0f) || (aspect==0.0f))
    {
        return;
    }
    cotangent=(GLfloat)(cos(radians)/sine);

    __gluMakeIdentityf(&m[0][0]);
    m[0][0] = cotangent / aspect;
    m[1][1] = cotangent;
    m[2][2] = -(zFar + zNear) / deltaZ;
    m[2][3] = -1.0f;
    m[3][2] = -2.0f * zNear * zFar / deltaZ;
    m[3][3] = 0;
    glMultMatrixf(&m[0][0]);
}
#undef __glPi 
 

static int engine_init_window(struct engine* engine) {
    // initialize OpenGL ES and EGL
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE, 8,
            EGL_NONE };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
 
               ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) return -1;
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->fovy = 45.0f;
    engine->zNear = 1.0f;
    engine->zFar = 100.0f;
    engine->animating = 1;
  	return 1;
}

static void engine_window_resize(struct engine* engine)
{
	glMatrixMode(GL_PROJECTION);             // project matrix
	glLoadIdentity();

	glViewport(0, 0, engine->width, engine->height);                  // view port
	if(engine->mode) gluPerspectivef(engine->fovy , 1.0f * engine->width / engine->height, engine->zNear , engine->zFar);
  else glOrthof(0,engine->width,0, engine->height,-engine->zFar, engine->zFar);
	glMatrixMode(GL_MODELVIEW);              // modelview matrix
	glLoadIdentity();
}

static void engine_init_egl(struct engine* engine) {
    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glShadeModel(GL_FLAT);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_egl(struct engine* engine) {
    if(engine->app){	engine->user->OnEvent(3,0,0,0);  if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }

  engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;	
    engine->app = 0;
 // engine->user = 0;
    }
}

 
static void engine_draw_frame(struct engine* engine) {
    if (engine->display != NULL && engine->animating != 0 && engine->user->OnFrame()) 
   eglSwapBuffers(engine->display, engine->surface);
}


/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        
        engine->user->OnEvent(0, AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0), 0);
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
		/*
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
			*/
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
              int rt = engine_init_window(engine);
     if(rt < 0) exit(0);
engine->user->OnEvent(2,engine->width,engine->height,0);	engine_window_resize(engine);
				engine_init_egl(engine);
			//	glVertexPointer(3, GL_FLOAT, 0, box);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
 engine_term_egl(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                        engine->accelerometerSensor, (1000L/60)*1000);
engine->user->OnEvent(4,0,0,0);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
            }
            // Also stop animating.
            engine->animating = 0;
  engine->user->OnEvent(5,0,0,0);          //wengine_draw_frame(engine);
            break;
    }
}
struct engine* __gAppEngine;

void android_main(struct android_app* state) {

    struct engine engine;
    __gAppEngine = &engine;   
   
   
    memset(&engine, 0, sizeof(engine));
    if( main() < 0 ) exit(-1);
    app_dummy();
 
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;
    
    engine.sensorManager = ASensorManager_getInstance();
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
            state->looper, LOOPER_ID_USER, NULL, NULL);

    while (1) {
        int ident, events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {


  if (source != NULL)
            source->process(state, source);
         

         
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                            &event, 1) > 0)     
	engine.user->OnEvent(1,	event.acceleration.x, event.acceleration.y,event.acceleration.z);
                    
                }
            }


            if (state->destroyRequested != 0) {
                engine_term_egl(&engine);
                return;
            }
        }            
            engine_draw_frame(&engine);
    }
}
/* *jjjjjj*  */
void engine_window_run(){}
void SetUser(UserBase* in)
{
	if ( in != NULL) 
	   __gAppEngine->user = in;
}
void SetMode(int mode = 0){
	__gAppEngine->mode = mode;
	if(__gAppEngine->display)
		  engine_window_resize( __gAppEngine);
}
inline int GetWidth(){
	return __gAppEngine->width;
}
inline int GetHeight(){
	return __gAppEngine->height;
}
inline int GetMode(){
	return __gAppEngine->mode;
}
enum { WIN_TOUCH = 0 ,WIN_SENSOR,WIN_INIT,WIN_QUIT, WIN_FOCUS,WIN_LOST };
//END_INCLUDE(all)





#include <stdio.h>
void log(const char* buf){
	static int f = 0;
	FILE* fp = fopen("/sdcard/jni.log",f?"a+":"w+");
	fputs(buf,fp);
	fclose(fp); f=1;
}

class MyApp : public UserBase{
public:
	int OnFrame(){
		static float t = 0;
		t += 0.1f; 
		if(t > 1) t = 0;
		    glClearColor(t, t, 1.0f, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return 1;
	}
	
	void OnEvent(int event,float x, float y, float z){
		const char* msg[] =  { "WIN_TOUCH", "WIN_SENSOR","WIN_INIT","WIN_QUIT", "WIN_FOCUS","WIN_LOST"};
		char buf[1024];
		sprintf(buf,"%s:%.f    %.f   %.f\n\r",msg[event],x,y,z);
		log(buf);
	}
};
int main(){
	SetUser(new MyApp);
	engine_window_run();
	return 0;
}