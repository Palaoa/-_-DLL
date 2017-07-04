#pragma once
/*
* =====================================================================================
*
*  Filename:  ReadFile.h
*
*  Description: 读取个各种文件中的数据，如pgm图片、一些配置文件中数据
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

#ifndef READFILE_H
#define READFILE_H

#include "ExtractGaborjet.h"

//#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
//#include <unistd.h> //-
#include <Windows.h> //+
#include <sys/types.h>
#include <errno.h>

#define ME( mat , i , j )  ( (mat)->cols[j][i] )
#define ROUND(v)    		( (int) ((v)+0.5) )
#define RANDOM   (fabs(((FTYPE)rand())/RAND_MAX))

#define LABELMAX 100

#define ELLIPSE_X    65.0
#define ELLIPSE_Y    50.0
#define ELLIPSE_A    64.0
#define ELLIPSE_B    118.0
#define EDGE_PRE     0
#define EDGE_POST    0
#define EYE_LX       30.0
#define EYE_LY       45.0
#define EYE_RX       100.0
#define EYE_RY       45.0
#define SIZE_WIDTH   130
#define SIZE_HEIGHT  150
#define NOISE        0.0
#define SCALE        1.0
#define SHIFT_X      0.0
#define SHIFT_Y      0.0
#define HIST_NONE    0
#define HIST_PRE     1
#define HIST_POST    2
#define CL_YES       1
#define CL_NO        0


typedef struct {
	FTYPE x1, y1, x2, y2;
}TwoPoints;  //人眼位置，左眼(x1,y1)，右眼(x2.y2)


struct finfo {
	char filename[MAX_FILENAME_LENGTH];
	TwoPoints source;
	struct finfo *next;
};//人脸集

typedef struct {
	int row_dim;  /* Number of rows    */
	int col_dim;  /* Number of columns */
	FTYPE *data;    /* Data stored in standard form: column major */
	FTYPE **cols;
}matrix;
typedef matrix* Matrix;


typedef struct {
	const char* eyeFile;
	const char* inputDir;
	const char* nrmDir;
	const char* sfiDir;
	const char* pgmDir;

	int geoType;
	int histType;
	int nrmType;
	int preNormType;
	int maskType;
	int preEdge;
	int postEdge;

	int sizeWidth;
	int sizeHeight;

	FTYPE scale;
	FTYPE noise;
	FTYPE shiftX;
	FTYPE shiftY;
	int reflect;
	int configSuffix;

	FTYPE eyeLx;
	FTYPE eyeLy;
	FTYPE eyeRx;
	FTYPE eyeRy;

	FTYPE ellipseX;
	FTYPE ellipseY;
	FTYPE ellipseA;
	FTYPE ellipseB;
}Arguments;

Image readPGMImage(const char*);
void  writePGMImage(const Image, const char*, int channel);
void  writePPMImage(const Image, const char*);

Matrix makeMatrix(int row_dim, int col_dim);        /* creates and allocates memory for a matrix */
void freeMatrix(Matrix A);                        /* frees the area allocated to a matrix      */
Matrix makeZeroMatrix(int row_dim, int col_dim);       /* creates, allocates, and initailizes memory for a matrix */
Matrix makeIdentityMatrix(int dim);           /* creates and allocates memory for an identity matrix */
Matrix multiplyMatrix(const Matrix A, const Matrix B);
Matrix invertRREF(Matrix);
void matrixRREF(Matrix m);
void rowSwap(Matrix m, int rSrc, int rDest);
void rowMult(Matrix m, int rSrc, FTYPE value);
void rowMultAdd(Matrix m, int rSrc, int rDest, FTYPE value);
FTYPE randBM();

void preProocess(const char* eyeFile, const char* inputDir, const char* pgmDir, Arguments* args);

void convertimage(GraphDiscription gd, const char* eyeFile, const  char* inputDir, const char* pgmDir);
void convertImages(char* eyeFile, char* inputDir, char* pgmDir);
void convertOneImages(TwoPoints sourc, char* imageDir, char* imageName, char* pgmDir);
GraphDiscription invertTransformPoints(GraphDiscription srcGd, const Matrix m);


void applyMask(Image im, const char **mask);

typedef struct hist_bin {
	int count;
	PIX_TYPE rangeStart;
	PIX_TYPE rangeEnd;
} HistogramBin;

void histEqual(Image im, int N);

FTYPE gaussianNoise(Image im, FTYPE noise);

void smoothImageEdge(Image im, int width);
void ZeroMeanOneStdDevMasked(Image im, const char** mask);
void ZeroMeanOneStdDev(Image im);

void histEqualMask(Image im, int N, const char **mask);
#endif
