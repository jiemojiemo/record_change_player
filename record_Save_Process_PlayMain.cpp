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
	rcinfo.seconds			= 5;

	CRecorder recorder( rcinfo );
	recorder.StartRecord();

#if SAVE_RECORD
	recorder.SavePcm2File( "1_32float_44100.pcm" );
#endif
	unsigned int bufferSize = recorder.GetDataSize();
	CProcesser pro;
	int c;
	cout << "1) 变速" << endl;
	cout << "2) 变调" << endl;
	cout << "3) 变速加变调" << endl;
	cin >>c;
	if( 1 == c )
	{
		double scale = 0.0;
		cout << "变速速率( 0.1 -- 5.0 )：";
		cin >> scale;
		//变速处理
		float* out = pro.TimeScaling( scale, (float*)recorder.GetData(), recorder.GetDataSize() );
		bufferSize = pro.GetAfterScaleSize();
		CRecordPlayer player( bufferSize, out );
		player.Init();
		player.StartPlay();
	}
	else if( 2 == c )
	{
		int pitch = 0;
		cout << "音高变化( -10 -- 10 ):";
		cin >> pitch;
		//变调处理
		float* out = pro.PitchShifting( pitch, (float*)recorder.GetData(), recorder.GetDataSize() );
		bufferSize = pro.GetAfterPitchSize();
		CRecordPlayer player( bufferSize, out );
		player.Init();
		player.StartPlay();
	}
	else if( 3 == c )
	{
		double scale = 0.0;
		cout << "变速速率( 0.1 -- 5.0 )：";
		cin >> scale;
		int pitch = 0;
		cout << "音高变化( -10 -- 10 ):";
		cin >> pitch;
		//变速处理
		float* scaleOut = pro.TimeScaling( scale, (float*)recorder.GetData(), recorder.GetDataSize() );
		bufferSize = pro.GetAfterScaleSize();
		//变调处理
		float* out = pro.PitchShifting( pitch, scaleOut, bufferSize );
		bufferSize = pro.GetAfterPitchSize();
		CRecordPlayer player( bufferSize, out );
		player.Init();
		player.StartPlay();

	}
//	//unsigned int bufferSize = recorder.GetDataSize();
//	//CProcesser pro;
//	//变速处理
//	//float* out = pro.TimeScaling( 0.3, (float*)recorder.GetData(), recorder.GetDataSize() );
//	//bufferSize = pro.GetAfterScaleSize();
//	//变调处理
//	float* out = pro.PitchShifting( 6, (float*)recorder.GetData(), recorder.GetDataSize() );
//	bufferSize = pro.GetAfterPitchSize();
//
//#if SAVE_PROCESS_RECORD_TO_WAV
//	Pcm2WavInfo info;
//	info.channels	= 1;
//	info.formatTag	= 3;	//3对应32bit float
//	info.inFileName = "";
//	info.outFileName= "pitchout_1_32float_44100.wav";
//	info.sampleBits = 32;
//	info.sampleRate = 44100;
//	pro.Pcm2Wav( out, bufferSize, info );
//#endif
//
//	CRecordPlayer player( bufferSize, out );
//	player.Init();
//	player.StartPlay();

}