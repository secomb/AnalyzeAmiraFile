/******************************************************
read_amirafile
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

/*  Find a string in the given buffer and return a pointer
    to the contents directly behind the SearchString.
    If not found, return the buffer. A subsequent sscanf()
    will fail then, but at least we return a decent pointer.
*/
const char* FindAndJump(const char* buffer, const char* SearchString)
{
    const char* FoundLoc = strstr(buffer, SearchString);
    if (FoundLoc) return FoundLoc + strlen(SearchString);
    return buffer;
}


int read_amirafile(const char fname[])
{
	extern int nVERTEX,nEDGE,nPOINT;
	extern int **EdgeConnectivity,*NumEdgePoints;
	extern float **VertexCoordinates,**EdgePointCoordinates,*thickness;
	int c,k;

	//const char* fname = "vesselness3d001.Smt.SptGraph.am";
	const char* errormess = "Something went wrong while reading the binary data section.\nPremature end of file?\n";

    FILE* fp = fopen(fname, "rb");
    if(!fp){
        printf("Could not find %s\n", fname);
        return 0;
    }

    printf("Reading %s\n", fname);

    //We read the first 2k bytes into memory to parse the header.
    //The fixed buffer size looks a bit like a hack, and it is one, but it gets the job done.
    char buffer[2048];
    fread(buffer, sizeof(char), 2047, fp);
    buffer[2047] = '\0'; //The following string routines prefer null-terminated strings

    if(!strstr(buffer, "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1")){
        printf("Not a proper AmiraMesh file.\n");
        fclose(fp);
        return 0;
    }

    sscanf(FindAndJump(buffer, "VERTEX"), "%d %d %d", &nVERTEX);
    printf("\tnVERTEX: %d\n", nVERTEX);
	sscanf(FindAndJump(buffer, "EDGE"), "%d %d %d", &nEDGE);
    printf("\tnEDGE: %d\n", nEDGE);
    sscanf(FindAndJump(buffer, "POINT"), "%d %d %d", &nPOINT);
    printf("\tnPOINT: %d\n", nPOINT);

	//set up arrays
	VertexCoordinates = matrix(1,nVERTEX,1,3);
	EdgeConnectivity = imatrix(1,nEDGE,1,2);
	NumEdgePoints = ivector(1,nEDGE);
	EdgePointCoordinates = matrix(1,nPOINT,1,3);
	thickness = vector(1,nPOINT);

    //Find the beginning of the data section
    const long idxStartData = strstr(buffer, "# Data section follows") - buffer;
    if(idxStartData > 0){        
        fseek(fp, idxStartData, SEEK_SET);  //Set the file pointer to the beginning of "# Data section follows"
        fgets(buffer, 128, fp);	//Consume this line, which is "# Data section follows"        
        fgets(buffer, 128, fp);	//Consume the next line, which is "@1"
		printf("Should be @1: %s", buffer);

        //Read the data        
		const size_t NumToRead = nVERTEX*3;     // - how much to read
        float* pData = new float[NumToRead];    // - prepare memory; use malloc() if you're using pure C
        if(pData){
            const size_t ActRead = fread((void*)pData, sizeof(float), NumToRead, fp);		// - do it
            if(NumToRead != ActRead){		// - ok?
                printf(errormess);
                delete[] pData;
                fclose(fp);
                return 0;
            }			
            printf("Putting VertexCoordinates in array\n");
            for(k=0; k<nVERTEX; k++) for(c=0; c<3; c++) VertexCoordinates[k+1][c+1] = pData[k*3 + c];
            delete[] pData;
        }
		fgets(buffer, 128, fp);
		fgets(buffer, 128, fp);
		printf("Should be @2: %s", buffer);

		//Read the data        
		const size_t NumToRead1 = nEDGE*2;     
        int* pData1 = new int[NumToRead1];    
        if(pData1){
            const size_t ActRead1 = fread((void*)pData1, sizeof(int), NumToRead1, fp);	
            if(NumToRead1 != ActRead1){		
                printf(errormess);
                delete[] pData1;
                fclose(fp);
                return 0;
            }			
            printf("Putting EdgeConnectivity in array\n");
            for(k=0; k<nEDGE; k++) for(c=0; c<2; c++) EdgeConnectivity[k+1][c+1] = pData1[k*2 + c];
            delete[] pData1;
        }
		fgets(buffer, 128, fp);
		fgets(buffer, 128, fp);
		printf("Should be @3: %s", buffer);

		//Read the data        
		const size_t NumToRead2 = nEDGE;    
        int* pData2 = new int[NumToRead2];    
        if(pData2){
            const size_t ActRead2 = fread((void*)pData2, sizeof(int), NumToRead2, fp);		
            if(NumToRead2 != ActRead2){		
                printf(errormess);
                delete[] pData2;
                fclose(fp);
                return 0;
            }			
            printf("Putting NumEdgePoints in array\n");
            for(k=0; k<nEDGE; k++) NumEdgePoints[k+1] = pData2[k];
            delete[] pData2;
        }
		fgets(buffer, 128, fp);
		fgets(buffer, 128, fp);
		printf("Should be @4: %s", buffer);

       //Read the data        
		const size_t NumToRead3 = nPOINT*3;    
        float* pData3 = new float[NumToRead3];    
        if(pData3){
            const size_t ActRead3 = fread((void*)pData3, sizeof(float), NumToRead3, fp);		
            if(NumToRead3 != ActRead3){		
                printf(errormess);
                delete[] pData3;
                fclose(fp);
                return 0;
            }			
            printf("Putting EdgePointCoordinates in array\n");
            for(k=0; k<nPOINT; k++) for(c=0; c<3; c++) EdgePointCoordinates[k+1][c+1] = pData3[k*3 + c];
            delete[] pData3;
        }
		fgets(buffer, 128, fp);
		fgets(buffer, 128, fp);
		printf("Should be @5: %s", buffer);

      //Read the data        
		const size_t NumToRead4 = nPOINT;     
        float* pData4 = new float[NumToRead4];    
        if(pData4){
            const size_t ActRead4 = fread((void*)pData4, sizeof(float), NumToRead4, fp);		
            if(NumToRead4 != ActRead4){		
                printf(errormess);
                delete[] pData4;
                fclose(fp);
                return 0;
            }			
            printf("Putting thickness in array\n");
            for(k=0; k<nPOINT; k++) thickness[k+1] = pData4[k];
            delete[] pData4;
        }
	}
    fclose(fp);
	return 1;
}

