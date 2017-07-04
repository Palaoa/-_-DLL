#include "stdafx.h"
#include "bpnn.h"
#include "ReadFile.h"
#include "ExtractGaborjet.h"

//#include <time.h>
#include <math.h>
#include <iostream>
#include <sstream>

//int ccim::maxtype = 0;

CBPNN::CBPNN(void)
{
	m_nInput = 0;
	m_nOutput = 0;
	m_nHidden = 0;
	m_InputUnit = NULL;
	m_HiddenUnit = NULL;
	m_OutputUnit = NULL;
	m_I2HWeight = NULL;
	m_H2OWeight = NULL;
	m_OutputError = NULL;
	m_HiddenError = NULL;
	m_Target = NULL;
	m_preI2HWeight = NULL;
	m_preH2OWeight = NULL;

	m_eta = m_momentum = 0.3;
}

CBPNN::~CBPNN(void)
{
	ReleaseBPNN();
}

bool CBPNN::InitBPNN(int nInput, int nOutput, int nHidden)
{
	ReleaseBPNN();

	m_nInput = nInput + 1;
	m_nOutput = nOutput;
	m_nHidden = nHidden + 1;

	m_InputUnit = new double[m_nInput];
	if (!m_InputUnit)
		return false;
	m_OutputUnit = new double[m_nOutput];
	if (!m_OutputUnit)
		return false;
	m_HiddenUnit = new double[m_nHidden];
	if (!m_HiddenUnit)
		return false;

	m_OutputError = new double[m_nOutput];
	if (!m_OutputError)
		return false;
	m_HiddenError = new double[m_nHidden];
	if (!m_HiddenError)
		return false;

	m_Target = new double[m_nOutput];
	if (!m_Target)
		return false;


	memset(m_InputUnit, 0, sizeof(double)*(m_nInput));
	memset(m_OutputUnit, 0, sizeof(double)*(m_nOutput));
	memset(m_HiddenUnit, 0, sizeof(double)*(m_nHidden));
	memset(m_OutputError, 0, sizeof(double)*(m_nOutput));
	memset(m_HiddenError, 0, sizeof(double)*(m_nHidden));
	memset(m_Target, 0, sizeof(double)*(m_nOutput));

	int nI2HW = (m_nHidden - 1)*(m_nInput);
	int nH2OW = (m_nHidden)*(m_nOutput);

	m_I2HWeight = new double[nI2HW];
	if (!m_I2HWeight)
		return false;
	m_H2OWeight = new double[nH2OW];
	if (!m_H2OWeight)
		return false;

//	srand((unsigned)time(NULL)); (

	double r = 0.0;
	int i = 0;
	for (i = 0; i<nI2HW; i++)
		m_I2HWeight[i] = 0;//rand()*1.0/RAND_MAX;//0.000005;//-0.005+rand()*1.0/RAND_MAX;//-0.05到0.05之间
						   //m_I2HWeight[i]=rand()*1.0/(double)(RAND_MAX);//rand()*1.0/RAND_MAX;//0.000005;//-0.005+rand()*1.0/RAND_MAX;//-0.05到0.05之间
	for (i = 0; i<nH2OW; i++)
		m_H2OWeight[i] = rand()*1.0 / (double)(RAND_MAX) / 100;//rand()*1.0/RAND_MAX/10000;//-0.5+rand()*1.0/RAND_MAX;

	m_preI2HWeight = new double[nI2HW];
	//	m_preI2HWeight= (double *)malloc(sizeof(double) *nI2HW);
	m_preH2OWeight = new double[nH2OW];
	memset(m_preI2HWeight, 0, sizeof(double)*nI2HW);
	memset(m_preH2OWeight, 0, sizeof(double)*nH2OW);

	//	memset(m_I2HWeight,0,sizeof(double)*nInput*nHidden);
	//	memset(m_H2OWeight,0,sizeof(double)*nHidden*nOutput);

	m_eta = DEFAULT_ETA;
	m_momentum = DEFAULT_MOMENTUM;

	return true;
}

void CBPNN::ReleaseBPNN()
{
	if (m_InputUnit)
		delete[] m_InputUnit;
	if (m_HiddenUnit)
		delete[] m_HiddenUnit;
	if (m_OutputUnit)
		delete[] m_OutputUnit;
	if (m_I2HWeight)
		delete[] m_I2HWeight;
	if (m_H2OWeight)
		delete[] m_H2OWeight;
	if (m_OutputError)
		delete[] m_OutputError;
	if (m_HiddenError)
		delete[] m_HiddenError;
	if (m_Target)
		delete[] m_Target;
	if (m_preH2OWeight)
		delete[] m_preH2OWeight;
	if (m_preI2HWeight)
		delete[]  m_preI2HWeight;

	m_InputUnit = NULL;
	m_HiddenUnit = NULL;
	m_OutputUnit = NULL;
	m_I2HWeight = NULL;
	m_H2OWeight = NULL;
	m_OutputError = NULL;
	m_HiddenError = NULL;
	m_Target = NULL;
	m_preH2OWeight = NULL;
	m_preI2HWeight = NULL;
}

