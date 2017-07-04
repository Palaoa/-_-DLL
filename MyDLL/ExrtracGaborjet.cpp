
#include "stdafx.h"
#include "ExtractGaborjet.h"
#include<fstream>
#include<iostream>

FTYPE WiskottDCFree = 0.0;

/* 模拟sin函数，简化sin函数运行时间 */
FTYPE tLerSin(GFTYPE x)
{
	FTYPE m1, m2;
	while (x >  PI) { x -= 2 * PI; }
	while (x < -PI) { x += 2 * PI; }
	m1 = x*x;
	m2 = m1*x;

	//FTYPE m3=m2*m1;
	//return x-m2/6+m3/120;
	return x - m2 / 6;
}

/* 模拟cos函数，简化cos函数运行时间 */
FTYPE tLerCos(GFTYPE x)
{
	FTYPE m1, m2;
	while (x >  PI) { x -= 2 * PI; }
	while (x < -PI) { x += 2 * PI; }
	m1 = x*x;
	//m2=m1*m1;
	//return 1-m1/2+m2/24;
	return 1 - m1 / 2;
}

/* 模拟atan函数，简化atan函数运行时间 */
FTYPE tLerAtan(GFTYPE x)
{
	FTYPE m1, m2, m3;
	while (x >  PI) { x -= 2 * PI; }
	while (x < -PI) { x += 2 * PI; }
	m1 = x*x;
	m2 = m1*x;
	m3 = m2*m1;
	return x - m2 / 3 + m3 / 5;
}

/*计算Gaborjet */
GaborJet extractJet(GFTYPE x, GFTYPE y, Image im, JetMasks masks) {
	GaborJet jet = NULL;
	if (masks->size) {

		int i;
		float rx = ROUND(x);
		float ry = ROUND(y);
		float dx = x - rx;
		float dy = y - ry;
		jet = makeGaborJet(masks->size / 2);//jet->length=masks->size/2;

											/* Add a pointer to the mask parameters for future use */
		jet->params = masks->params;


		/* Compute the jet coeffecents */
		for (i = 0; i < jet->length; i++) {
			jet->realPart[i] = convolvePoint(rx, ry, 0, im, masks->masks[2 * i]);//use most time
																				 // printf("*****n is :%f, *****realPart is:%f**\n",n,jet->realPart[i]);
			jet->imagPart[i] = convolvePoint(rx, ry, 0, im, masks->masks[2 * i + 1]);
			//  jet->imagPart[i] = 3.89768;
			// printf("In ExtractGabor realPart is:%f,imagPart is:%f\n",jet->realPart[i],jet->imagPart[i]);
			//exit(0);
			// FINITE(jet->realPart[i]);
			//FINITE(jet->imagPart[i]);
		}
		computePolar(jet);

		/* Adjust phase coordinates */
		/*     for( i = 0; i < jet->length; i++){
		jet->ang[i] +=  dx * jet->params->kx[2*i] + dy * jet->params->ky[2*i];
		}*/

		/* Recompute coeffecents based on phase change */
		/*       for( i = 0; i < jet->length; i++){
		jet->realPart[i] = jet->mag[i] * tLerCos(jet->ang[i]);
		jet->imagPart[i] = jet->mag[i] * tLerSin(jet->ang[i]);
		//jet->realPart[i] = jet->mag[i] ;
		//jet->imagPart[i] = jet->mag[i];
		}*/
	}
	else {
		jet = makeGaborJet(0);
	}

	/* Save the jet location */
	jet->x = x;
	jet->y = y;

	return jet;
}

//根据gaborjet的实部和虚部，计算角度
void computePolar(GaborJet jet) {
	int i;
	FTYPE m = 0.0;
	for (i = 0; i < jet->length; i++) {
		/* Find the magnitude of the imaginary points */
		jet->mag[i] = sqrt(SQR(jet->realPart[i]) + SQR(jet->imagPart[i]));
		/* Find the angle of the imaginary points */
		if (jet->realPart[i] != 0) {
			/* determine which quadrint the point lies in */
			m = jet->imagPart[i] / jet->realPart[i];
			if (jet->realPart[i] >= 0) {
				jet->ang[i] = atan(m);
			}
			else {
				jet->ang[i] = PI + atan(m);
			}
		}
		else {
			if (jet->imagPart[i] >= 0) {
				jet->ang[i] = PI / 2;
			}
			else {
				jet->ang[i] = -PI / 2;
			}
		}
	}
}


//为Gaborjet分配空间
GaborJet makeGaborJet(int length) {
	GaborJet jet = (GaborJet)malloc(sizeof(gabor_jet));
	assert(jet);

	if (length) {
		jet->length = length;
		//	printf("in makeGaborjet length is %d\n",jet->length);
		jet->realPart = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
		assert(jet->realPart);
		jet->imagPart = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
		assert(jet->imagPart);
		jet->mag = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
		assert(jet->mag);
		jet->ang = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
		assert(jet->ang);
	}
	else {
		length = 0;
	}

	jet->params = NULL;

	return jet;
}

// Free memory for the gabor jet 
void     freeGaborJet(GaborJet jet) {
	if (jet->length) {
		free(jet->realPart);
		free(jet->imagPart);
		free(jet->mag);
		free(jet->ang);
	}

	free(jet);
}

//对于图片中不存在的点，返回周围平均像素点
PIX_TYPE interpLinear(Image img, PIX_TYPE x, PIX_TYPE y, int c) {
	PIX_TYPE xfrac = (x - floor(x));//floor(x) return the integer. return deciamal
	PIX_TYPE yfrac = (y - floor(y));
	int xLower = INT_FLOOR(x);//Lower integer
	int xUpper = INT_CEIL(x);//High integer
	int yLower = INT_FLOOR(y);
	int yUpper = INT_CEIL(y);
	PIX_TYPE valUpper, valLower;

	valUpper = ie(img, xLower, yUpper, c)*(1.0 - xfrac) + ie(img, xUpper, yUpper, c)*(xfrac);
	valLower = ie(img, xLower, yLower, c)*(1.0 - xfrac) + ie(img, xUpper, yLower, c)*(xfrac);

	return valLower*(1.0 - yfrac) + valUpper*(yfrac);
}

