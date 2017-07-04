/*
* =====================================================================================
*
*    Filename:  qface.cpp
*
*    Description:
*
*       Version:  1.0
*       Created:  04/25/10 15:15:57
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  maozhi fan   1fanmaozhi@tongji.edu.cn
*        Company:  Tongji Unversity
*
* =====================================================================================
*/

#include "stdafx.h"
#include "qface.h"
#include "ExtractGaborjet.h"

#include <math.h>
#include <map>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>

#define MAXSTEP 14
#define MAXMEM 140
#define CLASS_DIR  "./data/"
#define HORRATE    0.4

typedef struct {
	int line;
	int sum;
}MaxMin;

void sobel(unsigned char *src, int width, int height, int line, TwoPoints &rrect, TwoPoints &lrect);

inline void CopyRectFeature(int x, int y, int w, int h, MyRect& rec)
{
	rec.x = x;
	rec.y = y;
	rec.height = h;
	rec.width = w;
}

void Change_f_0(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 0;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 2 * w, h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x, y, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}

void Change_f_1(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 1;
	haarfeature.count = 2;
	CopyRectFeature(x, y, w, 2 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x, y, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
void Change_f_2(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 2;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 3 * w, h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x - w, y, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 3;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
void Change_f_3(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 3;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 4 * w, h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x - w, y, 2 * w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;
}
void Change_f_4(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 4;
	haarfeature.count = 2;
	CopyRectFeature(x, y, w, 3 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x, y - h, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 3;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
void Change_f_5(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 5;
	haarfeature.count = 2;
	CopyRectFeature(x, y, w, 4 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x, y - h, w, 2 * h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
/////////////////////////////
void Change_f_6(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 6;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 3 * w, 3 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x - w, y - h, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 9;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
void Change_f_7(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 7;
	haarfeature.count = 3;
	CopyRectFeature(x, y, 2 * w, 2 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x - w, y, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(x, y - h, w, h, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 2;

}
void Change_f_8(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 8;
	haarfeature.count = 2;
	CopyRectFeature(x, y, w, 2 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x, y, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}

void Change_f_9(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 9;
	haarfeature.count = 2;
	CopyRectFeature(x, y, w, 3 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x - h, y - h, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 3;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}

void Change_f_10(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 10;
	haarfeature.count = 2;
	CopyRectFeature(x, y, w, 4 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x - h, y - h, w, 2 * h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
void Change_f_11(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 11;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 2 * w, h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x + w, y - w, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}

void Change_f_12(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 12;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 3 * w, h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x + w, y - w, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 3;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
void Change_f_13(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 13;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 4 * w, h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x + w, y - w, 2 * w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 2;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}

void Change_f_14(int x, int y, int w, int h, HaarFeature& haarfeature)
{
	haarfeature.type = 14;
	haarfeature.count = 2;
	CopyRectFeature(x, y, 3 * w, 3 * h, haarfeature.rect[0].r);
	haarfeature.rect[0].weight = -1;
	CopyRectFeature(x + w - h, y - w - h, w, h, haarfeature.rect[1].r);
	haarfeature.rect[1].weight = 9;
	CopyRectFeature(0, 0, 0, 0, haarfeature.rect[2].r);
	haarfeature.rect[2].weight = 0;

}
void change_R_Feature2HaarFeature(R_Feature r, HaarFeature& haarfeature)
{
	switch (r.i)
	{
	case 0: Change_f_0(r.x, r.y, r.w, r.h, haarfeature); break;
	case 1: Change_f_1(r.x, r.y, r.w, r.h, haarfeature); break;
	case 2:  Change_f_2(r.x, r.y, r.w, r.h, haarfeature); break;
	case 3:  Change_f_3(r.x, r.y, r.w, r.h, haarfeature); break;
	case 4:  Change_f_4(r.x, r.y, r.w, r.h, haarfeature); break;
	case 5:  Change_f_5(r.x, r.y, r.w, r.h, haarfeature); break;
	case 6:  Change_f_6(r.x, r.y, r.w, r.h, haarfeature); break;
	case 7:  Change_f_7(r.x, r.y, r.w, r.h, haarfeature); break;
	case 8:  Change_f_8(r.x, r.y, r.w, r.h, haarfeature); break;
	case 9:  Change_f_9(r.x, r.y, r.w, r.h, haarfeature); break;
	case 10:  Change_f_10(r.x, r.y, r.w, r.h, haarfeature); break;
	case 11:  Change_f_11(r.x, r.y, r.w, r.h, haarfeature); break;
	case 12:  Change_f_12(r.x, r.y, r.w, r.h, haarfeature); break;
	case 13:  Change_f_13(r.x, r.y, r.w, r.h, haarfeature); break;
	case 14:  Change_f_14(r.x, r.y, r.w, r.h, haarfeature); break;
	}
}

float LogRatio(float val)
{
	float tval;

	tval = MAX(LOGRATIO_THRESHOLD, MIN(1.0F - LOGRATIO_THRESHOLD, val));
	return logf(tval / (1.0F - tval));
}


int  Round(double val)
{
	double temp = val + 6755399441055744.0;
	return (int)*((uint64*)&temp);
}

//拷贝特征
void CopyHaarFeature(HaarFeature& src_hf, HaarFeature& dst_hf)
{
	dst_hf.type = src_hf.type;
	for (int i = 0; i < HAAR_FEATURE_MAX; i++)
	{
		dst_hf.rect[i].weight = src_hf.rect[i].weight;
		dst_hf.rect[i].r.x = src_hf.rect[i].r.x;
		dst_hf.rect[i].r.y = src_hf.rect[i].r.y;
		dst_hf.rect[i].r.width = src_hf.rect[i].r.width;
		dst_hf.rect[i].r.height = src_hf.rect[i].r.height;
	}

}

int longtochar(long number, char * string, int length1)
{
	if (length1 <= 4)
	{
		memcpy(string, &number, length1);
	}

	return 0;
}

long chartolong(char * string, int length1)
{
	long number;
	if (length1 <= 4)
	{
		memset(&number, 0x00, sizeof(long));
		memcpy(&number, string, length1);
	}
	return(number);
}

unsigned char* createImgMem(const char * pszFileName)
{
	BITMAPFILEHEADER_ bf;
	BITMAPINFOHEADER_ bi;
	int fd;
	long ImgSize = 0;
	long nread = 0;
	int readnum = sizeof(BITMAPFILEHEADER_);
	long count2 = 0;
	long imageSize;
	unsigned char *lpImgData = NULL;
	int i, j, l;
	int byteBitCount;
	int LineBytes = 0;
	int nImgWidth, nImgHeight;
	unsigned char * pbyteSrcImg = NULL;

//	fd = open(pszFileName, O_RDWR);//linux 代码，O_RDWR 读、写打开。
//	ofstream pszfile(pszFileName);
	FILE *pszFile;
	fstream pszfile(pszFileName, ios::in);
	printf("filename:%s", pszFileName);
	pszFile = fopen(pszFileName, "r");
//	if (fd == -1)
	if(!pszfile.is_open())
	{
		printf("The file is not opened!\n");
	//	close(fd);
		pszfile.close();
		return NULL;
	}

	//nread=pread(fd,&bf,sizeof(BITMAPFILEHEADER),0);
	nread = fread(&bf,1, sizeof(BITMAPFILEHEADER_),pszFile);
	if (nread != sizeof(BITMAPFILEHEADER_))
	{
		printf("read file header error!\n");
		//close(fd);
		pszfile.close();
		return NULL;
	}

//	nread = pread(fd, &bi, sizeof(BITMAPINFOHEADER), sizeof(BITMAPFILEHEADER_));
	nread = fread(&bi, 1, sizeof(BITMAPINFOHEADER_),pszFile);
	if (nread != sizeof(BITMAPINFOHEADER_))
	{
		printf("read info header error!\n");
	//	close(fd);
		pszfile.close();
		return NULL;
	}

	nImgWidth = (int)chartolong(bi.biWidth, 4);
	nImgHeight = (int)chartolong(bi.biHeight, 4);
	ImgSize = chartolong(bf.bfSize, 4) - sizeof(BITMAPFILEHEADER_) - sizeof(BITMAPINFOHEADER_);
	imageSize = nImgWidth*nImgHeight;


	pbyteSrcImg = (unsigned char *)malloc(imageSize);
	if (pbyteSrcImg == NULL)printf("\n malloce failed\n");

	return pbyteSrcImg;
}

unsigned char* GetImgData(Image im)
{
	int i, j;
	unsigned char * pbyteSrcImg = NULL;
	int imageSize = 0;

	cout << "Getimgdata.\n";

	imageSize = im->width*im->height;
	pbyteSrcImg = (unsigned char *)malloc(imageSize);
	if (pbyteSrcImg == NULL)printf("\n malloce failed\n");

	for (i = 0; i<im->height; i++)
		for (j = 0; j<im->width; j++)
		{
			pbyteSrcImg[i*im->width + j] = (unsigned char)IE(im, j, i, 0);
		}

	return pbyteSrcImg;
}

Image GetImageFromData(unsigned char *pbyteSrcImg, int width, int height)
{
	int x, y, i, j;
	Image im = (Image)malloc(sizeof(image));
	assert(im);
	im->data = (double***)malloc(sizeof(double**)*width);
	assert(im->data);

	for (x = 0; x < width; x++) {
		im->data[x] = (double**)malloc(sizeof(double*)*height);
		assert(im->data[x]);
		for (y = 0; y < height; y++) {
			im->data[x][y] = (double*)malloc(sizeof(double));
			assert(im->data[x][y]);
		}
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			IE(im, j, i, 0) = pbyteSrcImg[i*width + j];
		}
	}

	im->width = width;
	im->height = height;
	im->channels = 1;

	return im;
}

void freeImgMem(unsigned char *pbyteSrcImg)
{
	free(pbyteSrcImg);
}

void Thresholding(unsigned char *src, int width, int height)
{
	unsigned char  pmax, pmin;
	int x, y;
	pmax = *src;
	pmin = *src;

	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			pmax = MAX(*(src + x + y*width), pmax);
			pmin = MIN(*(src + x + y*width), pmin);
		}
	}

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (pmax - pmin <= 0) {
				*(src + y*width + x) = 0;
			}
			else {
				int tmp = ROUND(255 * (*(src + y*width + x) - pmin) / (pmax - pmin));
				if (tmp < 0) tmp = 0;
				if (tmp > 255) tmp = 255;
				*(src + y*width + x) = (unsigned char)tmp;
			}
		}
	}
}


/*
Description:读取bmp文件中数据
return :图片数据，长度和高度。

*/
int ReadBmpFile(const char* pszFileName, unsigned char *pbyteSrcImg, int& nImgWidth, int& nImgHeight, int NumColors)
{
	BITMAPFILEHEADER_ bf;
	BITMAPINFOHEADER_ bi;
	int fd;
	long ImgSize = 0;
	long nread = 0;
	int readnum = sizeof(BITMAPFILEHEADER_);
	long count2 = 0;
	unsigned char ff;
	long imageSize;
	unsigned char *lpImgData = NULL;
	int i, j, l;
	int byteBitCount;
	int LineBytes = 0;
//	fd = open(pszFileName, O_RDWR);
	fstream pszfile(pszFileName,ios::in);
	FILE *pszFile;
	pszFile = fopen(pszFileName, "r");
	
//	if (fd == -1)
	if (!pszfile.is_open())
	{
		printf("The file is not opened!\n");
	//	close(fd);
		pszfile.close();
		return -1;
	}

//	nread = pread(fd, &bf, sizeof(BITMAPFILEHEADER), 0);
	nread = fread(&bf, 1,sizeof(BITMAPFILEHEADER_),pszFile);

	cout << nread << "\n";
	cout << sizeof(BITMAPFILEHEADER_) << "\n";
	cout << sizeof(BITMAPFILEHEADER) << "HEHE";
	if (nread != sizeof(BITMAPFILEHEADER_))
	{
		printf("read file header error!\n");
//		close(fd);
		pszfile.close();
		return -1;
	}

//	nread = pread(fd, &bi, sizeof(BITMAPINFOHEADER), sizeof(BITMAPFILEHEADER));
	nread = fread(&bi, 1,sizeof(BITMAPINFOHEADER_),pszFile);
	cout << nread;
	cout << sizeof(BITMAPINFOHEADER_);
	if (nread != sizeof(BITMAPINFOHEADER_))
	{
		printf("read info header error!\n");
//		close(fd);
		pszfile.close();
		return -1;
	}

	nImgWidth = (int)chartolong(bi.biWidth, 4);
	nImgHeight = (int)chartolong(bi.biHeight, 4);
	ImgSize = chartolong(bf.bfSize, 4) - sizeof(BITMAPFILEHEADER_) - sizeof(BITMAPINFOHEADER_);
	lpImgData = (unsigned char *)malloc(ImgSize);
	if (lpImgData == NULL)printf("\n malloce failed\n");

//	pread(fd, lpImgData, ImgSize, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
	fread( lpImgData, ImgSize, sizeof(BITMAPFILEHEADER_) + sizeof(BITMAPINFOHEADER_),pszFile);

	//change buffer into normal

	byteBitCount = (int)chartolong(bi.biBitCount, 4) / 8;
	LineBytes = (int)WIDTHBYTES(nImgWidth*byteBitCount * 8);
	RGBQUAD** m_tOriPixelArray;//原始位图数据
	RGBQUAD** m_tResPixelArray;//参考位图数据

	m_tOriPixelArray = new RGBQUAD*[nImgHeight];
	m_tResPixelArray = new RGBQUAD*[nImgHeight];

	for (l = 0; l<nImgHeight; l++)
	{
		m_tOriPixelArray[l] = new RGBQUAD[nImgWidth];
		m_tResPixelArray[l] = new RGBQUAD[nImgWidth];
	}

	count2 = 0;

	//将倒序转换成正常顺序
	for (i = nImgHeight - 1; i >= 0; i--)
	{
		for (j = 0; j<nImgWidth; j++)
		{
			m_tOriPixelArray[i][j].rgbBlue = lpImgData[count2++];
			m_tOriPixelArray[i][j].rgbGreen = lpImgData[count2++];
			m_tOriPixelArray[i][j].rgbRed = lpImgData[count2++];
			m_tOriPixelArray[i][j].rgbReserved = 0;
			m_tResPixelArray[i][j] = m_tOriPixelArray[i][j];

			count2 += byteBitCount - 3;
		}
		count2 += (4 - (nImgWidth*byteBitCount) % 4) % 4;
	}

	//convert RGB to gray
	
	for (i = 0; i<nImgHeight; i++)
		for (j = 0; j<nImgWidth; j++)
		{
			ff = (3 * m_tOriPixelArray[i][j].rgbRed + 5.9*m_tOriPixelArray[i][j].rgbGreen + 1.1*m_tOriPixelArray[i][j].rgbBlue) / 10;
			cout << ff;
			pbyteSrcImg[0] = (unsigned char)ff;
			cout << pbyteSrcImg[0];
			pbyteSrcImg[i*nImgWidth + j + 0] = ff;
			pbyteSrcImg[i*nImgWidth + j + 1] = ff;
			pbyteSrcImg[i*nImgWidth + j + 2] = ff;

		}

	Thresholding(pbyteSrcImg, nImgWidth, nImgHeight);
//	close(fd);
	pszfile.close();

	free(lpImgData);
}

int writeBMPHeader(BITMAPFILEHEADER_ &bf, BITMAPINFOHEADER_ &bi, int nWidth, int nHeight)
{
	bf.bfType[0] = 'B';
	bf.bfType[1] = 'M';

	long number = ((nWidth * 3 + 3) / 4) * 4 * nHeight + sizeof(bi) + sizeof(bf) ;
	longtochar(number, bf.bfSize, 4);
	number = sizeof(bf) + sizeof(bi);
	longtochar(number, bf.bfoffBits, 4);
	longtochar(0, bf.bfReserved, 4);

	longtochar(nWidth, bi.biWidth, 4);
	longtochar(nHeight, bi.biHeight, 4);
	longtochar(0, bi.biPlanes, 4);
	longtochar(24, bi.biBitCount, 2);
	longtochar(3780, bi.biXPelsPerMeter, 4);
	longtochar(3780, bi.biYPelsPerMeter, 4);

	longtochar(40, bi.biSize, 4);
	longtochar(0, bi.biCompress, 4);
	longtochar(0, bi.biSizeImage, 4);
	longtochar(0, bi.biClrUsed, 4);
	longtochar(0, bi.biClrImportant, 4);
	
	return 0;
}

int  WriteBmpFile(const char* pszFileName, unsigned char* pbyteSrcImg, int nWidth, int nHeight)
{
	BITMAPFILEHEADER_ bf;
	BITMAPINFOHEADER_ bi;
	int i, j;
	long ImgSize = 0;
	int  nLineBytes = ((nWidth * 3 + 3) / 4) * 4;
	long imageSize = nLineBytes*nHeight;
	unsigned char *saveImage = new unsigned char[imageSize + sizeof(bf) + sizeof(bi)];
//	int fd1;
//	fstream file2;
	FILE * File2;
	File2 = fopen(pszFileName, "wb+");
	long count1;
	int nExtraNum = 0;

	writeBMPHeader(bf, bi, nWidth, nHeight);
	memcpy(saveImage, &bf, sizeof(bf));
	memcpy(saveImage + sizeof(bf), &bi, sizeof(bi));

	count1 = 0;
	nExtraNum = (4 - (nWidth * 3) % 4) % 4;
	//将正常顺序的数据转换成bmp图片需求的倒序	  
	for (i = 0; i<nHeight; i++)
	{
		for (j = 0; j<nWidth; j++)
		{
			saveImage[54 + count1++] = pbyteSrcImg[(nHeight - i - 1)*nWidth + j];
			saveImage[54 + count1++] = pbyteSrcImg[(nHeight - i - 1)*nWidth + j];
			saveImage[54 + count1++] = pbyteSrcImg[(nHeight - i - 1)*nWidth + j];
		}

		for (int k = 0; k < nExtraNum; k++)
		{
			saveImage[54 + count1++] = 0;
		}
	}

//	fd1 = open(pszFileName, O_RDWR | O_CREAT, O_RDWR);
//	file2.open(pszFileName);

/*	if (chmod(pszFileName, S_IRWXO | S_IRWXG | S_IRWXU) < 0)//要求所有人都有读写权
	{
		printf("Chmod ERROR");
	}
	*///感觉可能windows下不需要这个，先改了吧

	//if (fd1 == -1)fd1 = open(pszFileName, O_RDWR);

//	if (pwrite(fd1, saveImage, imageSize + 54, 0) == -1)printf("Write 1 ERROR");

	if (fwrite(saveImage,1,imageSize+54,File2) == -1)
		/*fwrite()函数从ptr指向的缓冲区中取出长度为size字节的nitems个数据项，
		写入到流指针stream指向的文件中，执行该操作后，文件指针将向后移动，
		移动的字节数等于写入文件的字节数目。该函数操作完成后，也将返回写入的数据项数。  */
  
	{
		printf("Write 1 error");
	}
	//close(fd1);
	//file2.close();
	fclose(File2);
	delete[]saveImage;

	return 0;
}

bool IntegralImage(unsigned char* pbyteSrcImg, int SAT[], int RSAT[], double SSAT[], int W, int H)
{
	assert(pbyteSrcImg != NULL);
	unsigned char* pixel = NULL;
	unsigned char* p_orignal = NULL;
	int j;


	pixel = pbyteSrcImg;
	//p_orignal=pixel;	    	
	for (j = 0; j<W*H; j++)
	{
		RSAT[j] = *pixel;
		SAT[j] = *(pixel++);
		// RSAT[j]=1;
		//   SAT[j]=1;
		SSAT[j] = (SAT[j] * SAT[j]);//存平方象素值			 
	}
	//	 delete []p_orignal;
	///////////////////////////////////////////////////////////////////////////									
	int  **const A = new int*[H];
	int temp = 0;
	int m, n, m0, n0, i;
	int a, b, c;   //a is A[m][n-1],b is A[m-1][n],c is A[m-1][n-1] 
	for (i = 0; i<H; i++)
	{
		A[i] = new int[W];
	}

	A[0][0] = SAT[0];

	for (m = 0; m<H; m++)
		for (n = 0; n<W; n++)
		{
			if (n == 0) a = 0;
			else a = A[m][n - 1];
			if (m == 0) b = 0;
			else b = A[m - 1][n];
			if ((n == 0) || (m == 0))
				c = 0;
			else c = A[m - 1][n - 1];
			A[m][n] = a + b + SAT[m*W + n] - c;//A[m][n]=A[m][n-1]+A[m-1][n]+x[m][n]-A[m-1][n-1]
											   //	cout<<A[m][n]<<" ";
		}
	temp = 0;

	for (m0 = 0; m0<H; m0++)
		for (n0 = 0; n0<W; n0++)
		{
			SAT[temp] = A[m0][n0];
			temp = temp + 1;
		}

	////////////////////////////以下计算SSAT
	double x1, x2, x3;
	double **const B = new double*[H];

	for (i = 0; i<H; i++)
	{
		B[i] = new double[W];
	}

	B[0][0] = SSAT[0];

	for (m = 0; m<H; m++)
		for (n = 0; n<W; n++)
		{
			if (n == 0) x1 = 0.0F;
			else x1 = B[m][n - 1];
			if (m == 0) x2 = 0.0F;
			else x2 = B[m - 1][n];
			if ((n == 0) || (m == 0))
				x3 = 0.0F;
			else x3 = B[m - 1][n - 1];
			B[m][n] = x1 + x2 + SSAT[m*W + n] - x3;//A[m][n]=A[m][n-1]+A[m-1][n]+x[m][n]-A[m-1][n-1]

		}

	temp = 0;
	for (m0 = 0; m0<H; m0++)
		for (n0 = 0; n0<W; n0++)
		{
			SSAT[temp] = B[m0][n0];

			temp = temp + 1;
		}
	// cout<<"*********** "<<Big_SSAT[i][W*H-1]<<endl;

	///////////////////////////////////////////
	////初始化，给第一行用象素值，第一例用，和最后一例自己先计算好

	int d, e;   //a: RSAT(x-1,y-1) b:RSAT(x+1,y-1) c:RSAT(x,y-2)
				// d: I(X,Y) e:I(x,y-1)
	for (int k = 0; k<W; k++) A[0][k] = RSAT[k];

	for (j = 0; j<H; j++)
	{
		int temp1 = 0, temp2 = 0;
		for (int h = j; h >= 0; h--)
			for (int count = 0; count <= j - h&&count<W; count++)
			{
				temp1 += RSAT[h*W + count];
				temp2 += RSAT[(h + 1)*W - count - 1];
			}

		A[j][0] = temp1; A[j][W - 1] = temp2;
	}

	for (int y = 1; y<H; y++)
		for (int x = 1; x<W - 1; x++)
		{
			b = A[y - 1][x + 1];
			a = A[y - 1][x - 1];
			d = RSAT[y*W + x];
			e = RSAT[(y - 1)*W + x];
			if (y == 1) c = 0;
			else c = A[y - 2][x];
			A[y][x] = a + b - c + d + e;
		}
	temp = 0;
	for (m0 = 0; m0<H; m0++)
	{
		for (n0 = 0; n0<W; n0++)
		{
			RSAT[temp] = A[m0][n0];
			temp = temp + 1;
		}
	}

	for (i = 0; i<H; i++)
	{
		delete[]B[i];
		delete[]A[i];
	}
	delete[]B;
	delete[]A;
	return true;

}



bool Rgb2Gray(BYTE* pbyteSrcImg, BYTE* pbyteDstImg, int nwidth, int nheight)
{
	if (pbyteSrcImg == NULL || pbyteDstImg == NULL)
		return FALSE;

	int nRowBytes = nwidth * 3;
	BYTE blue = 0, red = 0, green = 0;
	BYTE gray;

	int i = 0, j = 0, pos = 0;
	for (j = 0; j < nheight; j++)
		for (i = 0; i < nwidth; i++)
		{
			pos = (nheight - 1 - j) * nRowBytes + i * 3;
			blue = pbyteSrcImg[pos];
			green = pbyteSrcImg[pos + 1];
			red = pbyteSrcImg[pos + 2];
			gray = (30 * red + 59 * green + 11 * blue) / 100;
			pbyteDstImg[j*nwidth + i] = gray;
		}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////////////////

bool LoadStageClassifier(char* filename, StageClassifier* stageClassifier)
{
	if (NULL == filename || NULL == stageClassifier)
		return FALSE;



	FILE* file = NULL;

	file = fopen(filename, "r");
	if (!file)
		return FALSE;

	int count = 0;
	float threshold = 0.0F;
	fscanf(file, "%f", &threshold);
	fscanf(file, "%d", &count);
	if (count > 0)
	{
		stageClassifier->count = count;
		stageClassifier->threshold = threshold;
		stageClassifier->classifier = new HaarClassifier[count];

		for (int i = 0; i < stageClassifier->count; i++)
		{
			HaarClassifier* classifier = stageClassifier->classifier + i;
			//指针偏移
			R_Feature r_feature;
			fscanf(file, "%d %d %d %d %d", &r_feature.i,
				&r_feature.x, &r_feature.y,
				&r_feature.w, &r_feature.h);
			change_R_Feature2HaarFeature(r_feature, classifier->haarfeature);
			fscanf(file, "%f %d %f",
				&(classifier->threshold),
				&(classifier->parity),
				&(classifier->alpha));

		}/* i */
	}

	fclose(file);
	file = NULL;
	return TRUE;
}


ClassifierCascade* LoadClassifierCascade(char* directory, int maxstep, MySize origWindowSize)
{
	if (maxstep <= 0 || origWindowSize.width <= 0 || origWindowSize.height <= 0)
	{
		return NULL;
	}

	ClassifierCascade* cascade;

	if (directory && *directory)
	{
		cascade = new ClassifierCascade;
		if (NULL == cascade)
			return NULL;

		cascade->count = maxstep;        ///层数
		cascade->origWindowSize.width = origWindowSize.width;
		cascade->origWindowSize.height = origWindowSize.height;

		//	cascade->stageClassifier = NULL;
		cascade->stageClassifier = new StageClassifier[maxstep];//指向层分类器的数组
		if (NULL == cascade->stageClassifier)
		{
			delete cascade;
			cascade = NULL;
			return NULL;
		}
		memset(cascade->stageClassifier, 0, sizeof(StageClassifier)*maxstep);

		char strFileName[50];
		//string suffix = _T("");
		for (int i = 1; i <= maxstep; i++)
		{

			sprintf(strFileName, "%sStage%d.txt", directory, i);
			LoadStageClassifier(strFileName, &(cascade->stageClassifier[i - 1]));
		}
	}//end for if

	return cascade;
}



/************************************************************************
* 由ClassifierCascade创建一个DetCascadeClassifier
* 后面三个参数似乎没用？
************************************************************************/


DetCascadeClassifier* CreateDetCascadeClassifier(ClassifierCascade* cascade, int nImgWidth, int nImgHeight, double scale)
{
	assert(NULL != cascade);

	DetCascadeClassifier* out = NULL;
	out = new DetCascadeClassifier;
	assert(out != NULL);

	out->count = cascade->count;
	out->origWindowSize.width = cascade->origWindowSize.width;
	out->origWindowSize.height = cascade->origWindowSize.height;

	out->stageClassifier = NULL;
	out->stageClassifier = new DetStageClassifier[out->count];
	assert(NULL != out->stageClassifier);

	int count = 0;
	int i, j;
	for (i = 0; i < cascade->count; i++)
	{
		count = cascade->stageClassifier[i].count;
		out->stageClassifier[i].count = count;
		out->stageClassifier[i].threshold = cascade->stageClassifier[i].threshold;
		out->stageClassifier[i].classifier = new DetHaarClassifier[count];
		assert(NULL != out->stageClassifier[i].classifier);

		for (j = 0; j < count; j++)
		{
			out->stageClassifier[i].classifier[j].alpha = cascade->stageClassifier[i].classifier[j].alpha;
			out->stageClassifier[i].classifier[j].parity = cascade->stageClassifier[i].classifier[j].parity;
			out->stageClassifier[i].classifier[j].threshold = cascade->stageClassifier[i].classifier[j].threshold;
			CopyHaarFeature(cascade->stageClassifier[i].classifier[j].haarfeature, out->stageClassifier[i].classifier[j].origFeature);
		}
	}



	return out;

}



/************************************************************************
nImgWidth:待检图像宽度
nImgHeight:待检图像高度
SAT：存贮积分图像值的数组
scale: 检测窗口扩大倍数

************************************************************************/
void SetImgForDetCascadeClassifier(DetCascadeClassifier* cascade,
	int* SAT, int* RSAT, double SSAT[],
	int nImgWidth, int nImgHeight,
	double scale)
{
	assert(NULL != cascade);
	assert(NULL != SAT);
	assert(NULL != SSAT);
	assert(NULL != RSAT);
	assert(nImgWidth > 0 && nImgHeight > 0);
	assert(scale >= 1.0F);

	cascade->scale = scale;
	cascade->realWindowSize.width = Round(cascade->origWindowSize.width * scale);
	cascade->realWindowSize.height = Round(cascade->origWindowSize.height * scale);
	cascade->imgSize.width = nImgWidth;
	cascade->imgSize.height = nImgHeight;
	//	cascade->weight_scale=float(scale*scale);
	double weight_scale;
	weight_scale = 1 / (scale * scale);
	int step = cascade->imgSize.width;
	MyRect equ_rect;
	equ_rect.x = 0;
	equ_rect.y = 0;
	equ_rect.width = Round(cascade->origWindowSize.width * scale);
	equ_rect.height = Round(cascade->origWindowSize.height * scale);

	/*invWindowArea----当前检测窗口面积的倒数*/
	cascade->invWindowArea = 1.0 / (equ_rect.width * equ_rect.height);

	//p0：特征左上角积分图像值  p3:特征右下角积分图像值...lyh
	cascade->p0 = SAT + equ_rect.y*step + equ_rect.x;
	cascade->p1 = SAT + equ_rect.y*step + equ_rect.x + equ_rect.width;
	cascade->p2 = SAT + (equ_rect.y + equ_rect.height)*step + equ_rect.x;
	cascade->p3 = SAT + (equ_rect.y + equ_rect.height)*step + equ_rect.x + equ_rect.width;

	//用平方积分图像来计算ps0到ps3...lyh
	cascade->ps0 = SSAT + equ_rect.y*step + equ_rect.x;
	cascade->ps1 = SSAT + equ_rect.y*step + equ_rect.x + equ_rect.width;
	cascade->ps2 = SSAT + (equ_rect.y + equ_rect.height)*step + equ_rect.x;
	cascade->ps3 = SSAT + (equ_rect.y + equ_rect.height)*step + equ_rect.x + equ_rect.width;

	int i, j, k;
	for (i = 0; i < cascade->count; i++)
	{
		for (j = 0; j < cascade->stageClassifier[i].count; j++)
		{
			HaarFeature* origFeature =
				&(cascade->stageClassifier[i].classifier[j].origFeature);

			DetHaarFeature* detFeature =
				&(cascade->stageClassifier[i].classifier[j].feature);

			double sum0 = 0.0F, area0 = 0.0F;
			MyRect tr;
			if ((*origFeature).type<8)   //如果特征不是倾斜的...lyh
			{


				for (k = 0; k < HAAR_FEATURE_MAX; k++)
				{
					if (origFeature->rect[k].r.width == 0)
					{
						detFeature->rect[k].weight = 0;
						detFeature->rect[k].p0 = NULL;
						detFeature->rect[k].p1 = NULL;
						detFeature->rect[k].p2 = NULL;
						detFeature->rect[k].p3 = NULL;
						break;
					}

					// 矩形特征扩大以后...计算特征值...lyh
					tr.x = Round(origFeature->rect[k].r.x * scale);
					tr.y = Round(origFeature->rect[k].r.y * scale);
					tr.width = Round(origFeature->rect[k].r.width * scale);
					tr.height = Round(origFeature->rect[k].r.height * scale);

					/*  (x,y) 成了右下角坐标? ... lyh */
					detFeature->rect[k].p0 = SAT + tr.y * step + tr.x;
					detFeature->rect[k].p1 = SAT + (tr.y - tr.height)* step + tr.x;
					detFeature->rect[k].p2 = SAT + tr.y * step + tr.x - tr.width;
					detFeature->rect[k].p3 = SAT + (tr.y - tr.height) * step + tr.x - tr.width;
					detFeature->rect[k].weight = (float)(origFeature->rect[k].weight * weight_scale); /* weight_scale=1/(scale*scale) ...lyh*/

																									  //似乎是为了保证所有area[i]*weight[i]加起来之和为0...lyh
					if (k == 0)
						area0 = tr.width * tr.height;
					else
						sum0 += detFeature->rect[k].weight * tr.width * tr.height;
				}/* k */
			}/*if*/
			else   //如果特征是倾斜的...lyh
			{
				assert((*origFeature).type>7);
				for (k = 0; k < HAAR_FEATURE_MAX; k++)
				{
					if (origFeature->rect[k].r.width == 0)
					{
						detFeature->rect[k].weight = 0;
						detFeature->rect[k].p1 = NULL;
						detFeature->rect[k].p2 = NULL;
						detFeature->rect[k].p3 = NULL;
						break;
					}

					tr.x = Round(origFeature->rect[k].r.x * scale);
					tr.width = Round(origFeature->rect[k].r.width * scale);
					tr.y = Round(origFeature->rect[k].r.y * scale);
					tr.height = Round(origFeature->rect[k].r.height * scale);

					detFeature->rect[k].p0 = RSAT + tr.y * step + tr.x;
					detFeature->rect[k].p1 = RSAT + (tr.y - tr.width) * step + tr.x + tr.width;
					detFeature->rect[k].p2 = RSAT + (tr.y - tr.height) * step + tr.x - tr.height;
					detFeature->rect[k].p3 = RSAT + (tr.y - tr.width - tr.height) * step + tr.x + tr.width - tr.height;
					detFeature->rect[k].weight = (float)(origFeature->rect[k].weight * weight_scale);

					if (k == 0)
						area0 = tr.width * tr.height;
					else
						sum0 += detFeature->rect[k].weight * tr.width * tr.height;

				}

			}/* if */
			detFeature->rect[0].weight = (float)(-sum0 / area0);
		}/* j */
	}/* i */
}

int RunDetCascadeClassifier(DetCascadeClassifier* cascade, POS pt)
{
	assert(NULL != cascade);

	int sumImgWidth = cascade->imgSize.width;
	int sumImgHeight = cascade->imgSize.height;
	if (pt.x < 0 || pt.y < 0 ||
		pt.x + cascade->realWindowSize.width >= sumImgWidth - 2 ||
		pt.y + cascade->realWindowSize.height >= sumImgHeight - 2)
	{
		return 0;
	}

	double mean, norm_factor;
	int p_offset = pt.y * sumImgWidth + pt.x;
	int pq_offset = pt.y * sumImgWidth + pt.x;
	mean = (cascade->p0[p_offset] - cascade->p1[p_offset] -
		cascade->p2[p_offset] + cascade->p3[p_offset]) * cascade->invWindowArea;
	norm_factor = cascade->ps0[pq_offset] - cascade->ps1[pq_offset] -
		cascade->ps2[pq_offset] + cascade->ps3[pq_offset];
	norm_factor = sqrt(norm_factor*cascade->invWindowArea - mean*mean);
	//	if(norm_factor == 0.0 || mean == 0.0)
	//	return -1;

	float stage_sum = 0.0F;
	int i, j;
	for (i = 0; i < cascade->count; i++)
	{
		stage_sum = 0.0F;

		for (j = 0; j < cascade->stageClassifier[i].count; j++)
		{
			DetHaarClassifier* classifier = cascade->stageClassifier[i].classifier + j;
			DetHaarFeature* feature = &(classifier->feature);
			double sum = 0.0F;
			float thresh = classifier->threshold * norm_factor;

			sum += (feature->rect[0].p0[p_offset] -
				feature->rect[0].p1[p_offset] -
				feature->rect[0].p2[p_offset] +
				feature->rect[0].p3[p_offset])*
				feature->rect[0].weight;

			sum += (feature->rect[1].p0[p_offset] -
				feature->rect[1].p1[p_offset] -
				feature->rect[1].p2[p_offset] +
				feature->rect[1].p3[p_offset])*
				feature->rect[1].weight;

			if (feature->rect[2].weight)
				sum += (feature->rect[2].p0[p_offset] -
					feature->rect[2].p1[p_offset] -
					feature->rect[2].p2[p_offset] +
					feature->rect[2].p3[p_offset])*
				feature->rect[2].weight;

			//  assert(sum==0);
			if (0.5*sum * classifier->parity < thresh * classifier->parity)
				stage_sum += classifier->alpha;

		}

		if (stage_sum < cascade->stageClassifier[i].threshold)
		{
			return -1;
		}
	}/* i */

	return 1;
}

int is_equal(const void* _r1, const void* _r2)
{
	const MyRect* r1 = (const MyRect*)_r1;
	const MyRect* r2 = (const MyRect*)_r2;

	int distance = Round(r1->width*0.2);

	if (r2->x <= r1->x + distance &&
		r2->x >= r1->x - distance &&
		r2->y <= r1->y + distance &&
		r2->y >= r1->y - distance &&
		r2->width <= Round(r1->width * 1.2) &&
		Round(r2->width * 1.2) >= r1->width)
	{
		return 1;
	}

	return 0;
}



int RectsPartition(vector<MyRect>* vecFaces, vector<int>* labels)
{
	assert(vecFaces && labels);

	int class_idx = 0;
	vector<CvPTreeNode> *nodes = 0;
	nodes = new vector<CvPTreeNode>;
	nodes->clear();  //nodes 是容器指针，该容器里存放的是CvPTreeNode

	int i, j;

	// Initial O(N) pass. Make a forest of single-vertex trees.
	for (i = 0; i < vecFaces->size(); i++)
	{
		CvPTreeNode node = { 0, 0, 0 };
		node.element = &(vecFaces->at(i));
		nodes->push_back(node);//nodes 中存放的CvPTreeNode的element指向vecfaces中的RECT
	}

	// The main O(N^2) pass. Merge connected components.
	for (i = 0; i < nodes->size(); i++)
	{
		CvPTreeNode* node1 = &nodes->at(i);
		CvPTreeNode* root = node1;

		while (root->parent)
			root = root->parent;

		for (j = 0; j < nodes->size(); j++)
		{
			CvPTreeNode* node2 = &nodes->at(j);

			if (node2->element && node2 != node1 && (is_equal(node1->element, node2->element) == 1))
			{
				CvPTreeNode* root2 = node2;

				// unite both trees
				while (root2->parent)
					root2 = root2->parent;

				if (root2 != root)
				{
					if (root->rank > root2->rank)
						root2->parent = root;
					else
					{
						root->parent = root2;
						root2->rank += root->rank == root2->rank;
						root = root2;
					}
					assert(root->parent == 0);

					// compress path from node2 to the root
					while (node2->parent)
					{
						CvPTreeNode* temp = node2;
						node2 = node2->parent;
						temp->parent = root;
					}

					// compress path from node to the root
					node2 = node1;
					while (node2->parent)
					{
						CvPTreeNode* temp = node2;
						node2 = node2->parent;
						temp->parent = root;
					}
				}
			}
		}// end for j
	}// end for i

	labels->clear();
	for (i = 0; i < nodes->size(); i++)
	{
		CvPTreeNode* node = &nodes->at(i);
		int idx = -1;

		if (node->element)
		{
			while (node->parent)
				node = node->parent;
			if (node->rank >= 0)
				node->rank = ~class_idx++;
			idx = ~node->rank;
		}
		labels->push_back(idx);
	}

	nodes->clear();
	delete nodes;

	return class_idx;
}

void HaarDetectFaces(BYTE* pbyteSrcImg,
	int iw, int ih,
	MyRect detRegion,
	DetCascadeClassifier* cascade,
	vector<MyRect>* vecFaces,
	MySize min_size,
	MySize max_size,
	int min_neighbors,
	double scale_factor)
{
	assert(pbyteSrcImg != NULL);
	assert(NULL != cascade && NULL != vecFaces);
	assert(detRegion.x >= 0 && detRegion.y >= 0 &&
		detRegion.width <= iw && detRegion.height <= ih);
	assert(min_size.width > 0 && min_size.height > 0);
	assert(max_size.width <= detRegion.width&& max_size.width <= detRegion.height);
	assert(max_size.height <= detRegion.width&&max_size.height <= detRegion.height);


	cascade->imgSize.width = iw;
	cascade->imgSize.height = ih;
	int size = iw * ih;
	int *SAT = NULL;
	int *RSAT = NULL;
	double *SSAT = NULL;
	SAT = new int[size];
	RSAT = new int[size];
	SSAT = new double[size];
	IntegralImage(pbyteSrcImg, SAT, RSAT, SSAT, iw, ih);

	vector<MyRect> origFaces, tempFaces;
	origFaces.clear();
	tempFaces.clear();
	vecFaces->clear();

	double factor = 1.0;
	int iMaxSize = MIN(max_size.width, max_size.height);
	for (factor = 1; factor*cascade->origWindowSize.width < iMaxSize; factor *= scale_factor)
	{
		const double ystep = MAX(2, factor);
		MySize win_size = { Round(cascade->origWindowSize.width * factor),
			Round(cascade->origWindowSize.height * factor) };
		int stop_height = Round((detRegion.height - win_size.height) / ystep);

		if (win_size.width < min_size.width || win_size.height < min_size.height)
			continue;

		SetImgForDetCascadeClassifier(cascade, SAT, RSAT, SSAT, iw, ih, factor);

		for (int _iy = 1; _iy < stop_height; _iy++)
		{
			int iy = detRegion.y + Round(_iy*ystep);
			int _xstep = 1;
			int stop_width = Round((detRegion.width - win_size.width) / ystep);
			for (int _ix = 0; _ix < stop_width; _ix += _xstep)
			{
				int ix = detRegion.x + Round(_ix*ystep);
				POS pt;
				pt.x = ix;
				pt.y = iy;
				int result;

				result = RunDetCascadeClassifier(cascade, pt);
				if (result > 0)
				{
					MyRect rect;
					rect.x = pt.x;
					rect.y = pt.y;
					rect.width = win_size.width;
					rect.height = win_size.height;
					origFaces.push_back(rect);
				}
			}/* ix */
		}/* iy */
	}/* factor */
	if (min_neighbors != 0)
	{
		// group retrieved rectangles
		vector<int> labels;
		int nclasses = (int)RectsPartition(&origFaces, &labels);
		int i;
		int* neighbors = new int[nclasses];
		MyRect *rects = new MyRect[nclasses];
		memset(neighbors, 0, nclasses * sizeof(neighbors[0]));
		memset(rects, 0, nclasses * sizeof(rects[0]));

		// count number of neighbors
		for (i = 0; i < origFaces.size(); i++)
		{
			MyRect r = origFaces.at(i);
			int idx = labels.at(i);

			neighbors[idx]++;
			rects[idx].x += r.x;
			rects[idx].y += r.y;
			rects[idx].width += r.width;
			rects[idx].height += r.height;
		}

		// calculate average bounding box
		//		vector<int> tempNeighbors;
		//		tempNeighbors.clear();
		for (i = 0; i < nclasses; i++)
		{
			int n = neighbors[i];
			if (n >= min_neighbors)
			{
				MyRect rect;
				rect.x = (rects[i].x * 2 + n) / (2 * n);
				rect.y = (rects[i].y * 2 + n) / (2 * n);
				rect.width = (rects[i].width * 2 + n) / (2 * n);
				rect.height = (rects[i].height * 2 + n) / (2 * n);
				tempFaces.push_back(rect);
				//				tempNeighbors.push_back(neighbors[i]);
			}
		}
		delete[] neighbors;
		delete[] rects;

		// filter out small face rectangles inside large face rectangles
		for (i = 0; i < tempFaces.size(); i++)
		{
			MyRect r1 = tempFaces.at(i);
			//			int nb1 = tempNeighbors.at(i);
			int flag = 1;

			//			printf("%d %d %d %d %d\n", r1.x, r1.y, r1.width, r1.height, nb1);

			for (int j = 0; j < tempFaces.size(); j++)
			{
				MyRect r2 = tempFaces.at(j);
				//				int nb2 = tempNeighbors.at(j);
				int distance = Round(r2.width * 0.2);

				if ((i != j) &&
					(r1.x >= r2.x) &&
					(r1.y >= r2.y) &&
					(r1.x + r1.width <= r2.x + r2.width) &&
					(r1.y + r1.height <= r2.y + r2.height))//&&
														   //					(nb2 > MAX(3, nb1) || nb1 < 3 ) )
				{
					flag = 0;
					break;
				}
			}

			if (flag)
				vecFaces->push_back(r1);
		}
	}

	//释放内存
	delete[] SAT;
	delete[] RSAT;
	delete[]SSAT;
	SAT = NULL;
	RSAT = NULL;
	SSAT = NULL;
}

void ReleaseDetCascadeClassifier(DetCascadeClassifier* cascade)
{
	if (NULL == cascade)
		return;

	for (int i = 0; i < cascade->count; i++)
	{
		for (int j = 0; j < cascade->stageClassifier[i].count; j++)
		{
			DetHaarClassifier* classifier = &(cascade->stageClassifier[i].classifier[j]);
			for (int k = 0; k < HAAR_FEATURE_MAX; k++)
			{
				classifier->feature.rect[k].p0 = NULL;
				classifier->feature.rect[k].p1 = NULL;
				classifier->feature.rect[k].p2 = NULL;
				classifier->feature.rect[k].p3 = NULL;
			}
		}
		delete[] cascade->stageClassifier[i].classifier;
		cascade->stageClassifier[i].classifier = NULL;
	}

	delete[] cascade->stageClassifier;
	cascade->stageClassifier = NULL;

	delete cascade;
	cascade = NULL;
}

void ReleaseClassifierCascade(ClassifierCascade* cascade)
{
	if (NULL == cascade)
		return;

	for (int i = 0; i < cascade->count; i++)
	{
		delete[] cascade->stageClassifier[i].classifier;
		cascade->stageClassifier[i].classifier = NULL;
	}

	delete[] cascade->stageClassifier;
	cascade->stageClassifier = NULL;

	delete cascade;
	cascade = NULL;
}

//检测人脸
int detectFace(unsigned char *pbyteSrcImg, vector<MyRect>* vecFaces, int nImgWidth, int nImgHeight)
{
	MySize origWindowSize;
	origWindowSize.height = 20;
	origWindowSize.width = 20;

	ClassifierCascade* cascade = NULL;
	DetCascadeClassifier* detcascade = NULL;

	cascade = LoadClassifierCascade(CLASS_DIR, MAXSTEP, origWindowSize);

	if (cascade == NULL)
	{
		printf("can not load the data for ClassifierCascade\n");
		return -1;
	}

	MyRect detRegion;

	MySize min_size, max_size;

	int min_neighbors = 2;
	double scale_factor = 1.2F;

	int temp_nImgWidth = 0, temp_nImgHeight = 0;


	if ((temp_nImgHeight != nImgHeight) || (temp_nImgWidth != nImgWidth))
	{
		detcascade = CreateDetCascadeClassifier(cascade, nImgWidth, nImgHeight, 1);
		detRegion.x = 0; detRegion.y = 0; detRegion.height = nImgHeight - 1; detRegion.width = nImgWidth - 1;
		min_size.width = 20; min_size.height = 20; max_size.width = (nImgWidth * 4) / 5; max_size.height = (nImgHeight * 4) / 5;
		if (max_size.width>max_size.height)
		{
			max_size.width = max_size.height;
		}
		else
			max_size.height = max_size.width;

		temp_nImgHeight = nImgHeight;
		temp_nImgWidth = nImgWidth;

	}

	//时钟相关先注释掉 mabel
//	clock_t start, finish;
//	double  duration; start = clock();

	HaarDetectFaces(pbyteSrcImg, nImgWidth, nImgHeight, detRegion, detcascade, vecFaces, min_size, max_size, min_neighbors, scale_factor);


//	finish = clock();
//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	printf("Detect face %.4f seconds\n", duration);
	char time2Qstring[50];
//	sprintf(time2Qstring, "%.3f second", duration);

	ReleaseClassifierCascade(cascade);
	ReleaseDetCascadeClassifier(detcascade);

	cout << "renlianjiance\n";



	return 0;
}

//在原图上画出人脸区域
void drawFaceRect(unsigned char *pbyteSrcImg, vector<MyRect>* vecFaces, int nImgWidth, int nImgHeight)
{
	int x, y, w, h;
	vector<MyRect>::iterator iter_begin = vecFaces->begin();
	vector<MyRect>::iterator iter_end = vecFaces->end();
	vector<MyRect>::iterator iter_p;
	for (iter_p = iter_begin; iter_p<iter_end; iter_p++)
	{
		x = iter_p->x;
		y = iter_p->y;
		w = iter_p->width;
		h = iter_p->height;

		for (int i = 0; i<w; i++)
		{
			pbyteSrcImg[y*nImgWidth + i + x] = 255;
			pbyteSrcImg[(y + h)*nImgWidth + i + x] = 255;
		}

		for (int j = 0; j<h; j++)
		{
			pbyteSrcImg[(y + j)*nImgWidth + x] = 255;
			pbyteSrcImg[(y + j)*nImgWidth + x + w - 1] = 255;
		}
	}
	cout << "DRAW FACERECT" <<endl;
}

void drawAllFeature(unsigned char *pbyteSrcImg, GraphDiscription pos, int nImgWidth, int nImgHeight)
{
	int i, x, y, w, h;


	for (i = 0; i < 20; i++)
	{
		x = (int)pos->verts[i].x;
		y = (int)pos->verts[i].y;

		for (int k = -3; k < 3; k++)
		{
			pbyteSrcImg[y*nImgWidth + k + x] = 255;
		}

		for (int j = -3; j < 3; j++)
		{
			pbyteSrcImg[(y + j)*nImgWidth + x] = 255;
		}
	}
}

void drawLineRect(unsigned char *pbyteSrcImg, int nImgWidth, int nImgHeight, int line[])
{
	int i, x, y, w, h;


	for (i = 1; i <= line[0]; i++)
	{
		y = line[i];

		for (int k = 0; k < nImgWidth; k++)
		{
			pbyteSrcImg[y*nImgWidth + k] = 0;
		}
	}
}
unsigned char *makeSubFace(MyRect vecFaces)
{
	int x, y, w, h;

	x = vecFaces.x;
	y = vecFaces.y;
	w = vecFaces.width;
	h = vecFaces.height;

	unsigned char * mem = (unsigned char *)malloc(w*h);

	if (mem == NULL)printf("malloc failed!");

	return mem;
}

//从原图上切割出人脸区域
void cutFaceRect(unsigned char *pbyteSrcImg, unsigned char *face, MyRect vecFaces, int nImgWidth, int nImgHeight)
{
	int x, y, w, h;

	x = vecFaces.x;
	y = vecFaces.y;
	w = vecFaces.width;
	h = vecFaces.height;

	for (int j = 0; j<h; j++)
		for (int i = 0; i<w; i++)
		{
			face[j*w + i] = pbyteSrcImg[(y + j) * nImgWidth + x + i];
		}
}

void CalSubRectPixel(unsigned char *src, int width, int height, MySumRect &rect)
{
	int i, j;
	rect.sum = 0;
	for (i = 0; i < rect.height; i++)
		for (j = 0; j < rect.width; j++)
		{
			rect.sum += (unsigned int)*(src + (rect.y + i)*width + rect.x + j);
		}
}

void SortEyeRect(vector<MySumRect> &rect)
{
	for (vector<MySumRect>::iterator itr = rect.begin(); itr != rect.end(); itr++)
	{
		for (vector<MySumRect>::iterator itr_sec = itr; itr_sec != rect.end(); itr_sec++)
		{
			if (itr->sum > itr_sec->sum)
			{
				MySumRect tmp = *itr;
				*itr = *itr_sec;
				*itr_sec = tmp;
			}
		}
	}
}

void DeSortEyeRect(vector<MySumRect> &rect)
{
	for (vector<MySumRect>::iterator itr = rect.begin(); itr != rect.end(); itr++)
	{
		for (vector<MySumRect>::iterator itr_sec = itr; itr_sec != rect.end(); itr_sec++)
		{
			if (itr->sum < itr_sec->sum)
			{
				MySumRect tmp = *itr;
				*itr = *itr_sec;
				*itr_sec = tmp;
			}
		}
	}
}

void CalVariance(unsigned char *src, int width, int height, MySumRect &my)
{
	int nAverage = 0;
	int nVar = 0;

	for (int i = 0; i < my.height; i++)
	{
		for (int j = 0; j < my.width; j++)
		{
			nAverage += *(src + (i + my.y)*width + j + my.x);
		}
	}

	nAverage /= my.width * my.height;

	for (int i = 0; i < my.height; i++)
	{
		for (int j = 0; j < my.width; j++)
		{
			nVar += (*(src + (i + my.y)*width + j + my.x) - nAverage)*(*(src + (i + my.y)*width + j + my.x) - nAverage);
		}
	}

	my.var = nVar;
	if (my.var < 500 * my.width * my.height || my.var > 900 * my.width * my.height)my.flags = 1;
	cout << "variance is " << nVar << "x is " << my.x << "y is " << my.height << " flags " << my.flags << endl;
}

void DeleteUnbalance(unsigned char* src, int width, int height, vector<MySumRect> &rect)
{
	vector<MySumRect> vMinRect = rect;

	for (vector<MySumRect>::iterator itr = rect.begin(); itr != rect.end(); itr++)
	{
		CalVariance(src, width, height, *itr);
	}

	for (vector<MySumRect>::iterator itr_d = vMinRect.begin(); itr_d != vMinRect.end(); itr_d++)
	{
		if (itr_d->flags != 1)
		{
			if (itr_d->x < itr_d->width || itr_d->x >(width - 2 * itr_d->width) || ((itr_d->x > width / 2 - itr_d->width) && (itr_d->x < width / 2 + itr_d->width)))
			{
				itr_d->flags = 1;
			}
		}
	}

	int offset = 0;
	for (vector<MySumRect>::iterator itr_del = vMinRect.begin(); itr_del != vMinRect.end(); itr_del++)
	{
		if (itr_del->flags == 1)
		{
			rect.erase(rect.begin() + offset);
		}
		else
		{
			offset++;
		}
	}
}

void DeleteRedundEye(vector<MySumRect> &rect)
{
	vector<MySumRect> vMinRect = rect;

	for (vector<MySumRect>::iterator itr = rect.begin(); itr != rect.end() - 1; itr++)
	{
		if (itr->flags != 1)
		{
			for (vector<MySumRect>::iterator itr_red = itr + 1; itr_red != rect.end(); itr_red++)
			{
				if ((itr->x - itr_red->x <= itr->width   && itr->x - itr_red->x >= (-1) * itr->width) && (itr->y - itr_red->y <= itr->height && itr->y - itr_red->y >= (-1) * itr->height))
				{
					itr_red->flags = 1;
				}
			}
		}
	}

	vMinRect = rect;
	int offset = 0;
	for (vector<MySumRect>::iterator itr_del = vMinRect.begin(); itr_del != vMinRect.end(); itr_del++)
	{
		if (itr_del->flags == 1)
		{
			rect.erase(rect.begin() + offset);
		}
		else
		{
			offset++;
		}
	}
}

void PrintPredictRect(vector<MySumRect> vMinRect, int num)
{
	int a = 0;
	for (vector<MySumRect>::iterator itr = vMinRect.begin(); itr != vMinRect.end(); itr++, a++)
	{
		if (a<num)
			cout << "sum is " << itr->sum << " x: " << itr->x << "  y: " << itr->y << endl;
	}
}

unsigned int GetFixedSum(vector<MySumRect> &rect, int x, int y)
{
	for (vector<MySumRect>::iterator itr = rect.begin(); itr != rect.end(); itr++)
	{
		if (itr->x == x && itr->y == y)
		{
			//cout << "Sub  x " << x << " y " << y <<  "sum is " << itr->sum << endl;
			return itr->sum;
		}
	}
	//cout << "Sub  x " << x << " y " << y <<   endl;
	return 0;
}

unsigned int AddRoundDiff(vector<MySumRect> &rect, MySumRect &my, int width, int height)
{
	unsigned int sum_tmp = 0;
	unsigned int uThrehold = 60 * my.width*my.height;
	unsigned int lrThrehold = 60 * my.width*my.height;
	unsigned int l, r, d, u;
	l = r = d = u = 0;

	if (my.x > my.width && my.y > my.height)//left up
	{
		//sum_tmp += GetFixedSum(rect, my.x - my.width, my.y - my.height) - my.sum;
	}
	if (my.x > my.width)//left
	{
		l = (2 * (GetFixedSum(rect, my.x - my.width, my.y) + (GetFixedSum(rect, my.x - my.width, my.y + my.height))) - my.sum);
	}
	if (my.x > my.width && my.y + 2 * my.height < height)//left down
	{
		//sum_tmp += GetFixedSum(rect, my.x - my.width, my.y + my.height) - my.sum;
	}
	if (my.y > my.height)//up
	{
		u = (2 * (GetFixedSum(rect, my.x, my.y - my.height) + GetFixedSum(rect, my.x + my.width, my.y - my.height)) - my.sum);
	}
	if (my.y + 2 * my.height < height)//down
	{
		d = (2 * (GetFixedSum(rect, my.x, my.y + my.height) + GetFixedSum(rect, my.x + my.width, my.y + my.height)) - my.sum);
	}
	if (my.x + 2 * my.width < width && my.y > my.height)//right up
	{
		//sum_tmp += GetFixedSum(rect, my.x + my.width, my.y -  my.height) - my.sum;
	}
	if (my.x + 2 * my.width < width)//right
	{
		r = (2 * (GetFixedSum(rect, my.x + my.width, my.y) + GetFixedSum(rect, my.x + my.width, my.y + my.height)) - my.sum);
		//cout << "r is " << r << "    " <<  4*GetFixedSum(rect, my.x + my.width, my.y )<< " " <<  my.sum<<  endl;
	}
	if (my.x + 2 * my.width < width && my.y + my.height * 2 < height)//right down
	{
		//sum_tmp += GetFixedSum(rect, my.x + my.width, my.y + my.height) - my.sum;
	}

	if ((l > lrThrehold || l == 0) && (r > lrThrehold || r == 0) && (u > uThrehold || u == 0) && (d > uThrehold || d == 0))
	{
		sum_tmp += (l + r + u + d);
		cout << "double ------ x " << my.x << " y " << my.y << "sum  " << sum_tmp << endl;
	}
	else
		sum_tmp += (l + r + u + d) / 2;

	return sum_tmp;
}

void FindBestEye(vector<MySumRect> rect, vector<MySumRect> &vLeftRect, vector<MySumRect> &vRightRect, MyRect *left, MyRect *right, int width, int height)
{
	if (vLeftRect.size() > 1)
	{
		for (vector<MySumRect>::iterator itr = vLeftRect.begin(); itr != vLeftRect.end(); itr++)
		{
			itr->sum = 0;
			itr->sum += AddRoundDiff(rect, *itr, width, height);
			//cout << "left diff is " <<  itr->sum << " x is "<<itr->x<< " y " << itr->y <<endl;
		}
	}
	DeSortEyeRect(vLeftRect);
	if (vRightRect.size() > 1)
	{
		for (vector<MySumRect>::iterator itr = vRightRect.begin(); itr != vRightRect.end(); itr++)
		{
			itr->sum = 0;
			itr->sum += AddRoundDiff(rect, *itr, width, height);
			//cout << "right diff is " <<  itr->sum << " x is "<<itr->x<< " y " << itr->y <<endl;
		}
	}
	DeSortEyeRect(vRightRect);

	if (vLeftRect.size() >= 1)
	{
		left->x = vLeftRect[0].x;
		left->y = vLeftRect[0].y;
		left->width = vLeftRect[0].width;
		left->height = vLeftRect[0].height;
	}
	else
	{
		left->x = 0;
		left->y = 0;
		left->width = 0;
		left->height = 0;
	}

	if (vRightRect.size() >= 1)
	{
		right->x = vRightRect[0].x;
		right->y = vRightRect[0].y;
		right->width = vRightRect[0].width;
		right->height = vRightRect[0].height;
	}
	else
	{
		right->x = 0;
		right->y = 0;
		right->width = 0;
		right->height = 0;
	}
}

/*!
detect eye eara
*/
void  DetectEye(unsigned char *src, int width, int height, vector<MaxMin> &vMin, FPosition &pos)
{
	int i, j;
	unsigned int uThreh = 0;
	TwoPoints rrect, lrect;
	int start, end, line;
	vector<MySumRect> vMinRect, vAllRect;
	vector<MySumRect> vLeftRect, vRightRect;
	MySumRect vTmp, aTmp;
	unsigned char *z = (unsigned char *)malloc(width * height);
	for (i = 0; i < height; i++)
		for (j = 0; j< width; j++)
			*(z + i*width + j) = *(src + i*width + j);

	pos.h = height / 24;
	pos.w = width / 12;

	if (vMin.size() == 2)
	{
		start = vMin[1].line - height / 12;
		end = vMin[1].line + height / 12;
		line = vMin[1].line;
	}
	else
	{
		start = vMin[vMin.size() - 1].line - height / 12;
		end = vMin[vMin.size() - 1].line + height / 12;
		line = vMin[vMin.size() - 1].line;
	}

	//    sobel(z , width , height, line,rrect, lrect);
	sobel2(z, width, height);

	vTmp.width = pos.w;
	vTmp.height = pos.h;
	vTmp.flags = 0;
	aTmp.width = pos.w / 2;
	aTmp.height = pos.h / 2;
	aTmp.flags = 0;
	for (i = (int)lrect.y1; i < (int)lrect.y2 - pos.h; i++)
		for (j = (int)lrect.x1; j < (int)lrect.x2 - pos.w; j++)
		{
			vTmp.x = j;
			vTmp.y = i;
			CalSubRectPixel(src, width, height, vTmp);
			vMinRect.push_back(vTmp);
		}
	SortEyeRect(vMinRect);

	if (vMinRect.size() >= 1)
	{
		pos.rx = vMinRect[0].x;
		pos.ry = vMinRect[0].y;
	}
	else
	{
		pos.rx = 0;
		pos.ry = 0;
		pos.w = 0;
		pos.h = 0;
	}

	vMinRect.clear();
	for (i = (int)rrect.y1; i < (int)rrect.y2 - pos.h; i++)
		for (j = (int)rrect.x1; j < (int)rrect.x2 - pos.w; j++)
		{
			vTmp.x = j;
			vTmp.y = i;
			CalSubRectPixel(src, width, height, vTmp);
			vMinRect.push_back(vTmp);
		}
	SortEyeRect(vMinRect);

	if (vMinRect.size() >= 1)
	{
		pos.lx = vMinRect[0].x;
		pos.ly = vMinRect[0].y;
	}
	else
	{
		pos.lx = 0;
		pos.ly = 0;
	}

	free(z);
}

void Binaryzation(unsigned char *src, int width, int height)
{
	int u, p, t1, t2;
	u = p = 0;
	t1 = 122;
	t2 = 130;

	while (t2 - t1 < 2)
	{
		u = p = 0;
		t1 = t2;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (*(src + i*width + j) > t1) u += *(src + i*width + j);
				else  p += *(src + i*width + j);
			}
		}
		t2 = (u + p) / 2;
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (*(src + i*width + j) > t1)
			{
				*(src + i*width + j) = 255;
			}
			else
			{
				*(src + i*width + j) = 0;
			}
		}
	}
}

void HistGram(unsigned char *src, int width, int height)
{
	unsigned char*	lpSrc;
	long	lTemp;
	int i, j;
	unsigned char	bMap[256];
	long	lCount[256];

	// ?????Ϊ0
	for (i = 0; i < 256; i++)
	{
		lCount[i] = 0;
	}

	// ??????Ҷ???ļ??
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			lpSrc = (unsigned char *)src + i*width + j;

			lCount[*(lpSrc)]++;
		}
	}

	// ?????????	for (i = 0; i < 256; i++)
	{
		// ??ʼΪ0
		lTemp = 0;

		for (j = 0; j <= i; j++)
		{
			lTemp += lCount[j];
		}

		// ???Ӧ??»Ҷ??
		bMap[i] = (unsigned char)(lTemp * 255 / height / width);
	}

	// ÿ?
	for (i = 0; i < height; i++)
	{
		// ÿ??		for(j = 0; j < width; j++)
		{
			// ???µĻҶ??
			*(src + i*width + j) = bMap[*(src + i*width + j)];

		}
	}
}


void thresh(unsigned char *src, int width, int height, int line)
{
	int histo[256];
	int i, j, t;
	int start = line - height / 12;
	int end = line + height / 12;

	/* Compute a grey level histogram */
	for (i = 0; i<256; i++) histo[i] = 0;
	for (i = start + 1; i< end - 1; i++)
		for (j = 1; j<width - 1; j++)
		{
			histo[*(src + i*width + j)]++;
		}

	/* Threshold at the middle of the occupied levels */
	i = 255;
	while (histo[i] == 0) i--;
	j = 0;
	while (histo[j] == 0) j++;
	t = (int)((i + j) * 0.20);                  // a or b ,only one not two

												/* Apply the threshold */
	for (i = start + 1; i< end - 1; i++)
		for (j = 1; j<width - 1; j++)
			if (*(src + i*width + j) >= t) *(src + i*width + j) = 0;
			else  *(src + i*width + j) = 255;
}

/*	Apply a Sobel edge mask to the image X	*/

void sobel(unsigned char *src, int width, int height, int line, TwoPoints &rrect, TwoPoints &lrect)
{
	int i, j, n, m, k;
	int start = line - height / 14;
	int end = line + height / 12;
	unsigned char *z = (unsigned char *)malloc(width * height);


	for (i = start; i<end; i++)
		for (j = 0; j<width; j++)
			*(z + i*width + j) = 255;

	/* Now compute the convolution, scaling */
	for (i = start + 1; i<end - 1; i++)
	{
		for (j = 1; j<width - 1; j++)
		{
			n = (*(src + (i - 1)*width + j - 1) + 2 * (*(src + (i - 1)*width + j)) + *(src + (i - 1)*width + j + 1)) -
				(*(src + (i + 1)*width + j - 1) + 2 * (*(src + (i + 1)*width + j)) + *(src + (i + 1)*width + j + 1));
			m = (*(src + (i - 1)*width + j - 1) + 2 * (*(src + (i)*width + j - 1)) + *(src + (i + 1)*width + j - 1)) -
				(*(src + (i - 1)*width + j + 1) + 2 * (*(src + (i)*width + j + 1)) + *(src + (i + 1)*width + j + 1));
			k = (int)(sqrt((double)(n*n + m*m)) / 4.0);
			*(z + i*width + j) = k;
		}
	}
	thresh(z, width, height, line);

	for (i = start; i<end; i++)
		for (j = 0; j<width; j++)
		{
			*(src + i*width + j) = *(z + i*width + j);
		}

	rrect.x1 = rrect.y1 = 65531;
	rrect.x2 = rrect.y2 = 0;
	lrect.x1 = lrect.y1 = 65531;
	lrect.x2 = lrect.y2 = 0;

	for (i = start + 1; i<end - 1; i++)
	{
		for (j = 1; j<width - 1; j++)
		{
			if (j < width / 2)
			{
				if (*(src + i*width + j) == 0)
				{
					if (j <= rrect.x1)rrect.x1 = j;
					if (j > rrect.x2) rrect.x2 = j;
					if (i < rrect.y1) rrect.y1 = i;
					if (i > rrect.y2) rrect.y2 = i;
				}
			}
			else
			{
				if (*(src + i*width + j) == 0)
				{
					if (j <= lrect.x1)lrect.x1 = j;
					if (j > lrect.x2) lrect.x2 = j;
					if (i < lrect.y1) lrect.y1 = i;
					if (i > lrect.y2) lrect.y2 = i;
				}
			}
		}
	}

	free(z);
}

void thresh2(unsigned char *src, int width, int height)
{
	int histo[256];
	int i, j, t;

	/* Compute a grey level histogram */
	for (i = 0; i<256; i++) histo[i] = 0;
	for (i = 1; i< height - 1; i++)
		for (j = 1; j<width - 1; j++)
		{
			histo[*(src + i*width + j)]++;
		}

	/* Threshold at the middle of the occupied levels */
	i = 255;
	while (histo[i] == 0) i--;
	j = 0;
	while (histo[j] == 0) j++;
	t = (int)((i + j)*0.2);

	/* Apply the threshold */
	for (i = 1; i< height - 1; i++)
		for (j = 1; j<width - 1; j++)
			if (*(src + i*width + j) >= t) *(src + i*width + j) = 0;
			else  *(src + i*width + j) = 255;
}

void sobel2(unsigned char *src, int width, int height)
{
	int i, j, n, m, k;
	unsigned char *z = (unsigned char *)malloc(width * height);

	for (i = 0; i<height; i++)
		for (j = 0; j<width; j++)
			*(z + i*width + j) = 255;

	/* Now compute the convolution, scaling */
	for (i = 1; i<height - 1; i++)
		for (j = 1; j<width - 1; j++)
		{
			n = (*(src + (i - 1)*width + j + 1) + 2 * (*(src + (i)*width + j + 1)) + *(src + (i + 1)*width + j + 1)) -
				(*(src + (i - 1)*width + j - 1) + 2 * (*(src + (i)*width + j - 1)) + *(src + (i + 1)*width + j - 1));
			m = (*(src + (i + 1)*width + j - 1) + 2 * (*(src + (i + 1)*width + j)) + *(src + (i + 1)*width + j + 1)) -
				(*(src + (i - 1)*width + j - 1) + 2 * (*(src + (i - 1)*width + j)) + *(src + (i - 1)*width + j + 1));
			k = (int)(sqrt((double)(n*n + m*m)) / 4.0);
			*(z + i*width + j) = k;
		}
	thresh2(z, width, height);

	for (i = 0; i<height; i++)
		for (j = 0; j<width; j++)
			*(src + i*width + j) = *(z + i*width + j);

	free(z);
}


void  Erosion(unsigned char *src, int width, int height, int Hori)
{
	int               x, y;
	unsigned char      num;
	int                i;

	unsigned char * lpPtr = (unsigned char *)malloc(height*width);
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			*(lpPtr + y*width + x) = *(src + y*width + x);
		}

	if (Hori)
	{
		for (y = 0; y<height; y++) {
			for (x = 1; x<width - 1; x++) {
				num = *(src + y*width + x);
				if (num == 0) {
					*(src + y*width + x) = (unsigned char)0;
					for (i = 0; i<3; i++) {
						num = (unsigned char)*(lpPtr + y*width + x + i - 1);
						if (num == 255) {
							*(src + y*width + x) = (unsigned char)255;
							break;
						}
					}
				}
			}
		}
	}
	else {
		for (y = 1; y<height - 1; y++) {
			for (x = 0; x<width; x++) {
				num = (unsigned char)*(lpPtr + y*width + x);
				if (num == 0) {
					*(src + y*width + x) = (unsigned char)0;
					for (i = 0; i<3; i++) {
						num = (unsigned char)*(lpPtr + (i - 1 + y)*width + x);
						if (num == 255) {
							*(src + y*width + x) = (unsigned char)255;
							break;
						}
					}
				}
			}
		}
	}

	free(lpPtr);
}

void  Dilation(unsigned char *src, int width, int height, int Hori)
{
	int               x, y;
	unsigned char      num;
	int                i;
	unsigned char * lpPtr = (unsigned char *)malloc(height*width);

	if (Hori)
	{
		for (y = 0; y<height; y++) {
			for (x = 1; x<width - 1; x++) {
				num = (unsigned char)*(lpPtr + y*height + x);
				if (num == 255) {
					*(src + y*width + x) = (unsigned char)255;
					for (i = 0; i<3; i++) {
						num = (unsigned char)*(lpPtr + y*width + x + i - 1);
						if (num == 0) {
							*(src + y*width + x) = (unsigned char)0;
							break;
						}
					}
				}
				else *(src + y*width + x) = (unsigned char)0;
			}
		}
	}
	else {
		for (y = 1; y<height - 1; y++) {
			for (x = 0; x<width; x++) {
				num = (unsigned char)*(lpPtr + y*width + x);
				if (num == 255) {
					*(src + y*width + x) = (unsigned char)255;
					for (i = 0; i<3; i++) {
						num = (unsigned char)*(lpPtr + (y + i - 1)*width + x);
						if (num == 0) {
							*(src + y*width + x) = (unsigned char)0;
							break;
						}
					}
				}
				else *(src + y*width + x) = (unsigned char)0;
			}
		}
	}
	free(lpPtr);
}

void  MorphOpen(unsigned char *src, int width, int height)
{
	Erosion(src, width, height, 1);
	// Erosion(src, width, height, 0);
	Dilation(src, width, height, 1);
	// Dilation(src, width,  height, 0);
}

void CalHorRate(unsigned char *src, int width, int height)
{
	int nAverage = 0;
	double nVar = 0;

	for (int i = 0; i < height; i++)
	{
		nVar = 0;
		for (int j = 0; j < width; j++)
		{
			if (j + 1 <width && MIN(*(src + i*width + j), *(src + i*width + j + 1)) > 0)
			{
				double c = MAX(*(src + i*width + j), *(src + i*width + j + 1)) - MIN(*(src + i*width + j), *(src + i*width + j + 1));
				double e = c / MAX(*(src + i*width + j), *(src + i*width + j + 1));
				nVar += e;
			}
		}
		//cout << i << " line's rate "  << nVar << endl;
	}
}

void Horizon(unsigned char *src, int width, int height, int line[])
{
	int hsum[MAXMEM];
	int i = 0, j;
	int nMax, nMin;
	int num = 0;
	int n = 1;

	//for(i = 0; i < height*HORRATE; i++)
	for (i = 0; i < height; i++)
	{
		*(hsum + i) = 0;

		for (int j = 0; j < width; j++)
		{
			*(hsum + i) += *(src + i*width + j);
		}

		//        cout << *(hsum + i) << "  " <<  height  <<"  "<< i << endl;

	}
	for (i = 1; i < height - 9; i++)
	{
		num = 0;
		for (j = 0; j < 9; j++)
		{
			if (hsum[i + 4] < hsum[i + j]) num++;
		}
		if (num > 6)
		{
			line[n++] = i + 4;
			if (i < height - 9)i += 9;
		}
	}
	line[0] = n - 1;
	cout << line[0] << endl;
}

//计算水平sobel图像
void sobelhorizon(unsigned char *pbyteSrcImg, int width, int height)
{
	/*     for(int i = 0; i < height; i++)
	for(int j = 0 ;j < width ; j++)
	{
	if(*(pbyteSrcImg + i*width + j) < 140 ) *(pbyteSrcImg + i*width + j) = 0;
	else *(pbyteSrcImg + i*width + j) = 255;
	}
	*/
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			unsigned int a1 = 0;
			unsigned int a2 = 0;
			unsigned int a3 = 0;
			unsigned int b1 = 0;
			unsigned int b2 = 0;
			unsigned int b3 = 0;

			if (i - 1 < 0)
			{
				a1 = 0;
				a2 = 0;
				a3 = 0;
			}
			else if (i + 1 >= height)
			{
				b1 = 0;
				b2 = 0;
				b3 = 0;
			}

			if (j - 1 < 0)
			{
				b1 = 0;
				a1 = 0;
			}
			else if (j + 1 >= width)
			{
				a3 = 0;
				b3 = 0;
			}

			if (i - 1 >= 0 && j - 1 >= 0)a1 = *(pbyteSrcImg + (i - 1)*width + j - 1);
			if (i - 1 >= 0)a2 = *(pbyteSrcImg + (i - 1)*width + j);
			if (i - 1 >= 0 && j + 1 < width)a3 = *(pbyteSrcImg + (i - 1)*width + j + 1);
			if (i + 1 < height && j - 1 >= 0)b1 = *(pbyteSrcImg + (i + 1)*width + j - 1);
			if (i + 1 < height)b2 = *(pbyteSrcImg + (i + 1)*width + j);
			if (i + 1 < height && j + 1 < width)b3 = *(pbyteSrcImg + (i + 1)*width + j + 1);

//			int tmp = (-a1 - 2 * a2 - a3 + b1 + 2 * b2 + b3) / 4;
			int tmp = 0;
			tmp = tmp>0 ? tmp : (-tmp);
			*(pbyteSrcImg + i*width + j) = (unsigned char)tmp;
			//cout << (int)*(pbyteSrcImg + i*width + j) << " ";
			if (*(pbyteSrcImg + i*width + j) < 180) *(pbyteSrcImg + i*width + j) = 0;
			else *(pbyteSrcImg + i*width + j) = 255;
		}

	for (int i = 0; i < height; i++)
	{
		int a = 0;
		for (int j = 0; j < width; j++)
		{
			a += *(pbyteSrcImg + i*width + j);

		}
		cout << "horizon is " << a << endl;
	}
}

GraphDiscription FindAllFeature(unsigned char * src, int width, int height)
{
	int i, j;
	int line[100];
	int u, m, d;
	int line1, line2, line3, line4;

	GraphDiscription gd = ALLOCATE(graph_discription, 1);
	gd->numVert = 20;
	gd->numEdge = 0;

	gd->verts = ALLOCATE(Vert, gd->numVert);
	gd->vertLabels = ALLOCATE(char*, gd->numVert);
	gd->bunch = ALLOCATE(JetBunch, gd->numVert);
	gd->edges = ALLOCATE(Edge, gd->numEdge);
	for (i = 0; i < gd->numVert; i++) {
		gd->vertLabels[i] = ALLOCATE(char, LABELMAX);
		gd->bunch[i] = makeJetBunch();
	}

	for (i = 0; i < gd->numVert; i++)
	{
		gd->verts[i].x = 0;
		gd->verts[i].y = 0;
	}

	unsigned char *mem1 = (unsigned char *)malloc(width * height * sizeof(char));
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			*(mem1 + i*width + j) = *(src + i*width + j);
		}

	//	Binaryzation(mem1 , width , height);
	Horizon(mem1, width, height, line);

	u = m = d = 0;
	for (i = 1; i < line[0]; i++)
	{
		if (line[i] <= height / 3) u++;
		else if (line[i] <= height * 2 / 3)m++;
		else d++;
	}

	if (line[0] > 6)
	{
	}
	else
	{
		if (u == 2)
		{
			line1 = line[1];
			line2 = line[2];
		}
		else
		{
			line1 = line[1];
			line2 = line[2];
		}

		line3 = line[3];

		if (d > 1)
		{
			line4 = 0;
			for (int l = 4; l < line[0] + 1; l++)line4 += line[l];
			line4 /= (line[0] - 3);
		}
	}

	sobel2(mem1, width, height);
	//	drawLineRect(mem1,width,height,line);

	/*    for(i = 0; i < width/2; i++)
	{
	if(mem1[line1*width + i] == 0)
	{
	if(gd->verts[4].x > i || gd->verts[4].x == 0)
	{
	gd->verts[4].x = i;
	gd->verts[4].y = line1;
	}

	if(gd->verts[5].x < i )
	{
	gd->verts[5].x = i;
	gd->verts[5].y = line1;
	}
	}

	for(i = width/2; i < width; i++)
	{
	if(mem1[line1*width + i] == 0)
	{
	if(gd->verts[4].x > i || gd->verts[4].x == 0)
	{
	gd->verts[4].x = i;
	gd->verts[4].y = line1;
	}

	if(gd->verts[5].x < i )
	{
	gd->verts[5].x = i;
	gd->verts[5].y = line1;
	}
	}
	}
	}
	*/

	gd->verts[4].x = 2;
	gd->verts[4].y = line1;
	gd->verts[5].x = width / 2 - width / 10;
	gd->verts[5].y = line1;
	gd->verts[6].x = width / 2 + 2;
	gd->verts[6].y = line1;
	gd->verts[7].x = width - width / 10;
	gd->verts[7].y = line1;

	gd->verts[8].x = 0;
	gd->verts[8].y = line2;
	gd->verts[9].x = width / 10;
	gd->verts[9].y = line2;
	gd->verts[0].x = width / 5;
	gd->verts[0].y = line2;
	gd->verts[10].x = width / 2 - width / 5;
	gd->verts[10].y = line2;

	gd->verts[11].x = width * 3 / 5;
	gd->verts[11].y = line2;
	gd->verts[1].x = width * 7 / 10;
	gd->verts[1].y = line2;
	gd->verts[12].x = width * 4 / 5;
	gd->verts[12].y = line2;
	gd->verts[13].x = width - 2;
	gd->verts[13].y = line2;

	for (i = 0; i < width / 2; i++)
	{
		if (mem1[line3*width + i] == 0)
		{
		}
	}

	for (i = width / 2; i < width; i++)
	{
		if (mem1[line3*width + i] == 0)
		{
		}
	}

	gd->verts[14].x = width / 2;
	gd->verts[14].y = line3 - height / 20;
	gd->verts[15].x = width * 9 / 20;
	gd->verts[15].y = line3;
	gd->verts[16].x = width * 9 / 20;
	gd->verts[16].y = line3;


	gd->verts[2].x = width / 4;
	gd->verts[2].y = line4;
	gd->verts[3].x = width * 3 / 4;
	gd->verts[3].y = line4;
	gd->verts[17].x = width / 2;
	gd->verts[17].y = line4 - height / 6;
	gd->verts[18].x = width / 2;
	gd->verts[18].y = line4 + height / 6;

	cout << line1 << " " << line2 << " " << line3 << " " << line4 << endl;
	for (i = 0; i < gd->numVert; i++)
	{
		if (gd->verts[i].x < 0) gd->verts[i].x = 0;
		if (gd->verts[i].x > width) gd->verts[i].x = width;
		if (gd->verts[i].y < 0) gd->verts[i].y = 0;
		if (gd->verts[i].y > height) gd->verts[i].y = height;
	}


	//	WriteBmpFile("/mnt/hgfs/share/PaperImage/line.bmp" , mem1 , width , height);
	//    DetectEye(src width, height, vMin, pos);

	free(mem1);

	return gd;
}

//test
GraphDiscription FaceDetect(const char * path, unsigned char *pbyteSrcImg, int nImgWidth, int nImgHeight)
{
	GraphDiscription gd = ALLOCATE(graph_discription, 1);;

	string sImageName, sDestFace, sSubDetectFace, sSubFaceEye;
	stringstream sFileName;
	TwoPoints source;

	sFileName << path << ".bmp";
	sImageName = sFileName.str();

	sFileName.str("");
	sFileName << path << "_Dest.bmp";
	sDestFace = sFileName.str();

	sFileName.str("");
	sFileName << path << "_SubFace.bmp";
	sSubDetectFace = sFileName.str();

	sFileName.str("");
	sFileName << path << "_DetectEye.bmp";
	sSubFaceEye = sFileName.str();
	std::cout << sSubFaceEye;
	//分配空间
	vector<MyRect>* vecFaces = new vector<MyRect>;
	detectFace(pbyteSrcImg, vecFaces, nImgWidth, nImgHeight);

	drawFaceRect(pbyteSrcImg, vecFaces, nImgWidth, nImgHeight);
	WriteBmpFile(sDestFace.c_str(), pbyteSrcImg, nImgWidth, nImgHeight);
	if (vecFaces->size() > 1)
	{
		cout << "Detect " << vecFaces->size() << " faces" << endl;

		int nDel = 0;
		int temp = 0;
		for (vector<MyRect>::iterator iter_p = vecFaces->begin(); iter_p < vecFaces->end(); iter_p++, temp++)
		{
			unsigned char * mem1 = makeSubFace(*iter_p);
			int line[10];
			cutFaceRect(pbyteSrcImg, mem1, *iter_p, nImgWidth, nImgHeight);
			Binaryzation(mem1, iter_p->width, iter_p->height / 2);
			Horizon(mem1, iter_p->width, iter_p->height, line);
			if (line[2] == 0xffff)nDel = temp;
		}
		vecFaces->erase(vecFaces->begin() + nDel);
	}

	vector<MyRect>::iterator iter_begin = vecFaces->begin();
	vector<MyRect>::iterator iter_end = vecFaces->end();
	vector<MyRect>::iterator iter_p;
	for (iter_p = iter_begin; iter_p<iter_end; iter_p++)
	{
		unsigned char * mem = makeSubFace(*iter_p);
		unsigned char * mem1 = makeSubFace(*iter_p);
		vector<MaxMin> vMin;
		cutFaceRect(pbyteSrcImg, mem, *iter_p, nImgWidth, nImgHeight);
		cutFaceRect(pbyteSrcImg, mem1, *iter_p, nImgWidth, nImgHeight);
		//cout << vecFaces->size()  <<"The second stage "  << vMin.size() << "widht " << iter_p->width << "hei " << iter_p->height <<  endl;    

		gd = FindAllFeature(mem1, iter_p->width, iter_p->height);

		for (int i = 0; i < gd->numVert; i++)
		{
			gd->verts[i].x += iter_p->x;
			gd->verts[i].y += iter_p->y;
			cout << iter_p->x << "  " << iter_p->y << " " << gd->verts[i].x << " " << gd->verts[i].y << endl;
		}

		//       WriteBmpFile("/mnt/hgfs/share/PaperImage/ori.bmp" , mem, iter_p->width , iter_p->height);
		//        sobel2 (mem , iter_p->width , iter_p->height);
		//        WriteBmpFile("/mnt/hgfs/share/PaperImage/sobe.bmp" , mem, iter_p->width , iter_p->height);
		//WriteBmpFile(sSubDetectFace.c_str() , mem , iter_p->width , iter_p->height);
		//        DetectEye(mem, iter_p->width, iter_p->height, vMin, pos);
		// drawEyeRect(mem , pos , iter_p->width, iter_p->height);
		//sobelhorizon(mem , iter_p->width , iter_p->height);
		//  Horizon(mem , iter_p->width , iter_p->height);
		// HistGram(mem , iter_p->width , iter_p->height/2);
		//sobel(mem , iter_p->width , iter_p->height);
		// Binaryzation(mem , iter_p->width , iter_p->height/2);
		// MorphOpen(mem , iter_p->width , iter_p->height/2);

		// drawAllFeature(pbyteSrcImg , gd , nImgWidth, nImgHeight);
		// WriteBmpFile(sDestFace.c_str(), pbyteSrcImg,nImgWidth,nImgHeight);

		// drawAllFeature(mem , gd , iter_p->width, iter_p->height);
		WriteBmpFile(sSubFaceEye.c_str(), mem, iter_p->width, iter_p->height);

		freeImgMem(mem);
		freeImgMem(mem1);
	}

	delete vecFaces;

	saveGraphDiscription("bpnn/facedetect/detect.pts", gd);
	return gd;
}

