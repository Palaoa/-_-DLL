/*
* =====================================================================================
*
*  Filename:  GaborRecmain.c
*
*  Description:这是主程序，调用人脸识别函数进行识别。用一个提供双眼位置的图片，和
*              数据库中图片进行比较，找出最相似的一幅图片。
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
#include "StdAfx.h"
#include "Similarity.h"
#include "ExtractGaborjet.h"
#include "qface.h"
#include "bpnn.h"
//#include "bpnn2.h"

//#include<time.h>
//#include <sys/time.h>
#include <sstream>
#include <iostream>
#include <string>
#include<cstdio>

#define FACE_DETECT_NAME "./image/test6.bmp"
#define FACE_DETECT_PATH "./image/test6"

#define MASK_FILE  "data/gaborrg_jet.xml.masks"
#define MASK_FILE_NAME  "data/gaborrg_jet.masks"
#define EYE_FILE  "data/BioID.eye"
#define PGM_DIR "data/normalizeImage"
#define BUNCH_FILE "data/bunchjetName.pts"
#define INPUT_DIR  "data/inputImage"
#define INPUT_IMAGE  "outcome/inputImage"
#define NORMALIZE_IMAGE  "outcome/normalizeImage"
#define FACE_GRAPH  "outcome/faceGraph"
#define FACE_GRAPH_NORMA "data/faceGraph"

#define TEST_IMAGE_NAME  "BioID_0006"
#define TEST_IMAGE_FILE_NAME  "outcome/faceGraph/BioID_0006.pts"

//using namespace std;

/*测试一幅图片与人脸库中所有图片的相似度，并输出结果
*fileName1: 待测试图片
*eyeFile: 人脸库
*fileDir: 人脸库的特征值
*/
static void testAllImage(char *fileName1, char *eyeFile, char * fileDir)
{

	FILE* eyeList;
	FTYPE simila = 0.0;
	FTYPE simila1 = 0.0;
	char  filename[100];
	char line[100];
	struct finfo *llist_head, *llist_tail;
	FaceGraph graph1, graph2;
	struct finfo  *cur = NULL;
	struct finfo *n[MAXEYEFILE];
	int num = 0;

	eyeList = fopen(eyeFile, "r");
	llist_head = llist_tail = NULL;

	while (!feof(eyeList))
	{

		fgets(line, FILE_LINE_LENGTH, eyeList);

		if (feof(eyeList)) break;

		n[num] = (struct finfo*)malloc(sizeof(struct finfo));
		n[num]->next = NULL;

		if (sscanf(line, "%s %lf %lf %lf %lf", n[num]->filename, &(n[num]->source.x1), &(n[num]->source.y1), &(n[num]->source.x2), &(n[num]->source.y2)) != 5)
		{
			free(n[num]);
			exit(1);
		}
		else
		{
			if (!llist_head) {
				llist_head = llist_tail = n[num];

			}
			else
			{
				llist_tail->next = n[num];
				llist_tail = n[num];
			}
		}

		num++;
	}

	cur = llist_head;
	while (cur)
	{
		sprintf(filename, "%s/%s.pts", fileDir, cur->filename);
		graph2 = loadFaceGraph(filename);
		graph1 = loadFaceGraph(fileName1);
		simila = fgSimPhase02(graph1, graph2);

		printf("The similarity of %s and %s is %f   \n", cur->filename, fileName1, simila);

		cur = cur->next;
	}

	free(llist_head);
	free(llist_tail);
}
/*显示当前时间
*functionName： 输出标记
*/
/*void showTime(char *functionName)
{
	struct timeval startTime;
	struct tm *myTms = NULL;
	char *sntime = NULL;
	gettimeofday(&startTime, NULL);
	myTms = gmtime(&startTime.tv_sec);
	printf("%s is : %d MIN ; %d SEC %ld US\n", functionName, myTms->tm_min, myTms->tm_sec, startTime.tv_usec);
}
*/
void test_EGBM()
{
	TwoPoints source;
	int i;
	Image im1;
	Image im2;
	JetMasks masks;
	FaceGraph graph1, graph2, graph3;
	GraphDiscription  bunchGraph = NULL;
	FTYPE simila = 0.0;
	//	source=(TwoPoints *)malloc(sizeof(TwoPoints));

	source.x1 = 159;   //双眼位置
	source.y1 = 98;
	source.x2 = 231;
	source.y2 = 99;

	printf("x1:%f,y1:%f;x1:%f,y1:%f;\n", source.x1, source.y1, source.x2, source.y2);


	//showTime("StartTime");

	//	masks = readMasksFile(MASK_FILE);
	//	saveJetMasks(masks,MASK_FILE_NAME);

	masks = readJetMasks(MASK_FILE_NAME);//读取Gaborjet掩码

										 //showTime("readJetMasks ");
										 //bunchGraph=extractModelJets(EYE_FILE,PGM_DIR, PGM_DIR,masks);//建立人脸束图
										 //saveBunchGraph(BUNCH_FILE,bunchGraph);

										 //convertImages(EYE_FILE, INPUT_DIR,PGM_DIR);

	bunchGraph = readBunchGraph(BUNCH_FILE, masks);//读取人脸束图特征值

												   //showTime("readBunchGraph ");//-

												   //processAllFaceGraph(EYE_FILE,PGM_DIR,FACE_GRAPH_NORMA,masks);

	convertOneImages(source, INPUT_IMAGE, TEST_IMAGE_NAME, NORMALIZE_IMAGE);//标准化待测人脸
																			//showTime("convertOneImages ");

	locateElasticFeatures(TEST_IMAGE_NAME, bunchGraph, masks, NORMALIZE_IMAGE, NORMALIZE_IMAGE);//构建待测人脸的特征点网络

																								//showTime("locateElasticFeatures ");

	processOneFaceGraph(NORMALIZE_IMAGE, TEST_IMAGE_NAME, FACE_GRAPH, masks);//计算待测人脸特征值
																			 //multiprocessOneFaceGraph(NORMALIZE_IMAGE,TEST_IMAGE_NAME,FACE_GRAPH,masks);

																			 //	showTime("processOneFaceGraph ");

	testAllImage(TEST_IMAGE_FILE_NAME, EYE_FILE, FACE_GRAPH_NORMA);//测试与人脸库中所有图片的相似度


																   //	showTime("testAllImage ");

}

