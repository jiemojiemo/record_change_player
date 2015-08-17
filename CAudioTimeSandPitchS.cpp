#include "CAudioTimeSandPitchS.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

CAudioTimeSandPitchS::CAudioTimeSandPitchS()
{

}
CAudioTimeSandPitchS::~CAudioTimeSandPitchS()
{

}
float* CAudioTimeSandPitchS::WavReadFile(const char* filename)
{
	float* PCMOut;
	wav_struct WAV=m_wavread.wavread_head(filename);
	m_PCMSize=WAV.data_size/WAV.channel/WAV.sample_num_bit*8;
//	m_PCMSize=WAV.data_size/WAV.sample_num_bit*8;
	if (WAV.channel==2)
		PCMOut=m_wavread.wavread_data2(WAV);
	else if(WAV.channel==1)
		PCMOut=m_wavread.wavread_data1(WAV);

	return PCMOut;
}

float* CAudioTimeSandPitchS::WavReadBuffer(float* buffer,unsigned long bufferSize,int channel)
{
	//暂定32位的录音
	m_PCMSize = bufferSize/channel/32*8;
	if(channel==1)
		return buffer;
	//float* Out=new float[bufferSize/channel];
	if(channel==2)
	{
		
	}
	return 0;
}

float* CAudioTimeSandPitchS::TimeScaling(float* DataIn,int winSize,int hop,float scale)
{
	m_winSize=winSize;
	m_hop=hop;
	m_STFTOutRow=((m_PCMSize-winSize)/m_hop)+1;
	m_STFTOutCol=(winSize/2)+1;
	m_scale=scale;
	m_sampleRateScale=1;
	if( fabs(m_scale - 1.0) < 0.00001f )
	{
		m_timeScaleSize = m_PCMSize;
		return DataIn;
	}
	complex**	dataout1=STFT(DataIn);
	complex**	dataout2=PVsample(dataout1);
	float*		dataout3=ISTFT(dataout2);			
	return dataout3;

}

complex** CAudioTimeSandPitchS::STFT(float* dataIn)
{
	complex **DataOut;
	DataOut=new complex* [m_STFTOutRow];
	for (int i=0;i<m_STFTOutRow;i++)
	{
		DataOut[i]=new complex[m_STFTOutCol];
	}

	float* window=new float[m_winSize];
	float* DataWin=new float[m_winSize];
	complex* DataWinC=new complex[m_winSize];
	int halfWinsize=m_winSize/2;

	//汉宁窗
	for (int i=0;i<m_winSize;i++)
	{
		window[i]=0.5*(1+cos(PI*(i-halfWinsize)/(halfWinsize-1)));
		//printf("%f\t",window[i]);
	}

	int c=0;
	
	for (int i=0;i<m_PCMSize-m_winSize;i+=m_hop)
	{	
		for (int j=0;j<m_winSize;j++)
		{
			DataWin[j]=dataIn[i+j]*window[j];
			DataWinC[j].real=DataWin[j];
			DataWinC[j].imag=0;
		}

		m_FFT.fft(m_winSize,DataWinC);
		memcpy(DataOut[c],DataWinC,sizeof(complex)*m_STFTOutCol);

		/*printf("-----%d--------\n",c);
		for (int k=0;k<OUT_STFT_LIE;k++)
		{
			printf("%d**%f+%fi\n",k,DataOut[c][k].real,DataOut[c][k].imag);
		}*/	
		c++;
	
	}	

	delete[] window;
	delete[] DataWin;
	delete[] DataWinC;

	return DataOut;

}

