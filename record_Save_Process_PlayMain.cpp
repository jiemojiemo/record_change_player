#include "record_Save_Process_PlayHeader.h"
#include "record.h"
#include "processer.h"
#include "recordPlayer.h"
#include <iostream>
using std::cout;
using std::cin;

#define SAVE_RECORD 0
#define SAVE_PROCESS_RECORD_TO_WAV 0

int main()
{
	RecordeInfo rcinfo;
	rcinfo.channels			= 1;
	rcinfo.framesPerBuffer	= 512;
	rcinfo.sampleType		= SAMPLE_TYPE_FLOAT32;
	rcinfo.sampleRate		= 44100;
	rcinfo.seconds			= 10;

	CRecorder recorder( rcinfo );
	recorder.StartRecord();

#if SAVE_RECORD
	recorder.SavePcm2File( "1_32float_44100.pcm" );
#endif
	unsigned int bufferSize = recorder.GetDataSize();
	CProcesser pro;


	double scale = 0.0;
	printf( "变速速率( 0.1 -- 5.0 ):" );fflush(stdout);
	cin >> scale;
	int pitch = 0;
	printf( "音高变化( -10 -- 10 ): " ); fflush(stdout);
	cin >> pitch;
	float *out = pro.TimeScalingAndPitchShifting( pitch, scale,(float*)recorder.GetData(), recorder.GetDataSize() );
	bufferSize = pro.GetAfterPitchSize();
	CRecordPlayer player( bufferSize, out );
	player.Init();
	player.StartPlay();


}