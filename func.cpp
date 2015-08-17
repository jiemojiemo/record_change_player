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


GLuint g_width = 800;			//���ڵĿ��
GLuint g_height = 600;			//���ڵĸ߶�
GLfloat g_eye_y = 0;			//������ӽǵĽǶ�
bool g_dataReady =false;		//��Ƶ�����Ƿ�׼���ø�opengl����
SAMPLE* g_audioBuffer = NULL;	//�����Ƶ���ݵ�buffer
SAMPLE* g_copyaudioBuffer = NULL;	//��Ƶ���ݵĸ���
int g_bytes = FRAME_PER_BUFFER * CHANNEL_COUNT * sizeof(SAMPLE);	//Ҫ����ռ�Ĵ�С
unsigned int g_depth = 32;		//һ���ж�������
GLfloat g_space = .12f;			//����֮��ļ��
GLfloat g_mul	= 3.0f;			//Ϊ��͹��������һ����ϵ��


list<SWaves> g_waveList;			//���һ����������

/*******************************
**����������audioPlay��
**��������;������ʼ������Ƶ
**������ֵ��return�� �����ʾ��0��ʾ����
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
int audioPlay( void* data )
{
	PaError				err = paNoError;
	PaStream*			stream = (PaStream*)data;
	

	//����ռ�
	g_audioBuffer = (SAMPLE*)malloc( g_bytes );
	assert( g_audioBuffer != NULL );
	memset( g_audioBuffer, 0, g_bytes );

	if( stream )
	{
		//��ʼ����
		err = Pa_StartStream( stream );
		if( err != paNoError )
			return err;

		printf("Waiting for playback to finish.\n"); fflush(stdout);

		//�ȴ����Ž���
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
**����������DisplayInit��
**��������;������ʼ��ʾ
**������ֵ��return�� �����ʾ��0��ʾ����
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
int Display( void* data )
{
	int argc = 1;
	char *argv[] = {"Display"};

	//init
	glutInit( &argc, argv );
	//������ʾģʽ
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	//���ô��ڴ�С
	glutInitWindowSize( g_width, g_height );
	//������ʼλ��
	glutInitWindowPosition(200, 200 );
	//��������
	glutCreateWindow( "Display" );
	//����ȫ�ֵĻص�������ÿʱÿ�̶��ڻص���
	glutIdleFunc( IdelFunction );
	//���ûص�����
	glutDisplayFunc( DisplayFunction );
	//���ô��ڱ仯����
	glutReshapeFunc( ReshapeFunction );
	//����һЩ��ʼ��
	InitializeGraphics();

	//����openglʱ��ѭ��
	glutMainLoop();

	return 0;
}

/*******************************
**����������IdelFunction��
**��������;����ȫ�ֻص�
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
void IdelFunction()
{
	glutPostRedisplay();
}

/*******************************
**����������DisplayFunction��
**��������;�����ص�����
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��HW
**���޸����ڣ�ModifyDate��2015-08-17
**���汾��Version��1.1
*******************************/
void DisplayFunction()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	while( !g_dataReady )
		;
	static int count = 0;
	printf( "%d\n",count++ ); 
	//���л���
	DrawWaves();
	//����Ƶ���ݴ����list�У��������
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
	//	//���л���
	//	DrawWaves();
	//	//����Ƶ���ݴ����list�У��������
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
**����������DrawWaves��
**��������;������������
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
void DrawWaves()
{
	glPushMatrix();
		DrawWaveOnTop();
		DrawWavesOnButton();
	glPopMatrix();
}

/*******************************
**����������DrawWaveOnTop��
**��������;��������������������
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
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
**����������DrawWavesOnButton��
**��������;�������������һ������
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
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
**����������SetPoints��
**��������;����������Ҫ���ĵ�
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
void SetPoints()
{

}

/*******************************
**����������ReshapeFunction��
**��������;�������ڱ仯�ص�����
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
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
**����������InitializeGraphics��
**��������;������ʼ������
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
void InitializeGraphics()
{
	//���ñ�����ɫ
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	//������ȼ��
	glEnable( GL_DEPTH_TEST );

	glutPostRedisplay();
}

/*******************************
**����������KeyboradFunction��
**��������;�������̻ص�����
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
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
**����������playCallback��
**��������@Param inputBuffer: ��ʾ��Ƶ��������
**��������@Param outputBuffer����ʾ��Ƶ�������
**��������@Param framesPerBuffer����ʾ������Ƶ֡�ĸ���
**��������@Param timeInfo�� ��ʾʱ����Ϣ
**��������@Param statusFlags�� ��ʾ״̬��Ϣ
**��������@Param userData�� ��ʾ�Զ��������
**��������;������Ƶ���ŵĻص�����
**������ֵ��return�� 
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
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
