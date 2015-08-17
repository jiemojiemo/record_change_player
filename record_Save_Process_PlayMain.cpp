#include "record_Save_Process_PlayHeader.h"
#include "record.h"
#include "processer.h"
#include "recordPlayer.h"

#define SAVE_RECORD 0

int main()
{
	RecordeInfo rcinfo;
	rcinfo.channels			= 1;
	rcinfo.framesPerBuffer	= 512;
	rcinfo.sampleType		= SAMPLE_TYPE_FLOAT32;
	rcinfo.sampleRate		= 44100;
	rcinfo.seconds			= 3;

	CRecorder recorder( rcinfo );
	recorder.StartRecord();

#if SAVE_RECORD
	recorder.SavePcm2File( "pcm_2_44100_float_5" );
#endif


	unsigned int bufferSize;
	CProcesser pro;
	//变速处理
	float* out = pro.TimeScaling( 0.3, (float*)recorder.GetData(), recorder.GetDataSize() );
	//变调处理
	//float* out = pro.PitchShifting( 8, (float*)recorder.GetData(), recorder.GetDataSize() );
	bufferSize = pro.GetAfterScaleSize();

	Pcm2WavInfo info;
	info.channels	= 1;
	info.formatTag	= 3;
	info.inFileName = "";
	info.outFileName= "out_1_16int_44100.wav";
	info.sampleBits = 32;
	info.sampleRate = 44100;
	pro.Pcm2Wav( out, bufferSize, info );



	CRecordPlayer player( bufferSize, out );
	player.Init();
	player.StartPlay();

}