/*void unknown(int argc, char *argv[])
{
unsigned char *pbyteSrcImg = NULL;
if (argc > 1)
{
stringstream sm, sn;
FPosition fPos;
sn << argv[1];
sm << argv[1] << ".pgm";
// int nImgWidth,nImgHeight;//待测图片宽高
// pbyteSrcImg = createImgMem(FACE_DETECT_NAME);
// ReadBmpFile(sm.str().c_str(), pbyteSrcImg, nImgWidth,nImgHeight,24);
// FaceDetect(sn.str().c_str(), pbyteSrcImg,nImgWidth,nImgHeight);

Image im = readPGMImage(sm.str().c_str());
FaceGraph graph2 = loadFaceGraph("outcome/faceGraph/BioID_0006.pts");
for (int i = 0; i < 20; i++)
{
if ((int)graph2->jets[i]->x < im->width && (int)graph2->jets[i]->y < im->height)
{
fPos.x[i] = (int)graph2->jets[i]->x;
fPos.y[i] = (int)graph2->jets[i]->y;
}
else
{
fPos.x[i] = 10;
fPos.y[i] = 10;
}

cout << "x is " << fPos.x[i] << " y is " << fPos.y[i] << endl;
}
pbyteSrcImg = GetImgData(im);
cout << "so" << endl;
sobel2(pbyteSrcImg, im->width, im->height);

cout << "so" << endl;
WriteBmpFile("./bpnnimage/pos.bmp", pbyteSrcImg, im->width, im->height);
cout << "so" << endl;

//FaceDetect(sn.str().c_str(), pbyteSrcImg,im->width,im->height,fPos);
//FindAllFeature(pbyteSrcImg,im->width,im->height,fPos);


freeImage(im);
freeImgMem(pbyteSrcImg);
}
else
{
cout << "Please input image name!" << endl;
}


}
*/
void writeeye()
{
	TwoPoints source;
	source.x1 = 154;   //双眼位置
	source.y1 = 98;
	source.x2 = 236;
	source.y2 = 99;
	convertOneImages(source, "bpnnimage", "BioID_00006", "bpnnimage/out");//标准化待测人脸

	Image im = readPGMImage("bpnnimage/BioID_00006.pgm");
	unsigned char *pbyteSrcImg = GetImgData(im);
	FPosition pos;
	pos.x[0] = (int)source.x1;
	pos.y[0] = (int)source.y1;
	pos.x[1] = (int)source.x2;
	pos.y[1] = (int)source.y2;
	for (int i = 2; i< 20; i++)
	{
		pos.x[i] = (int)source.x1;
		pos.y[i] = (int)source.y1;
	}
	WriteBmpFile("./bpnnimage/eye.bmp", pbyteSrcImg, im->width, im->height);
	freeImage(im);
	freeImgMem(pbyteSrcImg);
}

