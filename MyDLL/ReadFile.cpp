/*
* =====================================================================================
*
*  Filename:  ReadFile.h
*
*  Description: 读取个各种文件中的数据，如pgm图片、一些配置文件中数据. 提供标准化图片的函数
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
#include "stdafx.h"
#include "ReadFile.h"
#include "iostream"
#include "sys/stat.h"


/*读取pgm图片文件，返回图片数据
* filename: pgm图片文件名称
*/
Image readPGMImage(const char* filename) {
	int  width, height, max, x, y;
	unsigned  val;
	char fchar ;
	char line[100];
	char ftype[16];
	FILE *infile;
	Image im;
//	std::cout << "readPGMImage:" << filename;
	/* Read first line, and test if it is propoer PNM type */

	infile = (FILE*)fopen(filename, "rb");
	printf("%s\n", filename);
	assert(infile);

	fgets(line, 100, infile);
	sscanf(line, " %s", ftype);

	if (!(strcmp(ftype, "P5") == 0)) {
		printf("Error <%s,%d>: Currenlty only binary pgm files (%s,|%s| is NOT), type P5, supported", __FILE__, __LINE__, filename, ftype);
		fflush(stdout);
		exit(1);
	}

	/* Read lines, ignoring those starting with Comment Character, until the
	Image Dimensions are read. */
	fchar = '#';
	while (fchar == '#') {
		fgets(line, 10, infile);
		sscanf(line, " %c", &fchar);
	}
	sscanf(line, " %d", &width);

	int temp=0;
	sscanf(line, " %*d%d", &temp);
	if(temp==0){
		fchar = '#';
		while (fchar == '#') {
			fgets(line, 10, infile);//100
			sscanf(line, " %c", &fchar);
		}
		sscanf(line, " %d", &height);
	}else{
		height=temp;
	}

	/* Read lines, ignoring those starting with Comment Character, until the
	maximum pixel value is read. */
	fchar = '#';
	while (fchar == '#') {
		fgets(line, 100, infile);
		sscanf(line, " %c", &fchar);
	}
	sscanf(line, "%d", &max);

	printf("%d,%d,%d\n", width, height, max);

	if (!(max == 255)) {
		fprintf(stdout, "readImagePGM: Warning, max value %d for pixels in image %s is not 255\n", max, filename); fflush(stdout);
	}

	im = makeImage(width, height, 1);

	val = fgetc(infile);
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (val == EOF) {
				printf("Error <%s,%d>: Unexpected end of file: %s", __FILE__, __LINE__, filename); fflush(stdout);
				exit(1);
			}

			IE(im, x, y, 0) = (unsigned char)val;
			val = fgetc(infile);
		}
	}

	std::cout << "readpgmfile over!\n";
	return(im);
	fclose(infile);

	
	
	
}


/*将图片数据写入pgm文件
* im : 数据
* channel: 是256还是RGB
* filename: pgm图片文件名称
*/
void  writePGMImage(const Image im, const char* filename, int channel) {
	FILE* outfile = fopen(filename, "wb");
	unsigned char pixel;
	FTYPE pmax, pmin;
	int x, y;

	assert(im);
	if (!outfile) {
		printf("could not open %s for writing.\n", filename);
		exit(1);
	}
	fprintf(outfile, "P5\n");
	fprintf(outfile, "%d\n%d\n", im->width, im->height);
	fprintf(outfile, "%d\n", 255);

	pmax = IE(im, 0, 0, channel);
	pmin = IE(im, 0, 0, channel);

	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			pmax = MAX(IE(im, x, y, channel), pmax);
			pmin = MIN(IE(im, x, y, channel), pmin);
		}
	}
	/*printf(" Min:%f Max:%f \n", pmin, pmax);*/

	for (y = 0; y < im->height; y++) {
		for (x = 0; x < im->width; x++) {
			if (pmax - pmin <= 0) {
				pixel = 0;
			}
			else {
				int tmp = ROUND(255 * (IE(im, x, y, channel) - pmin) / (pmax - pmin));
				printf("%f ",IE(im,x,y,channel));
				if (tmp < 0) tmp = 0;
				if (tmp > 255) tmp = 255;
				pixel = (unsigned char)tmp;
			}
			fputc(pixel, outfile);
		}
	}

	//exit(1);
	fclose(outfile);
}

/*将图片数据写入pgm文件
* im : 数据
* channel: 是256还是RGB
* filename: pgm图片文件名称
*/
void  writePPMImage(const Image im, const char* filename) {
	FILE* outfile = fopen(filename, "wb");
	unsigned char pixel;
	FTYPE pmax, pmin;
	int x, y, c;

	assert(im);
	assert(im->channels == 3);
	if (!outfile) {
		printf("could not open %s for writing.\n", filename);
		exit(1);
	}
	fprintf(outfile, "P6\n");
	fprintf(outfile, "%d %d\n", im->width, im->height);
	fprintf(outfile, "%d\n", 255);

	pmax = IE(im, 0, 0, 0);
	pmin = IE(im, 0, 0, 0);

	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				pmax = MAX(IE(im, x, y, c), pmax);
				pmin = MIN(IE(im, x, y, c), pmin);
			}
		}
	}

	for (y = 0; y < im->height; y++) {
		for (x = 0; x < im->width; x++) {
			for (c = 0; c < im->channels; c++) {
				if (pmax - pmin <= 0) {
					pixel = 0;
				}
				else {
					int tmp = ROUND(255 * (IE(im, x, y, c) - pmin) / (pmax - pmin));
					if (tmp < 0) tmp = 0;
					if (tmp > 255) tmp = 255;
					pixel = (unsigned char)tmp;
				}
				fputc(pixel, outfile);
			}
		}
	}


	fclose(outfile);
}

/**********************Image Nomalize******************************/


