//#include "wavread.h"
//#include "fft_complex.h"
#include "CFFT.h"
#include "CWavread.h"
#include <string.h>
#include <fstream>
using namespace std;

#define STH_ISTFT 0.66666667


class CAudioTimeScaling
{
private:
//	CAudioTimeScaling(const CAudioTimeScaling& cp){}
//	CAudioTimeScaling& operator = (const CAudioTimeScaling& cp){}
	complex** STFT(float* dataIn);
	complex** PVsample(complex** dataIn);
	float* ISTFT(complex** dataIn);

	unsigned long	m_PCMSize;
	int				m_hop;
	unsigned long	m_STFTOutRow;
	unsigned long   m_STFTOutCol;
	int				m_winSize;
	float			m_scale;
	unsigned long   m_timeScaleSize;
	CFFT			m_FFT;
	CWavread		m_wavread;

public:
	CAudioTimeScaling();
	~CAudioTimeScaling();
	float*	WavReadFile(const char* filename);
	float*  WavReadBuffer( float* buffer,unsigned long bufferSize,int channel);
	float*	TimeScaling(float* dataIn,int winSize,int hop,float scale);
	int		WavWrite(float* dataIn);	
	float*	wavresample(float * DataIn,int p,int q);
	unsigned long GetSize(){return m_timeScaleSize*sizeof(float);}

};