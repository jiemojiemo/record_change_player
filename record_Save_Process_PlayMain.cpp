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
	cout << "1) ����" << endl;
	cout << "2) ���" << endl;
	cout << "3) ���ټӱ��" << endl;
	cin >>c;
	if( 1 == c )
	{
		double scale = 0.0;
		cout << "��������( 0.1 -- 5.0 )��";
		cin >> scale;
		//���ٴ���
		float* out = pro.TimeScaling( scale, (float*)recorder.GetData(), recorder.GetDataSize() );
		bufferSize = pro.GetAfterScaleSize();
		CRecordPlayer player( bufferSize, out );
		player.Init();
		player.StartPlay();
	}
	else if( 2 == c )
	{
		int pitch = 0;
		cout << "���߱仯( -10 -- 10 ):";
		cin >> pitch;
		//�������
		float* out = pro.PitchShifting( pitch, (float*)recorder.GetData(), recorder.GetDataSize() );
		bufferSize = pro.GetAfterPitchSize();
		CRecordPlayer player( bufferSize, out );
		player.Init();
		player.StartPlay();
	}
	else if( 3 == c )
	{
		double scale = 0.0;
		cout << "��������( 0.1 -- 5.0 )��";
		cin >> scale;
		int pitch = 0;
		cout << "���߱仯( -10 -- 10 ):";
		cin >> pitch;
		//���ٴ���
		float* scaleOut = pro.TimeScaling( scale, (float*)recorder.GetData(), recorder.GetDataSize() );
		bufferSize = pro.GetAfterScaleSize();
		//�������
		float* out = pro.PitchShifting( pitch, scaleOut, bufferSize );
		bufferSize = pro.GetAfterPitchSize();
		CRecordPlayer player( bufferSize, out );
		player.Init();
		player.StartPlay();

	}
//	//unsigned int bufferSize = recorder.GetDataSize();
//	//CProcesser pro;
//	//���ٴ���
//	//float* out = pro.TimeScaling( 0.3, (float*)recorder.GetData(), recorder.GetDataSize() );
//	//bufferSize = pro.GetAfterScaleSize();
//	//�������
//	float* out = pro.PitchShifting( 6, (float*)recorder.GetData(), recorder.GetDataSize() );
//	bufferSize = pro.GetAfterPitchSize();
//
//#if SAVE_PROCESS_RECORD_TO_WAV
//	Pcm2WavInfo info;
//	info.channels	= 1;
//	info.formatTag	= 3;	//3��Ӧ32bit float
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