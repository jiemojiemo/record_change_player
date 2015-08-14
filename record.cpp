#include "recorder.h"
#include <string>


unsigned int CRecoder::m_recordBits = 0;

CRecoder::CRecoder( int seconds/*5*/ ):m_seconds(seconds)
{
	m_recordData.frameIndex = 0;
	m_recordData.maxFrameIndex = seconds * SAMPLE_RATE;
	m_recordData.totalBytes = m_recordData.maxFrameIndex*CHANNEL_COUNT * sizeof(SAMPLE);
	m_recordData.recordedSamples = (SAMPLE*)malloc(m_recordData.totalBytes );
	m_stream = NULL;
}

//CRecoder::CRecoder( const RecordeInfo& info, FLOAT32 )
//{
//	_InitInfo( info );
//	m_recordBits = FLOAT32_BITS;
//	m_recordData.totalBytes *= sizeof( float );
//	m_recordData.recordedSamples = (float*)malloc( m_recordData.totalBytes );
//}
//CRecoder::CRecoder( const RecordeInfo& info, INT16 )
//{
//	_InitInfo( info );
//	m_recordBits = INT16_BITS;
//	m_recordData.totalBytes *= sizeof( short );
//	m_recordData.recordedSamples = (short*)malloc( m_recordData.totalBytes );
//}
//CRecoder::CRecoder( const RecordeInfo& info, INT8 )
//{
//	_InitInfo( info );
//	m_recordBits = INT8_BITS;
//	m_recordData.totalBytes *= sizeof( char );
//	m_recordData.recordedSamples = (char*)malloc( m_recordData.totalBytes );
//}
//CRecoder::CRecoder( const RecordeInfo& info, UINT8 )
//{
//	_InitInfo( info );
//	m_recordBits = UINT8_BITS;
//	m_recordData.totalBytes *= sizeof( unsigned char );
//	m_recordData.recordedSamples = (unsigned char*)malloc( m_recordData.totalBytes );
//}


CRecoder::~CRecoder()
{
	if( m_recordData.recordedSamples )
	{
		free( m_recordData.recordedSamples );
		m_recordData.recordedSamples = NULL;
	}
}

void CRecoder::_InitInfo( const RecordeInfo& info )
{
	m_recordData.frameIndex = 0;
	m_recordData.maxFrameIndex = info.seconds * info.sampleRate;
	m_recordData.totalBytes = m_recordData.maxFrameIndex * info.channels;//* sizeof(bits)
	m_stream = NULL;
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

//void CRecoder::PrintMaxAndAver( unsigned int bits )
//{
//	MT_FLOAT32	m32;
//	MT_INT16	m16;
//	MT_INT8		m8;
//	MT_UINT8	mu8;
//	if( bits == FLOAT32_BITS )
//		_PrintMaxAndAver(m32);
//	else if( bits == INT16_BITS )
//		_PrintMaxAndAver(m16);
//	else if( bits == INT8_BITS )
//		_PrintMaxAndAver(m8);
//	else if( bits == UINT8_BITS )
//		_PrintMaxAndAver( mu8 );
//}
//
//void CRecoder::_PrintMaxAndAver( MT_FLOAT32 )
//{
//	//* Measure maximum peak amplitude. */
//	float max = 0;
//	double average = 0.0;
//	unsigned int channels = m_recordData.totalBytes / m_recordData.maxFrameIndex / sizeof(float);
//	double val;
//	for( int i=0; i<m_recordData.maxFrameIndex * CHANNEL_COUNT; i++ )
//	{
//		val = (float*)m_recordData.recordedSamples[i];
//		if( val < 0 ) val = -val; /* ABS */
//		if( val > max )
//		{
//			max = val;
//		}
//		average += val;
//	}
//
//	average = average / (double)m_recordData.maxFrameIndex * channels;
//
//	printf("sample max amplitude = %f\n", max );
//	printf("sample average = %lf\n", average );
//}
//void CRecoder::_PrintMaxAndAver( MT_INT16 )
//{
//	//* Measure maximum peak amplitude. */
//	int max = 0;
//	double average = 0;
//	unsigned int channels = m_recordData.totalBytes / m_recordData.maxFrameIndex / sizeof(short);
//	double val;
//	for( int i=0; i<m_recordData.maxFrameIndex * CHANNEL_COUNT; i++ )
//	{
//		val = (short*)m_recordData.recordedSamples[i];
//		if( val < 0 ) val = -val; /* ABS */
//		if( val > max )
//		{
//			max = val;
//		}
//		average += val;
//	}
//
//	average = average / (double)m_recordData.maxFrameIndex * channels;
//
//	printf("sample max amplitude = %d\n", max );
//	printf("sample average = %lf\n", average );
//}
//void CRecoder::_PrintMaxAndAver( MT_INT8 )
//{
//	//* Measure maximum peak amplitude. */
//	char max = 0;
//	double average = 0;
//	unsigned int channels = m_recordData.totalBytes / m_recordData.maxFrameIndex / sizeof(char);
//	double val;
//	for( int i=0; i<m_recordData.maxFrameIndex * CHANNEL_COUNT; i++ )
//	{
//		val = (char*)m_recordData.recordedSamples[i];
//		if( val < 0 ) val = -val; /* ABS */
//		if( val > max )
//		{
//			max = val;
//		}
//		average += val;
//	}
//
//	average = average / (double)m_recordData.maxFrameIndex * channels;
//
//	printf("sample max amplitude = %d\n", max );
//	printf("sample average = %lf\n", average );
//}
//void CRecoder::_PrintMaxAndAver( MT_UINT8 )
//{
//	//* Measure maximum peak amplitude. */
//	unsigned char max = 0;
//	double average = 0;
//	unsigned int channels = m_recordData.totalBytes / m_recordData.maxFrameIndex / sizeof(unsigned char);
//	double val;
//	for( int i=0; i<m_recordData.maxFrameIndex * CHANNEL_COUNT; i++ )
//	{
//		val = (unsigned char*)m_recordData.recordedSamples[i];
//		if( val < 0 ) val = -val; /* ABS */
//		if( val > max )
//		{
//			max = val;
//		}
//		average += val;
//	}
//
//	average = average / (double)m_recordData.maxFrameIndex * channels;
//
//	printf("sample max amplitude = %d\n", max );
//	printf("sample average = %lf\n", average );
//}

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