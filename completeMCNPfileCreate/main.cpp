// mcnp重复结构的读取
#include "datastructure.h"

int main()
{
	FILE* mcnpfileA;
	char* inputpath; // = "E://MCNP//AM3mm//AM3.txt";
	readMCNPFile(inputpath,&mcnpfileA);
	//mcnpfileA = fopen(inputpath, "r");
	McnpFillStruct mcnpgeoA;
	storeGeo(mcnpfileA, mcnpgeoA);

	// 储存密度和材料信息
    extern float ORGANSDESTINY[151];
    extern int ORGANMATERIAL[151];
    storeDensityAndMaterial(ORGANSDESTINY, ORGANMATERIAL, inputpath);
    ORGANMATERIAL[8] = 1; //something wrong with ORGANMATERIAL[8] */
	
    //lenceCopy(66, 68, mcnpgeoA);
	//lenceCopy(68, 66, mcnpgeoA);
	//for (int i = 0; i < mcnpgeoA.voxelcount; i++) 
	//{
 //       if (66 == *(mcnpgeoA.element + i))
	//	{
	//		*(mcnpgeoA.element + i) = 119;
	//	}
	//}
	//organCopy(69, 67, mcnpgeoA);
	//organCopy(63, 65, mcnpgeoA);
    organCopy(62, 64, mcnpgeoA);
	//organCopy(69, 67, mcnpgeoA);
	//organCopy(63, 65, mcnpgeoA);
	lenceGen(inputpath, 66, 67, mcnpgeoA);
	lenceGen(inputpath, 68, 69, mcnpgeoA);
	//adjustAdrenal(1, 6.896, mcnpgeoA);
	//adjustAdrenal(2, 6.696, mcnpgeoA);
    //adjustlence(66, 0.19, mcnpgeoA);
    //adjustlence(68, 0.19, mcnpgeoA);

	volAdjust(inputpath, mcnpgeoA);

    //calBox(mcnpgeoA, 66); //计算包围盒
    //calBox(mcnpgeoA, 67);
    //calBox(mcnpgeoA, 58);
    //calBox(mcnpgeoA, 97);
    //calBox(mcnpgeoA, 99);

    //calSurfaceWL(mcnpgeoA, 125.5);
    //calSurfaceWL(mcnpgeoA, 107.8);
    //calSurfaceWL(mcnpgeoA, 92.5);
    //calSurfaceWL(mcnpgeoA, 99.8);
    //calSurfaceWL(mcnpgeoA, 111.2);

    //correctPosition(mcnpgeoA);

	// 包围盒添加3层
	/*Coordinate3D* skinbox;

	skinbox = calBox(mcnpgeoA, 125);
	int Xnegatiove = mcnpgeoA.dimxinf - (*skinbox).x;
	int Xpositive = -mcnpgeoA.dimxsup + (*(skinbox + 1)).x;
	int Ynegatiove = mcnpgeoA.dimyinf - (*skinbox).y;
	int Ypositive = -mcnpgeoA.dimysup + (*(skinbox + 1)).y;
	int Znegatiove = mcnpgeoA.dimzinf - (*skinbox).z;
	int Zpositive = -mcnpgeoA.dimzsup + (*(skinbox + 1)).z;
	geoZoom(mcnpgeoA, Xnegatiove, Xpositive, Ynegatiove, Ypositive, Znegatiove, Zpositive);
	geoZoom(mcnpgeoA, 3, 3, 3, 3, 3, 3);*/

	// Output 
	outputGeo(mcnpgeoA);
	genForms(inputpath, mcnpgeoA);
	fclose(mcnpfileA);
	return 0;
}

