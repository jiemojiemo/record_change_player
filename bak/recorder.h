#ifndef RECODER_H
#define RECODER_H
#include "record_Save_Process_PlayHeader.h"
#include "portaudio.h"
#include "recordPlayer.h"

class FLOAT32{};
class INT16{};
class INT8{};
class UINT8{};

class CRecoder{
	friend class CRecordPlayer;
private:
	CRecoder( const CRecoder& re ){}
	CRecoder& operator=( const CRecoder& re ){}
	int _StartRecode();
private:
	unsigned int m_seconds;		//¼����ʱ��
	paTestData m_recordData;	//¼���������Լ�һЩ��Ϣ
	PaStream*	m_stream;
public:
	CRecoder();
	CRecoder( int seconds = 5 );
	CRecoder( const RecordeInfo& info, FLOAT32 );
	CRecoder( const RecordeInfo& info, INT16 );
	CRecoder( const RecordeInfo& info, INT8 );
	CRecoder( const RecordeInfo& info, UINT8 );


	/**
	* @brief : ��ʼ����Ƶ�豸
	*/
	int Init();


	/**
	* @brief : ��ʼ¼�� 
	*/
	int StartRecode();
	int StartRecode( const RecordeInfo& info );
	/**
	* @brief : �ر�¼�� 
	*/
	int CloseRocode();
	
	~CRecoder();


	/**
	* @brief : ¼���Ļص����� 
	*/
	static int recordCallback(
		const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);
};

#endif