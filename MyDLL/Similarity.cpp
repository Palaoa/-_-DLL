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

#include "stdafx.h"
#include "Similarity.h"


/*计算两个Gaborjet之间的相似度
*
*/
double DEPredictiveStep(GaborJet j1, GaborJet j2, FTYPE *tdx, FTYPE *tdy) {
	FTYPE j12 = 0;
	FTYPE j11 = 0;
	FTYPE j22 = 0;
	int i;
	FTYPE sim = 0.0;
	FTYPE dx = 0.0, dy = 0.0;
	FTYPE Gxx, Gxy, Gyx, Gyy;
	FTYPE Px, Py;
	FTYPE temp2, temp3;
//	FTYPE ang[j1->length];
//	FTYPE temp1[j1->length];//mabel注释掉的
	FTYPE *ang = (FTYPE*)malloc(j1->length * sizeof(FTYPE));
	FTYPE *temp1 = (FTYPE*)malloc(j1->length * sizeof(FTYPE));

	assert(j1 &&  j1->length && j2 && j1->length == j2->length);

	Gxx = 0;
	Gxy = 0;
	Gyx = 0;
	Gyy = 0;

	Px = 0;
	Py = 0;

	for (i = 0; i < j1->length; i++) {
		ang[i] = j1->ang[i] - j2->ang[i];

		/* Scale the angle such that it is closest to zero displacement. */
		while (ang[i] >  PI) { ang[i] -= 2 * PI; }
		while (ang[i] < -PI) { ang[i] += 2 * PI; }

		temp1[i] = j1->mag[i] * j2->mag[i];
		temp2 = temp1[i] * j1->params->kx[2 * i];
		temp3 = temp1[i] * j1->params->ky[2 * i];

		Gxx += temp2*j1->params->kx[2 * i];
		Gxy += temp2*j1->params->ky[2 * i];
		Gyy += temp3*j1->params->ky[2 * i];

		Px += temp2*(ang[i]);
		Py += temp3*(ang[i]);
	}
	Gyx = Gxy;

	/* Catch any divide by zero errors */
	temp2 = Gxx*Gyy - Gxy*Gyx;
	if (temp2 != 0.0) {

		dx = (Gyy*Px - Gyx*Py) / temp2;
		dy = (-Gxy*Px + Gxx*Py) / temp2;
	}
	else {
		/* Divide by zero occured - display warning */
		//	printf("In DEPredictiveStep Gxx :%f Gyy:%f Gxy:%f Gyx:%f\n",Gxx,Gyy,Gxy,Gyx);
		//        printf("Warning: Divide by zero -- Attempting to recover\n");
		dx = 0.0;
		dy = 0.0;
	}

	/* Compute the similarity with the estimated displacement. */
	j12 = 0.0;
	j11 = 0.0;
	j22 = 0.0;
	for (i = 0; i < j1->length; i++) {
		j12 += temp1[i] * cos(ang[i] - (dx * j1->params->kx[2 * i] + dy * j1->params->ky[2 * i]));
		j11 += j1->mag[i] * j1->mag[i];
		j22 += (j2->mag[i])*(j2->mag[i]);
	}

	temp2 = j11*j22;

	if (temp2 == 0)
	{
		sim = 1.0;//pretend error
				  // printf("In DEPretiveDEp  have the two same Jets in Similarity\n");
	}
	else   sim = j12 / sqrt(temp2);

	if ((sim>1000) || (sim<-1000))
	{
		sim = 1.0;//pretend error
	}

	*tdx = dx;
	*tdy = dy;

	return sim;
}

/*计算两个Gaborjet之间的相似度
*
*/
FTYPE JetSimilarityDEPredictiveStep(GaborJet j1, GaborJet j2) {
	FTYPE dx = 0.0, dy = 0.0;
	return DEPredictiveStep(j1, j2, &dx, &dy);
}

/* 计算两幅图片的相似度 */
FTYPE fgSimPhase02(FaceGraph f1, FaceGraph f2) {
	FTYPE totalSim = 0.0;
	int i;

	for (i = 0; i < f1->totalsize; i++) {
		totalSim += JetSimilarityDEPredictiveStep(f1->jets[i], f2->jets[i]);
		//if(totalSim>1)printf("In fgSimPhase02 --i:%d---totalSim:%f\n",i,totalSim);
	}
	totalSim = totalSim / f1->totalsize;

	//    printf(" In fgSimPhase02 ----%f \n",totalSim);
	return totalSim;
}