inline void PgmToBmp(const char * dest, const char *src)
{
	unsigned char *pbyteSrcImg = NULL;
	Image im = readPGMImage(src);
	pbyteSrcImg = GetImgData(im);
	WriteBmpFile(dest, pbyteSrcImg, im->width, im->height);

	freeImage(im);
	freeImgMem(pbyteSrcImg);
}

void face_dect()
{
	unsigned char *pbyteSrcImg = NULL;

	stringstream sm, sn;
	FPosition fPos;
	sn << "BioID_1101";
	sm << "image/BioID_0006.pgm";

	Image im = readPGMImage(sm.str().c_str());
	pbyteSrcImg = GetImgData(im);

	//   FaceDetect(sn.str().c_str(), pbyteSrcImg,im->width,im->height,fPos);
	WriteBmpFile("/mnt/hgfs/share/PaperImage/detct.bmp", pbyteSrcImg, im->width, im->height);

	freeImage(im);
	freeImgMem(pbyteSrcImg);
}

/*void b_a_feature()
{
	int i;
	unsigned char *pbyteSrcImg = NULL;
	unsigned char *finimg = NULL;
	GraphDiscription gd;
	gd = readPosition("bpnn/bpnnimage/BioID_0000.pts");

	Image im = readPGMImage("bpnn/bpnnimage/BioID_0000.pgm");
	pbyteSrcImg = GetImgData(im);
	drawAllFeature(pbyteSrcImg, gd, im->width, im->height);
	WriteBmpFile("bpnn/outcome/feature.bmp", pbyteSrcImg, im->width, im->height);

	convertimage(gd, "bpnn/bpnnimage", "BioID_0000", "bpnn/outcome");

	GraphDiscription gd1;
	gd1 = readBinaryGraphDiscription("bpnn/outcome/BioID_0000.pts");

	Image im1 = readPGMImage("bpnn/outcome/BioID_0000.pgm");
	finimg = GetImgData(im1);
	drawAllFeature(finimg, gd1, im1->width, im1->height);
	WriteBmpFile("bpnn/outcome/fin_feature.bmp", finimg, im1->width, im1->height);

	freeGraphDiscription(gd);
	freeImage(im);
	freeImgMem(pbyteSrcImg);
	freeImgMem(finimg);
	freeImage(im1);
}*///似乎没用到

void GenerateFaceGraph(const char *inputdirname, const char *imname, const char *outputdir)
{
	int i;
	unsigned char *pbyteSrcImg = NULL;
	char  filename[100];
	GraphDiscription gd;
	JetMasks masks;

	sprintf(filename, "%s/%s.pts", inputdirname, imname);
	gd = readPosition(filename);
	//	gd = readPosition("bpnn/facedetect/detect.pts");

	//cout <<  gd->verts[0].x << "  "<<  gd->verts[0].y << "  " << gd->verts[1].x << "  " << " "  <<gd->verts[1].y << endl;
	/*   gd->verts[0].x = 165;
	gd->verts[0].y = 108;
	gd->verts[1].x = 228;
	gd->verts[1].y = 108;*/
	convertimage(gd, inputdirname, imname, outputdir);

	masks = readJetMasks(MASK_FILE_NAME);
	BPNNFaceGraph(outputdir, imname, outputdir, masks);

//	freeGraphDiscription(gd);
	freeImgMem(pbyteSrcImg);
	freeJetMasks(masks);//有 free SIZE 需要注意一下
}


