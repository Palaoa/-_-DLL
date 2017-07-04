#pragma once
/*
* =====================================================================================
*
*    Filename:  bpnn.h
*
*    Description:
*
*        Version:  1.0
*        Created:  09/01/11 15:16:51
*       Revision:  none
*       Compiler:  gcc
*
*         Author: maozhi fan , 1fanmaozhi@tongji.edu.cn
*        Company:  Tongji Unversity
*
* =====================================================================================
*/
#ifndef __BPNN_H__
#define  __BPNN_H__
#include "ExtractGaborjet.h"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <sys/stat.h>
#include <fcntl.h>
//#include <unistd.h>//-
#include <Windows.h>//+
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <string.h>
#include <cmath>

using namespace std;

#define DEFAULT_ETA			0.3		//Ĭ��ѧϰ��
#define DEFAULT_MOMENTUM	0.3			//Ĭ�ϳ���
#define		TARGET_ONE		(double(1.0))
#define		TARGET_ZERO		(double(0.0))
#define FEATURE_NUM   2


#define MAX_TRAINING_ERROR	9999999.0
#define MIN_TRAINING_ERROR	0.00003

#define PIC_HEIGHT	150
#define PIC_WIDTH	130
#define PIC_PIXELS	PIC_HEIGHT*PIC_WIDTH
#define PIC_SCALE	255

#define BPNN_SAVEFILE_FLAG			"BPNN"	//BP���籣���ļ��ı��
#define BPNN_SAVEFILE_SEPCHAR		'\n'	//�����ļ��ķָ���

#define ABS(x)	(x)>0?(x):(-(x))

typedef std::vector<std::pair<std::string, std::vector<double> > >	target_type;

typedef struct ccim
{
	string m_sFilePath;
	string m_sName;
	int type;
	//static int maxtype;
	int maxtype;
	double* im;
}cimage;
typedef cimage * CImage;

CImage makeCImage(char * filename, int type);
void freeCImage(CImage im);
void InitInputImage(vector<CImage> &vecImage, int max);

class CBPNN
{
public:
	CBPNN(void);
	~CBPNN(void);
public:
	bool InitBPNN(int nInput, int nOutput, int nHidden);
	bool LoadInputUnit(const double* data, int count, double scale = 1.0);
	bool LoadTarget(const double* data, int count, double scale = 1.0);
	double Train(double eta, double momentum);
	void Test(const double* data, int nDimentions, double scale, std::vector<double>& vecOutput);
	const double* GetOutput();
	bool SaveBPNNFile(const char* sSavePath, int nIterateTime, double fStopError);
	bool LoadBPNNFile(const char* sSavePath);
protected:
	void ReleaseBPNN();
	double Sigmoid(double x);//�����
	void LayerForward();
	double OutputError();
	double HiddenError();
	void AdjustWeights();
	//protected:
public:
	int m_nInput;
	int m_nOutput;
	int m_nHidden;

	double* m_InputUnit;
	double* m_HiddenUnit;
	double* m_OutputUnit;

	//����ͼ(�ȱ���o1��Ԫ������Ȩ,�ٱ���o2��Ȩ,�ϲ�ͬ��)
	//       output layer      o
	//                        / \
		//       hidden layer   o1  o2
//			           /|\
	//       input layer  o o o
//���²�Ϊ�����,���ϲ�Ϊ�����,����������:��������

	double* m_I2HWeight;

	double* m_H2OWeight;

	double* m_preI2HWeight;
	double* m_preH2OWeight;

	double* m_OutputError;
	double* m_HiddenError;

	double* m_Target;

	double m_eta;//learning rate
	double m_momentum;//momentum for updating the weight
};

int Btn_train();

#endif