/* 计算Gaborjet实部和虚部 */
PIX_TYPE convolvePoint(PIX_TYPE x, PIX_TYPE y, int c, Image im, Image mask) {
	int i, j, x1, y1;
	int v = 0;
	PIX_TYPE mysum = 0.0;
	// PIX_TYPE a = 0.0;
	//PIX_TYPE b = 0.0;
	//PIX_TYPE cc = 0.0;
	// PIX_TYPE offsetx = x - mask->width/2.0  - 0.5;
	// PIX_TYPE offsety = y - mask->height/2.0 - 0.5;
	PIX_TYPE offsetx = x - mask->width*0.5 - 0.5;
	PIX_TYPE offsety = y - mask->height*0.5 - 0.5;
	int intoffsetx = TRUNC(offsetx);
	int intoffsety = TRUNC(offsety);
	mysum = 0.0;
	for (j = 0; j < mask->height; j++) {
		for (i = 0; i < mask->width; i++) {
			x1 = i + intoffsetx;
			y1 = j + intoffsety;
			mysum += ie(im, x1, y1, c) * IE(mask, i, j, c);
			//	printf("ie(im,x1,y1,c) is ;%f  %f\n",ie(im,x1,y1,c),IE(mask,i,j,c));
			//v += 111*122;
		}
	}
	//mysum=v/10000.0;
	//   printf("musum is %f\n",mysum);
	//exit(1);
	/*  if(EQUAL_ZERO(offsetx - TRUNC(offsetx),.01) && EQUAL_ZERO(offsety - TRUNC(offsety),.01)){
	mysum = 0.0;
	for(j = 0; j < mask->height; j++){
	for(i = 0; i < mask->width; i++){
	mysum += ie(im,i+TRUNC(offsetx),j+TRUNC(offsety),c) * IE(mask,i,j,c);
	}
	}
	}
	else{
	printf("%d    %f  %d\n",EQUAL_ZERO(offsetx - TRUNC(offsetx),.01),offsetx,TRUNC(offsetx));
	printf("%d    %f  %d\n",EQUAL_ZERO(offsety - TRUNC(offsety),.01),offsety,TRUNC(offsety));
	mysum = 0.0;
	for(j = 0; j < mask->height; j++){
	for(i = 0; i < mask->width; i++){
	//a=interpLinear(im,i+offsetx,j+offsety,c);
	//b=IE(mask,i,j,c);
	//cc=a*b;

	mysum += interpLinear(im,i+offsetx,j+offsety,c) * IE(mask,i,j,c);
	printf("in convolvePoint 2 interpLinear(im,i+offsetx,j+offsety,c) is:%f\n",a);
	printf("in convolvePoint 2 IE(mask,i,j,c) is:%f\n",b);
	printf("in convolvePoint 2 a*b is:%f\n",cc);
	printf("in convolvePoint 2 mysum is:%f\n",mysum);
	//v++;
	}
	}
	exit(1);
	}*/


	//  printf("in convolvePoint Ending ======= mysum is:%f=======================\n",mysum);
	return mysum;
}


//从文件中读取Gaborjet掩码参数
JetMasks readMasksFile(const char* filename) {
	JetMasks masks = NULL;
	int maskCount;
	FTYPE lambda, angle, phase, gama, sigma; int maskSize;
	int i;
	FILE* file = fopen(filename, "r");

	if (!file) {
		printf("Error opening file: %s", filename);
		exit(1);
	}

	/* Read in the number of Masks. */
	fscanf(file, "%d", &maskCount);
	masks = makeJetMasks(maskCount);//masks->size=maskcount;

									/* Read in mask parameters and create masks. */

	for (i = 0; i < maskCount; i++) {
		if (fscanf(file, "%lf %lf %lf %lf %lf %d", &lambda, &angle, &phase, &gama, &sigma, &maskSize) != 6) {
			printf("Error reading mask file: %s line: %d", filename, i);
			exit(1);
		}
		masks->params->wavelength[i] = lambda;
		masks->params->angle[i] = angle;
		masks->params->phase[i] = phase;
		masks->params->aspect[i] = gama;
		masks->params->radius[i] = sigma;
		masks->params->kx[i] = 2.0*PI*cos(angle) / lambda;
		masks->params->ky[i] = 2.0*PI*sin(angle) / lambda;
		masks->params->size[i] = maskSize;    //图像模板大小

		masks->masks[i] = makeGaborMask(lambda, angle, phase, gama, sigma, maskSize);
		ZeroMeanUnitLength(masks->masks[i]);
	}

	fclose(file);

	return masks;
}