void BPNN_FaceRecognition(const char *inputdirname, const char *imname, const char *outputdir)
{
	char  filename[100];
	double max = 0.0;
	int max_num = 0;
	int n = 0;

	class CBPNN m_BPNN;
	m_BPNN.LoadBPNNFile("./bpnn/bpnn_weight.txt");

	sprintf(filename, "%s/%s", outputdir, imname);
	//	sprintf(filename,"%s/%s", "bpnn/train", imname);
	CImage im = makeCImage(filename, 111);

//	double data[m_BPNN.m_nInput - 1]; //原函数 代码，报错，改为下一行
	double* data = (double*) malloc((m_BPNN.m_nInput - 1) * sizeof( double));
	
	for (int m = 0; m<m_BPNN.m_nInput - 1; m++)
	{
		data[m] = *(im->im + m);
	}
	vector<double> out;
	m_BPNN.Test(data, m_BPNN.m_nInput - 1, 1.0, out);

	for (vector<double>::iterator itr = out.begin(); itr != out.end(); itr++, n++)
	{
		cout << "value is " << *itr << endl;

		if (max < *itr)
		{
			max = *itr;
			max_num = n;
		}
	}

	double total_error = 0.0;
	for (int k = 0; k < m_BPNN.m_nOutput; k++)
	{
		if (k == max_num)total_error += 1.0 - out[k];
		else total_error += out[k];
	}

	cout << "error is  " << total_error << endl;

	if (total_error > 0.8) cout << "This guy is not in the database" << endl;
	else cout << "This guy is the " << max_num << endl;
	freeCImage(im);
}
/*未使用，注释--mabel*//*
void BPNN2_FaceRecognition(const char *inputdirname, const char *imname, const char *outputdir)
{
	char  filename[100];
	bp_net bp;						//\B6\A8\D2\E5bp\BDṹ\CC\E5

	LoadBPNNFile("bpnn/bpnn2_weight.txt", &bp);

	sprintf(filename, "%s/%s", outputdir, imname);
	CImage im = makeCImage(filename, 111);

	vector<double> out;

	Test_Bp(&bp, im);

	freeCImage(im);
}
*/

/*主函数
*/

void detect(){
	cout << "detect";
	string name;
	GraphDiscription gd;
	stringstream tmp, tmp2;
	unsigned char *pbyteSrcImg = NULL;
	cout << "Please input image name ,for example BioID_0000" << endl;
	cin >> name;
	tmp << "bpnn/facedetect/" << name;
	tmp2 << "bpnn/bpnnimage/" << name << ".pgm";

	Image im = readPGMImage(tmp2.str().c_str());
	pbyteSrcImg = GetImgData(im);
	gd = FaceDetect(tmp.str().c_str(), pbyteSrcImg, im->width, im->height);
	cout << "face detect is over and result is writed in file" << endl << endl;

	freeImgMem(pbyteSrcImg);
	freeImage(im);
	freeGraphDiscription(gd);
}

void recongize(){
	cout << "recongize";
	string name;
	cout << "Please input image name ,for example BioID_0000" << endl;
	cin >> name;
	GenerateFaceGraph("bpnn/bpnnimage", name.c_str(), "bpnn/outcome");
	BPNN_FaceRecognition("bpnn/bpnnimage", name.c_str(), "bpnn/outcome");

	cout << "face recognition is over" << endl << endl;
}
/*int main(int argc, char *argv[])
{
	int num;
	int jishu = 0;

node:    cout << "1. train " << endl
	<< "2. face detect " << endl
	<< "3. recogniiotn " << endl
	<< "4. quit " << endl;
		 cin >> num;

		 switch (num)
		 {
		 case 1:
			 Btn_train();

			 cout << "Training is over" << endl << endl;
			 break;
		 case 2:
		 {
			 detect();
			 break;
		 }
		 case 3:
		 {
			 recongize();
			 
			 break;
		 }
		 case 4:
			 exit(0);
		 default:
			 exit(0);
			 break;

		 }

		 goto node;




		 // bpnn_train();
		 //BPNN2_FaceRecognition("bpnn/bpnnimage","BioID_0020","bpnn/train");


		 // PgmToBmp("/mnt/hgfs/share/PaperImage/BioId_0050.bmp","./data/inputImage/BioID_0050.pgm");

		 // Image im = readPGMImage("outcome/inputImage/BioID_0000.pgm");
		 // pbyteSrcImg = GetImgData(im);
		 //FaceDetect("test5", pbyteSrcImg,im->width,im->height);
		 //Image im = GetImageFromData(pbyteSrcImg,nImgWidth,nImgHeight);
		 //WriteBmpFile("./test3.bmp", pbyteSrcImg, im->width,im->height);
		 //Image im = GetImageFromData(pbyteSrcImg,nImgWidth,nImgHeight);
		 //writePGMImage(im,"outcome/ert.pgm",0);

		 printf("Finish the work!\n");
		 return 0;
}*/