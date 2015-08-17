#include "func.h"

#include "portaudio.h"
#include "recordplayer.h"
#include <stdio.h>
#include <assert.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <math.h>
#include <list>
using std::list;


GLuint g_width = 800;			//窗口的宽度
GLuint g_height = 600;			//窗口的高度
GLfloat g_eye_y = 0;			//照相机视角的角度
bool g_dataReady =false;		//音频数据是否准备好给opengl绘制
SAMPLE* g_audioBuffer = NULL;	//存放音频数据的buffer
SAMPLE* g_copyaudioBuffer = NULL;	//音频数据的副本
int g_bytes = FRAME_PER_BUFFER * CHANNEL_COUNT * sizeof(SAMPLE);	//要申请空间的大小
unsigned int g_depth = 32;		//一共有多少音波
GLfloat g_space = .12f;			//音波之间的间隔
GLfloat g_mul	= 3.0f;			//为了凸显音波，一个乘系数


list<SWaves> g_waveList;			//存放一堆音波数据

/*******************************
**（函数名）audioPlay：
**（功能用途）：开始播放音频
**（返回值）return： 错误标示，0标示正常
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
int audioPlay( void* data )
{
	PaError				err = paNoError;
	PaStream*			stream = (PaStream*)data;
	

	//申请空间
	g_audioBuffer = (SAMPLE*)malloc( g_bytes );
	assert( g_audioBuffer != NULL );
	memset( g_audioBuffer, 0, g_bytes );

	if( stream )
	{
		//开始播放
		err = Pa_StartStream( stream );
		if( err != paNoError )
			return err;

		printf("Waiting for playback to finish.\n"); fflush(stdout);

		//等待播放结束
		while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
		if( err < 0 )
			return err;

		printf("Done.\n"); fflush(stdout);
	}
	if( g_audioBuffer != NULL )
	{
		free( g_audioBuffer );
		g_audioBuffer = NULL;
	}
	return err;
}


/*******************************
**（函数名）DisplayInit：
**（功能用途）：开始显示
**（返回值）return： 错误标示，0标示正常
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
int Display( void* data )
{
	int argc = 1;
	char *argv[] = {"Display"};

	//init
	glutInit( &argc, argv );
	//设置显示模式
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	//设置窗口大小
	glutInitWindowSize( g_width, g_height );
	//窗口起始位置
	glutInitWindowPosition(200, 200 );
	//创建窗口
	glutCreateWindow( "Display" );
	//设置全局的回调函数（每时每刻都在回调）
	glutIdleFunc( IdelFunction );
	//设置回调函数
	glutDisplayFunc( DisplayFunction );
	//设置窗口变化函数
	glutReshapeFunc( ReshapeFunction );
	//进行一些初始化
	InitializeGraphics();

	//进入opengl时间循环
	glutMainLoop();

	return 0;
}

/*******************************
**（函数名）IdelFunction：
**（功能用途）：全局回调
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void IdelFunction()
{
	glutPostRedisplay();
}

/*******************************
**（函数名）DisplayFunction：
**（功能用途）：回调函数
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：HW
**（修改日期）ModifyDate：2015-08-17
**（版本）Version：1.1
*******************************/
void DisplayFunction()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	while( !g_dataReady )
		;
	static int count = 0;
	printf( "%d\n",count++ ); 
	//进行绘制
	DrawWaves();
	//将音频数据存放在list中，方便绘制
	if( count % 10 == 9 )
	{
		if( g_waveList.size() <= g_depth )
		{
			if( g_waveList.size() == g_depth )
				g_waveList.pop_back();
			g_waveList.push_front( g_audioBuffer );
		}
	}

	
	//if( g_dataReady )
	//{
	//	static int count = 0;
	//	printf( "%d\n",count++ ); 
	//	//进行绘制
	//	DrawWaves();
	//	//将音频数据存放在list中，方便绘制
	//	if( g_waveList.size() <= g_depth )
	//	{
	//		if( g_waveList.size() == g_depth )
	//			g_waveList.pop_back();
	//		g_waveList.push_front( g_audioBuffer );
	//	}
	//}
	glutSwapBuffers();
}
/*******************************
**（函数名）DrawWaves：
**（功能用途）：绘制音波
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void DrawWaves()
{
	glPushMatrix();
		DrawWaveOnTop();
		DrawWavesOnButton();
	glPopMatrix();
}

/*******************************
**（函数名）DrawWaveOnTop：
**（功能用途）：绘制上面那条音波
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void DrawWaveOnTop()
{
	GLfloat x = -3.0f, inc = 6.0f / FRAME_PER_BUFFER, y = 0.70f;
	GLint ii = FRAME_PER_BUFFER / 2;
	glColor3f( 0.0f, 0.8f, 0.0f );
	glBegin( GL_LINE_STRIP );
		for( int i =0; i < FRAME_PER_BUFFER; ++i )
		{
			glVertex3f( x, g_mul*g_audioBuffer[i] + y, 0.0f );
			x += inc;
		}
	glEnd();


	//glVertex3f( -0.5f, -0.5f, 0.0f );
	//glVertex3f( 0.5f, -0.5f, 0.0f );
	//glVertex3f( 0.0f, 0.8f, 0.0f );
}


/*******************************
**（函数名）DrawWavesOnButton：
**（功能用途）：绘制下面的一堆音波
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void DrawWavesOnButton()
{
	GLfloat x = -2.50f, inc = 5.f / FRAME_PER_BUFFER;
	GLfloat z = 0.0f;
	GLfloat y = -1.00f;
	GLint ii = FRAME_PER_BUFFER / 2;
	int count = 0;
	glColor3f( 0.60f, 0.6f, 0.00f );
	for( auto it = g_waveList.begin(); it != g_waveList.end(); ++it )
	{
		x = -2.20f;
		glBegin( GL_LINE_STRIP );

			for( int i = 0; i < FRAME_PER_BUFFER; ++i )
			{
				glVertex3f( x, g_mul*(*it)[i] + y, count * g_space + z );
				x += inc;
			}
		count--;
		glEnd();
	}
}
/*******************************
**（函数名）SetPoints：
**（功能用途）：设置需要画的点
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void SetPoints()
{

}

/*******************************
**（函数名）ReshapeFunction：
**（功能用途）：窗口变化回调函数
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void ReshapeFunction( int w, int h )
{
	if( 0 == h )
		h = 1;

	glViewport( 0,0,w,h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 100.0 );
	//glOrtho( -1.0, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f );

	gluLookAt( 0.0f, .80f, 2.5f , 
		0.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

}

/*******************************
**（函数名）InitializeGraphics：
**（功能用途）：初始化函数
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void InitializeGraphics()
{
	//设置背景颜色
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	//开启深度检测
	glEnable( GL_DEPTH_TEST );

	glutPostRedisplay();
}

/*******************************
**（函数名）KeyboradFunction：
**（功能用途）：键盘回调函数
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
void KeyboradFunction( unsigned char key, int x, int y )
{
	switch( key )
	{
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
}

/*******************************
**（函数名）playCallback：
**（参数）@Param inputBuffer: 表示音频的输入流
**（参数）@Param outputBuffer：表示音频的输出流
**（参数）@Param framesPerBuffer：表示输入音频帧的个数
**（参数）@Param timeInfo： 表示时间信息
**（参数）@Param statusFlags： 表示状态信息
**（参数）@Param userData： 表示自定义的数据
**（功能用途）：音频播放的回调函数
**（返回值）return： 
** (作者)Creator：
** (日期)Date：
**（修改人）Modifier：
**（修改日期）ModifyDate：
**（版本）Version：
*******************************/
int CRecordPlayer::playCallback(const void *inputBuffer, 
	void *outputBuffer, unsigned long framesPerBuffer, 
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	paTestData *data = (paTestData*)userData;
	SAMPLE *rptr = &data->recordedSamples[data->frameIndex * CHANNEL_COUNT];
	SAMPLE *wptr = (SAMPLE*)outputBuffer;
	unsigned int i;
	int finished;
	unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

	(void) inputBuffer; /* Prevent unused variable warnings. */
	(void) timeInfo;
	(void) statusFlags;
	(void) userData;
	memset( g_audioBuffer, 0, FRAME_PER_BUFFER * CHANNEL_COUNT * sizeof(SAMPLE));
	if( framesLeft < framesPerBuffer )
	{
		g_dataReady = false;
		memcpy( g_audioBuffer, rptr, framesLeft * CHANNEL_COUNT * sizeof(SAMPLE) );
		g_dataReady = true;
		/* final buffer... */
		for( i=0; i<framesLeft*CHANNEL_COUNT; i++ )
		{
			*wptr++ = *rptr++;  /* left */
			
		}
		for( ; i<framesPerBuffer*CHANNEL_COUNT; i++ )
		{
			*wptr++ = 0;  /* left */
			
		}
		data->frameIndex += framesLeft;
		finished = paComplete;
	}
	else
	{
		g_dataReady = false;
		memcpy( g_audioBuffer, rptr, framesPerBuffer * CHANNEL_COUNT * sizeof(SAMPLE) );
		g_dataReady = true;
		for( i=0; i<framesPerBuffer*CHANNEL_COUNT; i++ )
		{

			*wptr++ = *rptr++;  /* left */

		}
		data->frameIndex += framesPerBuffer;
		finished = paContinue;
	}

	return finished;
}
