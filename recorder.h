#ifndef RECODER_H
#define RECODER_H
#include "record_Save_Process_PlayHeader.h"
#include "portaudio.h"
#include "recordPlayer.h"

#define FLOAT32_BITS 32
#define INT16_BITS	 16
#define INT8_BITS	 8
#define UINT8_BITS	 8

class MT_FLOAT32{};
class MT_INT16{};
class MT_INT8{};
class MT_UINT8{};

class CRecoder{
	friend class CRecordPlayer;
private:
	CRecoder( const CRecoder& re ){}
	CRecoder& operator=( const CRecoder& re ){}
	int _StartRecode();
	void _InitInfo( const RecordeInfo& info );

	//void PrintMaxAndAver( unsigned int bits );
	//void _PrintMaxAndAver( MT_FLOAT32 );
	//void _PrintMaxAndAver( MT_INT16 );
	//void _PrintMaxAndAver( MT_INT8 );
	//void _PrintMaxAndAver( MT_UINT8 );
private:
	unsigned int m_seconds;		//录音的时长
	static unsigned int m_recordBits;
	paTestData m_recordData;	//录音的数据以及一些信息
	PaStream*	m_stream;
public:
	//CRecoder();
	CRecoder( int seconds = 5 );
	//CRecoder( const RecordeInfo& info, FLOAT32 );
	//CRecoder( const RecordeInfo& info, INT16 );
	//CRecoder( const RecordeInfo& info, INT8 );
	//CRecoder( const RecordeInfo& info, UINT8 );


	/**
	* @brief : 初始化音频设备
	*/
	int Init();

	SAMPLE* GetRecordData(){ return m_recordData.recordedSamples; }
	unsigned int GetRecordDataSize() { return m_recordData.totalBytes; }

	/**
	* @brief : 开始录音 
	*/
	int StartRecode();
	int StartRecode( const RecordeInfo& info );
	/**
	* @brief : 关闭录音 
	*/
	int CloseRocode();
	
	~CRecoder();


	/**
	* @brief : 录音的回调函数 
	*/
	static int recordCallback(
		const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);
};

#endif