#include "processer.h"

CProcesser::CProcesser()
{

}

int CProcesser::Pcm2Wav( const Pcm2WavInfo& info )
{
	return m_pcm2wav.Pcm2Wav( info );
}

int CProcesser::Pcm2Wav( const float* buffer, unsigned long bufferSize,const Pcm2WavInfo& info )
{
	return m_pcm2wav.Pcm2Wav( buffer, bufferSize, info );
}

float* CProcesser::TimeScaling( float scale,float* buffer, unsigned long bufferSize, 
	int channels /* = 1 */, 
	int winSize/* =1024 */,
	int hop /* = 256 */ )
{
	float* outbuffer = m_audioTimeScaling.WavReadBuffer( buffer, bufferSize, channels );
	float* out = m_audioTimeScaling.TimeScaling( outbuffer, winSize, hop, scale );
	return out;
}
