#ifndef PROCESSER_H
#define PROCESSER_H

#include "pcm2wav.h"
#include "CAudioTimeSandPitchS.h"

class CProcesser
{
private:
	CProcesser& operator=( const CProcesser& cp ){}
	CProcesser( const CProcesser& cp ){}

	CPcm2Wav m_pcm2wav;		//pcm ×ª wav´¦Àí	
	CAudioTimeSandPitchS	m_audioTimeScalingS;
public:
	CProcesser();

	int Pcm2Wav( const Pcm2WavInfo& info );
	int Pcm2Wav( const float* buffer, unsigned long bufferSize,const Pcm2WavInfo& info );
	float* TimeScalingAndPitchShifting(int shift, float scale,float* dataIn,unsigned int bufferSize,int channels = 1,int winSize=1024,int hop=256);
	float* TimeScaling( float scale,float* buffer, unsigned long bufferSize, int channels = 1, int winSize=1024,
		int hop = 256
		);
	float* PitchShiftingFile( int shift, float* buffer, unsigned long bufferSize, int channels = 1, int winSize=1024,
		int hop = 256  );
	float* PitchShifting( int shift, float* buffer, unsigned long bufferSize, int channels = 1, int winSize=1024,
		int hop = 256  );
	float* PitchShiftingFile( int shift, const char* filename );
	unsigned long GetAfterScaleSize(){ return m_audioTimeScalingS.GetSize(); }
	unsigned long GetAfterPitchSize(){ return m_audioTimeScalingS.GetResampleSize(); }
	float GetAfterPitchShitingSampleRate(){ return m_audioTimeScalingS.GetSampleRateScale(); }

}; 

#endif