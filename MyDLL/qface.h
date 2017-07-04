#pragma once
/*
* =====================================================================================
*
*    Filename:  qface.h
*
*    Description:
*
*        Version:  1.0
*        Created:  04/25/10 15:16:51
*       Revision:  none
*       Compiler:  gcc
*
*         Author: maozhi fan , 1fanmaozhi@tongji.edu.cn
*        Company:  Tongji Unversity
*
* =====================================================================================
*/
#ifndef __QFACE_H__
#define  __QFACE_H__
#include "ExtractGaborjet.h"
#include "ReadFile.h"

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <sys/stat.h>
#include <fcntl.h>
//#include <unistd.h>//-
#include<Windows.h> //+
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <math.h>

using namespace std;

#define HAAR_FEATURE_MAX  3
#define MAX_DIRNAME  256
#define MAX_FILENAME 20
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
#define LOGRATIO_THRESHOLD  0.00001F
#define THRESHOLD_EPS  0.00001
#define RATE_EPS       0.00001

#define FALSE 0
#define TRUE 1

typedef  long long uint64;
typedef unsigned   char BYTE;

typedef struct
{
	int lx;  //left eye
	int ly;
	int rx;  // right eye
	int ry;

	int w;  // eye width
	int h;  // eye height

			//all feature position
	int x[20];
	int y[20];
}FPosition;

//与系统文件的struct 重名，顾加上下划线以示区别
typedef struct
{
	char bfType[2];
	char bfSize[4];
	char bfReserved[4];
	char bfoffBits[4];
}BITMAPFILEHEADER_;


typedef struct
{
	char biSize[4];
	char biWidth[4];
	char biHeight[4];
	char biPlanes[2];
	char biBitCount[2];
	char biCompress[4];
	char biSizeImage[4];
	char biXPelsPerMeter[4];
	char biYPelsPerMeter[4];
	char biClrUsed[4];
	char biClrImportant[4];
}BITMAPINFOHEADER_;
/*
typedef struct tagRGBQUAD
{
	unsigned char rgbBlue; //蓝色的强度
	unsigned char rgbGreen; //绿色的强度
	unsigned char rgbRed; //红色的强度
	unsigned char rgbReserved; //保留字节，为0
} RGBQUAD;
*/
typedef struct
{
	int x;
	int y;
	int width;
	int height;

	int sum;
	int var;
	int flags;
}MySumRect;

typedef struct MyRect
{
	int x;  //宽
	int y;  //高
	int width;
	int height;
}MyRect;

typedef struct MySize
{
	int width;
	int height;
}MySize;

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////      原始特征和分类器      //////////////////////////
////////////////////////////////////////////////////////////////////////////////////
typedef struct R_Feature
{
	int i;
	int x;
	int y;
	int w;
	int h;
}R_Feature;

typedef struct HaarFeature
{
	int count;
	int type;

	struct
	{
		MyRect r;
		int weight;
	}rect[HAAR_FEATURE_MAX];  //haar feature parameters

}HaarFeature;

typedef struct HaarClassifier
{
	HaarFeature haarfeature; //the haar feature 

	float   threshold;             //the threshold of the weak classifier
	int   parity;                //the parity of the weak classifier
	float alpha;                 //the coefficient of the weak classifier

}HaarClassifier;

typedef struct StageClassifier
{
	HaarClassifier* classifier; //the weak classifiers
	int count;       //the number of weakclassifiers included in the stage
	float threshold; //the threshold of the stage classifier

}StageClassifier;

typedef struct ClassifierCascade
{
	int count; //number of stages
	MySize origWindowSize;
	StageClassifier* stageClassifier; //the stage classifiers

}ClassifierCascade;


typedef struct CvPTreeNode
{
	struct CvPTreeNode* parent;
	MyRect* element;
	int rank;
}CvPTreeNode;



////////////////////////////////////////////////////////////////////////////////////
//////////////////////////      检测用的特征和分类器      //////////////////////////
////////////////////////////////////////////////////////////////////////////////////

typedef struct POS
{
	int x;
	int y;

}POS;

typedef struct DetHaarFeature
{
	struct
	{
		int *p0, *p1, *p2, *p3;
		float weight;
	}
	rect[HAAR_FEATURE_MAX];
}DetHaarFeature;

typedef struct DetHaarClassifier
{
	HaarFeature origFeature;
	DetHaarFeature  feature;
	float threshold;
	int parity;
	float alpha;
}DetHaarClassifier;

typedef struct DetStageClassifier
{
	int count;
	float threshold;
	DetHaarClassifier* classifier;
	float alpha0;
	float threshold0;
	int parity0;
}DetStageClassifier;

typedef struct DetCascadeClassifier
{
	int count;
	MySize origWindowSize;
	MySize realWindowSize;
	double scale, invWindowArea;
	MySize imgSize;
	DetStageClassifier* stageClassifier;
	//	float weight_scale;  //为检测扩大的窗口时将计算出的特征值与阀值乘以weight_scale 作比较

	int *p0, *p1, *p2, *p3;
	double *ps0, *ps1, *ps2, *ps3;
}DetCascadeClassifier;




Image GetImageFromData(unsigned char *pbyteSrcImg, int width, int height);

unsigned char* GetImgData(Image im);


unsigned char* createImgMem(const char * pszFileName);
void freeImgMem(unsigned char *pbyteSrcImg);
int ReadBmpFile(const char* pszFileName, unsigned char *pbyteSrcImg, int& nImgWidth, int& nImgHeight, int NumColors);

int  WriteBmpFile(const char* pszFileName, unsigned char* pbyteSrcImg, int nWidth, int nHeight);

void drawAllFeature(unsigned char *pbyteSrcImg, GraphDiscription pos, int nImgWidth, int nImgHeight);

void drawEyeRect(unsigned char *pbyteSrcImg, FPosition &pos, int nImgWidth, int nImgHeight);
GraphDiscription FindAllFeature(unsigned char * src, int width, int height);

GraphDiscription FaceDetect(const char * path, unsigned char *pbyteSrcImg, int nImgWidth, int nImgHeight);

void sobel2(unsigned char *src, int width, int height);
#endif