/*为每个线程分配任务
*
*//*
void * multi_pthread(void *ptr)
{
	multi_Process_Jet_Args *args = (multi_Process_Jet_Args *)ptr;
	FTYPE x, y;
	int i;

	if (args->i == 0)
	{
		for (i = 0; i < args->gd->numVert - 10; i++) {
			args->graph->jets[i] = extractJet(args->gd->verts[i].x, args->gd->verts[i].y, args->face, args->masks);
		}
	}
	if (args->i == 1)
	{
		for (i = 10; i < args->gd->numVert; i++) {
			args->graph->jets[i] = extractJet(args->gd->verts[i].x, args->gd->verts[i].y, args->face, args->masks);
		}
	}
	if (args->i == 2)
	{
		for (i = 0; i < args->gd->numEdge; i++) {
			x = 0.5*(args->gd->verts[args->gd->edges[i].vert1].x + args->gd->verts[args->gd->edges[i].vert2].x);
			y = 0.5*(args->gd->verts[args->gd->edges[i].vert1].y + args->gd->verts[args->gd->edges[i].vert2].y);


			args->graph->jets[args->gd->numVert + i] = extractJet(x, y, args->face, args->masks);
		}
	}

	pthread_exit((void *)3);

	return 0;
}

/*创建3个线程计算特征值。根据已计算出的特征点位置，计算输入图片的特征值
*
*//*
int multiProcessOneFaceGraph(char* fileDir, char *fileName, char *outFaceGraphDir, JetMasks masks) {
	GraphDiscription gd;
	Image face;
	FaceGraph graph;
	FTYPE x, y;
	char  filename[100];
	int i, j, status;
	pthread_t thread[20];
	void *tret;
	multi_Process_Jet_Args *args = (multi_Process_Jet_Args *)malloc(sizeof(multi_Process_Jet_Args));

	sprintf(filename, "%s/%s.pts", fileDir, fileName);
	printf("Processing: %s  FaceGraph\n", filename);
	//gd = readGraphDiscription(filename);
	gd = readBinaryGraphDiscription(filename);
	sprintf(filename, "%s/%s.pgm", fileDir, fileName);
	face = readPGMImage(filename);

	printf("in processFaceGraph =-=-%d, %d=-1\n",gd->numVert, gd->numVert+gd->numEdge);
	graph = makeFaceGraph(gd->numVert, gd->numVert + gd->numEdge);
	graph->params = masks->params;

	args->gd = gd;
	args->face = face;
	args->masks = masks;
	args->graph = graph;

	for (j = 0; j < 3; j++) {
		args->i = j;
		pthread_create(&thread[j], NULL, multi_pthread, (void *)args);

	}

	for (j = 0; j < 3; j++)
	{
		pthread_join(thread[j], &tret);
	}
	/*
	sprintf(filename,"%s/%s.pts", outFaceGraphDir, fileName);
	saveFaceGraph(filename,graph);
	*//*
	printf("Process FaceGraph finished\n");;
	freeImage(face);
	freeGraphDiscription(gd);
	freeFaceGraph(graph);

	return 0;
}*/
//多线程相关，取消


int BPNNFaceGraph(const char* fileDir, const char *fileName, const char *outFaceGraphDir, JetMasks masks) {
	GraphDiscription gd;
	Image face;
	FaceGraph graph;
	FTYPE x, y;
	char  filename[100];
	int i;

	sprintf(filename, "%s/%s_.pts", fileDir, fileName);
	printf("BPNN_Processing: %s  FaceGraph\n", filename);
	//gd = readGraphDiscription(filename);
	gd = readBinaryGraphDiscription(filename);
	// gd =  readBinaryGraphDiscription("bpnn/outcome/BioID_0000_position.pts");      
	sprintf(filename, "%s/%s.pgm", fileDir, fileName);
	face = readPGMImage(filename);

	//printf("in processFaceGraph =-=-%d, %d=-1\n",gd->numVert, gd->numVert+gd->numEdge);
	graph = makeFaceGraph(gd->numVert, gd->numVert + gd->numEdge);

	graph->params = masks->params;

	for (i = 0; i < gd->numVert; i++) {
		graph->jets[i] = extractJet(gd->verts[i].x, gd->verts[i].y, face, masks);
	}

	sprintf(filename, "%s/%s.pts", outFaceGraphDir, fileName);
	saveFaceGraph(filename, graph);

	//  free(filename);
	freeImage(face);
//	freeGraphDiscription(gd);
	freeFaceGraph(graph);
	return 0;
}