bool CBPNN::LoadInputUnit(const double* data, int count, double scale)
{
	if (count != m_nInput - 1)
		return false;

	//m_InputUnit[0] 即 x0 始终为 1
	for (int i = 0; i<count; i++)
		m_InputUnit[i + 1] = data[i] / scale;

	m_InputUnit[0] = 1.0;

	return true;
}

bool CBPNN::LoadTarget(const double* data, int count, double scale)
{
	if (count != m_nOutput)
		return false;

	//m_Target[0] 即 x0 始终为 1
	for (int i = 0; i<count; i++)
		m_Target[i] = data[i] / scale;

	return true;
}

double CBPNN::Sigmoid(double x)
{
	return (1.0 / (1.0 + exp(-x)));
}


//////////////////////////////////////////////////////////////////////////
//测试时注意
//////////////////////////////////////////////////////////////////////////
void CBPNN::LayerForward()
{
	double sum;
	int i = 0, j = 0;

	m_InputUnit[0] = 1.0;

	for (i = 1; i < m_nHidden; i++)
	{
		sum = 0.0;
		for (j = 0; j < m_nInput; j++)
		{
			double s_tmp = (double)(m_I2HWeight[(i - 1)*m_nInput + j] * m_InputUnit[j]);
			sum += s_tmp;
		}
		m_HiddenUnit[i] = Sigmoid(sum);
	}

	//////////////////////////////////////////////////////////////////////////

	m_HiddenUnit[0] = 1.0;
	for (i = 0; i < m_nOutput; i++)
	{
		sum = 0.0;
		for (j = 0; j < m_nHidden; j++)
			sum += m_H2OWeight[i*m_nHidden + j] * m_HiddenUnit[j];

		m_OutputUnit[i] = Sigmoid(sum);
	}

}

double CBPNN::OutputError()
{
	double o, t, errsum;
	errsum = 0.0;

	for (int i = 0; i < m_nOutput; i++)
	{
		o = m_OutputUnit[i];
		t = m_Target[i];
		m_OutputError[i] = o * (1.0 - o) * (t - o);
		//		m_OutputError[i] = (t - o);
		errsum += ABS(m_OutputError[i]);
	}

	return errsum;
}

/***********************容易出错***************************/
double CBPNN::HiddenError()
{
	double h, sum, errsum;

	errsum = 0.0;
	//对网络的每个隐藏单元计算它的误差项
	for (int i = 1; i < m_nHidden; i++)
	{
		h = m_HiddenUnit[i];
		sum = 0.0;
		for (int j = 0; j < m_nOutput; j++)
		{
			sum += m_OutputError[j] * m_H2OWeight[j*m_nOutput + i];
		}
		m_HiddenError[i] = h * (1.0 - h) * sum;
		errsum += ABS(m_HiddenError[i]);
	}

	return errsum;
}

