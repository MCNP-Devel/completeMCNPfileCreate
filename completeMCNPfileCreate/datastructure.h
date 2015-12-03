#pragma once


#include <iostream>
#include <cstdio>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>

const float TOTALWEIGHT[10] = {63000, 54000, 55000, 50000, 32000, 32000, 19000, 19000, 10000, 10000};
const float SKINWEIGHT[10] = {2400, 1800, 2200, 1700, 1200, 1200, 775, 775, 450, 450};

typedef struct McnpFillStruct
{
	short int* element;
	int dimxsup; int dimxinf;
	int dimysup; int dimyinf;
	int dimzsup; int dimzinf;
	float voxelsizex;
	float voxelsizey;
	float voxelsizez;
	int voxelcount;
}McnpFillStruct;
typedef struct Coordinate3D
{
	int x;
	int y;
	int z;
}Coordinate3D;
typedef struct VoxelReplaceRecord
{
	int countnum;  //count id nums
	int* record;   // id + num of times + id + num of times + .....
}VoxelReplaceRecord;

////////////////////////////////////////////////////////////////

int readMCNPFile(char*& inputpath, FILE** mcnpFile);
int storeGeo(FILE* mcnpfile, McnpFillStruct& mcnpgeo999);
void storeDensityAndMaterial(float *ORGANSDESTINY, int *ORGANMATERIAL, char* inputpath);
int convertXYZToSequence(int x, int y, int z, McnpFillStruct mcnpgeo999);
void correctPosition(McnpFillStruct& mcnpgeo999);
int outputGeo(McnpFillStruct mcnpgeo999);
int compareGeo(McnpFillStruct mcnpgeoA, McnpFillStruct mcnpgeoB);
int replacEyeLence(McnpFillStruct mcnpgeoA, McnpFillStruct mcnpgeoB, unsigned int idnum);
int replaceVoxelId(McnpFillStruct mcnpgeoA, unsigned int idnum, unsigned int idnumed);
Coordinate3D convertSequenceToXYZ(int sequence, McnpFillStruct mcnpgeo999);
void lenceCopy(unsigned int lenceleft, unsigned int lenceright, McnpFillStruct mcnpgeo999);
int lenceGen(char* inputpath, unsigned int lence, unsigned int bulb, McnpFillStruct& mcnpgeo999);
//计算对称轴，体模朝向Y正方向，计算X方向对称轴
int calSymmetryAxisX(McnpFillStruct mcnpgeo999);
void organCopy(unsigned int organfrom, unsigned int organfor, McnpFillStruct mcnpgeo999);
Coordinate3D* calBox(McnpFillStruct mcnpgeoA, int organid);  // test
void calSurfaceWL(McnpFillStruct mcnpgeoA, float zheight);  //计算高度zheight时的体模断层截面的宽和长 

void adjustAdrenal(unsigned int adrenalid, float aimvol, McnpFillStruct mcnpgeo999);
void adjustlence(unsigned int lenseid, float aimvol, McnpFillStruct mcnpgeo999);
void volAdjust(char* inputpath, McnpFillStruct mcnpgeo999);
void volAdjustAOrgan(int organid, float refvol, McnpFillStruct mcnpgeo999);;
float calOrganVol(unsigned int organid, McnpFillStruct mcnpgeo999);
void genForms(char* inputpath, McnpFillStruct mcnpgeo999);

// X方向增加negative(靠近x负方向一侧)和Xpositive(靠近x正方向一侧)，注意数值为负时代表消减一定体素层数，其他方向同理
int geoZoom(McnpFillStruct& mcnpgeo999, int Xnegative, int Xpositive, int Ynegatiove, int Ypositive, int Znegatiove, int Zpositive);
// dir方向增加negatiove(靠近dir负方向一侧)和positive(靠近dir正方向一侧),dir可以取'X','Y','Z'
int geoZoomSingledir(McnpFillStruct& mcnpgeo999, int negatiove, int positive, char dir);