/*为预处理图片输入参数
*/
void preProocess(const char* eyeFile, const  char* inputDir, const char* pgmDir, Arguments* args) {
	int i;
	int param_num = 0;

	args->geoType = CL_YES;
	args->histType = HIST_POST;
	args->nrmType = CL_YES;
	args->preNormType = CL_NO;
	args->maskType = CL_YES;
	args->preEdge = EDGE_PRE;
	args->postEdge = EDGE_POST;

	args->sizeWidth = SIZE_WIDTH;
	args->sizeHeight = SIZE_HEIGHT;

	args->eyeLx = EYE_LX;
	args->eyeLy = EYE_LY;
	args->eyeRx = EYE_RX;
	args->eyeRy = EYE_RY;

	args->scale = SCALE;
	args->noise = NOISE;
	args->shiftX = SHIFT_X;
	args->shiftY = SHIFT_Y;
	args->reflect = CL_NO;
	args->configSuffix = CL_NO;

	args->ellipseX = ELLIPSE_X;
	args->ellipseY = ELLIPSE_Y;
	args->ellipseA = ELLIPSE_A;
	args->ellipseB = ELLIPSE_B;

	args->inputDir = inputDir;
	args->nrmDir = NULL;
	args->sfiDir = NULL;
	args->pgmDir = pgmDir;
	args->eyeFile = eyeFile;


}

/*  */
FTYPE ellipseFunc(FTYPE i, FTYPE j, FTYPE x, FTYPE y, FTYPE a, FTYPE b) {
	return 1 - (SQR(x - i) / SQR(a) + SQR(y - j) / SQR(b));
}

char** generateMask(int width, int height, FTYPE x, FTYPE y, FTYPE a, FTYPE b) {
	char **mask;
	int i, j;

	mask = (char**)malloc(sizeof(char*)*width);
	assert(mask); /* problem allocating space for mask */

	for (i = 0; i < width; i++) {
		mask[i] = (char*)malloc(sizeof(char)*height);
		assert(mask[i]); /* problem allocating space for mask */
	}

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (ellipseFunc(i, j, x, y, a, b) > 0) {
				mask[i][j] = 1;
			}
			else {
				mask[i][j] = 0;
			}
		}
	}

	return mask;
}


/********************* Geometric Transformation Code *************************/

/*
* 数学转换，将一个x*y图片转换为a1*y1的图片
*/
Image transformImage(Image source, int newWidth, int newHeight, const Matrix m) {
	Image dest = makeImage(newWidth, newHeight, source->channels);//build a new image which size is new
	Matrix point = makeMatrix(3, 1);
	Matrix inv;
	Matrix pt;
	int x, y, c;
	int x1, y1;

	assert(m->row_dim == 3);
	assert(m->col_dim == 3);

	/* initialize homogenius point */
	ME(point, 2, 0) = 1;

	/* find the inverse transformation to convert from dest to source */
	inv = invertRREF(m);

	for (x = 0; x < dest->width; x++) {
		for (y = 0; y < dest->height; y++) {
			/* calculate source point */
			ME(point, 0, 0) = x;
			ME(point, 1, 0) = y;
			ME(point, 2, 0) = 1.0;
			pt = multiplyMatrix(inv, point);
			ME(pt, 0, 0) = ME(pt, 0, 0) / ME(pt, 2, 0);
			ME(pt, 1, 0) = ME(pt, 1, 0) / ME(pt, 2, 0);

			for (c = 0; c < dest->channels; c++) {
				/* interpolate value */
				// IE(dest,x,y,c) = interpLinear(source, ME(pt,0,0),ME(pt,1,0),c);//return ME(pt,0,0) ME(pt,1,0) average value
				x1 = TRUNC(ME(pt, 0, 0));
				y1 = TRUNC(ME(pt, 1, 0));
				IE(dest, x, y, c) = ie(source, x1, y1, c);
			}
			/* clean up */
			freeMatrix(pt);
		}
	}
	freeMatrix(point);
	freeMatrix(inv);

	return dest;
}

/* Return a translation matrix
1 0 0
0 1 0
dx dy 1
*/
Matrix translateMatrix(FTYPE dx, FTYPE dy) {
	Matrix transform = makeIdentityMatrix(3);//makeMatrix(3,3);

	ME(transform, 0, 2) = dx;
	ME(transform, 1, 2) = dy;

	return transform;
}

/* Return a scale Matrix */
Matrix scaleMatrix(FTYPE s) {
	Matrix transform = makeIdentityMatrix(3);
	ME(transform, 0, 0) = s;
	ME(transform, 1, 1) = s;

	return transform;
}

/* Return a rotation matrix */
Matrix rotateMatrix(FTYPE theta) {
	Matrix transform = makeIdentityMatrix(3);

	ME(transform, 0, 0) = cos(theta);
	ME(transform, 1, 1) = cos(theta);
	ME(transform, 0, 1) = -sin(theta);
	ME(transform, 1, 0) = sin(theta);

	return transform;
}

Matrix reflectMatrix(int bool_x, int bool_y) {
	Matrix transform = makeIdentityMatrix(3);

	if (bool_x) ME(transform, 0, 0) = -1;
	if (bool_y) ME(transform, 1, 1) = -1;
	return transform;
}


/**********************************************************************************88
cur->source.x1 += args->shiftX;  SHIFT_X;  0.0
cur->source.y1 += args->shiftY;  SHIFT_Y;  0.0
cur->source.x2 += args->shiftX;  SHIFT_X;  0.0
cur->source.y2 += args->shiftY;  SHIFT_Y;  0.0
dest.x1 = args->eyeLx;   30
dest.y1 = args->eyeLy;   45
dest.x2 = args->eyeRx;   100
dest.y2 = args->eyeRy;   45
source=&cur->source
dest=&dest
*******************************************************************************8 */
Matrix generateTransform(const TwoPoints *source, const TwoPoints *dest, int reflect) {
	FTYPE sourceMidX = (source->x1 + source->x2)*0.5;
	FTYPE sourceMidY = (source->y1 + source->y2)*0.5;
	FTYPE destMidX = (dest->x1 + dest->x2)*0.5;
	FTYPE destMidY = (dest->y1 + dest->y2)*0.5;
	Matrix translate1 = translateMatrix(-sourceMidX, -sourceMidY); /* translate the left point to the origin */
	Matrix translate2 = translateMatrix(destMidX, destMidY); /* translate the origin to the left destination */

															 /* compute the scale that needs to be applyed to the image */
	FTYPE sdist = sqrt(SQR(source->x1 - source->x2) + SQR(source->y1 - source->y2));
	FTYPE ddist = sqrt(SQR(dest->x1 - dest->x2) + SQR(dest->y1 - dest->y2));
	FTYPE s = ddist / sdist;
	Matrix scale = scaleMatrix(s);

	/* compute the rotation that needs to be applyed to the image */
	FTYPE stheta = atan((source->y2 - source->y1) / (source->x2 - source->x1));
	FTYPE dtheta = atan((dest->y2 - dest->y1) / (dest->x2 - dest->x1));
	FTYPE theta = dtheta - stheta;
	Matrix rotate = rotateMatrix(theta);

	/* compute the reflection if nessicary */
	Matrix reflection = reflectMatrix(reflect, 0);

	/* build the final transformation */
	Matrix tmp1 = multiplyMatrix(scale, translate1);
	Matrix tmp2 = multiplyMatrix(rotate, tmp1);
	Matrix tmp3 = multiplyMatrix(reflection, tmp2);
	Matrix transform = multiplyMatrix(translate2, tmp3);

	/* free temporary matricies */
	freeMatrix(translate1);
	freeMatrix(translate2);
	freeMatrix(scale);
	freeMatrix(rotate);
	freeMatrix(reflection);
	freeMatrix(tmp1);
	freeMatrix(tmp2);
	freeMatrix(tmp3);

	/* return final transformation */
	return transform;
}

