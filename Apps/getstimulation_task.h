#ifndef __CCA_TASK_H__
#define __CCA_TASK_H__

#include <stdint.h>

#define ROWS 256
#define COLS_X 8
#define COLS_Y 6

extern char gFFTRes;

extern double mSin(double x);
extern double mCos(double x);
void CalculateMean(double * data, int rows, int cols, double * mean);
void CalculateCovariance(double * data, int rows, int cols, double * cov_matrix);
void CCA(double *X, double *Y, int rows, double *correlation_coefficients);
void computeCovarianceMatrix(double *signal1, double *signal2, int numSamples, double *covMatrix);
double computeCorrelation(double *eegSignal, double refSignals[COLS_Y][ROWS], int numSamples);
void CCATaskInit(void);
void * CCATaskEntry(void * argument);
void * FFTTaskEntry(void * argument);

#endif //__CCA_TASK_H__
