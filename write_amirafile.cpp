/******************************************************
write_amirafile
Version for AnalyzeAmiraFile
TWS July 2017
*******************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"

void write_amirafile(const char fname[])
{
	extern int nVERTEX,nEDGE,nPOINT;
	extern int **EdgeConnectivity,*NumEdgePoints;
	extern float **VertexCoordinates,**EdgePointCoordinates,*thickness;
	int c,k;
	FILE *ofp;

	ofp = fopen(fname, "wb");
	fprintf(ofp,"# AmiraMesh BINARY-LITTLE-ENDIAN 2.1\n");
	fprintf(ofp,"\n\n");
	fprintf(ofp,"define VERTEX %i\n",nVERTEX);
	fprintf(ofp,"define EDGE %i\n",nEDGE);
	fprintf(ofp,"define POINT %i\n",nPOINT);
	fprintf(ofp,"\n");
	fprintf(ofp,"Parameters {\n");
	fprintf(ofp,"    ContentType \"HxSpatialGraph\"\n");
	fprintf(ofp,"}\n\n");
	fprintf(ofp,"VERTEX { float[3] VertexCoordinates } @1\n");
	fprintf(ofp,"EDGE { int[2] EdgeConnectivity } @2\n");
	fprintf(ofp,"EDGE { int NumEdgePoints } @3\n");
	fprintf(ofp,"POINT { float[3] EdgePointCoordinates } @4\n");
	fprintf(ofp,"POINT { float thickness } @5\n");
	fprintf(ofp,"\n# Data section follows");	

	//Write the data 
	fprintf(ofp,"\n@1\n");       
	const size_t NumToWrite5 = nVERTEX*3;
    float* pData5 = new float[NumToWrite5];
	for(k=0; k<nVERTEX; k++) for(c=0; c<3; c++) pData5[k*3 + c] = VertexCoordinates[k+1][c+1];
	printf("Writing VertexCoordinates in file\n");
	fwrite((void*)pData5, sizeof(float), NumToWrite5, ofp);
	delete[] pData5;

	fprintf(ofp,"\n@2\n");
	const size_t NumToWrite6 = nEDGE*2;
    int* pData6 = new int[NumToWrite6];
	for(k=0; k<nEDGE; k++) for(c=0; c<2; c++) pData6[k*2 + c] = EdgeConnectivity[k+1][c+1];
	printf("Writing EdgeConnectivity in file\n");
	fwrite((void*)pData6, sizeof(int), NumToWrite6, ofp);
	delete[] pData6;

	fprintf(ofp,"\n@3\n");
	const size_t NumToWrite7 = nEDGE;
    int* pData7 = new int[NumToWrite7];
	for(k=0; k<nEDGE; k++) pData7[k] = NumEdgePoints[k+1];
	printf("Writing NumEdgePoints in file\n");
	fwrite((void*)pData7, sizeof(int), NumToWrite7, ofp);
	delete[] pData7;

	fprintf(ofp,"\n@4\n");
	const size_t NumToWrite8 = nPOINT*3;
    float* pData8 = new float[NumToWrite8];
	for(k=0; k<nPOINT; k++) for(c=0; c<3; c++) pData8[k*3 + c] = EdgePointCoordinates[k+1][c+1];
	printf("Writing EdgePointCoordinates in file\n");
	fwrite((void*)pData8, sizeof(float), NumToWrite8, ofp);
	delete[] pData8;

	fprintf(ofp,"\n@5\n");
	const size_t NumToWrite9 = nPOINT;
    float* pData9 = new float[NumToWrite9];
	for(k=0; k<nPOINT; k++) pData9[k] = thickness[k+1];
	printf("Writing thickness in file\n");
	fwrite((void*)pData9, sizeof(float), NumToWrite9, ofp);
	delete[] pData9;

	fprintf(ofp,"\n");
	fclose(ofp);
}

