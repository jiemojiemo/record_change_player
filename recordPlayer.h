#ifndef RECORDPLAYER_H
#define RECORDPLAYER_H
#include "record_Save_Process_PlayHeader.h"
#include "recorder.h"
#include "portaudio.h"
/**
* @brief : ������Ƶ����ʾ��Ƶ��
*/
class CRecordPlayer{
	friend class CRecoder;
private:
	CRecordPlayer( const CRecordPlayer& p ){}
	CRecordPlayer& operator=( const CRecordPlayer& p ){}

	/**
	* @brief : ��Ƶ�豸��ʼ��
	*/
	int AudioInit();

	/**
	* @brief : opengl��ʼ��
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
	* @brief : ��ʼ����Ƶ�豸��opengl
	*/
	int Init();

	/**
	* @brief : ��ʼ���ź���ʾ
	*/
	int StartPlay();

	/**
	* @brief : �رղ���
	*/
	int ClosePlay();
	/**
	* @brief : ������Ƶ�Ļص�����
	*/
	static int playCallback(
		const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);
	
};

#endif