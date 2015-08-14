#ifndef RECORD_SAVE_PROCESS_PLAYHEADER_H
#define RECORD_SAVE_PROCESS_PLAYHEADER_H
#include <assert.h>
#include <string>

#define SAMPLE_RATE (44100)
#define CHANNEL_COUNT (1)
#define FRAME_PER_BUFFER 1024

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 0
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

#define ISERR(err)\
	if( (err) != paNoError ) goto error;

typedef struct
{
	int          frameIndex;  /* Index into sample array. */
	int          maxFrameIndex;
	unsigned int totalBytes;
	SAMPLE       *recordedSamples;
}
paTestData;


typedef struct Waves{
	SAMPLE waveData[FRAME_PER_BUFFER*CHANNEL_COUNT];
	Waves( const SAMPLE* data )
	{
		memcpy( waveData, data, FRAME_PER_BUFFER*CHANNEL_COUNT*sizeof(SAMPLE) );
	}
	Waves& operator=( const Waves& wave )
	{
		if( this == &wave )
			return *this;
		memcpy( waveData, wave.waveData, FRAME_PER_BUFFER*CHANNEL_COUNT*sizeof(SAMPLE) );
		return *this;
	}
	Waves& operator=( const SAMPLE* data )
	{
		assert( data != NULL );
		memcpy( waveData, data, FRAME_PER_BUFFER*CHANNEL_COUNT*sizeof(SAMPLE) );
	}
	SAMPLE& operator[]( int pos )
	{
		return waveData[pos];
	}
	const SAMPLE& operator[]( int pos)const
	{
		return waveData[pos];
	}
}SWaves;

#endif