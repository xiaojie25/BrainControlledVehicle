#include "getstimulation_task.h"
#include "ads1299_task.h"
#include "ring_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"
//#include "arm_const_structs.h"
#include "dip_task.h"

#define STIMULATION_FREQUENCY_FORWARD 7
#define STIMULATION_FREQUENCY_BACK 9
#define STIMULATION_FREQUENCY_LEFT 11
#define STIMULATION_FREQUENCY_RIGHT 13

#define STIMULATION_FREQUENCY_FORWARD_INTEX 0
#define STIMULATION_FREQUENCY_BACK_INTEX 1
#define STIMULATION_FREQUENCY_LEFT_INTEX 2
#define STIMULATION_FREQUENCY_RIGHT_INTEX 3

#define ROWS 256
#define COLS_X 8
#define COLS_Y 6

#define LSB 0.0000224

double YForward[COLS_Y][ROWS];
double YBack[COLS_Y][ROWS];
double YLeft[COLS_Y][ROWS];
double YRight[COLS_Y][ROWS];
char gFFTRes;

 

double mSin(double x){
	double temp = x;
	double SinX = 0;
	
	for(int i = 1; i <= 5; i++){
		SinX += temp;
		temp *= -x * x / ((2 * i) * (2 * i + 1));
	}
	return SinX;
}

double mCos(double x){
	return mSin(x+PI/2);
}

void CalculateMean(double * data, int rows, int cols, double * mean){
	for(int j = 0; j < cols; j++){
		mean[j] = 0.0;
		for(int i = 0; i < rows; i++){
			mean[j] += data[i * cols + j];
		}
		mean[j] /= rows; 
	}
}

void CalculateCovariance(double * data, int rows, int cols, double * cov_matrix){
	double * mean = (double *)malloc(cols * sizeof(double));
	CalculateMean(data, rows, cols, mean);
	
	for(int i = 0; i < cols; i++){
		for(int j = 0; j < cols; j++){
			cov_matrix[i * cols + j] = 0.0;
			for(int k = 0; k < rows; k++){
				cov_matrix[i * cols + j] += (data[k * cols + i] - mean[i]) * (data[k * cols + j] - mean[j]);
			}
			cov_matrix[i*cols + j] /= (rows - 1);
		}
	}
	free(mean);
}

void CCA(double *X, double *Y, int rows, double *correlation_coefficients){
	double *C_xx = (double *)malloc(COLS_X * COLS_X * sizeof(double));
	double *C_yy = (double *)malloc(COLS_Y * COLS_Y * sizeof(double));
	double *C_xy = (double *)malloc(COLS_X * COLS_Y * sizeof(double));

	CalculateCovariance(X, rows, COLS_X, C_xx);
	CalculateCovariance(Y, rows, COLS_Y, C_yy);

	for(int i = 0; i < COLS_X; i++){
		for(int j = 0; j < COLS_Y; j++){
			C_xy[i * COLS_Y + j] = 0.0;
			for(int k = 0; k < rows; k++){
				C_xy[i * COLS_Y + j] += (X[k * COLS_X + i]) * (Y[k * COLS_Y + j]);
			}
			C_xy[i * COLS_Y + j] /= (rows - 1);
		}
	}

	for(int i = 0; i < COLS_Y; i++){
		correlation_coefficients[i] = 0.0;
		for(int j = 0; j < COLS_X; j++){
			correlation_coefficients[i] += C_xy[j * COLS_Y + i]; // 实际应使用特征值分解
		}
	}

	free(C_xx);
	free(C_yy);
	free(C_xy);
}

void computeCovarianceMatrix(double *signal1, double *signal2, int numSamples, double *covMatrix) {
    double mean1 = 0.0, mean2 = 0.0;

    // 计算均值
    for (int i = 0; i < numSamples; i++) {
        mean1 += signal1[i];
        mean2 += signal2[i];
    }
    mean1 /= numSamples;
    mean2 /= numSamples;

    // 计算协方差
    double covariance = 0.0;
    for (int i = 0; i < numSamples; i++) {
        covariance += (signal1[i] - mean1) * (signal2[i] - mean2);
    }
    covariance /= (numSamples - 1);

    *covMatrix = covariance;
}

