#include "config.h"
#include "math.h"

double KalmanGain;//  ����������
double EstimateCovariance;//����Э����
double MeasureCovariance;//����Э����
double EstimateValue;//����ֵ

void 	KalmanFilterInit(void)
{
	EstimateValue=0;
	EstimateCovariance=0.1;
	MeasureCovariance=0.02;
}

double KalmanFilter(double Measure)
{
	//���㿨��������
	KalmanGain = EstimateCovariance * sqrt(1/(EstimateCovariance*EstimateCovariance+MeasureCovariance*MeasureCovariance));

	//���㱾���˲�����ֵ
	EstimateValue = (EstimateValue + KalmanGain*(Measure-EstimateValue));
	//���¹���Э����

	EstimateCovariance=sqrt(1-KalmanGain)*EstimateCovariance;
	//���²�������
	MeasureCovariance=sqrt(1-KalmanGain)*MeasureCovariance;
	
	//���ع���ֵ
	return EstimateValue;
}