void scaleArgs(Arguments* args, FTYPE scale) {
	args->eyeLx *= scale;
	args->eyeLy *= scale;
	args->eyeRx *= scale;
	args->eyeRy *= scale;

	args->ellipseX *= scale;
	args->ellipseY *= scale;
	args->ellipseA *= scale;
	args->ellipseB *= scale;

	args->sizeWidth = ROUND(args->sizeWidth*scale);
	args->sizeHeight = ROUND(args->sizeHeight*scale);
}

/*	The code to generate a file suffix will build a string that is
compact and easily selected for using standard unix "ls". However,
it is not obvious. Here is the encoding:
c<0/1>			Apply mask, 0=no, 1=yes
f<0/1>			Pixel Value Normalization, 0=no, 1=yes
g<0/1>			Geometric Normalization, 0=no, 1=yes
h<0/1/2>		Histogram Equalization, 0=no, 1=pre, 2=post
n<0/1>			Indepenent Pixel Gaussian Noise, 0=no, 1=yes
x<0/1>			Reduce image size by half, 0=no, 1=yes
m<0/1>          Mirror reflect the image, 0=no, 1=yes
<l/r/u/d><#>	Shift source image left, right, up or down by amount #
or t0           If there is no translational shift.

All but the last are always included in a file name. The shift tag only
appears if an image is shifted.

Normal defaulst will produce suffix "c1f1g1h2n0x0"
*/

char* imageSuffix(Arguments* args) {
	const int len = 32; /* large enough to include shift indicators plus slack */
	char* s = (char*)malloc(sizeof(char)*len);
	strcpy(s, "c0f0g0h0n0x0m0");  /* Starts out with zeroes as default settings */
	if (args->maskType == CL_YES) 		s[1] = '1';
	if (args->nrmType == CL_YES) 		s[3] = '1';
	if (args->geoType == CL_YES)		s[5] = '1';
	if (args->histType == HIST_PRE)  	s[7] = '1';
	if (args->histType == HIST_POST) 	s[7] = '2';
	if (args->noise != 0.0)			s[9] = '1';
	if (args->scale == 0.5)			s[11] = '1';
	if (args->reflect == CL_YES)	    s[13] = '1';
	if (args->shiftX < 0.0) 			sprintf(s, "%s%s%d", s, "r", (int)ceil(-args->shiftX));
	if (args->shiftX > 0.0) 			sprintf(s, "%s%s%d", s, "l", (int)ceil(args->shiftX));
	if (args->shiftY < 0.0) 			sprintf(s, "%s%s%d", s, "d", (int)ceil(-args->shiftY));
	if (args->shiftY > 0.0) 			sprintf(s, "%s%s%d", s, "u", (int)ceil(args->shiftY));
	if (args->shiftX == 0.0 && args->shiftY == 0.0) sprintf(s, "%s%s", s, "t0");
	return s;
}


