#ifndef _KALMAN_H_
#define _KALMAN_H_

extern	double KalmanGain;//  ����������
extern	double EstimateCovariance;//����Э����
extern	double MeasureCovariance;//����Э����
extern	double  EstimateValue;//����ֵ

extern	void 	KalmanFilterInit(void);
extern	double KalmanFilter(double Measure);

#endif