void CBPNN::AdjustWeights()//double* delta, int ndelta, double* ly, int nly, double** w, double** oldw, double eta, double momentum)
{
	int i = 0, j = 0;
	double delta_w = 0.0;
	for (j = 0; j<m_nOutput; j++)
	{
		for (i = 0; i<m_nHidden; i++)
		{
			delta_w = ((m_eta * m_OutputError[j] * m_HiddenUnit[i]) + (m_momentum*m_preH2OWeight[j*m_nHidden + i]));
			m_H2OWeight[j*m_nHidden + i] += delta_w;
			m_preH2OWeight[j*m_nHidden + i] = delta_w;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	for (j = 1; j<m_nHidden; j++)
	{
		for (i = 0; i<m_nInput; i++)
		{
			delta_w = ((m_eta * m_HiddenError[j] * m_InputUnit[i]) + (m_momentum*m_preI2HWeight[(j - 1)*m_nInput + i]));
			m_I2HWeight[(j - 1)*m_nInput + i] += delta_w;
			m_preI2HWeight[(j - 1)*m_nInput + i] = delta_w;
		}
	}
}


double CBPNN::Train(double eta, double momentum)
{
	m_eta = eta;
	m_momentum = momentum;

	double err_o = 0.0;
	double err_h = 0.0;
	double err = 0.0;
	//1.
	LayerForward();

	//2.
	err_o = OutputError();
	err_h = HiddenError();

	//3.
	AdjustWeights();

	err = err_o + err_h;

	return err;//
}

const double* CBPNN::GetOutput()
{
	return m_OutputUnit;
}

void CBPNN::Test(const double* data, int nDimentions, double scale, std::vector<double>& vecOutput)
{
	double err_o = 0.0;
	double err_h = 0.0;
	double err = 0.0;

	LoadInputUnit(data, nDimentions, scale);
	LayerForward();

	err_o = OutputError();
	err_h = HiddenError();
	err = err_o + err_h;

	vecOutput.clear();

	for (int i = 0; i<m_nOutput; i++)
	{
		vecOutput.push_back(m_OutputUnit[i]);
	}
	cout << "total err is " << err << endl;
}

//将训练好的神经网络保存至文件
bool CBPNN::SaveBPNNFile(const char* sSavePath, int nIterateTime, double fStopError)
{
	FILE* fp = fopen(sSavePath, "w");
	if (fp == NULL)
		return false;

	int i = 0;

	fprintf(fp, "%s%c", BPNN_SAVEFILE_FLAG, BPNN_SAVEFILE_SEPCHAR);
	fprintf(fp, "%d%c", nIterateTime, BPNN_SAVEFILE_SEPCHAR);//Iterate Time
	fprintf(fp, "%lf%c", fStopError, BPNN_SAVEFILE_SEPCHAR);//fStopError,注意lf符号
	fprintf(fp, "%lf%c", m_eta, BPNN_SAVEFILE_SEPCHAR);//eta, the learning rate
	fprintf(fp, "%lf%c", m_momentum, BPNN_SAVEFILE_SEPCHAR);//momentum
	fprintf(fp, "%d%c", m_nInput, BPNN_SAVEFILE_SEPCHAR);//count of input unit
	fprintf(fp, "%d%c", m_nHidden, BPNN_SAVEFILE_SEPCHAR);//count of input unit
	fprintf(fp, "%d%c", m_nOutput, BPNN_SAVEFILE_SEPCHAR);//count of input unit

	int nI2HW = (m_nHidden - 1)*(m_nInput);
	int nH2OW = (m_nHidden)*(m_nOutput);

	fprintf(fp, "%d%c", nI2HW, BPNN_SAVEFILE_SEPCHAR);
	fprintf(fp, "%d%c", nH2OW, BPNN_SAVEFILE_SEPCHAR);

	//保存两层权值wij
	for (i = 0; i<nI2HW; i++)
	{
		fprintf(fp, "%lf%c", m_I2HWeight[i], BPNN_SAVEFILE_SEPCHAR);
	}

	for (i = 0; i<nH2OW; i++)
	{
		fprintf(fp, "%lf%c", m_H2OWeight[i], BPNN_SAVEFILE_SEPCHAR);
	}

	fclose(fp);
	return true;
}

//从文件读取网络的各参数值
bool CBPNN::LoadBPNNFile(const char* sSavePath)
{
	FILE* fp = fopen(sSavePath, "r");
	if (fp == NULL)
		return false;

	char temp[100];
	fscanf(fp, "%s", temp);
	if (strcmp(temp, BPNN_SAVEFILE_FLAG) != 0)
	{
		fclose(fp);
		return false;
	}
	int nIterateTime;
	double fStopError;
	double eta;
	double momentum;
	int nInput;
	int nHidden;
	int nOutput;
	int nI2HW;
	int nH2OW;
	fscanf(fp, "%d", &nIterateTime);
	fscanf(fp, "%lf", &fStopError);
	fscanf(fp, "%lf", &eta);
	fscanf(fp, "%lf", &momentum);
	fscanf(fp, "%d", &nInput);
	fscanf(fp, "%d", &nHidden);
	fscanf(fp, "%d", &nOutput);

	fscanf(fp, "%d", &nI2HW);
	fscanf(fp, "%d", &nH2OW);

	//根据读到的参数创建BP神经网络
	InitBPNN(nInput - 1, nOutput, nHidden - 1);

	int i = 0;

	for (i = 0; i<nI2HW; i++)
	{
		fscanf(fp, "%lf", &(m_I2HWeight[i]));
		// 		sprintf(a,"%lf",m_I2HWeight[i]);
		// 		MessageBox(NULL,A2T(a),_T(""),MB_OK);
	}
	for (i = 0; i<nH2OW; i++)
	{
		fscanf(fp, "%lf", &(m_H2OWeight[i]));
	}

	fclose(fp);

	return true;
}

double * computeim(char * filename)
{
	stringstream tmp;
	tmp << filename << ".pts";

	FaceGraph graph = loadFaceGraph(tmp.str().c_str());

	double * data = (double *)malloc(graph->geosize * graph->jets[0]->length * sizeof(double));

	for (int i = 0; i < graph->geosize; i++)
	{
		for (int j = 0; j < graph->jets[0]->length; j++)
		{
			data[i*graph->params->length / 2 + j] = graph->jets[i]->ang[j];
		}
	}

	freeFaceGraph(graph);

	return data;
}

CImage makeCImage(char * filename, int type)
{
	int x, y;
	CImage im = (CImage)malloc(sizeof(cimage));
	im->im = computeim(filename);
	im->type = type;
	if (im->maxtype < type)im->maxtype = type + 1;

	return im;
}

void freeCImage(CImage im)
{
	if (im)
	{
		if (im->im)free(im->im);
		im->im = NULL;
		free(im);
		im = NULL;
	}
}

void InitInputImage(vector<CImage> &vecImage, int max)
{
	vecImage.clear();

	CImage ctmp = makeCImage("./bpnn/train/BioID_0000", 0);
	vecImage.push_back(ctmp);
	vecImage[0]->maxtype = max;

	ctmp = makeCImage("./bpnn/train/BioID_0001", 0);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0020", 1);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0023", 1);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0050", 2);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0052", 2);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0081", 3);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0082", 3);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0090", 4);
	vecImage.push_back(ctmp);

	ctmp = makeCImage("./bpnn/train/BioID_0091", 4);
	vecImage.push_back(ctmp);
}