/*为输入目录下所有图片转换成标准格式图片（大小等）
*
*/
void convertImages(char* eyeFile, char* inputDir, char* pgmDir) {
	char** mask = NULL;
	Arguments *args;
	TwoPoints source, dest;
	FILE* eyeList;
	char line[FILE_LINE_LENGTH];
	char filename[MAX_FILENAME_LENGTH];
	char imagename[MAX_FILENAME_LENGTH];
	char suffix[MAX_FILENAME_LENGTH];
	int i;
	struct finfo *llist_head, *llist_tail;
	struct finfo *n[MAXEYEFILE];
	int num = 0;
	struct finfo *cur = NULL;
	Image pgm;
	Image geo;
	Matrix transform;
	GraphDiscription srcGd, gd;

	args = (Arguments *)malloc(sizeof(Arguments));
	preProocess(eyeFile, inputDir, pgmDir, args);
	scaleArgs(args, args->scale);
	dest.x1 = args->eyeLx;
	dest.y1 = args->eyeLy;
	dest.x2 = args->eyeRx;
	dest.y2 = args->eyeRy;

	/* Prepare file suffix encoding preprocessing settings, blank if not requested */
	if (args->configSuffix) {
		sprintf(suffix, "_%s", imageSuffix(args));
	}
	else {
		suffix[1] = '\0';
	}

	if (args->maskType == CL_YES) {
		mask = generateMask(args->sizeWidth, args->sizeHeight, args->ellipseX, args->ellipseY, args->ellipseA, args->ellipseB);
	}
	printf("args->eyeFile = %s\n", args->eyeFile);
	eyeList = fopen(args->eyeFile, "r");



	llist_head = llist_tail = NULL;

	fprintf(stdout, "Opened file\n");

	while (!feof(eyeList)) {
		fgets(line, FILE_LINE_LENGTH, eyeList);
		if (feof(eyeList)) break;
		n[num] = (struct finfo*)malloc(sizeof(struct finfo));
		n[num]->next = NULL;
		if (sscanf(line, "%s %lf %lf %lf %lf", n[num]->filename, &(n[num]->source.x1), &(n[num]->source.y1), &(n[num]->source.x2), &(n[num]->source.y2)) != 5) {
			//free(n[num]);
			exit(1);
		}
		else {
			if (!llist_head) {
				llist_head = llist_tail = n[num];
			}
			else {
				llist_tail->next = n[num];
				llist_tail = n[num];
			}
		}
		num++;
	}


	//fprintf(stdout, "No of lines in queue#%d = %d\n", i + 1, nlines_p[i]);
	cur = llist_head;
	while (cur) {
		/* shift the eye coordinates if neccessary */
		cur->source.x1 += args->shiftX;
		cur->source.y1 += args->shiftY;
		cur->source.x2 += args->shiftX;
		cur->source.y2 += args->shiftY;
		sprintf(imagename, "%s/%s.pgm", args->inputDir, cur->filename);
		printf("%s\n", imagename);


		pgm = readPGMImage(imagename);

		if (args->histType == HIST_PRE) {
			histEqual(pgm, 256);
		}
		if (args->preNormType == CL_YES) {//0
			ZeroMeanOneStdDev(pgm);
		}

		if (args->preEdge) {//0
			smoothImageEdge(pgm, args->preEdge);
		}

		if (args->geoType == CL_YES)
		{//1
			transform = generateTransform(&cur->source, &dest, args->reflect);
			geo = transformImage(pgm, args->sizeWidth, args->sizeHeight, transform);//pgm-Source  
		}
		else {
			transform = makeIdentityMatrix(3);
			geo = transformImage(pgm, args->sizeWidth, args->sizeHeight, transform);
		}

		sprintf(filename, "%s/%s.pts", args->inputDir, cur->filename);
		srcGd = readGraphDiscription(filename);
		gd = invertTransformPoints(srcGd, transform);
		sprintf(filename, "%s/%s.pts", args->pgmDir, cur->filename);
		//saveGraphDiscription(filename,gd);
		saveBinaryGraphDiscription(filename, gd);
		freeGraphDiscription(srcGd);
		freeGraphDiscription(gd);
		//noise is zero 
		if (args->noise != 0.0) {
			gaussianNoise(geo, args->noise);
		}

		if (args->histType == HIST_POST) {
			histEqualMask(geo, 256, (const char**)mask);
		}

		if (args->nrmType == CL_YES) {
			ZeroMeanOneStdDevMasked(geo, (const char **)mask);
		}
		else {
			applyMask(geo, (const char **)mask);
		}

		if (args->postEdge) {
			smoothImageEdge(geo, args->postEdge);
		}

		if (args->pgmDir) {
			//sprintf(imagename,"%s.pgm", args->pgmDir);
			sprintf(imagename, "%s/%s%s.pgm", args->pgmDir, cur->filename, suffix);
			writePGMImage(geo, imagename, 0);
		}

		freeImage(geo);
		freeImage(pgm);
		freeMatrix(transform);

		cur = cur->next;
	}


	fclose(eyeList);

}

void convertimage(GraphDiscription srcgd, const char* imageDir, const  char* imageName, const char* pgmDir) {
	char** mask = NULL;
	Arguments *args;
	TwoPoints source, dest;
	char line[FILE_LINE_LENGTH];
	char filename[MAX_FILENAME_LENGTH];
	char imagename[MAX_FILENAME_LENGTH];
	char posname[MAX_FILENAME_LENGTH];
	char suffix[MAX_FILENAME_LENGTH];
	int i;
	char* eyeF = "NULL";
	Image pgm;
	Image geo;
	Matrix transform;
	GraphDiscription gd;

	args = (Arguments *)malloc(sizeof(Arguments));

	preProocess(eyeF, imageName, pgmDir, args);

	scaleArgs(args, args->scale);

	source.x1 = srcgd->verts[0].x;
	source.y1 = srcgd->verts[0].y;
	source.x2 = srcgd->verts[1].x;
	source.y2 = srcgd->verts[1].y;
	dest.x1 = args->eyeLx;
	dest.y1 = args->eyeLy;
	dest.x2 = args->eyeRx;
	dest.y2 = args->eyeRy;

	/* Prepare file suffix encoding preprocessing settings, blank if not requested */
	if (args->configSuffix) {
		sprintf(suffix, "_%s", imageSuffix(args));
	}
	else {
		suffix[1] = '\0';
	}

	if (args->maskType == CL_YES) {
		mask = generateMask(args->sizeWidth, args->sizeHeight, args->ellipseX, args->ellipseY, args->ellipseA, args->ellipseB);
	}

	/* shift the eye coordinates if neccessary */
	source.x1 += args->shiftX;
	source.y1 += args->shiftY;
	source.x2 += args->shiftX;
	source.y2 += args->shiftY;

	sprintf(imagename, "%s/%s.pgm", imageDir, imageName);
	pgm = readPGMImage(imagename);

	if (args->histType == HIST_PRE) {
		histEqual(pgm, 256);
	}

	if (args->preNormType == CL_YES) {//0
		ZeroMeanOneStdDev(pgm);
	}

	if (args->preEdge) {//0
		smoothImageEdge(pgm, args->preEdge);
	}

	if (args->geoType == CL_YES) {//1
		transform = generateTransform(&source, &dest, args->reflect);
		geo = transformImage(pgm, args->sizeWidth, args->sizeHeight, transform);//pgm-Source  
	}
	else {
		transform = makeIdentityMatrix(3);
		geo = transformImage(pgm, args->sizeWidth, args->sizeHeight, transform);
	}

	gd = invertTransformPoints(srcgd, transform);
//	sprintf(filename, "%s/%s_position.pts", args->pgmDir, imageName);
	sprintf(filename, "%s/%s_.pts", args->pgmDir, imageName);
	printf("ProcessoneImage GrapDiscription name :%s\n", filename);
	saveBinaryGraphDiscription(filename, gd);
	sprintf(filename, "%s/%s_position_TXT.pts", args->pgmDir, imageName);
	saveGraphDiscription(filename, gd);
//	freeGraphDiscription(gd);

	if (args->histType == HIST_POST) {
		histEqualMask(geo, 256, (const char**)mask);
	}

	if (args->nrmType == CL_YES) {
		ZeroMeanOneStdDevMasked(geo, (const char **)mask);
	}
	else {
		applyMask(geo, (const char **)mask);
	}

	if (args->postEdge) {
		smoothImageEdge(geo, args->postEdge);
	}

	if (args->pgmDir) {
		//sprintf(imagename,"%s.pgm", args->pgmDir);
		sprintf(imagename, "%s/%s.pgm", args->pgmDir, imageName);

		writePGMImage(geo, imagename, 0);
	}

	freeImage(geo);
	freeImage(pgm);
	freeMatrix(transform);
}



