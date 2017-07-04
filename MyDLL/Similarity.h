#pragma once
/*
* =====================================================================================
*
*  Filename:  Similarity.h
*
*  Description: 计算两张图片相似度
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

#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "ReadFile.h"
#include "ExtractGaborjet.h"

#include <pthread.h>
//#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
//#include <unistd.h>
#include<Windows.h>
#include <sys/types.h>
#include <errno.h>

#pragma comment(lib, "pthreadVC2.lib")  //必须加上这句
typedef struct {
	int i;
	GraphDiscription gd;
	Image face;
	FaceGraph graph;
	JetMasks masks;
}multi_Process_Jet_Args;//为多进程建立的数据结构


static FTYPE DEPredictiveStep(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);
static FTYPE JetSimilarityDEPredictiveStep(GaborJet j1, GaborJet j2);
FTYPE fgSimPhase02(FaceGraph f1, FaceGraph f2);
int multiProcessOneFaceGraph(char* fileDir, char *fileName, char *outFaceGraphDir, JetMasks masks);
int BPNNFaceGraph(const char* fileDir, const char *fileName, const char *outFaceGraphDir, JetMasks masks);
int processOneFaceGraph(char* fileDir, char *fileName, char *outFaceGraphDir, JetMasks masks);
int processAllFaceGraph(char* eyeFile, char * fileDir, char *outFaceGraphDir, JetMasks masks);

GraphDiscription extractModelJets(char* eyeFile, char* imageDir, char* graphDir, JetMasks masks);

typedef FTYPE(*JetDisplacementEstimator)(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);
static FTYPE DEPredictiveStep(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy);

static void guessVertexLocation(int vert, int maxvert, GraphDiscription graphTemplate, GraphDiscription bunchGraph);
static void LocatePoint(FTYPE * x, FTYPE * y, JetBunch bunch, Image novel, JetMasks masks, JetDisplacementEstimator dispEst);
void locateElasticFeatures(char *imageName, GraphDiscription bunchGraph, JetMasks masks, char* imageDir, char* outputDir);

#endif // READFILE_H