int Btn_train()
{
	double eta = DEFAULT_ETA;
	int bUseIterateTime = 1;
	int nIterateTime = 1000000;
	int in_num = 800;
	int out_num = 5;
	int hid_num = 10;

	FaceGraph graph = loadFaceGraph("./data/faceGraph/BioID_0000.pts");
	int ppix = graph->geosize * graph->jets[0]->length;

	in_num = ppix;
	//	sscanf(str,"%lf", &eta);

	double momentum = DEFAULT_MOMENTUM;
	//	sscanf(str,"%lf", &momentum);

//	double data[ppix];//源代码，mabel改，写一行
	double* data = (double*)malloc(ppix*(sizeof(double)));

	class CBPNN m_BPNN;

	int q = 0;

	vector<CImage> vecImage;
	InitInputImage(vecImage, out_num);

	int im_num = vecImage.size();

	m_BPNN.InitBPNN(in_num, out_num, hid_num);

	FILE* fp = fopen("bpnn_error_list.txt", "w");

	int nTime = 0;
	char aa[100];
	string sError;

	double err;
	double total_err = 0;
	double stop_err = MIN_TRAINING_ERROR;

	string sTrainTime;

	while (bUseIterateTime ? (nTime < nIterateTime) : (total_err / im_num >= stop_err))
	{
		total_err = 0.0;

		for (int i = 0; i<im_num; i++)
		{
			memset(data, 0, sizeof(double)*ppix);

			for (int m = 0; m<ppix; m++)
			{
				data[m] = (vecImage[i]->im)[m];
			}
			m_BPNN.LoadInputUnit(data, ppix, PIC_SCALE);

			double* t = new double[out_num];
			for (int j = 0; j < out_num; j++)
			{
				if (vecImage[i]->type == j)
				{
					t[j] = TARGET_ONE;
				}
				else
				{
					t[j] = TARGET_ZERO;
				}
			}

			m_BPNN.LoadTarget(t, out_num);


			delete[] t;

			err = m_BPNN.Train(eta, momentum);
			total_err += err;
			memset(aa, 0, 100);
			sprintf(aa, "T_it=%d\t,L_it=%d, err_o=%g\t\n", nTime + 1, i, err);
			fputs(aa, fp);

			if (nTime == 0)
			{
				memset(aa, 0, 100);
				sprintf(aa, "time=%d            err=%g\n ", nTime + 1, err);
				cout << "List error is " << aa << endl;
			}
			nTime++;

			//		cout << "Train time is " << nTime+1 << endl;
			cout << m_BPNN.m_OutputUnit[0] << "  " << m_BPNN.m_OutputUnit[1] << " " << m_BPNN.m_OutputUnit[2] << " " << m_BPNN.m_OutputUnit[3] << " " << m_BPNN.m_OutputUnit[4] << endl;
		}

		memset(aa, 0, 100);
		sprintf(aa, "time=%d   cur_err=%g  avg_err=%g, %lf    %lf  %lf   %lf   %lf\n ", nTime + 1, err, total_err / im_num, m_BPNN.m_OutputUnit[0], m_BPNN.m_OutputUnit[1], m_BPNN.m_OutputUnit[2], m_BPNN.m_OutputUnit[3], m_BPNN.m_OutputUnit[4]);

		cout << "err is " << aa << endl;
	}

	vector<CImage>::iterator itr = vecImage.begin();
	for (; itr != vecImage.end(); itr++)
	{
		CImage cim = *itr;
		freeCImage(cim);
		*itr = NULL;
	}
	vecImage.clear();

	fclose(fp);
	m_BPNN.SaveBPNNFile("./bpnn/bpnn_weight.txt", nIterateTime, total_err);

	return 0;
}
