#include "recordPlayer.h"
#include "record_Save_Process_PlayHeader.h"
#include "SDL.h"
#include "func.h"
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <string>
#include <stdio.h>
#include <assert.h>


CRecordPlayer::CRecordPlayer( unsigned long byteSize,const SAMPLE* data )
{
	m_data.maxFrameIndex = byteSize / sizeof(SAMPLE) / CHANNEL_COUNT;
	m_data.frameIndex = 0;
	m_data.totalBytes = byteSize;
	m_data.recordedSamples = ( SAMPLE* )malloc( byteSize );

	//assert( m_data.recordedSamples != NULL );
	memcpy( m_data.recordedSamples, data, byteSize );
}

CRecordPlayer::CRecordPlayer( const CRecorder& recorder )
{
	m_data.maxFrameIndex = recorder.m_Data.maxFrameIndex;
	m_data.totalBytes = recorder.m_Data.totalBytes;
	m_data.frameIndex = 0;
	m_data.recordedSamples = (SAMPLE *)malloc( m_data.totalBytes );

	//assert( m_data.recordedSamples != NULL );
	memcpy( m_data.recordedSamples, recorder.m_Data.recordedSamples,
		m_data.totalBytes);
}
CRecordPlayer::CRecordPlayer( const char* url )
{
	FILE* file = fopen( url, "rb" );
	//assert( file );
	fseek( file, 0, SEEK_END );
	int fileSize = ftell( file );
	fseek( file, 0, SEEK_SET );
	m_data.maxFrameIndex = fileSize / sizeof(SAMPLE) / CHANNEL_COUNT;
	m_data.frameIndex = 0;
	m_data.totalBytes = fileSize;
	m_data.recordedSamples = (SAMPLE*)malloc( fileSize );
//	assert( m_data.recordedSamples != NULL );
	fread( m_data.recordedSamples, 1, fileSize, file );
	fclose( file );
}
CRecordPlayer::~CRecordPlayer()
{
	if( m_data.recordedSamples )
	{
		free( m_data.recordedSamples );
		m_data.recordedSamples = NULL;
	}
}

void CRecordPlayer::SetData( unsigned long byteSize, const SAMPLE* data )
{
	if( m_data.recordedSamples != NULL )
	{
		free( m_data.recordedSamples );
		m_data.recordedSamples = NULL;
	}


	m_data.maxFrameIndex = byteSize / sizeof(SAMPLE) / CHANNEL_COUNT;
	m_data.frameIndex = 0;
	m_data.totalBytes = byteSize;
	m_data.recordedSamples = ( SAMPLE* )malloc( byteSize );

	//assert( m_data.recordedSamples != NULL );
	memcpy( m_data.recordedSamples, data, byteSize );
}
/*******************************
**����������StartPlay��
**��������;������ʼ����Ƶ�豸��opengl
**������ֵ��return�� �����ʾ��0��ʾ����
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
int CRecordPlayer::Init()
{
	if( AudioInit() != 0)
	{
		fprintf( stderr,"Error:Cannot Initialize Audio\n" );
		return -1;
	}

	//if( DisplayInit() != 0 )
	//{
	//	fprintf( stderr,"Error:Cannot Initialize Audio\n" );
	//	return -1;
	//}
	//SDL_Thread* displaythread = SDL_CreateThread( Display, "dispaly", NULL );
	return 0;
}

/*******************************
**����������AudioInit��
**��������;������ʼ����Ƶ�豸
**������ֵ��return�� �����ʾ��0��ʾ����
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
int CRecordPlayer::AudioInit()
{
	PaError					err;				
	PaStreamParameters		outputParameters;
	//portaudio������ʼ��
	err = Pa_Initialize();
	if( err != paNoError )
		return err;

	outputParameters.device = Pa_GetDefaultOutputDevice();
	if( outputParameters.device == paNoDevice )
	{
		fprintf( stderr, "Error: No default output device.\n" );
		return -1;
	}

	outputParameters.channelCount = CHANNEL_COUNT;
	outputParameters.sampleFormat = PA_SAMPLE_TYPE;
	outputParameters.suggestedLatency = 
		Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	 printf("\n=== Now playing back. ===\n"); fflush(stdout);

	 err = Pa_OpenStream( &m_stream,
			NULL, //no input
			&outputParameters,
			SAMPLE_RATE,
			FRAME_PER_BUFFER,
			paClipOff,
			playCallback,
			&m_data
		 );
	 if( err != paNoError ) 
		 return err;

	 return 0;
}

/*******************************
**����������StartPlay
**��������;������ʼ������Ƶ����ʾ
**������ֵ��return�� �����ʾ��0��ʾ����
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
int CRecordPlayer::StartPlay()
{
	SDL_Thread* audiothread = SDL_CreateThread( audioPlay,"audio", m_stream );
	Display(NULL);
	int lots;
	SDL_WaitThread( audiothread,&lots );
	return 0;
}


/*******************************
**����������ClosePlay��
**��������;�����رղ���
**������ֵ��return�� �����ʾ��0��ʾ����
** (����)Creator��
** (����)Date��
**���޸��ˣ�Modifier��
**���޸����ڣ�ModifyDate��
**���汾��Version��
*******************************/
int CRecordPlayer::ClosePlay()
{
	return Pa_CloseStream(m_stream);
}