/*将待识别图片转换成标准格式图片（大小等）
*
*/
void convertOneImages(TwoPoints sourc, char* imageDir, char* imageName, char* pgmDir) {
	char** mask = NULL;
	Arguments *args;
	TwoPoints source, dest;
	char line[FILE_LINE_LENGTH];
	char filename[MAX_FILENAME_LENGTH];
	char imagename[MAX_FILENAME_LENGTH];
	char posname[MAX_FILENAME_LENGTH];
	char suffix[MAX_FILENAME_LENGTH];
	int i;
	char* eyeF = "NULL";
	Image pgm;
	Image geo;
	Matrix transform;
	char *srcGdName = "data/inputImage/BioID_0000.pts";
	GraphDiscription srcGd, gd;

	args = (Arguments *)malloc(sizeof(Arguments));

	preProocess(eyeF, imageName, pgmDir, args);

	scaleArgs(args, args->scale);

	source.x1 = sourc.x1;
	source.y1 = sourc.y1;
	source.x2 = sourc.x2;
	source.y2 = sourc.y2;
	dest.x1 = args->eyeLx;
	dest.y1 = args->eyeLy;
	dest.x2 = args->eyeRx;
	dest.y2 = args->eyeRy;

	/* Prepare file suffix encoding preprocessing settings, blank if not requested */
	if (args->configSuffix) {
		sprintf(suffix, "_%s", imageSuffix(args));
	}
	else {
		suffix[1] = '\0';
	}

	if (args->maskType == CL_YES) {
		mask = generateMask(args->sizeWidth, args->sizeHeight, args->ellipseX, args->ellipseY, args->ellipseA, args->ellipseB);
	}

	/* shift the eye coordinates if neccessary */
	source.x1 += args->shiftX;
	source.y1 += args->shiftY;
	source.x2 += args->shiftX;
	source.y2 += args->shiftY;

	sprintf(imagename, "%s/%s.pgm", imageDir, imageName);
	pgm = readPGMImage(imagename);

	if (args->histType == HIST_PRE) {
		histEqual(pgm, 256);
	}

	if (args->preNormType == CL_YES) {//0
		ZeroMeanOneStdDev(pgm);
	}

	if (args->preEdge) {//0
		smoothImageEdge(pgm, args->preEdge);
	}

	if (args->geoType == CL_YES) {//1
		transform = generateTransform(&source, &dest, args->reflect);
		geo = transformImage(pgm, args->sizeWidth, args->sizeHeight, transform);//pgm-Source  
	}
	else {
		transform = makeIdentityMatrix(3);
		geo = transformImage(pgm, args->sizeWidth, args->sizeHeight, transform);
	}

	sprintf(posname, "%s/%s.pts", imageDir, imageName);
	srcGd = readGraphDiscription(posname);
	srcGd->verts[0].x = sourc.x1;
	srcGd->verts[0].y = sourc.y1;
	srcGd->verts[1].x = sourc.x2;
	srcGd->verts[1].y = sourc.y2;
	gd = invertTransformPoints(srcGd, transform);
	sprintf(filename, "%s/%s.pts", args->pgmDir, imageName);
	printf("ProcessoneImage GrapDiscription name :%s\n", filename);
	saveBinaryGraphDiscription(filename, gd);
	freeGraphDiscription(srcGd);
	freeGraphDiscription(gd);
	//noise is zero 
	if (args->noise != 0.0) {
		gaussianNoise(geo, args->noise);
	}

	if (args->histType == HIST_POST) {
		histEqualMask(geo, 256, (const char**)mask);
	}

	if (args->nrmType == CL_YES) {
		ZeroMeanOneStdDevMasked(geo, (const char **)mask);
	}
	else {
		applyMask(geo, (const char **)mask);
	}

	if (args->postEdge) {
		smoothImageEdge(geo, args->postEdge);
	}

	if (args->pgmDir) {
		//sprintf(imagename,"%s.pgm", args->pgmDir);
		sprintf(imagename, "%s/%s.pgm", args->pgmDir, imageName);

		writePGMImage(geo, imagename, 0);
	}

	freeImage(geo);
	freeImage(pgm);
	freeMatrix(transform);
}

/*转换特征点位置，计算出转换后图片的特征点位置，根据转换规则将原图片特征点位置转换。
*
*/
GraphDiscription invertTransformPoints(GraphDiscription srcGd, const Matrix m) {
	GraphDiscription gd = ALLOCATE(graph_discription, 1);
	Matrix point = makeMatrix(3, 1);
	Matrix pt;
	int i;

	gd->numVert = srcGd->numVert;
	gd->numEdge = srcGd->numEdge;
	/* allocate space for vericies and load discriptions */
	gd->verts = ALLOCATE(Vert, gd->numVert);
	gd->vertLabels = ALLOCATE(char*, gd->numVert);
	gd->bunch = ALLOCATE(JetBunch, gd->numVert);
	gd->edges = ALLOCATE(Edge, gd->numEdge);



	assert(m->row_dim == 3);
	assert(m->col_dim == 3);

	/* initialize homogenius point */
	ME(point, 2, 0) = 1;

	for (i = 0; i < srcGd->numVert; i++)
	{
		ME(point, 0, 0) = srcGd->verts[i].x;
		ME(point, 1, 0) = srcGd->verts[i].y;
		ME(point, 2, 0) = 1.0;
		pt = multiplyMatrix(m, point);
		gd->verts[i].x = ME(pt, 0, 0) / ME(pt, 2, 0);
		gd->verts[i].y = ME(pt, 1, 0) / ME(pt, 2, 0);

		gd->vertLabels[i] = ALLOCATE(char, LABELMAX);
		strcpy(gd->vertLabels[i], srcGd->vertLabels[i]);
		gd->bunch[i] = makeJetBunch();
		//gd->bunch[i] = srcGd->bunch[i];

		/* clean up */
		freeMatrix(pt);
	}
	freeMatrix(point);

	for (i = 0; i < srcGd->numEdge; i++)
	{
		gd->edges[i] = srcGd->edges[i];
	}

	return gd;
}