/*根据已计算出的特征点位置，计算输入图片的特征值
*
*/
int processOneFaceGraph(char* fileDir, char *fileName, char *outFaceGraphDir, JetMasks masks) {
	GraphDiscription gd;
	Image face;
	FaceGraph graph;
	FTYPE x, y;
	char  filename[100];
	int i;

	sprintf(filename, "%s/%s.pts", fileDir, fileName);
	printf("Processing: %s  FaceGraph\n", filename);
	//gd = readGraphDiscription(filename);
	gd = readBinaryGraphDiscription(filename);
	sprintf(filename, "%s/%s.pgm", fileDir, fileName);
	face = readPGMImage(filename);

	printf("in processFaceGraph =-=-%d, %d=-1\n",gd->numVert, gd->numVert+gd->numEdge);
	graph = makeFaceGraph(gd->numVert, gd->numVert + gd->numEdge);

	graph->params = masks->params;

	for (i = 0; i < gd->numVert; i++) {
		graph->jets[i] = extractJet(gd->verts[i].x, gd->verts[i].y, face, masks);
	}


	for (i = 0; i < gd->numEdge; i++) {
		x = 0.5*(gd->verts[gd->edges[i].vert1].x + gd->verts[gd->edges[i].vert2].x);
		y = 0.5*(gd->verts[gd->edges[i].vert1].y + gd->verts[gd->edges[i].vert2].y);


		graph->jets[gd->numVert + i] = extractJet(x, y, face, masks);
	}
	sprintf(filename, "%s/%s.pts", outFaceGraphDir, fileName);
	saveFaceGraph(filename, graph);

	//  free(filename);
	freeImage(face);
	freeGraphDiscription(gd);
	freeFaceGraph(graph);

	return 0;
}