double computeCorrelation(double *eegSignal, double refSignals[COLS_Y][ROWS], int numSamples) {
    double covEEG_Sin, covEEG_Cos, covSin_Sin, covCos_Cos;

    // 计算 EEG 和正弦参考信号的协方差
    computeCovarianceMatrix(eegSignal, refSignals[0], numSamples, &covEEG_Sin);
    computeCovarianceMatrix(eegSignal, refSignals[1], numSamples, &covEEG_Cos);

    // 计算正弦信号与自身的协方差
    computeCovarianceMatrix(refSignals[0], refSignals[0], numSamples, &covSin_Sin);
    computeCovarianceMatrix(refSignals[1], refSignals[1], numSamples, &covCos_Cos);

    // 计算典型相关系数
    double correlation = (covEEG_Sin * covEEG_Sin / covSin_Sin + covEEG_Cos * covEEG_Cos / covCos_Cos);
    return correlation;
}



void CCATaskInit(void){
	int FS = 250;
	for(int j = 0; j < COLS_Y; j+=2){
		for(int i = 0; i < ROWS; i++){
			YForward[j][i] = mSin(2*PI*(j/2+1)*STIMULATION_FREQUENCY_FORWARD*i/FS);
			YBack[j][i] = mSin(2*PI*(j/2+1)*STIMULATION_FREQUENCY_BACK*i/FS);
			YLeft[j][i] = mSin(2*PI*(j/2+1)*STIMULATION_FREQUENCY_LEFT*i/FS);
			YRight[j][i] = mSin(2*PI*(j/2+1)*STIMULATION_FREQUENCY_RIGHT*i/FS);
		}
	}
	for(int j = 1; j < COLS_Y; j+=2){
		for(int i = 0; i < ROWS; i++){
			YForward[j][i] = mCos(2*PI*(j/2+1)*STIMULATION_FREQUENCY_FORWARD*i/FS);
			YBack[j][i] = mCos(2*PI*(j/2+1)*STIMULATION_FREQUENCY_BACK*i/FS);
			YLeft[j][i] = mCos(2*PI*(j/2+1)*STIMULATION_FREQUENCY_LEFT*i/FS);
			YRight[j][i] = mCos(2*PI*(j/2+1)*STIMULATION_FREQUENCY_RIGHT*i/FS);
		}
	}
}

void * CCATaskEntry(void * argument){
	
	int Len = RingBufferGetDistanceToBoundary(&mADS1299DataRingBuffer);
	
	uint8_t * tail_addr = RingBufferGetTail(&mADS1299DataRingBuffer);

	
	if(Len < (ADS1299_DATA_LENGTH+2)*ROWS){
		return NULL;
	}
	
	double **ADS1299CCAData = (double **)malloc(COLS_X*sizeof(double));
	for (int i = 0; i < COLS_X; i++){
    ADS1299CCAData[i] = (double *)malloc(ROWS*sizeof(double));
	}
	
	int ptr_move = 0;
	double MaxADS1299CCAData[COLS_X];
	for(int i = 0; i < ROWS; i++){
		ptr_move++;
		for(int j = 0; j < COLS_X; j++){
			ADS1299CCAData[j][i] = (double)((((*(tail_addr + ptr_move)) << 16)|((*(tail_addr + ptr_move + 1)) << 8)|((*(tail_addr + ptr_move + 2))))/10000.0);
			//ADS1299CCAData[j][i] = (double)(ADS1299CCAData[i][j]/1000.0);
			ptr_move += 3;
		}
		ptr_move++;
	}
//	uint32_t temp = 0;
//	for(int i=0;i<COLS_X;i++){
//		arm_max_f32((float *)ADS1299CCAData[i], ROWS, (float *)MaxADS1299CCAData+i,&temp);
//	}
//	
//		for(int i = 0; i < ROWS; i++){
//		for(int j = 0; j < COLS_X; j++){
//			ADS1299CCAData[j][i] = ADS1299CCAData[j][i] / MaxADS1299CCAData[j];
//		}
//	}
	
	double totalCorrelationForward = 0.0;
	double totalCorrelationBack = 0.0;
	double totalCorrelationLeft = 0.0;
	double totalCorrelationRight = 0.0;
	for(int ch = 0; ch < COLS_X; ch++){
		totalCorrelationForward += computeCorrelation(ADS1299CCAData[ch], YForward, ROWS);
	}
	totalCorrelationForward /= COLS_X;
	for(int ch = 0; ch < COLS_X; ch++){
		totalCorrelationBack += computeCorrelation(ADS1299CCAData[ch], YBack, ROWS);
	}
	totalCorrelationBack /= COLS_X;
	for(int ch = 0; ch < COLS_X; ch++){
		totalCorrelationLeft += computeCorrelation(ADS1299CCAData[ch], YLeft, ROWS);
	}
	totalCorrelationLeft /= COLS_X;
	for(int ch = 0; ch < COLS_X; ch++){
		totalCorrelationRight += computeCorrelation(ADS1299CCAData[ch], YRight, ROWS);
	}
	totalCorrelationRight /= COLS_X;
	
	for(int i = 0; i < COLS_X; i++){
		free(ADS1299CCAData[i]);
	}
	free(ADS1299CCAData);
	return NULL;
}

