#pragma once
/*
* =====================================================================================
*
*  Filename:  ExtractGaborJet.h
*
*  Description: 抽取图片中一个位置的Gabor特征值
*
*  Versitr.n:  1.0
*  Created:  6/27/2010 13:17:18 PM
*  Revisitr.n:  none
*  Compiler:  gcc
*
*  Author:  fanmaozhi(fanmaozhi@hotmail.com),
*
* =====================================================================================
*/
#ifndef EXTRACTGABORJET_H
#define EXTRACTGABORJET_H

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
//#include <unistd.h> //-
#include<Windows.h> //+
//#include <sys/types.h>
//#include <errno.h>

//using namespace std;

#define PIX_TYPE double
#define FTYPE  double
#define GFTYPE double

#define JET_BUNCH_DEFAULT_SIZE  32
#define LABELMAX                100
#define PI                      3.141592654
#define LABELMAX                100

#define FILE_LINE_LENGTH        512
#define MAX_FILENAME_LENGTH     2048


#define  ME( mat , i , j )  ( (mat)->cols[j][i] )
#define  ROUND(v)    		    ( (int) ((v)+0.5) )

#define INT_FLOOR(a) ((int)(a))
#define INT_CEIL(a)  ((int)(a)+1)

#define MIN(v1, v2) 		( ((v1) > (v2)) ? (v2) : (v1) )
#define MAX(v1, v2) 		( ((v1) < (v2)) ? (v2) : (v1) )
#define ABS(v)      		( ((v) < 0)     ? -(v) : (v)  )
#define SQR(v)      		( (v) * (v) )
#define ROUND(v)    		( (int) ((v)+0.5) )

#define TRUNC(v)  ((int)v)
#define EQUAL_ZERO(v,tol)  	( ABS(v) < tol )

#define FINITE(n) if(!finite(n)) printf("ERROR: Not Finite - %10s:%5f <%10s,%10s,%3d>\n",#n,n,__FILE__,__FUNCTION__,__LINE__);
#define IE( img , x , y , c ) 	( (img)->data[(x)][(y)][(c)] )
#define ie( img , x , y , c ) ((x < img->width && y < img->height && c < img->channels && x >= 0 && y >=  0 && c >= 0) ? IE(img, x, y, c) : 0.0 )

#define ALLOCATE( type , number ) ( (type*)csu_allocate(#type, sizeof(type),(number), __FILE__, __LINE__) )

#define MAXEYEFILE  100


typedef struct {
	int width;
	int height;
	int channels;
	PIX_TYPE*** data;//图像数据
}image; //用浮点型存储图像
typedef image* Image;

typedef struct {
	int width;
	int height;
	int channels;
	int*** data;
}intimage; //用整型存储图像
typedef intimage* IntImage;

typedef struct {
	int length;
	GFTYPE* wavelength;//小波波长
	GFTYPE* angle;//小波方向
	GFTYPE* phase;//小波相角
	GFTYPE* aspect;//高斯半径方向
	GFTYPE* radius;//高斯半径
	int*    size;

	/* Precomputed parameters for displacement estimation */
	GFTYPE* kx;
	GFTYPE* ky;
}gabor_jet_params;//gabor jet参数
typedef gabor_jet_params *GaborJetParams;

typedef struct {
	int size;
	Image *masks;//掩码

	GaborJetParams params;//参数
}jet_masks;//gabor——jet掩码

typedef jet_masks  *JetMasks;

typedef struct {
	int size;
	IntImage *masks;

	GaborJetParams params;
}int_jet_masks;//整型掩码

typedef int_jet_masks  *IntJetMasks;

typedef struct {
	int length;
	GFTYPE x, y;//位置

				/* Complex specification for the coeffiecients */
	GFTYPE* realPart;//实部
	GFTYPE* imagPart;//虚部

					 /* Polar transformation of the above complex numbers*/
	GFTYPE* mag;//复数的幅值
	GFTYPE* ang;//复数的相角

				/* Parameters used to generate gabor kernels */
	GaborJetParams params;
}gabor_jet;
typedef gabor_jet *GaborJet;


