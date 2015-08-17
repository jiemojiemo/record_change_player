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
	float* outbuffer = m_audioTimeScalingS.WavReadBuffer( buffer, bufferSize, channels );
	float* out = m_audioTimeScalingS.TimeScaling( outbuffer, winSize, hop, scale );
	return out;
}

float* CProcesser::PitchShiftingFile( int shift, float* buffer, unsigned long bufferSize, int channels /* = 1 */,
	int winSize/* =1024 */, int hop /* = 256 */ )
{
	float* outbuffer = m_audioTimeScalingS.WavReadBuffer( buffer, bufferSize, channels );
	float* out = m_audioTimeScalingS.PitchShiftingFile( outbuffer, winSize, hop, shift );
	return out;
}
float* CProcesser::PitchShiftingFile( int shift, const char* filename )
{
	float* outbuffer = m_audioTimeScalingS.WavReadFile( filename );
	float* out = m_audioTimeScalingS.PitchShifting( outbuffer, 1024, 256,shift );
	return out;
}
float* CProcesser::PitchShifting( int shift, float* buffer, 
	unsigned long bufferSize, int channels /* = 1 */, 
	int winSize/* =1024 */, int hop /* = 256 */ )
{
	float* outbuffer = m_audioTimeScalingS.WavReadBuffer( buffer, bufferSize, channels );
	float* out = m_audioTimeScalingS.PitchShifting( outbuffer, winSize, hop, shift );
	return out;

}

float* CProcesser::TimeScalingAndPitchShifting(int shift, float scale,
	float* dataIn,unsigned int bufferSize,int channels, int winSize,int hop)
{
	float* outbuffer = m_audioTimeScalingS.WavReadBuffer( dataIn, bufferSize, channels );
	return m_audioTimeScalingS.TimeScalingAndPitchShifting( shift,scale,outbuffer,winSize,hop );
}