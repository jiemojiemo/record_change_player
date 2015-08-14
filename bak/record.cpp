#include "recorder.h"
#include <string>


CRecoder::CRecoder( int seconds /* = 5 */ ):m_seconds(seconds)
{
	m_recordData.frameIndex = 0;
	m_recordData.maxFrameIndex = seconds * SAMPLE_RATE;
	m_recordData.totalBytes = m_recordData.maxFrameIndex*CHANNEL_COUNT * sizeof(SAMPLE);
	m_recordData.recordedSamples = (SAMPLE*)malloc(m_recordData.totalBytes );
	m_stream = NULL;
}
CRecoder::CRecoder( const RecordeInfo& info, FLOAT32 )
{

}
CRecoder::CRecoder( const RecordeInfo& info, INT16 )
{

}
CRecoder::CRecoder( const RecordeInfo& info, INT8 )
{

}
CRecoder::CRecoder( const RecordeInfo& info, UINT8 )
{

}


CRecoder::~CRecoder()
{
	if( m_recordData.recordedSamples )
	{
		free( m_recordData.recordedSamples );
		m_recordData.recordedSamples = NULL;
	}
}

int CRecoder::_StartRecode()
{
	PaError					err = paNoError;

	//重置音频帧下标
	m_recordData.frameIndex = 0;

	err = Pa_StartStream( m_stream );
	if( err != paNoError )
		return err;
	printf("\n=== Now recording!! Please speak into the microphone. ===\n"); fflush(stdout);

	while( ( err = Pa_IsStreamActive( m_stream ) ) == 1 )
	{
		Pa_Sleep(1000);
		printf("index = %d\n", m_recordData.frameIndex ); fflush(stdout);
	}
	if( err < 0 )	return err;

	err = Pa_StopStream( m_stream );
	if( err < 0 )	return err;

	//* Measure maximum peak amplitude. */
	double max = 0;
	double average = 0.0;
	double val;
	for( int i=0; i<m_recordData.maxFrameIndex * CHANNEL_COUNT; i++ )
	{
		val = m_recordData.recordedSamples[i];
		if( val < 0 ) val = -val; /* ABS */
		if( val > max )
		{
			max = val;
		}
		average += val;
	}

	average = average / (double)m_recordData.maxFrameIndex * CHANNEL_COUNT;

	printf("sample max amplitude = "PRINTF_S_FORMAT"\n", max );
	printf("sample average = %lf\n", average );

	return 0;
}

int CRecoder::StartRecode()
{
	return _StartRecode();
}

int CRecoder::StartRecode( const RecordeInfo& info )
{
	return _StartRecode();
}

int CRecoder::CloseRocode()
{
	PaError		err = paNoError;
	err = Pa_CloseStream( m_stream );
	if( err != paNoError )
		return err;
	return 0;
}
int CRecoder::Init()
{
	
	PaStreamParameters		inputParameters;
	PaError					err = paNoError;

	printf( "Record Init\n" );fflush(stdout);

	err = Pa_Initialize();					//设备初始化
	if( err != paNoError ) return err;

	inputParameters.device = Pa_GetDefaultInputDevice();	//获取默认的输入设备
	if( inputParameters.device == paNoDevice )
	{
		fprintf( stderr, "Error:No default input device.\n" );
		return -1;
	}

	inputParameters.channelCount = CHANNEL_COUNT;
	inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	inputParameters.suggestedLatency =
		Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	//打开音频流，进行准备
	err = Pa_OpenStream( &m_stream,
					&inputParameters,
					NULL,
					SAMPLE_RATE,
					FRAME_PER_BUFFER,
					paClipOff,
					recordCallback,
					&m_recordData
		);

	return 0;
}
int CRecoder::recordCallback(const void *inputBuffer,
	void *outputBuffer, unsigned long framesPerBuffer, 
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, 
	void *userData)
{
	paTestData *data = (paTestData*)userData;
	const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
	SAMPLE *wptr = &data->recordedSamples[data->frameIndex * CHANNEL_COUNT];
	long framesToCalc;
	long i;
	int finished;
	unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

	(void) outputBuffer; /* Prevent unused variable warnings. */
	(void) timeInfo;
	(void) statusFlags;
	(void) userData;
	if( framesLeft < framesPerBuffer )
	{
		framesToCalc = framesLeft;
		finished = paComplete;
	}
	else
	{
		framesToCalc = framesPerBuffer;
		finished = paContinue;
	}

	if( inputBuffer == NULL )
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = SAMPLE_SILENCE; /* left */
			if( CHANNEL_COUNT == 2 )
				*wptr++ = SAMPLE_SILENCE;  /* right */

		}
	}
	else
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = *rptr++;  /* left */
			if( CHANNEL_COUNT == 2 ) 
				*wptr++ = *rptr++;  /* right */
		}
	}
	data->frameIndex += framesToCalc;
	return finished;
}