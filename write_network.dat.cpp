/******************************************************
write_network.dat
label segments with start node number (from nPOINT, not nVERTEX)
from and to nodes are from nVERTEX at ends of edge, from nPOINT otherwise
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

void write_network(const char fname[])
{
	extern int nVERTEX,nEDGE,nPOINT,d;
	extern int **EdgeConnectivity,*NumEdgePoints;
	extern float **VertexCoordinates,**EdgePointCoordinates,*thickness;

	int i,j,k,inod,max=100,idiammax,nseg,nnod;
	int segtyp = 5,segnodname1,segnodname2,count;
	float diam,q = 0.,hd = 0.4,diammax = 0.;
	float zoffset = 10.;	//increase all z coordinates by 10 to pad tissue region
	FILE *ofp;

	nseg = nPOINT - nEDGE;
	nnod = nVERTEX + nPOINT - 2*nEDGE;

	printf("Writing network.dat file\n");
	ofp = fopen(fname, "w");
	fprintf(ofp,"Network structure derived from Amira Spatial Graph\n");
	fprintf(ofp,"512 512 512 box dimensions in microns - adjust as needed\n");	//box large enough to include all networks, including zoffset
	fprintf(ofp,"32 32 32 number of tissue points in x,y,z directions - adjust as needed\n");
	fprintf(ofp,"10	outer bound distance - adjust as needed\n");
	fprintf(ofp,"100	max. segment length - adjust as needed\n");
	fprintf(ofp,"30		maximum number of segments per node - adjust as needed\n");
	fprintf(ofp,"%i	total number of segments\n",nseg);
	fprintf(ofp,"SegName Type StartNode EndNode Diam   Flow[nl/min]    Hd\n");
	k = 0;
	count = 0;
	diammax = 0.;
	for(i=1; i<=nEDGE; i++){
		for(j=1; j<NumEdgePoints[i]; j++){
			k++;
			if(j == 1) segnodname1 = EdgeConnectivity[i][1] + 1;	//node from nVERTEX
			else segnodname1 = nVERTEX + k;	//node from nPOINT
			if(j == NumEdgePoints[i] - 1) segnodname2 = EdgeConnectivity[i][2] + 1;	//node from nVERTEX
			else segnodname2 = nVERTEX + k + 1;	//node from nPOINT
			diam = FMAX(2.*thickness[k],4.);
			if(diam > diammax){
				diammax = diam;
				idiammax = k;
			}
			fprintf(ofp,"%i %i %i %i %f %f %f\n",k,segtyp,segnodname1,segnodname2,diam,q,hd);
			count++;
		}
		k++;
	}
	printf("max diam = %f, k = %i\n",diammax,idiammax);
	if(count != nseg) printf("*** Error: incorrect number of segments\n");
	fprintf(ofp,"%i	  number of nodes\n",nnod);
	fprintf(ofp,"Name    x       y       z\n");
	count = 0;
	for(i=1; i<=nVERTEX; i++){	//nodes from nVERTEX
		fprintf(ofp,"%i %f %f %f\n",i,VertexCoordinates[i][1],VertexCoordinates[i][2],VertexCoordinates[i][3] + zoffset);
		count++;
	}
	k = 0;
	inod = nVERTEX;
	for(i=1; i<=nEDGE; i++){	//nodes from nPOINT
		for(j=1; j<=NumEdgePoints[i]; j++){
			k++;
			inod++;
			if(j > 1 && j < NumEdgePoints[i]){
				fprintf(ofp,"%i %f %f %f\n",inod,EdgePointCoordinates[k][1],
					EdgePointCoordinates[k][2],EdgePointCoordinates[k][3] + zoffset);
				count++;
			}
		}
	}
	if(count != nnod) printf("*** Error: incorrect number of nodes\n");
	fclose(ofp);
}