/*根据已标记出的特征点位置，计算人脸库中图片的特征值
*
*/
int processAllFaceGraph(char* eyeFile, char * fileDir, char *outFaceGraphDir, JetMasks masks)
{
	GraphDiscription gd;
	Image face;
	FaceGraph graph;
	FILE* eyeList;
	char  filename[100];
	int i;
	char line[100];
	struct finfo *llist_head, *llist_tail;
	struct finfo *n[MAXEYEFILE];
	int num = 0;
	struct finfo *cur = NULL;


	eyeList = fopen(eyeFile, "r");

	llist_head = llist_tail = NULL;


	fprintf(stdout, "Opened file\n");

	while (!feof(eyeList)) {
		fgets(line, FILE_LINE_LENGTH, eyeList);

		if (feof(eyeList)) break;
		n[num] = (struct finfo*)malloc(sizeof(struct finfo));
		n[num]->next = NULL;

		if (sscanf(line, "%s %lf %lf %lf %lf", n[num]->filename, &(n[num]->source.x1), &(n[num]->source.y1), &(n[num]->source.x2), &(n[num]->source.y2)) != 5) {
			free(n[num]);
			exit(1);
		}
		else {
			if (!llist_head)
			{
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
	while (cur) {
		sprintf(filename, "%s/%s.pts", fileDir, cur->filename);
		printf("Processing: %s  \n", cur->filename);
		//gd = readGraphDiscription(filename);
		gd = readBinaryGraphDiscription(filename);
		sprintf(filename, "%s/%s.pgm", fileDir, cur->filename);
		face = readPGMImage(filename);

		graph = makeFaceGraph(gd->numVert, gd->numVert + gd->numEdge);

		graph->params = masks->params;

		for (i = 0; i < gd->numVert; i++) {
			graph->jets[i] = extractJet(gd->verts[i].x, gd->verts[i].y, face, masks);
		}

		for (i = 0; i < gd->numEdge; i++) {
			FTYPE x = 0.5*(gd->verts[gd->edges[i].vert1].x + gd->verts[gd->edges[i].vert2].x);
			FTYPE y = 0.5*(gd->verts[gd->edges[i].vert1].y + gd->verts[gd->edges[i].vert2].y);


			graph->jets[gd->numVert + i] = extractJet(x, y, face, masks);
		}

		sprintf(filename, "%s/%s.pts", outFaceGraphDir, cur->filename);
		saveFaceGraph(filename, graph);
		free(filename);
		freeImage(face);
		freeGraphDiscription(gd);
		freeFaceGraph(graph);
		cur = cur->next;
	}
}

/*根据人脸库，建立人脸束图。人脸束图包含每副图片特征点位置及特征值。
*
*/
GraphDiscription extractModelJets(char* eyeFile, char* imageDir, char* graphDir, JetMasks masks) {

	GraphDiscription mj = NULL;
	int count = 0, i;
	FILE* eyeList;
	char  filename[100];
	char line[100];
	struct finfo *llist_head, *llist_tail;
	struct finfo *n[MAXEYEFILE];
	int num = 0;
	struct finfo  *cur = NULL;
	GraphDiscription gd;
	Image model;

	llist_head = llist_tail = NULL;
	eyeList = fopen(eyeFile, "r");

	while (!feof(eyeList))
	{
		fgets(line, FILE_LINE_LENGTH, eyeList);
		if (feof(eyeList)) break;

		n[num] = (struct finfo*)malloc(sizeof(struct finfo));
		n[num]->next = NULL;

		if (sscanf(line, "%s %lf %lf %lf %lf", n[num]->filename, &(n[num]->source.x1), &(n[num]->source.y1), &(n[num]->source.x2), &(n[num]->source.y2)) != 5) {
			free(n[num]);
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

	cur = llist_head;
	while (cur) {

		/* load the model graph */
		sprintf(filename, "%s/%s.pts", graphDir, cur->filename);
		// gd = readGraphDiscription(filename);
		gd = readBinaryGraphDiscription(filename);
		/* Load the model image */
		sprintf(filename, "%s/%s.pgm", imageDir, cur->filename);
		model = readPGMImage(filename);


		if (count == 0)
		{
			/* load up a default graph */
			sprintf(filename, "%s/%s.pts", graphDir, cur->filename);
			mj = readGraphDiscription(filename);
		}
		else {
			assert(mj->numVert == gd->numVert);
			for (i = 0; i < mj->numVert; i++) {
				/* Accumulate mean locations */
				mj->verts[i].x += gd->verts[i].x;
				mj->verts[i].y += gd->verts[i].y;
			}
		}

		for (i = 0; i < gd->numVert; i++) {
			addJetToBunch(mj->bunch[i], extractJet(gd->verts[i].x, gd->verts[i].y, model, masks));
		}

		freeImage(model);
		freeGraphDiscription(gd);
		count++;


		cur = cur->next;
	}

	for (i = 0; i < mj->numVert; i++) {
		/* Accumulate mean locations */
		mj->verts[i].x /= count;
		mj->verts[i].y /= count;
	}

	return mj;
}

/*根据人脸库中的特征点位置，粗略计算出待测人脸特征点位置
*
*/
void guessVertexLocation(int vert, int maxvert, GraphDiscription graphTemplate, GraphDiscription bunchGraph) {
	FTYPE locx = 0.0, locy = 0.0, tw = 0.0;
	int i;
	if (vert < 2) {
		/* Little basis for an estimate so just return guess .--Only two eyes
		-- assume these are eye coordiantates. */
		graphTemplate->verts[vert].x = bunchGraph->verts[vert].x;
		graphTemplate->verts[vert].y = bunchGraph->verts[vert].y;
		return;
	}
	for (i = 0; i < maxvert; i++) {
		if (i != vert) {
			/* compute the displacement in the bunch graph */
			FTYPE dx = -bunchGraph->verts[i].x + bunchGraph->verts[vert].x;
			FTYPE dy = -bunchGraph->verts[i].y + bunchGraph->verts[vert].y;
			/* compute the distance */
			FTYPE dist = sqrt(SQR(dx) + SQR(dy));
			/* compute the wieght */
			FTYPE w = exp(-dist);

			/* compute the sum of the location */
			locx += w*(graphTemplate->verts[i].x + dx);
			locy += w*(graphTemplate->verts[i].y + dy);
			tw += w;
		}
	}

	/* compute the final vert location */
	graphTemplate->verts[vert].x = locx / tw;
	graphTemplate->verts[vert].y = locy / tw;
}

/*根据模糊计算出的特征点位置，进一步精确计算出位置。
*/
void LocatePoint(FTYPE * x, FTYPE * y, JetBunch bunch, Image novel, JetMasks masks, JetDisplacementEstimator dispEst) {
	int i, best = 0;
	FTYPE dx = 0.0, dy = 0.0;
	FTYPE bestsim = -1.0e300;
	/* Extract Gabor Jet from image*/
	GaborJet njet = extractJet(*x, *y, novel, masks);

	/* Select the closest matching training jet */
	for (i = 0; i < bunch->size; i++) {
		FTYPE sim;
		dx = 0.0; dy = 0.0;
		sim = dispEst(bunch->jets[i], njet, &dx, &dy);

		if (sim > bestsim) {
			bestsim = sim;
			best = i;
		}
	}

	/* Compute the new x and y */
	dispEst(bunch->jets[best], njet, &dx, &dy);

	*x += dx;
	*y += dy;

	/* Free the jet */
	freeGaborJet(njet);
}

void permuteArray(int arr[], int size) {
	int i;
	for (i = 0; i < size * 2; i++) {
		int x = rand() % size;
		int y = rand() % size;
		int tmp = arr[x];
		arr[x] = arr[y];
		arr[y] = tmp;
	}
}

/*根据人脸库中特征点位置弹性匹配出待测人脸人脸的特征点位置
*imagename:待测人脸
*bunchGraph:人脸束图
*masks:Gaborjet掩码
*imageDir:daice renlian 路径
*outputDir:输出路径
*/
void locateElasticFeatures(char *imageName, GraphDiscription bunchGraph, JetMasks masks, char* imageDir, char* outputDir) {
	int i;
	char filename[100];
	JetDisplacementEstimator dispEst;
	GraphDiscription graphTemplate;
	Image novel;
	int *verts1;
	/*******************build GraphDiscription **************************************************/

	dispEst = DEPredictiveStep;

	sprintf(filename, "%s/%s.pts", imageDir, imageName);
	//graphTemplate=readGraphDiscription(filename);
	graphTemplate = readBinaryGraphDiscription(filename);
	sprintf(filename, "%s/%s.pgm", imageDir, imageName);
	novel = readPGMImage(filename);
	// printf("bunchGraph->numVert :%d\n",bunchGraph->numVert);
	verts1 = (int*)malloc(sizeof(int)*bunchGraph->numVert);


	//	    printf("x1:%f,y1:%f;x1:%f,y1:%f;\n", graphTemplate->verts[0].x, graphTemplate->verts[0].y, graphTemplate->verts[1].x, graphTemplate->verts[1].y);
	for (i = 0; i < bunchGraph->numVert; i++)
	{
		guessVertexLocation(i, i, graphTemplate, bunchGraph);
		//	    	printf("x1:%f,y1:%f;x1:%f,y1:%f;\n", graphTemplate->verts[0].x, graphTemplate->verts[0].y, graphTemplate->verts[1].x, graphTemplate->verts[1].y);
		graphTemplate->verts[i].x = graphTemplate->verts[i].x;
		graphTemplate->verts[i].y = graphTemplate->verts[i].y;

		LocatePoint(&(graphTemplate->verts[i].x), &(graphTemplate->verts[i].y), bunchGraph->bunch[i], novel, masks, dispEst);
		//		printf("x1:%f,y1:%f;x1:%f,y1:%f;\n", graphTemplate->verts[0].x, graphTemplate->verts[0].y, graphTemplate->verts[1].x, graphTemplate->verts[1].y);
	}

	sprintf(filename, "%s/%s.pts", outputDir, imageName);
	saveBinaryGraphDiscription(filename, graphTemplate);


}