/********************************************************Matrix****************************************/
/* 生成一个长为row_dim宽为col_dim的矩阵*/
Matrix makeMatrix(int row_dim, int col_dim) {
	/** creates and allocates memory for a matrix */
	int i;
#ifdef SSE2
	int new_row = (row_dim % 2) ? row_dim + 1 : row_dim;
#endif

	/* Allocate a matrix header */
	Matrix A = (Matrix)malloc(sizeof(matrix));
	assert(A);

	/* keep track of the number of matricies allcoated */
#ifdef STHR
	alloc_matrix++;
#endif

	/** Set the matrix dimensions */
	A->row_dim = row_dim;
	A->col_dim = col_dim;

	/** Allocate the data section of the matrix */
#ifdef SSE2
	A->data = (FTYPE*)_mm_malloc(sizeof(FTYPE) * new_row * col_dim, 16);
#else
	A->data = (FTYPE*)malloc(sizeof(FTYPE) * row_dim * col_dim);
#endif


	if (!A->data) {
		fprintf(stderr, "Matrix Size: %d X %d\n", A->row_dim, A->col_dim);
		fflush(stderr);
	}
	assert(A->data);
	A->cols = (FTYPE**)malloc(sizeof(FTYPE*)*col_dim);
	assert(A->cols);

	for (i = 0; i < col_dim; i++) {
#ifdef SSE2
		A->cols[i] = A->data + i*new_row;/*row_dim;*/
#else
		A->cols[i] = A->data + i*row_dim;
#endif
	}

	return A;
}

/* 生成一个全为0的矩阵*/
Matrix makeZeroMatrix(int row_dim, int col_dim) {
	/** creates and allocates memory for a matrix */
	int i, j;
	Matrix A = makeMatrix(row_dim, col_dim);


	for (i = 0; i < row_dim; i++) {
		for (j = 0; j < col_dim; j++) {
			ME(A, i, j) = 0;
		}
	}
	return A;
}

void matrixRREF(Matrix m) {
#ifdef STHR
	int prealloc = alloc_matrix;
#endif
	int pivotCol = 0;
	int pivotRow = 0;
	int row;
	FTYPE absVal;
	int tmp = 0;

	while (1) {
		/* Select the row with the largest absolute value, or move to the next row
		if there is no value int the column */
		absVal = 0.0;
		while (absVal == 0.0 && pivotCol < m->col_dim) {
			absVal = ABS(ME(m, pivotRow, pivotCol));
			tmp = pivotRow;

			for (row = pivotRow + 1; row < m->row_dim; row++) {
				if (ABS(ME(m, row, pivotCol)) > absVal) {
					absVal = ABS(ME(m, row, pivotCol));
					tmp = row;
				}
			}
			if (absVal == 0) {
				pivotCol++;
			}
		}

		/* return if the RREF has been found */
		if (pivotCol >= m->col_dim || pivotRow >= m->row_dim) return;

		/* make sure that the pivot row is in the correct position */
		if (pivotRow != tmp) rowSwap(m, tmp, pivotRow);

		/* rescale the Pivot Row */
		rowMult(m, pivotRow, 1.0 / ME(m, pivotRow, pivotCol));

		/* This part of the algorithm is not as effecent as it could be,
		but it works for now. */
		for (row = 0; row < m->row_dim; row++) {
			if (row != pivotRow) {
				rowMultAdd(m, pivotRow, row, -ME(m, row, pivotCol));
			}
		}
		pivotRow++;
		pivotCol++;
	}

#ifdef STHR
	if (prealloc != alloc_matrix) {
		printf("Error deallocating matricies <%s>: pre=%d post=%d", __FUNCTION__, prealloc, alloc_matrix);
		exit(1);
	}
#endif
}


void freeMatrix(Matrix A) {
	/** frees the area allocated to a matrix */

	/*MESSAGE("Free matrix called - We should NOT do this");*/

	/* keep track of the number of matricies allcoated */
#ifdef STHR
	alloc_matrix--;
#endif

#ifdef SSE2
	_mm_free(A->data);
#else
	free(A->data);
#endif
	free(A->cols);
	free(A);
}

/* 生成一个大小为dim的标准矩阵
*1------
*01-----
*-------
*------1
* */
Matrix makeIdentityMatrix(int dim) {
	/** creates and allocates memory for an identity matrix */
	Matrix I;
	int i, j;


	I = makeMatrix(dim, dim);
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			ME(I, i, j) = (i == j ? 1 : 0);
		}
	}

	return I;
}

/*
*返回两个矩阵相乘的矩阵
*/
Matrix multiplyMatrix(const Matrix A, const Matrix B) {
	/** creates a new matrix that is the product of A and B */

	int i, j, k;                                   /* Variables used as indices */
	Matrix P = makeMatrix(A->row_dim, B->col_dim); /* Product A * B */


	assert(A->col_dim == B->row_dim); /* Verify that the Matrices can be multiplied */

									  /** Build the product matrix P */
	for (j = 0; j < B->col_dim; j++) {
		for (i = 0; i < A->row_dim; i++) {
			ME(P, i, j) = 0;
			for (k = 0; k < A->col_dim; k++) {
				ME(P, i, j) += ME(A, i, k) * ME(B, k, j);
			}
		}
	}

	return P;
}

/*
*矩阵的行列交换
*/
void rowSwap(Matrix m, int rSrc, int rDest) {
	int col = 0;
	FTYPE tmp;

	for (col = 0; col < m->col_dim; col++) {
		tmp = ME(m, rSrc, col);
		ME(m, rSrc, col) = ME(m, rDest, col);
		ME(m, rDest, col) = tmp;
	}
}

/*
*矩阵的行乘以一个值
*/
void rowMult(Matrix m, int rSrc, FTYPE value) {
	int col = 0;

	for (col = 0; col < m->col_dim; col++) {
		ME(m, rSrc, col) *= value;
	}
}

/*
*矩阵的行加上一个值乘以该行
*/
void rowMultAdd(Matrix m, int rSrc, int rDest, FTYPE value) {
	int col = 0;

	for (col = 0; col < m->col_dim; col++) {
		ME(m, rDest, col) += value*ME(m, rSrc, col);
	}
}