complex** CAudioTimeSandPitchS::PVsample(complex** dataIn)
{

	int N=2*(m_STFTOutCol-1);
	int scalelen=(m_STFTOutRow-2)/m_scale+1;

	float *t=new float[scalelen];
	for (int i=0;i<scalelen;i++)
	{
		t[i]=i*m_scale;
		//printf("%f\t",t[i]);
	}

	complex **DataOut;
	DataOut=new complex* [scalelen];
	for (int i=0;i<scalelen;i++)
	{
		DataOut[i]=new complex[m_STFTOutCol];
	}

	complex **bcols=new complex *[2];
	for (int i=0;i<2;i++)
	{
		bcols[i]=new complex[m_STFTOutCol];
	}

	float *dphi=new float[N/2+1];
	dphi[0]=0;
	for (int i=1;i<(N/2+1);i++)
	{
		dphi[i]=2*PI*m_hop*i/N;
		//	printf("%f\t",dphi[i]);

	}
	float *ph=new float[m_STFTOutCol];
	for (int i=0;i<m_STFTOutCol;i++)
	{
		ph[i]=-1*atan2f(dataIn[0][i].imag,dataIn[0][i].real);//和matlab算出的符号相反
		//	printf("%f\t",ph[i]);
	}
	float tt,tf;
	int a;
	int ocol=0;
	float *bmag=new float[m_STFTOutCol];
	float *dp=new float[m_STFTOutCol];
	for (int i=0;i<scalelen;i++)
	{
		tt=t[i];
		a=floor(tt);
		memcpy(bcols[0],dataIn[a],m_STFTOutCol*sizeof(complex));
		memcpy(bcols[1],dataIn[a+1],m_STFTOutCol*sizeof(complex));
		//	printf("%d\t",a);
		tf=tt-floor(tt);
		//	printf("----------------\n");
		for (int j=0;j<m_STFTOutCol;j++)
		{
			bmag[j]=(1-tf)*sqrt(bcols[0][j].real*bcols[0][j].real+bcols[0][j].imag*bcols[0][j].imag)
				+tf*sqrt(bcols[1][j].real*bcols[1][j].real+bcols[1][j].imag*bcols[1][j].imag);

			dp[j]=atan2f(bcols[1][j].imag,bcols[1][j].real)-atan2f(bcols[0][j].imag,bcols[0][j].real)-dphi[j];
			float adjust=2*PI*(int)((dp[j]/(2*PI)));//matlab计算出的值较小
			dp[j]=dp[j]-adjust;
			//printf("%f\n",dp[j]);

			DataOut[ocol][j].real=bmag[j]*cos(ph[j]);
			DataOut[ocol][j].imag=bmag[j]*sin(ph[j]);
			//	printf("%f+%fi\n",DataOut[ocol][j].real,DataOut[ocol][j].imag);
		}
		//printf("%d------------\n",ocol);
		for (int k=0;k<m_STFTOutCol;k++)
		{
			ph[k]=ph[k]+dphi[k]+dp[k];
			//	printf("%f\t",ph[k]);
		}
		ocol++;		
	}

	delete []t;
	delete[] dphi;
	delete[] ph;

	for (int i=0;i<2;i++)
		delete[] bcols[i];
	delete[] bcols;

	return DataOut;

}
float* CAudioTimeSandPitchS::ISTFT(complex** dataIn)
{

	int ftsize=m_winSize;
	int cols=(m_STFTOutRow-2)/m_scale+1;
	int w=m_winSize;
	int halflen=m_winSize/2;
	int h=m_hop;
	float *window=new float[m_winSize];
	for (int i=0;i<m_winSize;i++)
	{
		window[i]=0.5*(1+cos(PI*(i-halflen)/(m_winSize/2-1)))*STH_ISTFT;
		//printf("%f\t",window[i]);
	}
	int xlen=ftsize+(cols-1)*h;
	m_timeScaleSize=xlen;
	float *Out=new float[xlen];
	memset(Out,0,xlen*sizeof(float));
	complex *ft=new complex [ftsize];
	float *px=new float[ftsize];
	for (int i=0;i<h*(cols-1);i+=h)
	{
		memcpy(ft,dataIn[i/h],sizeof(complex)*m_STFTOutCol);
		for (int l=ftsize-1;l>ftsize/2-1;l--)
		{
			ft[l].real=ft[ftsize-l-1].real;
			ft[l].imag=-ft[ftsize-l-1].imag;
		}

		//matlab ft 对称	
		m_FFT.ifft(ftsize,ft);

		for (int j=0;j<ftsize;j++)
		{
			px[j]=ft[j].real;		
		}
	
		for (int k=0;k<ftsize;k++)
		{

			Out[i+k]=Out[i+k]+px[k]*window[k];
		}

	}

	delete[] ft;
	delete[] px;

	/*FILE* file = fopen( "test", "wb" );

	fwrite( Out, 1, xlen*sizeof(float), file );

	fclose( file );*/

	/*fstream f;
	f.open("TS_Out.txt",ios::app);
	for (int i=0;i<xlen;i++)
	{
		f<<Out[i]<<" ";
	}
	printf("写文件完成！\n");
	*/
	
	return Out;
}