//将Gaborjet掩码保存到文件中
void saveJetMasks(JetMasks masks, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	int i, j, k;
	int dnum = sizeof(FTYPE);
	if (!file) {
		printf("Error opening file: %s", filename);
		exit(1);
	}

	fwrite(&(masks->size), sizeof(int), 1, file);

	for (i = 0; i < masks->size; i++) {
		if (fwrite(&(masks->params->wavelength[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		if (fwrite(&(masks->params->angle[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		if (fwrite(&(masks->params->phase[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		if (fwrite(&(masks->params->aspect[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		if (fwrite(&(masks->params->radius[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		if (fwrite(&(masks->params->kx[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		if (fwrite(&(masks->params->ky[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		if (fwrite(&(masks->params->size[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}


		for (j = 0; j < masks->params->size[i]; j++) {
			for (k = 0; k< masks->params->size[i]; k++) {
				fwrite(&(IE(masks->masks[i], k, j, 0)), dnum, 1, file);

			}
		}

	}

	fclose(file);

}

//将Gaborjet掩码放大变为整数后保存到文件中，为了减少在嵌入式芯片上运行时间
void saveIntJetMasks(JetMasks masks, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	int i, j, k;
	int intmasks;
	int dnum = sizeof(FTYPE);
	int inum = sizeof(int);

	if (!file) {
		printf("Error opening file: %s", filename);
		exit(1);
	}

	fwrite(&(masks->size), sizeof(int), 1, file);

	for (i = 0; i < masks->size; i++) {
		if (fwrite(&(masks->params->wavelength[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}
		if (fwrite(&(masks->params->angle[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}
		if (fwrite(&(masks->params->phase[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}
		if (fwrite(&(masks->params->aspect[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}
		if (fwrite(&(masks->params->radius[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}
		if (fwrite(&(masks->params->kx[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}
		if (fwrite(&(masks->params->ky[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}
		if (fwrite(&(masks->params->size[i]), dnum, 1, file) != 1) {
			printf("Error ");
			exit(1);
		}

		for (j = 0; j < masks->params->size[i]; j++) {
			for (k = 0; k< masks->params->size[i]; k++) {
				intmasks = IE(masks->masks[i], k, j, 0) * 1000000;
				fwrite(&intmasks, inum, 1, file);

			}
		}

	}

	fclose(file);

}

//从文件中读取Gaborjet掩码
JetMasks readJetMasks(const char* filename)
{
	JetMasks masks = NULL;
	int maskCount;
	FILE* file = fopen(filename, "rb");
	int i, j, k;
	int dnum = sizeof(FTYPE);

	if (!file) {
		printf("Error opening file: %s", filename);
		exit(1);
	}

	fread(&maskCount, sizeof(int), 1, file);

	masks = makeJetMasks(maskCount);//masks->size=maskcount;

	for (i = 0; i < maskCount; i++) {
		if (fread(&(masks->params->wavelength[i]), dnum, 1, file) != 1) {
			printf("Error :masks->params->wavelength[i]:%f\n", masks->params->wavelength[i]);
			exit(1);
		}

		if (fread(&(masks->params->angle[i]), dnum, 1, file) != 1) {
			printf("Error 1 \n");
			exit(1);
		}

		if (fread(&(masks->params->phase[i]), dnum, 1, file) != 1) {
			printf("Error 2\n");
			exit(1);
		}

		if (fread(&(masks->params->aspect[i]), dnum, 1, file) != 1) {
			printf("Error 3\n");
			exit(1);
		}

		if (fread(&(masks->params->radius[i]), dnum, 1, file) != 1) {
			printf("Error 4\n");
			exit(1);
		}

		if (fread(&(masks->params->kx[i]), dnum, 1, file) != 1) {
			printf("Error 5\n");
			exit(1);
		}

		if (fread(&(masks->params->ky[i]), dnum, 1, file) != 1) {
			printf("Error 6\n");
			exit(1);
		}

		if (fread(&(masks->params->size[i]), dnum, 1, file) != 1) {
			printf("Error 7\n");
			exit(1);
		}

		masks->masks[i] = readGaborMask(masks->params->size[i]);

		for (j = 0; j < masks->params->size[i]; j++) {
			for (k = 0; k < masks->params->size[i]; k++) {
				if (fread(&(IE(masks->masks[i], k, j, 0)), dnum, 1, file) != 1) {
					printf("Error 9\n");
					exit(1);
				}
				//if(IE(masks->masks[i],k,j,0)>0.1)printf("IE(masks->masks[i],k,j,0):%f\n",IE(masks->masks[i],k,j,0));
			}
		}

	}

	fclose(file);
	return masks;
}

//从文件中读取整型Gaborjet掩码
IntJetMasks readIntJetMasks(const char* filename)
{
	IntJetMasks masks = NULL;
	int maskCount;
	FILE* file = fopen(filename, "rb");
	int i, j, k;
	int dnum = sizeof(FTYPE);
	int inum = sizeof(int);
	if (!file) {
		printf("Error opening file: %s", filename);
		exit(1);
	}

	fread(&maskCount, inum, 1, file);

	masks = makeIntJetMasks(maskCount);//masks->size=maskcount;

	for (i = 0; i < maskCount; i++) {
		if (fread(&(masks->params->wavelength[i]), dnum, 1, file) != 1) {
			printf("Error :masks->params->wavelength[i]:%f\n", masks->params->wavelength[i]);
			exit(1);
		}
		if (fread(&(masks->params->angle[i]), dnum, 1, file) != 1) {
			printf("Error 1 \n");
			exit(1);
		}

		if (fread(&(masks->params->phase[i]), dnum, 1, file) != 1) {
			printf("Error 2\n");
			exit(1);
		}

		if (fread(&(masks->params->aspect[i]), dnum, 1, file) != 1) {
			printf("Error 3\n");
			exit(1);
		}

		if (fread(&(masks->params->radius[i]), dnum, 1, file) != 1) {
			printf("Error 4\n");
			exit(1);
		}

		if (fread(&(masks->params->kx[i]), dnum, 1, file) != 1) {
			printf("Error 5\n");
			exit(1);
		}

		if (fread(&(masks->params->ky[i]), dnum, 1, file) != 1) {
			printf("Error 6\n");
			exit(1);
		}

		if (fread(&(masks->params->size[i]), dnum, 1, file) != 1) {
			printf("Error 7\n");
			exit(1);
		}

		masks->masks[i] = readIntGaborMask(masks->params->size[i]);

		for (j = 0; j < masks->params->size[i]; j++) {
			for (k = 0; k < masks->params->size[i]; k++) {
				if (fread(&(IE(masks->masks[i], k, j, 0)), inum, 1, file) != 1) {
					printf("Error 9\n");
					exit(1);
				}
				//if(IE(masks->masks[i],k,j,0)>0.1)printf("IE(masks->masks[i],k,j,0):%f\n",IE(masks->masks[i],k,j,0));
			}
		}

	}

	fclose(file);
	return masks;
}

/* 为size大小的图片生成空掩码 */
Image readGaborMask(int size)
{
	Image mask = makeImage(size, size, 1);

	return mask;
}

/* 为size大小的图片生成空整型掩码 */
IntImage readIntGaborMask(int size)
{
	IntImage mask = makeIntImage(size, size, 1);

	return mask;
}

/*根据输入参数为图片生成Gaborjet掩码 */
Image makeGaborMask(FTYPE lambda, FTYPE theta, FTYPE phi,
	FTYPE gamma, FTYPE sigma, int size)
{
	Image mask = makeImage(size, size, 1);
	int i, j;


	for (j = 0; j < size; j++) {
		for (i = 0; i < size; i++) {
			FTYPE x = size / 2.0 - size + i;
			FTYPE y = size / 2.0 - size + j;
			FTYPE xp = x*cos(theta) + y*sin(theta);
			FTYPE yp = -x*sin(theta) + y*cos(theta);
			FTYPE tmp1 = -(xp*xp + gamma*gamma*yp*yp) / (2 * sigma*sigma);
			FTYPE tmp2 = (2 * PI*xp / lambda) + phi;
			IE(mask, i, j, 0) = exp(tmp1)*(cos(tmp2) - (phi == 0.0)*(WiskottDCFree)*exp(-sigma*sigma*0.5));
		}
	}

	/* Rescale the pixel values to have a standard total length */
	ZeroMeanUnitLength(mask);

	return mask;
}

void ZeroMeanUnitLength(Image im) {
	int i, j, c;

	FTYPE mean = 0; FTYPE sqrsum = 0; FTYPE invlength = 0;

	for (j = 0; j < im->height; j++) {
		for (i = 0; i < im->width; i++) {
			for (c = 0; c < im->channels; c++) {
				mean += IE(im, i, j, c);
			}
		}
	}

	mean = mean / (im->height*im->width*im->channels);
	/* printf("mean: %f\n",mean); */

	for (j = 0; j < im->height; j++) {
		for (i = 0; i < im->width; i++) {
			for (c = 0; c < im->channels; c++) {
				IE(im, i, j, c) = IE(im, i, j, c) - mean;
				sqrsum += SQR(IE(im, i, j, c));
			}
		}
	}

	/* printf("sqrsum: %f\n",sqrsum); */
	if (sqrsum != 0) {
		invlength = 1.0 / sqrt(sqrsum);
	}
	else {
		invlength = 1.0;
	}

	for (j = 0; j < im->height; j++) {
		for (i = 0; i < im->width; i++) {
			for (c = 0; c < im->channels; c++) {
				IE(im, i, j, c) *= invlength;
			}
		}
	}

}



JetMasks makeJetMasks(int size) {
	JetMasks masks = (JetMasks)malloc(sizeof(jet_masks));
	assert(masks);

	masks->size = size;
	masks->masks = (Image*)malloc(sizeof(Image)*size);
	masks->params = makeJetParams(size);
	assert(masks->masks);

	return masks;
}

/*释放掩码分配的空间  */
void freeJetMasks(JetMasks masks) {
	int n;

	for (n = 0; n < masks->size; n++) {
		freeImage(masks->masks[n]);
	}
	freeJetParams(masks->params);
	free(masks->masks);
	free(masks);
}

IntJetMasks makeIntJetMasks(int size) {
	IntJetMasks masks = (IntJetMasks)malloc(sizeof(int_jet_masks));
	assert(masks);

	masks->size = size;
	masks->masks = (IntImage*)malloc(sizeof(IntImage)*size);
	masks->params = makeJetParams(size);
	assert(masks->masks);

	return masks;
}
/* This function frees space that was used to hold on to gabor
* masks */
void     freeIntJetMasks(IntJetMasks masks) {
	int n;

	for (n = 0; n < masks->size; n++) {
		freeIntImage(masks->masks[n]);
	}
	freeJetParams(masks->params);
	free(masks->masks);
	free(masks);
}



/*为图片分配存储空间 */
Image makeImage(int width, int height, int channels) {
	int x, y;
	Image i = (Image)malloc(sizeof(image));
	assert(i);
	i->data = (PIX_TYPE***)malloc(sizeof(PIX_TYPE**)*width);
	assert(i->data);

	for (x = 0; x < width; x++) {
		i->data[x] = (PIX_TYPE**)malloc(sizeof(PIX_TYPE*)*height);
		assert(i->data[x]);
		for (y = 0; y < height; y++) {
			i->data[x][y] = (PIX_TYPE*)malloc(sizeof(PIX_TYPE)*channels);
			assert(i->data[x][y]);
		}
	}

	i->width = width;
	i->height = height;
	i->channels = channels;

	return i;
}

/*释放图片的存储空间 */
void freeImage(Image i) {
	int x, y;
	if (i == 0) {
		printf("Warning: free null image\n");
		return;
	}
	for (x = 0; x < i->width; x++) {
		for (y = 0; y < i->height; y++) {
			free(i->data[x][y]);
		}
		free(i->data[x]);
	}
	free(i->data);
	free(i);
}

IntImage makeIntImage(int width, int height, int channels) {
	int x, y;
	IntImage i = (IntImage)malloc(sizeof(intimage));
	assert(i);
	i->data = (int***)malloc(sizeof(int**)*width);
	assert(i->data);

	for (x = 0; x < width; x++) {
		i->data[x] = (int**)malloc(sizeof(int*)*height);
		assert(i->data[x]);
		for (y = 0; y < height; y++) {
			i->data[x][y] = (int*)malloc(sizeof(int)*channels);
			assert(i->data[x][y]);
		}
	}

	i->width = width;
	i->height = height;
	i->channels = channels;

	return i;
}

void freeIntImage(IntImage i) {
	int x, y;
	if (i == 0) {
		printf("Warning: free null image\n");
		return;
	}
	for (x = 0; x < i->width; x++) {
		for (y = 0; y < i->height; y++) {
			free(i->data[x][y]);
		}
		free(i->data[x]);
	}
	free(i->data);
	free(i);
}

//为Gaborjet参数分配空间
GaborJetParams makeJetParams(int length) {
	GaborJetParams params = (GaborJetParams)malloc(sizeof(gabor_jet_params));
	assert(params);

	params->length = length;

	params->wavelength = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
	params->angle = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
	params->phase = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
	params->aspect = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
	params->radius = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
	params->kx = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
	params->ky = (GFTYPE*)malloc(sizeof(GFTYPE)*length);
	params->size = (int*)malloc(sizeof(int)*length);

	return params;
}

//释放Gaborjet参数存储空间
void freeJetParams(GaborJetParams params) {

	free(params->wavelength);
	free(params->angle);
	free(params->phase);
	free(params->aspect);
	free(params->radius);
//	free(params->size);//如果有会报错，不知道原因
	free(params->kx);
	free(params->ky);

	free(params);
}

/*===============================Bunch======================================================*/


// 给束图分配空间
JetBunch makeJetBunch() {
	JetBunch jb = ALLOCATE(jet_bunch, 1);
	jb->allocsize = 0; /* only allocate memory if a jet is added. */
	jb->size = 0;

	jb->jets = ALLOCATE(GaborJet, jb->allocsize);
	return jb;
}

//释放束图存储空间
void freeJetBunch(JetBunch jb) {
	int i;
	for (i = 0; i < jb->size; i++) {
		if (jb->jets[i]) freeGaborJet(jb->jets[i]);
	}
	free(jb->jets);
	free(jb);
}

//给束图中添加一个Gaborjet
int addJetToBunch(JetBunch bunch, GaborJet jet) {
	if (bunch->size >= bunch->allocsize) {
		int i;
		GaborJet* tmpjets = ALLOCATE(GaborJet, bunch->allocsize + JET_BUNCH_DEFAULT_SIZE);
		bunch->allocsize = bunch->allocsize + JET_BUNCH_DEFAULT_SIZE;
		for (i = 0; i < bunch->size; i++) {
			tmpjets[i] = bunch->jets[i];
		}
		free(bunch->jets);
		bunch->jets = tmpjets;
	}
	bunch->jets[bunch->size] = jet;
	bunch->size++;
	return bunch->size - 1;
}



/******************************* Face Graph ********************************/

static int alloc_faceGraph = 0;

//分配FaceGraph结构体存储空间
FaceGraph makeFaceGraph(int geosize, int totalsize) {
	int i;
	FaceGraph fg = ALLOCATE(face_graph, 1);
	assert(fg);
	fg->geosize = geosize;
	fg->totalsize = totalsize;
	fg->params = NULL;
	fg->jets = ALLOCATE(GaborJet, totalsize);
	for (i = 0; i < totalsize; i++) {
		fg->jets[i] = NULL;
	}

	alloc_faceGraph++;
	return fg;
}

//释放FaceGraph结构体存储空间
void freeFaceGraph(FaceGraph fg) {
	int i;
	for (i = 0; i < fg->totalsize; i++) {
		if (fg->jets[i] != NULL) {
			freeGaborJet(fg->jets[i]);
		}
	}
	free(fg->jets);
	free(fg);
	alloc_faceGraph--;
}

void  writeint(FILE* file, int i) {
	fprintf(file, "%d\n", i);
	//  char en='\n';
	//  fwrite(&i,sizeof(int),1,file);
	//    fwrite(&en,sizeof(char),1,file);
}

void  writereal(FILE* file, FTYPE r) {
	fprintf(file, "%f\n", r);
	//   fwrite(&r,sizeof(FTYPE),1,file);
}

int  readint(FILE* file) {
	int i;
	fscanf(file, "%d", &i);
	// fread(&i,sizeof(int),1,file);
	return i;
}

FTYPE  readreal(FILE* file) {
	float f;
	fscanf(file, "%f", &f);
	return f;
}

//计算两幅图片的相似度
int compareFaceGraph(FaceGraph fg1, FaceGraph fg2) {
	int i, j;

	for (i = 0; i < fg1->params->length; i++) {
		if (fg1->params->wavelength[i] != fg2->params->wavelength[i])printf("Error");

		if (fg1->params->angle[i] != fg2->params->angle[i])printf("Error");

		if (fg1->params->kx[i] != fg2->params->kx[i])printf("Error");
		if (fg1->params->ky[i] != fg2->params->kx[i])printf("Error");
	}

	for (i = 0; i < fg1->totalsize; i++) {
		if (fg1->jets[i]->x != fg2->jets[i]->x)printf("%f   %f\n", fg1->jets[i]->x, fg2->jets[i]->x);
		if (fg1->jets[i]->y != fg2->jets[i]->y)printf("%f   %f\n", fg1->jets[i]->y, fg2->jets[i]->y);

		for (j = 0; j < fg1->jets[i]->length; j++) {
			if (fg1->jets[i]->realPart[j] != fg2->jets[i]->realPart[j])printf("%f   %f\n", fg1->jets[i]->realPart[j], fg2->jets[i]->realPart[j]);

			if (fg1->jets[i]->imagPart[j] != fg2->jets[i]->imagPart[j])printf("%f   %f\n", fg1->jets[i]->imagPart[j], fg2->jets[i]->imagPart[j]);

			if (fg1->jets[i]->mag[j] != fg2->jets[i]->mag[j])printf("%f   %f\n", fg1->jets[i]->mag[j], fg2->jets[i]->mag[j]);
			if (fg1->jets[i]->ang[j] != fg2->jets[i]->ang[j])printf("%f   %f\n", fg1->jets[i]->ang[j], fg2->jets[i]->ang[j]);
		}
	}

	return 0;
}

//从文件中读取图片的所有特征点的特征值
FaceGraph loadFaceGraph(const char *filename) {
	int i, j;
	int totalsize, geosize, params;
	int gaborsize;
	int length;
	int dnum = sizeof(FTYPE);
	int inum = sizeof(int);
	FaceGraph fg;
	FILE* file = fopen(filename, "rb");

	if (file == NULL)
	{
		printf("open : %s   failed \n", filename);
		exit(1);
	}
	// assert(file);
	/* Write out the size */
	fread(&geosize, inum, 1, file);
	fread(&totalsize, inum, 1, file);

	fg = makeFaceGraph(geosize, totalsize);

	fread(&params, inum, 1, file);

	if (params)
	{
		fread(&gaborsize, inum, 1, file);
		fg->params = makeJetParams(gaborsize);

		for (i = 0; i < fg->params->length; i++) {
			fread(&(fg->params->wavelength[i]), dnum, 1, file);
			fread(&(fg->params->angle[i]), dnum, 1, file);
			fread(&(fg->params->phase[i]), dnum, 1, file);
			fread(&(fg->params->aspect[i]), dnum, 1, file);
			fread(&(fg->params->radius[i]), dnum, 1, file);
			fread(&(fg->params->size[i]), inum, 1, file);
			fread(&(fg->params->kx[i]), dnum, 1, file);
			fread(&(fg->params->ky[i]), dnum, 1, file);
		}
	}

	for (i = 0; i < fg->totalsize; i++) {
		/* write out jet i */
		fread(&length, inum, 1, file);
		fg->jets[i] = makeGaborJet(length);
		fg->jets[i]->params = fg->params;
		fread(&(fg->jets[i]->x), dnum, 1, file);
		fread(&(fg->jets[i]->y), dnum, 1, file);

		for (j = 0; j < fg->jets[i]->length; j++) {
			fread(&(fg->jets[i]->realPart[j]), dnum, 1, file);
			fread(&(fg->jets[i]->imagPart[j]), dnum, 1, file);
			fread(&(fg->jets[i]->mag[j]), dnum, 1, file);
			fread(&(fg->jets[i]->ang[j]), dnum, 1, file);
		}
	}
	fclose(file);
	return fg;
}

//保存图片的所有特征点的特征值
void saveFaceGraph(char *filename, FaceGraph fg) {
	int i, j, y;
	int dnum = sizeof(FTYPE);
	int inum = sizeof(int);
	FILE* file = fopen(filename, "wb");
	//assert(file);
	/* Write out the size */
	fwrite(&(fg->geosize), inum, 1, file);
	fwrite(&(fg->totalsize), inum, 1, file);
	if (fg->params != NULL)y = 1;
	else y = 0;
	fwrite(&y, inum, 1, file);

	if (fg->params) {
		fwrite(&(fg->params->length), inum, 1, file);
		for (i = 0; i < fg->params->length; i++) {
			fwrite(&(fg->params->wavelength[i]), dnum, 1, file);
			fwrite(&(fg->params->angle[i]), dnum, 1, file);
			fwrite(&(fg->params->phase[i]), dnum, 1, file);
			fwrite(&(fg->params->aspect[i]), dnum, 1, file);
			fwrite(&(fg->params->radius[i]), dnum, 1, file);
			fwrite(&(fg->params->size[i]), inum, 1, file);
			fwrite(&(fg->params->kx[i]), dnum, 1, file);
			fwrite(&(fg->params->ky[i]), dnum, 1, file);
		}
	}

	for (i = 0; i < fg->totalsize; i++) {
		/* write out jet i */
		fwrite(&(fg->jets[i]->length), inum, 1, file);
		fwrite(&(fg->jets[i]->x), dnum, 1, file);
		fwrite(&(fg->jets[i]->y), dnum, 1, file);
		for (j = 0; j < fg->jets[i]->length; j++) {
			fwrite(&(fg->jets[i]->realPart[j]), dnum, 1, file);

			fwrite(&(fg->jets[i]->imagPart[j]), dnum, 1, file);

			//   printf("In saveFile realPart is:%f,imagPart is:%f\n",fg->jets[i]->realPart[j],fg->jets[i]->imagPart[j]);
			fwrite(&(fg->jets[i]->mag[j]), dnum, 1, file);
			fwrite(&(fg->jets[i]->ang[j]), dnum, 1, file);
		}
	}

	fclose(file);
}



/*===============================================================================*/
void* csu_allocate(const char* type, int sof, int num, const char* file, int line) {
	void* tmp = (void*)malloc(sof*num);
	if (tmp == NULL) {
		printf("ERROR: failed to allocate memory.\n"
			"       type: %s - number: %d\n"
			"       in <%s:%d>\n", type, num, file, line);
		exit(1);
	}
	return tmp;
}

GraphDiscription readPosition(const char* filename) {
	FILE* file;
	int i;
	char str[120];

	GraphDiscription gd = ALLOCATE(graph_discription, 1);

	file = fopen(filename, "r");
	std::cout << "readPosition"<<filename;
//	printf("%s\n",filename);


	/* Read version of the file */
	fscanf(file, "%s %d", str, &(gd->numVert));
//	printf("%s %d\n",str,gd->numVert);
	std::cout << str << "str\n" ;
	std::cout << " 11111" ;

	/* Read the number of vericies in the file */
	if (fscanf(file, "%s %d", str, &(gd->numVert)) != 2)printf("Error: could not determin the number of verticies in file %s\n", filename);
//	printf("%s %d\n",str,gd->numVert);

	/*read '{'*/
	fscanf(file, "%s", str);
//	printf("%s\n",str);

	/* allocate space for vericies and load discriptions */

	std::cout << "gd-<numVert" << gd->numVert;
	gd->verts = ALLOCATE(Vert, gd->numVert);
	gd->vertLabels = ALLOCATE(char*, gd->numVert);
	gd->bunch = ALLOCATE(JetBunch, gd->numVert);
	for (i = 0; i < gd->numVert; i++) {
		gd->vertLabels[i] = ALLOCATE(char, LABELMAX);
		if (fscanf(file, "%lf %lf", &(gd->verts[i].x), &(gd->verts[i].y)) != 2)printf("Error Parsing vertex %d in file: %s\n", i, filename);
		gd->bunch[i] = makeJetBunch();

	}

	gd->numEdge = 0;
	gd->edges = NULL;

	fclose(file);

	return gd;
	std::cout <<"readPosition over\n";
}


/*****************************************************************************
从文件中读取特征点位置
****************************************************************************/
GraphDiscription readGraphDiscription(const char* filename) {
	FILE* file;
	int i;
	char str[120];

	GraphDiscription gd = ALLOCATE(graph_discription, 1);

	file = fopen(filename, "r");

	printf("%s\n",filename);


	/* Read version of the file */
	fscanf(file, "%s %d", str, &(gd->numVert));
//	printf("%s %d\n",str,gd->numVert);

	/* Read the number of vericies in the file */
	if (fscanf(file, "%s %d", str, &(gd->numVert)) != 2)printf("Error: could not determin the number of verticies in file %s\n", filename);
//	printf("%s %d\n",str,gd->numVert);

	/*read '{'*/
	fscanf(file, "%s", str);
//	printf("%s\n",str);

	/* allocate space for vericies and load discriptions */
	gd->verts = ALLOCATE(Vert, gd->numVert);
	gd->vertLabels = ALLOCATE(char*, gd->numVert);
	gd->bunch = ALLOCATE(JetBunch, gd->numVert);
	for (i = 0; i < gd->numVert; i++) {
		gd->vertLabels[i] = ALLOCATE(char, LABELMAX);
		if (fscanf(file, "%lf %lf", &(gd->verts[i].x), &(gd->verts[i].y)) != 2)printf("Error Parsing vertex %d in file: %s\n", i, filename);
		gd->bunch[i] = makeJetBunch();

	}
	/*read '}'*/
	fscanf(file, "%s", str);
//	printf("%s\n",str);

	/* Read the number of edges in the file */
	if (fscanf(file, "%d", &(gd->numEdge)) != 1)printf("Error: could not determin the number of edges in file %s\n", filename);

	/* allocate space for vericies and load discriptions */
	gd->edges = ALLOCATE(Edge, gd->numEdge);
	for (i = 0; i < gd->numEdge; i++) {
		if (fscanf(file, "%d %d", &(gd->edges[i].vert1), &(gd->edges[i].vert2)) != 2)printf("Error Parsing vertex %d in file: %s\n", i, filename);
	}
	fclose(file);

	return gd;
}

/*****************************************************************************
从文件中读取特征点位置
****************************************************************************/
GraphDiscription readBinaryGraphDiscription(const char* filename) {
	FILE* file;
	int i;
	char str[120];
	int inum = sizeof(int);
	int dnum = sizeof(FTYPE);
	GraphDiscription gd = ALLOCATE(graph_discription, 1);

	file = fopen(filename, "rb");


	/* Read the number of vericies in the file */
	fread(&(gd->numVert), inum, 1, file);

	/* allocate space for vericies and load discriptions */
	gd->verts = ALLOCATE(Vert, gd->numVert);
	gd->vertLabels = ALLOCATE(char*, gd->numVert);
	gd->bunch = ALLOCATE(JetBunch, gd->numVert);
	for (i = 0; i < gd->numVert; i++) {
		gd->vertLabels[i] = ALLOCATE(char, LABELMAX);
		fread(&(gd->verts[i].x), dnum, 1, file);
		fread(&(gd->verts[i].y), dnum, 1, file);

		gd->bunch[i] = makeJetBunch();

	}

	/* Read the number of edges in the file */
	fread(&(gd->numEdge), inum, 1, file);

	/* allocate space for vericies and load discriptions */
	gd->edges = ALLOCATE(Edge, gd->numEdge);
	for (i = 0; i < gd->numEdge; i++) {
		fread(&(gd->edges[i].vert1), inum, 1, file);
		fread(&(gd->edges[i].vert2), inum, 1, file);
	}
	fclose(file);

	return gd;
}

/*保存gd结构体中的特征点位置数据
*/
void saveGraphDiscription(const char* filename, GraphDiscription gd) {
	FILE* file;
	int i;
	file = fopen(filename, "w");

	/* Read the number of vericies in the file */
	// fprintf(file,"%d\n",(gd->numVert));

	/* Write version of the file */
	fprintf(file, "version: 1\n");

	/* Write the number of vericies in the file */
	fprintf(file, "n_points: %d\n", (gd->numVert));

	/* Write '{'*/
	fprintf(file, "{\n");

	/* allocate space for vericies and load discriptions */
	for (i = 0; i < gd->numVert; i++) {
		//fprintf(file, "%s %f %f\n",  gd->vertLabels[i], (gd->verts[i].x), (gd->verts[i].y));
		fprintf(file, "%f %f\n", (gd->verts[i].x), (gd->verts[i].y));
	}

	/* Write '}'*/
	fprintf(file, "}\n");

	/* Read the number of edges in the file */
	fprintf(file, "%d\n", (gd->numEdge));

	/* allocate space for vericies and load discriptions */
	for (i = 0; i < gd->numEdge; i++) {
		fprintf(file, "%d %d\n", (gd->edges[i].vert1), (gd->edges[i].vert2));
	}


	fclose(file);
}


/*以二进制方式保存gd结构体中的特征点位置数据到文件中
*/
void saveBinaryGraphDiscription(const char* filename, GraphDiscription gd) {
	FILE* file;
	int i;

	int inum = sizeof(int);
	int dnum = sizeof(FTYPE);
	file = fopen("d://luo//1.pts","wb");
	char* source = "d://luo//1.pts";
	char* destination = (char*) filename;
//	CopyFile(source, destination, FALSE);
	double res;
	try
	{
//		file = fopen("bpnn/outcome/BioID_0000_position.pts", "wb");
		file = fopen(filename, "wb");
	}
	catch (const std::exception& e)
	{
		std::cout<<e.what() << std::endl;
	}
	


	printf("SaveBinaryGraphDiscription:%s\n", filename);

	/* Write the number of vericies in the file */
	fwrite(&(gd->numVert), inum, 1, file);

	/* allocate space for vericies and load discriptions */
	for (i = 0; i < gd->numVert; i++) {
		fwrite(&(gd->verts[i].x), dnum, 1, file);
		fwrite(&(gd->verts[i].y), dnum, 1, file);
	}

	/* Read the number of edges in the file */
	fwrite(&(gd->numEdge), inum, 1, file);

	/* allocate space for vericies and load discriptions */
	for (i = 0; i < gd->numEdge; i++) {
		fwrite(&(gd->edges[i].vert1), inum, 1, file);
		fwrite(&(gd->edges[i].vert2), inum, 1, file);
	}


	fclose(file);
}

/*从文件中读取人脸束图 */
GraphDiscription readBunchGraph(const char* filename, JetMasks masks) {
	FILE* file;
	int i, j, k;
	int inum = sizeof(int);
	int dnum = sizeof(FTYPE);
	char str[120];

	GraphDiscription gd = ALLOCATE(graph_discription, 1);

	printf("Read BunchGraph :%s\n", filename);

	file = fopen(filename, "rb");

	/* Read the number of vericies in the file */
	fread(&(gd->numVert), inum, 1, file);

//	printf("%d\n",gd->numVert);


	/* allocate space for vericies and load discriptions */
	gd->verts = ALLOCATE(Vert, gd->numVert);
	gd->vertLabels = ALLOCATE(char*, gd->numVert);
	gd->bunch = ALLOCATE(JetBunch, gd->numVert);
	for (i = 0; i < gd->numVert; i++) {
		gd->vertLabels[i] = ALLOCATE(char, LABELMAX);

		fread(&(gd->verts[i].x), dnum, 1, file);
		fread(&(gd->verts[i].y), dnum, 1, file);

		gd->bunch[i] = makeJetBunch();

	}


	/* Read the number of edges in the file */
	fread(&(gd->numEdge), inum, 1, file);

	/* allocate space for vericies and load discriptions */
	gd->edges = ALLOCATE(Edge, gd->numEdge);
	// gd->edges = (Edge *)malloc(gd->numEdge*sizeof(Edge));
	for (i = 0; i < gd->numEdge; i++) {
		fread(&(gd->edges[i].vert1), inum, 1, file);
		fread(&(gd->edges[i].vert2), inum, 1, file);


		printf("In ReadBunchJet gd->numEdge:%d %d\n",gd->edges[i].vert1,gd->edges[i].vert2);
	}

	for (i = 0; i < gd->numVert; i++) {
		if (fread(&(gd->bunch[i]->size), inum, 1, file) != 1)printf("Error \n");

		gd->bunch[i]->jets = ALLOCATE(GaborJet, gd->bunch[i]->size);

		for (j = 0; j<gd->bunch[i]->size; j++) {
			int length;
			if (fread(&(length), inum, 1, file) != 1)printf("Error \n");

			gd->bunch[i]->jets[j] = makeGaborJet(length);
			gd->bunch[i]->jets[j]->params = NULL;
			// gd->bunch[i]->jets[j]->params=makeJetParams(masks->params->length);

			// printf("In readBunchJet     masks-length:%d\n",masks->params->length);
			gd->bunch[i]->jets[j]->params = masks->params;

			if (fread(&(gd->bunch[i]->jets[j]->x), dnum, 1, file) != 1)printf("Error \n");
			if (fread(&(gd->bunch[i]->jets[j]->y), dnum, 1, file) != 1)printf("Error \n");

			for (k = 0; k < gd->bunch[i]->jets[j]->length; k++) {
				if (fread(&(gd->bunch[i]->jets[j]->realPart[k]), dnum, 1, file) != 1)printf("Error \n");
				if (fread(&(gd->bunch[i]->jets[j]->imagPart[k]), dnum, 1, file) != 1)printf("Error \n");
				if (fread(&(gd->bunch[i]->jets[j]->mag[k]), dnum, 1, file) != 1)printf("Error \n");
				if (fread(&(gd->bunch[i]->jets[j]->ang[k]), dnum, 1, file) != 1)printf("Error \n");
			}
		}
	}

	fclose(file);

	return gd;
}

/*保存人脸束图到文件filename中 */
void saveBunchGraph(const char* filename, GraphDiscription gd) {
	FILE* file;
	int i, j, k;
	int inum = sizeof(int);
	int dnum = sizeof(FTYPE);
	file = fopen(filename, "wb");


	/* Write the number of vericies in the file */
	fwrite(&(gd->numVert), inum, 1, file);


	/* allocate space for vericies and load discriptions */
	for (i = 0; i < gd->numVert; i++) {
		fwrite(&(gd->verts[i].x), dnum, 1, file);
		fwrite(&(gd->verts[i].y), dnum, 1, file);
	}


	/* write the number of edges in the file */
	fwrite(&(gd->numEdge), inum, 1, file);

	/* allocate space for vericies and load discriptions */
	for (i = 0; i < gd->numEdge; i++) {
		fwrite(&(gd->edges[i].vert1), inum, 1, file);
		fwrite(&(gd->edges[i].vert2), inum, 1, file);
	}

	for (i = 0; i < gd->numVert; i++) {
		fwrite(&(gd->bunch[i]->size), inum, 1, file);

		for (j = 0; j<gd->bunch[i]->size; j++) {
			//	printf("In save BunchGraph (gd->bunch[i]->jets[j]->length):%d\n",(gd->bunch[i]->jets[j]->length));
			fwrite(&(gd->bunch[i]->jets[j]->length), inum, 1, file);
			fwrite(&(gd->bunch[i]->jets[j]->x), dnum, 1, file);
			fwrite(&(gd->bunch[i]->jets[j]->y), dnum, 1, file);

			for (k = 0; k < gd->bunch[i]->jets[j]->length; k++) {
				fwrite(&(gd->bunch[i]->jets[j]->realPart[k]), dnum, 1, file);
				fwrite(&(gd->bunch[i]->jets[j]->imagPart[k]), dnum, 1, file);

				fwrite(&(gd->bunch[i]->jets[j]->mag[k]), dnum, 1, file);
				fwrite(&(gd->bunch[i]->jets[j]->ang[k]), dnum, 1, file);
			}
		}
	}
	fclose(file);
}

void freeGraphDiscription(GraphDiscription gd) {
	int i;
//	printf("NAOCAN\n");
	for (i = 0; i < gd->numVert; i++) {
		free(gd->vertLabels[i]);
		freeJetBunch(gd->bunch[i]);
	}

	free(gd->bunch);
	free(gd->verts);
	free(gd->vertLabels);
	free(gd->edges);
	free(gd);
}