/* 用高斯消去法求逆矩阵*/
Matrix invertRREF(Matrix m) {
#ifdef STHR
	int prealloc = alloc_matrix;
#endif
	int i, j;
	Matrix tmp = makeZeroMatrix(m->row_dim, m->col_dim * 2);
	Matrix inverse = makeMatrix(m->row_dim, m->col_dim);
	/* build the tmp Matrix which will be passed to RREF */
	for (i = 0; i < m->row_dim; i++) {
		for (j = 0; j < m->col_dim; j++) {
			ME(tmp, i, j) = ME(m, i, j);
			if (i == j) {
				ME(tmp, i, j + m->col_dim) = 1;
			}
		}
	}
	matrixRREF(tmp);

	for (i = 0; i < m->row_dim; i++) {
		for (j = 0; j < m->col_dim; j++) {
			ME(inverse, i, j) = ME(tmp, i, j + m->col_dim);
		}
	}

	freeMatrix(tmp);

#ifdef STHR
	if (prealloc != alloc_matrix - 1) {
		printf("Error deallocating matricies <%s>: pre=%d post=%d", __FUNCTION__, prealloc, alloc_matrix);
		exit(1);
	}
#endif
	return inverse;
}



void applyMask(Image im, const char **mask) {
	int x, y, c;

	if (!mask) return;

	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				if (mask[x][y]) {
					/* do nothing */
				}
				else {
					IE(im, x, y, c) = 0.0;
				}
			}
		}
	}

}


/* 直方图均衡化图片  */
void histEqual(Image im, int N) {
	HistogramBin *bins = (HistogramBin*)malloc(sizeof(HistogramBin)*N);
	PIX_TYPE minVal, maxVal, binSize;
	PIX_TYPE totalPixels = im->width*im->height*im->channels;
	int x, y, c;

	assert(bins);

	/*************** find the minimum and maximum values ***************/
	minVal = IE(im, 0, 0, 0);
	maxVal = IE(im, 0, 0, 0);
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				minVal = MIN(minVal, IE(im, x, y, c));
				maxVal = MAX(maxVal, IE(im, x, y, c));
			}
		}
	}

	/********************** determine the bin size ********************/
	binSize = (maxVal - minVal) / N;

	/*************** Find probablity distribution ***************/
	/* initialize counts to zero */
	for (x = 0; x < N; x++) { bins[x].count = 0; }

	/* count individual pixels */
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				int tmp = TRUNC((IE(im, x, y, c) - minVal) / binSize); /* select bin */
				int bin = MAX(0, MIN(N - 1, tmp)); /* Make sure value is in range */
				bins[bin].count++;
			}
		}
	}

	/***************** Distribute bins *****************/
	c = 0; /* c is an accumulated count value */
	for (x = 0; x < N; x++) {
		bins[x].rangeStart = c / totalPixels;
		c += bins[x].count;
		bins[x].rangeEnd = c / totalPixels;
	}

	/***************** create equalized image *************/
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				int tmp = TRUNC((IE(im, x, y, c) - minVal) / binSize); /* select bin */
				PIX_TYPE frac = ((IE(im, x, y, c) - minVal) / binSize) - tmp;
				int bin = MAX(0, MIN(N - 1, tmp)); /* Make sure value is in range */

												   /* compute the linear interpolation for the final pixel value */
				IE(im, x, y, c) = bins[bin].rangeStart*(1.0 - frac) + bins[bin].rangeEnd*(frac);
			}
		}
	}

	free(bins);
}


/* 直方图均衡化图片  */
void histEqualMask(Image im, int N, const char **mask) {
	HistogramBin *bins = (HistogramBin*)malloc(sizeof(HistogramBin)*N);
	PIX_TYPE minVal, maxVal, binSize;
	PIX_TYPE totalPixels = 0.0;
	int x, y, c;

	assert(bins);

	/*************** find the minimum and maximum values ***************/
	minVal = IE(im, 0, 0, 0);
	maxVal = IE(im, 0, 0, 0);
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				if (!mask || mask[x][y]) {
					minVal = MIN(minVal, IE(im, x, y, c));
					maxVal = MAX(maxVal, IE(im, x, y, c));
					totalPixels += 1;
				}
			}
		}
	}

	/********************** determine the bin size ********************/
	binSize = (maxVal - minVal) / N;

	/*************** Find probablity distribution ***************/
	/* initialize counts to zero */
	for (x = 0; x < N; x++) { bins[x].count = 0; }

	/* count individual pixels */
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				if (!mask || mask[x][y]) {
					int tmp = TRUNC((IE(im, x, y, c) - minVal) / binSize); /* select bin */
					int bin = MAX(0, MIN(N - 1, tmp)); /* Make sure value is in range */
					bins[bin].count++;
				}
			}
		}
	}

	/***************** Distribute bins *****************/
	c = 0; /* c is an accumulated count value */
	for (x = 0; x < N; x++) {
		bins[x].rangeStart = c / totalPixels;
		c += bins[x].count;
		bins[x].rangeEnd = c / totalPixels;
	}

	/***************** create equalized image *************/
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				if (!mask || mask[x][y]) {
					int tmp = TRUNC((IE(im, x, y, c) - minVal) / binSize); /* select bin */
					PIX_TYPE frac = ((IE(im, x, y, c) - minVal) / binSize) - tmp;
					int bin = MAX(0, MIN(N - 1, tmp)); /* Make sure value is in range */

													   /* compute the linear interpolation for the final pixel value */
					IE(im, x, y, c) = bins[bin].rangeStart*(1.0 - frac) + bins[bin].rangeEnd*(frac);
				}
				else {
					IE(im, x, y, c) = 0;
				}
			}
		}
	}

	free(bins);
}

