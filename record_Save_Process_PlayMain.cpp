#include "record_Save_Process_PlayHeader.h"
#include "recorder.h"
#include "processer.h"

#define SECONDS 3

int main()
{
	const char *url = "recorded.raw";
	CRecoder recoder( SECONDS );
	recoder.Init();
	recoder.StartRecode();




	unsigned int bufferSize;
	CProcesser pro;
	Pcm2WavInfo info;
	info.channels	= 1;
	info.formatTag	= 3;
	info.sampleRate = 44100;
	info.sampleBits = 32;
	info.inFileName = "";
	info.outFileName= "out_1_16int_44100.wav";
	//pro.Pcm2Wav( (float*)recoder.GetRecordData(), bufferSize, info );
	

	//Processer
	//......

	float* out = pro.TimeScaling( 0.3, (float*)recoder.GetRecordData(), recoder.GetRecordDataSize() );
	bufferSize = pro.GetAfterScaleSize();
	pro.Pcm2Wav( out, bufferSize, info );

	CRecordPlayer player( bufferSize, out );
	player.Init();
	player.StartPlay();

	//bufferSize = pro.GetAfterScaleSize();
	//Pcm2WavInfo info;
	//info.channels	= 1;
	//info.formatTag	= 3;
	//info.sampleRate = 44100;
	//info.sampleBits = 32;
	//info.inFileName = "";
	//info.outFileName= "out_1_16int_44100.wav";
	//pro.Pcm2Wav( (const float*)out, bufferSize, info );

	//unsigned long afterScaleSize = pro.GetAfterScaleSize();


	//CRecordPlayer player( recoder );
	//player.Init();
	//player.StartPlay();
}