typedef struct {
	int allocsize;//分配大小
	int size; //当前大小

	GaborJet* jets;
} jet_bunch;//束图即人脸某一个特征点的集合
typedef jet_bunch *JetBunch;
JetBunch makeJetBunch();
void freeJetBunch(JetBunch bunch);
int addJetToBunch(JetBunch bunch, GaborJet jet);



/******************************* Face Graph ********************************/

typedef struct {
	int geosize; /* numbers of the located point. */
	int totalsize; /* number fo the located points and interpolated points */
	GaborJetParams params;
	GaborJet* jets;
} face_graph;//用来表示人脸
typedef face_graph* FaceGraph;


FaceGraph makeFaceGraph(int geosize, int totalsize);
void freeFaceGraph(FaceGraph);
FaceGraph loadFaceGraph(const char *filename);
void saveFaceGraph(char *filename, FaceGraph);
int compareFaceGraph(FaceGraph fg1, FaceGraph fg2);
/************************* Gabor Graph Discription *************************/

typedef struct {
	int vert1, vert2;
} Edge;//构建特征点网络的边

typedef struct {
	FTYPE x, y;
} Vert;//构建特征点网络的点

typedef struct {
	int numVert;
	int numEdge;
	Vert* verts;//点
	Edge* edges;//边
	char** vertLabels;
	JetBunch* bunch;//所有特征点的束图
} graph_discription;//人脸束图

typedef graph_discription *GraphDiscription;


GraphDiscription readPosition(const char* filename);
GraphDiscription readGraphDiscription(const char* filename);
GraphDiscription readBinaryGraphDiscription(const char* filename);
void saveGraphDiscription(const char* filename, GraphDiscription);
void saveBinaryGraphDiscription(const char* filename, GraphDiscription gd);

GraphDiscription readBunchGraph(const char* filename, JetMasks masks);
void saveBunchGraph(const char* filename, GraphDiscription gd);
void freeGraphDiscription(GraphDiscription gd);

/*****************************************************************************/

Image makeImage(int width, int height, int channels);
void freeImage(Image i);
IntImage makeIntImage(int width, int height, int channels);
void freeIntImage(IntImage i);

GaborJet extractJet(GFTYPE x, GFTYPE y, Image im, JetMasks masks);
PIX_TYPE convolvePoint(PIX_TYPE x, PIX_TYPE y, int c, Image im, Image mask);
GaborJet makeGaborJet(int length);
void     freeGaborJet(GaborJet jet);
void computePolar(GaborJet jet);
PIX_TYPE interpLinear(Image img, PIX_TYPE x, PIX_TYPE y, int c);


GaborJetParams makeJetParams(int length);
void freeJetParams(GaborJetParams params);

void saveJetMasks(JetMasks masks, const char* filename);
JetMasks readJetMasks(const char* filename);
void saveIntJetMasks(JetMasks masks, const char* filename);
IntJetMasks readIntJetMasks(const char* filename);
JetMasks readMasksFile(const char* filename);
JetMasks makeJetMasks(int size);
void freeJetMasks(JetMasks masks);
IntJetMasks makeIntJetMasks(int size);
void freeIntJetMasks(IntJetMasks masks);
Image makeGaborMask(FTYPE wavelength, FTYPE angle, FTYPE phase, FTYPE aspect, FTYPE radius, int size);
Image readGaborMask(int size);
IntImage readIntGaborMask(int size);
void ZeroMeanUnitLength(Image im);


void* csu_allocate(const char* type, int sof, int num, const char* file, int line);

void  writeint(FILE* file, int i);
void  writereal(FILE* file, FTYPE r);
int  readint(FILE* file);
FTYPE  readreal(FILE* file);

#endif