void * FFTTaskEntry(void * argument){
	if(gSysMode != SysMode_2){
		return NULL;
	}
	
	float fft_outputbuf[ROWS];
	float fft_inputbuf[ROWS*2];
	int ADS1299CCAData[COLS_X][ROWS];
	uint8_t Uint8Data[ROWS * COLS_X * 3];
	uint32_t Combined;
	
	int Len = ROWS * COLS_X * 3;
	int NowLen = RingBufferGetDistanceToBoundary(&mADS1299DataRingBuffer);
	
	
	if(Len > NowLen){
		return NULL;
	}

	RingBufferPop(&mADS1299DataRingBuffer, Uint8Data, Len);
	
	int ptr_move = 0;
	for(int j = 0; j < ROWS; j++){
		//ptr_move++;
		for(int i = 0; i < COLS_X; i++){//CHANNEL
			Combined = (Uint8Data[ptr_move] << 16)|(Uint8Data[ptr_move+1] << 8)|Uint8Data[ptr_move+2];
			if (Combined & 0x800000){ // 检查第23位（符号位）
        ADS1299CCAData[i][j] = ((int)(Combined | 0xFF000000)); // 扩展符号位
			}else{
        ADS1299CCAData[i][j] =((int) Combined); // 保持原值
			}
			//ADS1299CCAData[i][j] = (float)((int)((((*(tail_addr + ptr_move)) << 16)|((*(tail_addr + ptr_move + 1)) << 8)|((*(tail_addr + ptr_move + 2)))))/LSB);
			//ADS1299CCAData[j][i] = (double)(ADS1299CCAData[i][j]/1000.0);
			ptr_move += 3;
		}
		//ptr_move++;
	}
	//arm_cmplx_mag_f32
	float StimulusWeighting[4]={0,0,0,0};
	float MaxWeighting;
	uint32_t MaxIntex;
	for(int j = 0; j<COLS_X;j++){
		for(int i = 0;i<ROWS;i++){
			
			fft_inputbuf[i*2] = ((float)ADS1299CCAData[j][i])*LSB;//mV
			fft_inputbuf[i*2 + 1] = 0; 
		}
		arm_cfft_radix4_instance_f32 scfft;
		arm_cfft_radix4_init_f32(&scfft, ROWS, 0, 1);
		arm_cfft_radix4_f32(&scfft,fft_inputbuf);
		arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,ROWS);
		StimulusWeighting[STIMULATION_FREQUENCY_FORWARD_INTEX] += fft_outputbuf[STIMULATION_FREQUENCY_FORWARD];
		StimulusWeighting[STIMULATION_FREQUENCY_BACK_INTEX] += fft_outputbuf[STIMULATION_FREQUENCY_BACK];
		StimulusWeighting[STIMULATION_FREQUENCY_LEFT_INTEX] += fft_outputbuf[STIMULATION_FREQUENCY_LEFT];
		StimulusWeighting[STIMULATION_FREQUENCY_RIGHT_INTEX] += fft_outputbuf[STIMULATION_FREQUENCY_RIGHT];
	}
	arm_max_f32(StimulusWeighting, 4, NULL, &MaxIntex);
	
	switch(MaxIntex){
		case STIMULATION_FREQUENCY_FORWARD_INTEX:{
			gFFTRes='F';
			break;
		}
		case STIMULATION_FREQUENCY_BACK_INTEX:{
			gFFTRes='B';
			break;
		}
		case STIMULATION_FREQUENCY_LEFT_INTEX:{
			gFFTRes='L';
			break;
		}
		case STIMULATION_FREQUENCY_RIGHT_INTEX:{
			gFFTRes='R';
			break;
		}
	}
	
	mADS1299DataRingBuffer.head_index = 0;
	mADS1299DataRingBuffer.tail_index = 0;
	mADS1299DataRingBuffer.is_full = 0;
	

}
	
 






