/* 直方图均衡化图片  */
void histEqualRect(Image im, int N, int top, int left, int bottom, int right) {
	HistogramBin *bins = (HistogramBin*)malloc(sizeof(HistogramBin)*N);
	PIX_TYPE minVal, maxVal, binSize;
	PIX_TYPE totalPixels = im->width*im->height*im->channels;
	int x, y, c;

	assert(bins);
	assert(top >= 0);
	assert(left >= 0);
	assert(bottom <= im->height);
	assert(right <= im->width);
	/*************** find the minimum and maximum values ***************/
	minVal = IE(im, 0, 0, 0);
	maxVal = IE(im, 0, 0, 0);
	for (x = left; x < right; x++) {
		for (y = top; y < bottom; y++) {
			for (c = 0; c < im->channels; c++) {
				minVal = MIN(minVal, IE(im, x, y, c));
				maxVal = MAX(maxVal, IE(im, x, y, c));
			}
		}
	}

	/********************** determine the bin size ********************/
	binSize = (maxVal - minVal) / N;

	/*************** Find probablity distribution ***************/
	/* initialize counts to zero */
	for (x = 0; x < N; x++) { bins[x].count = 0; }

	/* count individual pixels */
	for (x = left; x < right; x++) {
		for (y = top; y < bottom; y++) {
			for (c = 0; c < im->channels; c++) {
				int tmp = TRUNC((IE(im, x, y, c) - minVal) / binSize); /* select bin */
				int bin = MAX(0, MIN(N - 1, tmp)); /* Make sure value is in range */
				bins[bin].count++;
			}
		}
	}

	/***************** Distribute bins *****************/
	c = 0; /* c is an accumulated count value */
	for (x = 0; x < N; x++) {
		bins[x].rangeStart = c / totalPixels;
		c += bins[x].count;
		bins[x].rangeEnd = c / totalPixels;
	}

	/***************** Zero Center bins *****************/
	for (x = 0; x < N; x++) {
		int tmp = TRUNC((0.0 - minVal) / binSize); /* select bin */
		PIX_TYPE frac = ((0.0 - minVal) / binSize) - tmp;
		int bin = MAX(0, MIN(N - 1, tmp)); /* Make sure value is in range */
		FTYPE zeroshift;
		zeroshift = bins[bin].rangeStart*(1.0 - frac) + bins[bin].rangeEnd*(frac);

		bins[x].rangeStart -= zeroshift;
		bins[x].rangeEnd -= zeroshift;
	}

	/***************** create equalized image *************/
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				int tmp = TRUNC((IE(im, x, y, c) - minVal) / binSize); /* select bin */
				PIX_TYPE frac = ((IE(im, x, y, c) - minVal) / binSize) - tmp;
				int bin = MAX(0, MIN(N - 1, tmp)); /* Make sure value is in range */

												   /* crop frac if it is out of bounds */
												   /* compute the linear interpolation for the final pixel value */
				IE(im, x, y, c) = bins[bin].rangeStart*(1.0 - frac) + bins[bin].rangeEnd*(frac);
			}
		}
	}

	free(bins);
}

/*高斯去噪 */
FTYPE gaussianNoise(Image im, FTYPE noise) {
	FTYPE max;
	FTYPE min;
	FTYPE stddev;
	int x, y, c;

	/* compute the image range */
	max = IE(im, 0, 0, 0);
	min = IE(im, 0, 0, 0);

	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				max = MAX(IE(im, x, y, c), max);
				min = MIN(IE(im, x, y, c), min);
			}
		}
	}

	stddev = noise*(max - min);
	/* Add gaussian noise */
	for (x = 0; x < im->width; x++) {
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				FTYPE tmp = randBM()*stddev;
				IE(im, x, y, c) += tmp;
			}
		}
	}
	return stddev;
}

/* 光滑图片边缘 */
void smoothImageEdge(Image im, int width) {
	int i, x, y, c;
	for (i = 0; i < width; i++) {
		FTYPE scale = ((FTYPE)i) / width;

		/* Scale the horizontal lines */
		for (x = 0; x < im->width; x++) {
			for (c = 0; c < im->channels; c++) {
				IE(im, x, i, c) *= scale;
				IE(im, x, im->height - i - 1, c) *= scale;
			}
		}
		/* Scale the vertical lines */
		for (y = 0; y < im->height; y++) {
			for (c = 0; c < im->channels; c++) {
				IE(im, i, y, c) *= scale;
				IE(im, im->width - i - 1, y, c) *= scale;
			}
		}
	}

}

/*像素均匀分布，使图像更清晰  */
void ZeroMeanOneStdDev(Image im) {
	ZeroMeanOneStdDevMasked(im, NULL);
}

/*像素均匀分布，使图像更清晰  */
void ZeroMeanOneStdDevMasked(Image im, const char** mask) {
	/* scales the pixel so that they have zero mean and the image is unit length*/
	int i, j, c;

	FTYPE sum = 0.0;
	FTYPE sqsum = 0.0;
	FTYPE mean;
	FTYPE vari;
	FTYPE stddev;
	FTYPE invlength;
	int pixels = 0;

	for (j = 0; j < im->height; j++) {
		for (i = 0; i < im->width; i++) {
			for (c = 0; c < im->channels; c++) {
				if (!mask || mask[i][j]) {
					sum += IE(im, i, j, c);
					sqsum += SQR(IE(im, i, j, c));
					pixels++;
				}
			}
		}
	}

	mean = sum / pixels;
	vari = sqsum / pixels - SQR(mean);
	stddev = sqrt(vari);

	invlength = 1.0 / stddev;

	for (j = 0; j < im->height; j++) {
		for (i = 0; i < im->width; i++) {
			for (c = 0; c < im->channels; c++) {
				if (!mask || mask[i][j]) {
					IE(im, i, j, c) = (IE(im, i, j, c) - mean) * invlength;
				}
				else {
					IE(im, i, j, c) = 0.0;
				}
			}
		}
	}
}

//返回随机数
FTYPE randBM() {
	static int iset = 0;
	static FTYPE gset;
	FTYPE v1, v2, fac, rsq;
	if (iset == 0) {

		/* Pick a point in the unit circle */
		do {
			v1 = 2.0* RANDOM - 1.0;
			v2 = 2.0* RANDOM - 1.0;
			rsq = SQR(v1) + SQR(v2);
		} while (rsq >= 1.0 || rsq == 0);

		/* Perform the Box-Muller transformation */
		fac = sqrt(-2 * log(rsq) / rsq);
		gset = v1*fac;
		iset = 1;
		return v2*fac;
	}
	else {
		/* Return the deviate that was not returned last time */
		iset = 0;
		return gset;
	}
}
