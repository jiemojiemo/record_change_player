#ifndef RECORDPLAYER_H
#define RECORDPLAYER_H
#include "record_Save_Process_PlayHeader.h"
#include "recorder.h"
#include "portaudio.h"
/**
* @brief : 播放音频和显示音频波
*/
class CRecordPlayer{
	friend class CRecoder;
private:
	CRecordPlayer( const CRecordPlayer& p ){}
	CRecordPlayer& operator=( const CRecordPlayer& p ){}

	/**
	* @brief : 音频设备初始化
	*/
	int AudioInit();

	/**
	* @brief : opengl初始化
	*/
private:
	paTestData m_data;
	PaStream* m_stream;
public:
	CRecordPlayer( unsigned long byteSize, const SAMPLE* data );
	CRecordPlayer( const CRecoder& recorder );
	CRecordPlayer( const char* url );
	~CRecordPlayer();

	/**
	* @brief : 初始化音频设备和opengl
	*/
	int Init();

	/**
	* @brief : 开始播放和显示
	*/
	int StartPlay();

	/**
	* @brief : 关闭播放
	*/
	int ClosePlay();
	/**
	* @brief : 播放音频的回调函数
	*/
	static int playCallback(
		const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);
	
};

#endif