float* CAudioTimeSandPitchS::PitchShiftingFile(float* dataIn,int winSize,int hop,int shift)
{
	if(shift>19)
	{
		printf("音高调整尺度有误！\n");
		assert(shift<20);
	}
	float scale=(float)(20-shift)/20;
	float* Out=TimeScaling(dataIn,winSize,hop,scale);
	m_sampleRateScale=1/scale;
	return Out;

}

float* CAudioTimeSandPitchS::PitchShifting(float* dataIn,int winSize,int hop,int shift)
{
	if(shift>20)
	{
		printf("音高调整尺度有误！\n");
		assert(shift<20);
	}
	float scale=(float)(20-shift)/20;
	float* Out=TimeScaling(dataIn,winSize,hop,scale);
	Out=resample(Out,shift);
	return Out;

}

float* CAudioTimeSandPitchS::resample(float* dataIn,int scale)
{
	

	if (scale>10)
	{
		printf("输入变调参数有误");
		return 0;
	}

	int scale_s=scale > 0 ? scale : (0-scale);
	m_resampleSize=m_timeScaleSize*(20.0-scale)/20.0;
	if( scale == 0 )
		return dataIn;

	float* dataOut=new float[m_resampleSize];
	memset( dataOut, 0, m_resampleSize*sizeof(float) );
	int a=0;
	int k;
	if (scale>0)
	{
		if (scale>6)
		{
			for (int i=0;i<m_timeScaleSize;i+=20)
			{
				for (int j=0;j<20-(10-scale)*2;j+=2)
				{
					dataOut[a++]=dataIn[i+j];
				}
				for (int k=20-(10-scale)*2;k<20;k++)
				{
					dataOut[a++]=dataIn[i+k];
				}

			}

		}
		else
		{
			for (int i=0;i<m_timeScaleSize;i++)
			{

				if (!(i%(20/scale_s)))
				{
					i++;
				}
				dataOut[a++]=dataIn[i];
				//printf("%d,%d\n",a,i);

			}
		}
	}

	else if( scale < 0 )
	{
		for (int i=0;i<m_timeScaleSize;i++)
		{
			dataOut[a]=dataIn[i];
			//	printf("a:%d,i%d\n",a,i);
			int test=(i+1)%(20/scale_s);
			if (!test)
			{
				a=a+1;
				dataOut[a]=(dataIn[i]+dataIn[i+1])/2.0;
				//	printf("a:%d,i%d\n",a,i);
			}
			a=a+1;

		}

	}
	else 
		return dataIn;
	fstream f;
	f.open("D:/1.txt",ios::app);
	for (int i=0;i<m_resampleSize;i++)
	{
		f<<dataOut[i]<<std::endl;
	}
	return dataOut;

}

bool CAudioTimeSandPitchS::Ismiss( int scale )
{
	double rate = double(scale) / 20.0;
	srand( (unsigned)time(0) );
	int num = rand() % 100;	//0~99

	if( double(num)/100.0 < rate )
		return true;
	return false;
}

float* CAudioTimeSandPitchS::TimeScalingAndPitchShifting(int shift, float scale,float* dataIn,int winSize,int hop)
{
	float scaleTime=scale*(float)(20-shift)/20.0;
	float* out=TimeScaling(dataIn,winSize,hop,scaleTime);
	out=resample(out,shift);
	return out;

}