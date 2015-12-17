#include "datastructure.h"
#include "objdata.h"

const float PHOTONERG[20] = { 0.02, 0.03, 0.04, 0.05, 0.06, 0.08, 0.1, 0.15, 0.2, 0.3, 0.4 , 0.5, 0.6, 0.8, 1, 2, 4, 6, 8, 10 }; //初始光子能量MeV

int dataOutput(McnpFillStruct mcnpgeo999, FILE* &editfile, char* filepath)
{
	int ageindex = 1;
	std::cout << "*****************************************\nPlease input age index:" << std::endl;
	std::cout << "(1 AM, 2 AF, 3 15M, 4 15F, 5 10M, 6 10F, 7 5M, 8 5F, 9 1M, 10 1F, 11 RPIAM, 12 RPIAF)" << std::endl;
	do
	{
		std::cin >> ageindex;
		if (ageindex > 12 || ageindex < 0)
		{
			std::cout << "Wrong input, continue(y or no) ?" << std::endl;
			char yorn;
			std::cin >> yorn;
			if ('y' == yorn)
				continue;
			else
			{
				std::cout << "Do nothing" << std::endl;
				return 1;
			}
		}
		else
			break;
	} while (1);

	char *agename = new char[100];
	switch (ageindex)
	{
	case 1:
		strcpy(agename, "am");
		break;
	case 2:
		strcpy(agename, "af");
		break;
	case 3:
		strcpy(agename, "15m");
		break;
	case 4:
		strcpy(agename, "15f");
		break;
	case 5:
		strcpy(agename, "10m");
		break;
	case 6:
		strcpy(agename, "10f");
		break;
	case 7:
		strcpy(agename, "5m");
		break;
	case 8:
		strcpy(agename, "5f");
		break;
	case 9:
		strcpy(agename, "1m");
		break;
	case 10:
		strcpy(agename, "1f");
		break;
	case 11:
		strcpy(agename, "rpiam");
		break;
	case 12:
		strcpy(agename, "rpiaf");
		break;
	default:
		break;
	}

	char dataoutputlocate[400];
	for (int i = 0; i < 20; i++) // 能量循环，总数是20个，每个年龄段的生成20个data，命名规则 年龄_ + 方向_ + 能量_(整数代号1~20)  要全部小写
	// int i = 6;
	{
		for (int j = 0; j < 6; j++) //方向循环，分成ap pa llat rlat 
		{
			dataoutputlocate[1] = '\n';
			int dirindex = finddirpath(filepath);
			strcpy(dataoutputlocate, filepath);
			dataoutputlocate[dirindex + 1] = '\0';
			strcat(dataoutputlocate, "data_mc\\");
			strcat(dataoutputlocate, agename);
			strcat(dataoutputlocate, "_");
			switch (j)
			{
			case 0:
				strcat(dataoutputlocate, "ap_");
				break;
			case 1:
				strcat(dataoutputlocate, "pa_");
				break;
			case 2:
				strcat(dataoutputlocate, "llat_");
				break;
			case 3:
				strcat(dataoutputlocate, "rlat_");
				break;
			case 4:
				strcat(dataoutputlocate, "rot_");
				break;
			case 5:
				strcat(dataoutputlocate, "iso_");
				break;
			default:
				break;
			}
			sprintf(dataoutputlocate + strlen(dataoutputlocate), "%d", i);

			puts(dataoutputlocate); putchar('\n');

			//FILE* dataoutputfile = fopen(dataoutputlocate, "w+");

			switch (j)
			{
			case 0:
				datacardAP(mcnpgeo999, agename, i, dataoutputlocate);
				break;
			case 1:
				datacardPA(mcnpgeo999, agename, i, dataoutputlocate);
				break;
			case 2:
				datacardLLAT(mcnpgeo999, agename, i, dataoutputlocate);
				break;
			case 3:
				datacardRLAT(mcnpgeo999, agename, i, dataoutputlocate);
				break;
			case 4:
				datacardROT(mcnpgeo999, agename, i, dataoutputlocate);
				break;
			case 5:
				datacardISO(mcnpgeo999, agename, i, dataoutputlocate);
				break;
			default:
				break;
			}
		}
	}

	// fprintf(editfile, "C ******************************************************************************\n"); 
	// fprintf(editfile, "C                               data cards\n"); 
	// fprintf(editfile, "C ******************************************************************************\n");
	// 
	// fprintf(editfile, "C ------------------------elemental composition------------------------\n");
	// fprintf(editfile, "read file=elecommon.txt noecho \n");
	// fprintf(editfile, "read file=");
	// fprintf(editfile, agename);
	// fprintf(editfile, "ele.txt noecho \n");
	// fprintf(editfile, "C ------------------------source tally .... ------------------------\n");

	free(agename);
	return 0;
}

int datacardAP(McnpFillStruct mcnpgeo999, char* agename, int ergindex, char* dataoutputlocate)
{
	FILE* dataoutputfile = fopen(dataoutputlocate, "w+");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "C Begin\n");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "read file=../geo/");
	fprintf(dataoutputfile, agename);
	fprintf(dataoutputfile, ".geo noecho\n\n");

	if (0 == strcmp(agename, "rpiam") || 0 == strcmp(agename, "rpiaf"))
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/rpiele.txt noecho\n");
	}
	else
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/elecommon.txt noecho\n");
		fprintf(dataoutputfile, "read file=../material/"); fprintf(dataoutputfile, agename); fprintf(dataoutputfile, "ele.txt noecho\n");
	}

	//计算中心线
	float xmedium, ymedium, zmedium;
	xmedium = mcnpgeo999.voxelsizex * (mcnpgeo999.dimxsup + mcnpgeo999.dimxinf) / 2;
	ymedium = mcnpgeo999.voxelsizey * (mcnpgeo999.dimysup + mcnpgeo999.dimyinf) / 2;
	zmedium = mcnpgeo999.voxelsizez * (mcnpgeo999.dimzsup + mcnpgeo999.dimzinf) / 2;

	float sourceBoxXMax = xmedium + 68 / 2;
	float sourceBoxXMin = xmedium - 68 / 2;
	float sourceBoxYMax = ymedium + 42 / 2;
	float sourceBoxYMin = ymedium - 42 / 2;
	float sourceBoxZMax = zmedium + 178 / 2;
	float sourceBoxZMin = zmedium - 178 / 2;

	fprintf(dataoutputfile, "C ------------------------source tally .... ------------------------\n");
	fprintf(dataoutputfile, "SDEF X=d1 Y=");
	fprintf(dataoutputfile, "%.2f ", sourceBoxYMax);
	fprintf(dataoutputfile, "Z=d3 ERG="); fprintf(dataoutputfile, " %.2f ", PHOTONERG[ergindex]); 
	fprintf(dataoutputfile, "par=2 POS=0 ");
	fprintf(dataoutputfile, "%.2f ", sourceBoxYMax);
	fprintf(dataoutputfile, " 0 \n      VEC = 0 1 0 DIR = -1 $AP\n");
	fprintf(dataoutputfile, "si1 ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxXMin, sourceBoxXMax);
	fprintf(dataoutputfile, "sp1 0 1\n");
	fprintf(dataoutputfile, "si3   ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxZMin, sourceBoxZMax);
	fprintf(dataoutputfile, "sp3   0 1\n");
	fprintf(dataoutputfile, "nps   1e7\nmode   p e\n");
	fprintf(dataoutputfile, "imp:p,e 0 1 144r   $ need change number JY\nphys:p,e 10\ncut:e j 0.0070 $ e- cutoff 7 keV corresponds to CSDA range (0.3/10) in lung w/ rho=0.25\n");
	fprintf(dataoutputfile, "dbcn   17j 1 $ ITS indexing\n");
	fprintf(dataoutputfile, "prdmp j -600 1 1  $ outp at the end, runtpe 10h, MCTAL, 1 dump\n");
	fprintf(dataoutputfile, "print -30 -85 -86 -128 110 $ -140\n");

	fprintf(dataoutputfile, "C  tally\n");
	fprintf(dataoutputfile, "*f1008:P,E\n      ");

	int organcount[200];
	for (int i = 0; i < 200; i++)
		organcount[i] = 0;

	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		organcount[*(mcnpgeo999.element + i)]++;
	}

	int entertmp = 0; // 输出三个元素后换行
	char linetmp[100]; linetmp[0] = '\0';
	for (int i = 0; i < 200; i++)
	{
		if (0 != organcount[i])
		{
			strcat(linetmp, "( ");
			sprintf(linetmp + strlen(linetmp), "%d", i);
			strcat(linetmp, "< 666 < 555)       ");
			entertmp++;
			if (entertmp == 3)
			{
				entertmp = 0;
				fprintf(dataoutputfile, linetmp);
				fprintf(dataoutputfile, "\n      ");
				linetmp[0] = '\0';
			}
		}
	}

	if (0 == entertmp)
	{
		fprintf(dataoutputfile, "$ the end of *f8\n");
	}
	else
	{
		fprintf(dataoutputfile, linetmp);
		fprintf(dataoutputfile, "\n");
	}

	fprintf(dataoutputfile, "f14:p   ( 14< 666 < 555)  $Humeri, upper half, spongiosa  \n");
	fprintf(dataoutputfile, "de14    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10\n");
	fprintf(dataoutputfile, "df14    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f24:p   ( 25< 666 < 555)  $Clavicles, spongiosa  \n");
	fprintf(dataoutputfile, "de24    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df24    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f34:p   ( 27< 666 < 555)   $Cranium, spongiosa  \n");
	fprintf(dataoutputfile, "de34    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df34    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f44:p   ( 29< 666 < 555)   $Femora, upper half, spongiosa\n");
	fprintf(dataoutputfile, "de44    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df44    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f54:p   ( 40< 666 < 555)   $Mandible, spongiosa  \n");
	fprintf(dataoutputfile, "de54    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df54    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f64:p   ( 42< 666 < 555)    $Pelvis, spongiosa   \n");
	fprintf(dataoutputfile, "de64    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df64    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f74:p   ( 44< 666 < 555)    $Ribs, spongiosa \n");
	fprintf(dataoutputfile, "de74    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df74    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f84:p   ( 46< 666 < 555)     $Scapulae, spongiosa  \n");
	fprintf(dataoutputfile, "de84    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df84    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f94:p   ( 48< 666 < 555)      $Cervical spine, spongiosa \n");
	fprintf(dataoutputfile, "de94    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df94    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f104:p  ( 50< 666 < 555)     $ Thoracic spine, spongiosa\n");
	fprintf(dataoutputfile, "de104    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df104    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f114:p  ( 52< 666 < 555)      $Lumbar spine, spongiosa  \n");
	fprintf(dataoutputfile, "de114    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df114    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f124:p  ( 54< 666 < 555)      $Sacrum, spongiosa\n");
	fprintf(dataoutputfile, "de124    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df124    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f134:p  ( 56< 666 < 555)      $Sternum, spongiosa\n");
	fprintf(dataoutputfile, "de134    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df134    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "c Tally For ALL\n");
	fprintf(dataoutputfile, "f144:p  ((14 25 27 29 40 42 44 46 48 50 52 54 56 )< 666 < 555)      $ALL \n");
	fprintf(dataoutputfile, "de144    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df144    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");

	fprintf(dataoutputfile, "\nC -------end of the file--------");

	fclose(dataoutputfile);
	return 1;
}

int datacardPA(McnpFillStruct mcnpgeo999, char* agename, int ergindex, char* dataoutputlocate)
{
	FILE* dataoutputfile = fopen(dataoutputlocate, "w+");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "C Begin\n");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "read file=../geo/");
	fprintf(dataoutputfile, agename);
	fprintf(dataoutputfile, ".geo noecho\n\n");

	if (0 == strcmp(agename, "rpiam") || 0 == strcmp(agename, "rpiaf"))
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/rpiele.txt noecho\n");
	}
	else
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/elecommon.txt noecho\n");
		fprintf(dataoutputfile, "read file=../material/"); fprintf(dataoutputfile, agename); fprintf(dataoutputfile, "ele.txt noecho\n");
	}

	//计算中心线
	float xmedium, ymedium, zmedium;
	xmedium = mcnpgeo999.voxelsizex * (mcnpgeo999.dimxsup + mcnpgeo999.dimxinf) / 2;
	ymedium = mcnpgeo999.voxelsizey * (mcnpgeo999.dimysup + mcnpgeo999.dimyinf) / 2;
	zmedium = mcnpgeo999.voxelsizez * (mcnpgeo999.dimzsup + mcnpgeo999.dimzinf) / 2;

	float sourceBoxXMax = xmedium + 68 / 2;
	float sourceBoxXMin = xmedium - 68 / 2;
	float sourceBoxYMax = ymedium + 42 / 2;
	float sourceBoxYMin = ymedium - 42 / 2;
	float sourceBoxZMax = zmedium + 178 / 2;
	float sourceBoxZMin = zmedium - 178 / 2;

	fprintf(dataoutputfile, "C ------------------------source tally .... ------------------------\n");
	fprintf(dataoutputfile, "SDEF X=d1 Y=");
	fprintf(dataoutputfile, "%.2f ", sourceBoxYMin);
	fprintf(dataoutputfile, "Z=d3 ERG="); fprintf(dataoutputfile, " %.2f ", PHOTONERG[ergindex]);
	fprintf(dataoutputfile, "par=2 POS=0 ");
	fprintf(dataoutputfile, "%.2f ", sourceBoxYMin);
	fprintf(dataoutputfile, " 0 \n      VEC = 0 1 0 DIR = 1 $PA\n");
	fprintf(dataoutputfile, "si1 ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxXMin, sourceBoxXMax);
	fprintf(dataoutputfile, "sp1 0 1\n");
	fprintf(dataoutputfile, "si3   ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxZMin, sourceBoxZMax);
	fprintf(dataoutputfile, "sp3   0 1\n");
	fprintf(dataoutputfile, "nps   1e7\nmode    p e\n");
	fprintf(dataoutputfile, "imp:p,e 0 1 144r   $ need change number JY\nphys:p,e 10\ncut:e j 0.0070 $ e- cutoff 7 keV corresponds to CSDA range (0.3/10) in lung w/ rho=0.25\n");
	fprintf(dataoutputfile, "dbcn   17j 1 $ ITS indexing\n");
	fprintf(dataoutputfile, "prdmp j -600 1 1  $ outp at the end, runtpe 10h, MCTAL, 1 dump\n");
	fprintf(dataoutputfile, "print -30 -85 -86 -128 110 $ -140\n");

	fprintf(dataoutputfile, "C  tally\n");
	fprintf(dataoutputfile, "*f1008:P,E\n      ");

	int organcount[200];
	for (int i = 0; i < 200; i++)
		organcount[i] = 0;

	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		organcount[*(mcnpgeo999.element + i)]++;
	}

	int entertmp = 0; // 输出三个元素后换行
	char linetmp[100]; linetmp[0] = '\0';
	for (int i = 0; i < 200; i++)
	{
		if (0 != organcount[i])
		{
			strcat(linetmp, "( ");
			sprintf(linetmp + strlen(linetmp), "%d", i);
			strcat(linetmp, "< 666 < 555)       ");
			entertmp++;
			if (entertmp == 3)
			{
				entertmp = 0;
				fprintf(dataoutputfile, linetmp);
				fprintf(dataoutputfile, "\n      ");
				linetmp[0] = '\0';
			}
		}
	}

	if (0 == entertmp)
	{
		fprintf(dataoutputfile, "$ the end of *f8\n");
	}
	else
	{
		fprintf(dataoutputfile, linetmp);
		fprintf(dataoutputfile, "\n");
	}

	fprintf(dataoutputfile, "f14:p   ( 14< 666 < 555)  $Humeri, upper half, spongiosa  \n");
	fprintf(dataoutputfile, "de14    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10\n");
	fprintf(dataoutputfile, "df14    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f24:p   ( 25< 666 < 555)  $Clavicles, spongiosa  \n");
	fprintf(dataoutputfile, "de24    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df24    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f34:p   ( 27< 666 < 555)   $Cranium, spongiosa  \n");
	fprintf(dataoutputfile, "de34    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df34    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f44:p   ( 29< 666 < 555)   $Femora, upper half, spongiosa\n");
	fprintf(dataoutputfile, "de44    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df44    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f54:p   ( 40< 666 < 555)   $Mandible, spongiosa  \n");
	fprintf(dataoutputfile, "de54    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df54    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f64:p   ( 42< 666 < 555)    $Pelvis, spongiosa   \n");
	fprintf(dataoutputfile, "de64    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df64    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f74:p   ( 44< 666 < 555)    $Ribs, spongiosa \n");
	fprintf(dataoutputfile, "de74    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df74    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f84:p   ( 46< 666 < 555)     $Scapulae, spongiosa  \n");
	fprintf(dataoutputfile, "de84    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df84    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f94:p   ( 48< 666 < 555)      $Cervical spine, spongiosa \n");
	fprintf(dataoutputfile, "de94    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df94    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f104:p  ( 50< 666 < 555)     $ Thoracic spine, spongiosa\n");
	fprintf(dataoutputfile, "de104    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df104    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f114:p  ( 52< 666 < 555)      $Lumbar spine, spongiosa  \n");
	fprintf(dataoutputfile, "de114    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df114    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f124:p  ( 54< 666 < 555)      $Sacrum, spongiosa\n");
	fprintf(dataoutputfile, "de124    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df124    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f134:p  ( 56< 666 < 555)      $Sternum, spongiosa\n");
	fprintf(dataoutputfile, "de134    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df134    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "c Tally For ALL\n");
	fprintf(dataoutputfile, "f144:p  ((14 25 27 29 40 42 44 46 48 50 52 54 56 )< 666 < 555)      $ALL \n");
	fprintf(dataoutputfile, "de144    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df144    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");

	fprintf(dataoutputfile, "\nC -------end of the file--------");

	fclose(dataoutputfile);
	return 1;
}

int datacardLLAT(McnpFillStruct mcnpgeo999, char* agename, int ergindex, char* dataoutputlocate)
{
	FILE* dataoutputfile = fopen(dataoutputlocate, "w+");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "C Begin\n");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "read file=../geo/");
	fprintf(dataoutputfile, agename);
	fprintf(dataoutputfile, ".geo noecho\n\n");

	if (0 == strcmp(agename, "rpiam") || 0 == strcmp(agename, "rpiaf"))
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/rpiele.txt noecho\n");
	}
	else
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/elecommon.txt noecho\n");
		fprintf(dataoutputfile, "read file=../material/"); fprintf(dataoutputfile, agename); fprintf(dataoutputfile, "ele.txt noecho\n");
	}

	//计算中心线
	float xmedium, ymedium, zmedium;
	xmedium = mcnpgeo999.voxelsizex * (mcnpgeo999.dimxsup + mcnpgeo999.dimxinf) / 2;
	ymedium = mcnpgeo999.voxelsizey * (mcnpgeo999.dimysup + mcnpgeo999.dimyinf) / 2;
	zmedium = mcnpgeo999.voxelsizez * (mcnpgeo999.dimzsup + mcnpgeo999.dimzinf) / 2;

	float sourceBoxXMax = xmedium + 68 / 2;
	float sourceBoxXMin = xmedium - 68 / 2;
	float sourceBoxYMax = ymedium + 42 / 2;
	float sourceBoxYMin = ymedium - 42 / 2;
	float sourceBoxZMax = zmedium + 178 / 2;
	float sourceBoxZMin = zmedium - 178 / 2;

	fprintf(dataoutputfile, "C ------------------------source tally .... ------------------------\n");
	fprintf(dataoutputfile, "SDEF X=");
	fprintf(dataoutputfile, "%.2f ", sourceBoxXMin);
	fprintf(dataoutputfile, "Y = d2");
	fprintf(dataoutputfile, " Z=d3 ERG="); fprintf(dataoutputfile, " %.2f ", PHOTONERG[ergindex]);
	fprintf(dataoutputfile, "par=2 POS=0 ");
	fprintf(dataoutputfile, "%.2f ", sourceBoxYMin);
	fprintf(dataoutputfile, " 0 VEC = 1 0 0 DIR = 1 $LLAT\n");
	fprintf(dataoutputfile, "si2    ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxYMin, sourceBoxYMax);
	fprintf(dataoutputfile, "sp2   0 1\n");
	fprintf(dataoutputfile, "si3   ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxZMin, sourceBoxZMax);
	fprintf(dataoutputfile, "sp3   0 1\n");
	fprintf(dataoutputfile, "nps   1e7\nmode    p e\n");
	fprintf(dataoutputfile, "imp:p,e 0 1 144r   $ need change number JY\nphys:p,e 10\ncut:e j 0.0070 $ e- cutoff 7 keV corresponds to CSDA range (0.3/10) in lung w/ rho=0.25\n");
	fprintf(dataoutputfile, "dbcn   17j 1 $ ITS indexing\n");
	fprintf(dataoutputfile, "prdmp j -600 1 1  $ outp at the end, runtpe 10h, MCTAL, 1 dump\n");
	fprintf(dataoutputfile, "print -30 -85 -86 -128 110 $ -140\n");

	fprintf(dataoutputfile, "C  tally\n");
	fprintf(dataoutputfile, "*f1008:P,E\n      ");

	int organcount[200];
	for (int i = 0; i < 200; i++)
		organcount[i] = 0;

	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		organcount[*(mcnpgeo999.element + i)]++;
	}

	int entertmp = 0; // 输出三个元素后换行
	char linetmp[100]; linetmp[0] = '\0';
	for (int i = 0; i < 200; i++)
	{
		if (0 != organcount[i])
		{
			strcat(linetmp, "( ");
			sprintf(linetmp + strlen(linetmp), "%d", i);
			strcat(linetmp, "< 666 < 555)       ");
			entertmp++;
			if (entertmp == 3)
			{
				entertmp = 0;
				fprintf(dataoutputfile, linetmp);
				fprintf(dataoutputfile, "\n      ");
				linetmp[0] = '\0';
			}
		}
	}

	if (0 == entertmp)
	{
		fprintf(dataoutputfile, "$ the end of *f8\n");
	}
	else
	{
		fprintf(dataoutputfile, linetmp);
		fprintf(dataoutputfile, "\n");
	}

	fprintf(dataoutputfile, "f14:p   ( 14< 666 < 555)  $Humeri, upper half, spongiosa  \n");
	fprintf(dataoutputfile, "de14    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10\n");
	fprintf(dataoutputfile, "df14    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f24:p   ( 25< 666 < 555)  $Clavicles, spongiosa  \n");
	fprintf(dataoutputfile, "de24    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df24    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f34:p   ( 27< 666 < 555)   $Cranium, spongiosa  \n");
	fprintf(dataoutputfile, "de34    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df34    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f44:p   ( 29< 666 < 555)   $Femora, upper half, spongiosa\n");
	fprintf(dataoutputfile, "de44    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df44    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f54:p   ( 40< 666 < 555)   $Mandible, spongiosa  \n");
	fprintf(dataoutputfile, "de54    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df54    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f64:p   ( 42< 666 < 555)    $Pelvis, spongiosa   \n");
	fprintf(dataoutputfile, "de64    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df64    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f74:p   ( 44< 666 < 555)    $Ribs, spongiosa \n");
	fprintf(dataoutputfile, "de74    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df74    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f84:p   ( 46< 666 < 555)     $Scapulae, spongiosa  \n");
	fprintf(dataoutputfile, "de84    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df84    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f94:p   ( 48< 666 < 555)      $Cervical spine, spongiosa \n");
	fprintf(dataoutputfile, "de94    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df94    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f104:p  ( 50< 666 < 555)     $ Thoracic spine, spongiosa\n");
	fprintf(dataoutputfile, "de104    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df104    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f114:p  ( 52< 666 < 555)      $Lumbar spine, spongiosa  \n");
	fprintf(dataoutputfile, "de114    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df114    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f124:p  ( 54< 666 < 555)      $Sacrum, spongiosa\n");
	fprintf(dataoutputfile, "de124    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df124    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f134:p  ( 56< 666 < 555)      $Sternum, spongiosa\n");
	fprintf(dataoutputfile, "de134    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df134    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "c Tally For ALL\n");
	fprintf(dataoutputfile, "f144:p  ((14 25 27 29 40 42 44 46 48 50 52 54 56 )< 666 < 555)      $ALL \n");
	fprintf(dataoutputfile, "de144    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df144    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");

	fprintf(dataoutputfile, "\nC -------end of the file--------");

	fclose(dataoutputfile);
	return 1;
}

int datacardRLAT(McnpFillStruct mcnpgeo999, char* agename, int ergindex, char* dataoutputlocate)
{
	FILE* dataoutputfile = fopen(dataoutputlocate, "w+");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "C Begin\n");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "read file=../geo/");
	fprintf(dataoutputfile, agename);
	fprintf(dataoutputfile, ".geo noecho\n\n");

	if (0 == strcmp(agename, "rpiam") || 0 == strcmp(agename, "rpiaf"))
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/rpiele.txt noecho\n");
	}
	else
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/elecommon.txt noecho\n");
		fprintf(dataoutputfile, "read file=../material/"); fprintf(dataoutputfile, agename); fprintf(dataoutputfile, "ele.txt noecho\n");
	}

	//计算中心线
	float xmedium, ymedium, zmedium;
	xmedium = mcnpgeo999.voxelsizex * (mcnpgeo999.dimxsup + mcnpgeo999.dimxinf) / 2;
	ymedium = mcnpgeo999.voxelsizey * (mcnpgeo999.dimysup + mcnpgeo999.dimyinf) / 2;
	zmedium = mcnpgeo999.voxelsizez * (mcnpgeo999.dimzsup + mcnpgeo999.dimzinf) / 2;

	float sourceBoxXMax = xmedium + 68 / 2;
	float sourceBoxXMin = xmedium - 68 / 2;
	float sourceBoxYMax = ymedium + 42 / 2;
	float sourceBoxYMin = ymedium - 42 / 2;
	float sourceBoxZMax = zmedium + 178 / 2;
	float sourceBoxZMin = zmedium - 178 / 2;

	fprintf(dataoutputfile, "C ------------------------source tally .... ------------------------\n");
	fprintf(dataoutputfile, "SDEF X=");
	fprintf(dataoutputfile, "%.2f ", sourceBoxXMax);
	fprintf(dataoutputfile, "Y = d2");
	fprintf(dataoutputfile, " Z=d3 ERG="); fprintf(dataoutputfile, " %.2f ", PHOTONERG[ergindex]);
	fprintf(dataoutputfile, "par=2 POS=0 ");
	fprintf(dataoutputfile, "%.2f ", sourceBoxYMin);
	fprintf(dataoutputfile, " 0 VEC = 1 0 0 DIR = -1 $RLAT\n");
	fprintf(dataoutputfile, "si2    ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxYMin, sourceBoxYMax);
	fprintf(dataoutputfile, "sp2   0 1\n");
	fprintf(dataoutputfile, "si3   ");
	fprintf(dataoutputfile, "%.2f %.2f\n", sourceBoxZMin, sourceBoxZMax);
	fprintf(dataoutputfile, "sp3   0 1\n");
	fprintf(dataoutputfile, "nps   1e7\nmode   p e\n");
	fprintf(dataoutputfile, "imp:p,e 0 1 144r   $ need change number JY\nphys:p,e 10\ncut:e j 0.0070 $ e- cutoff 7 keV corresponds to CSDA range (0.3/10) in lung w/ rho=0.25\n");
	fprintf(dataoutputfile, "dbcn   17j 1 $ ITS indexing\n");
	fprintf(dataoutputfile, "prdmp j -600 1 1  $ outp at the end, runtpe 10h, MCTAL, 1 dump\n");
	fprintf(dataoutputfile, "print -30 -85 -86 -128 110 $ -140\n");

	fprintf(dataoutputfile, "C  tally\n");
	fprintf(dataoutputfile, "*f1008:P,E\n      ");

	int organcount[200];
	for (int i = 0; i < 200; i++)
		organcount[i] = 0;

	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		organcount[*(mcnpgeo999.element + i)]++;
	}

	int entertmp = 0; // 输出三个元素后换行
	char linetmp[100]; linetmp[0] = '\0';
	for (int i = 0; i < 200; i++)
	{
		if (0 != organcount[i])
		{
			strcat(linetmp, "( ");
			sprintf(linetmp + strlen(linetmp), "%d", i);
			strcat(linetmp, "< 666 < 555)       ");
			entertmp++;
			if (entertmp == 3)
			{
				entertmp = 0;
				fprintf(dataoutputfile, linetmp);
				fprintf(dataoutputfile, "\n      ");
				linetmp[0] = '\0';
			}
		}
	}

	if (0 == entertmp)
	{
		fprintf(dataoutputfile, "$ the end of *f8\n");
	}
	else
	{
		fprintf(dataoutputfile, linetmp);
		fprintf(dataoutputfile, "\n");
	}

	fprintf(dataoutputfile, "f14:p   ( 14< 666 < 555)  $Humeri, upper half, spongiosa  \n");
	fprintf(dataoutputfile, "de14    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10\n");
	fprintf(dataoutputfile, "df14    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f24:p   ( 25< 666 < 555)  $Clavicles, spongiosa  \n");
	fprintf(dataoutputfile, "de24    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df24    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f34:p   ( 27< 666 < 555)   $Cranium, spongiosa  \n");
	fprintf(dataoutputfile, "de34    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df34    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f44:p   ( 29< 666 < 555)   $Femora, upper half, spongiosa\n");
	fprintf(dataoutputfile, "de44    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df44    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f54:p   ( 40< 666 < 555)   $Mandible, spongiosa  \n");
	fprintf(dataoutputfile, "de54    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df54    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f64:p   ( 42< 666 < 555)    $Pelvis, spongiosa   \n");
	fprintf(dataoutputfile, "de64    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df64    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f74:p   ( 44< 666 < 555)    $Ribs, spongiosa \n");
	fprintf(dataoutputfile, "de74    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df74    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f84:p   ( 46< 666 < 555)     $Scapulae, spongiosa  \n");
	fprintf(dataoutputfile, "de84    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df84    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f94:p   ( 48< 666 < 555)      $Cervical spine, spongiosa \n");
	fprintf(dataoutputfile, "de94    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df94    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f104:p  ( 50< 666 < 555)     $ Thoracic spine, spongiosa\n");
	fprintf(dataoutputfile, "de104    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df104    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f114:p  ( 52< 666 < 555)      $Lumbar spine, spongiosa  \n");
	fprintf(dataoutputfile, "de114    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df114    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f124:p  ( 54< 666 < 555)      $Sacrum, spongiosa\n");
	fprintf(dataoutputfile, "de124    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df124    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f134:p  ( 56< 666 < 555)      $Sternum, spongiosa\n");
	fprintf(dataoutputfile, "de134    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df134    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "c Tally For ALL\n");
	fprintf(dataoutputfile, "f144:p  ((14 25 27 29 40 42 44 46 48 50 52 54 56 )< 666 < 555)      $ALL \n");
	fprintf(dataoutputfile, "de144    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df144    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");

	fprintf(dataoutputfile, "\nC -------end of the file--------");

	fclose(dataoutputfile);
	return 1;
}

int datacardROT(McnpFillStruct mcnpgeo999, char* agename, int ergindex, char* dataoutputlocate)
{
	FILE* dataoutputfile = fopen(dataoutputlocate, "w+");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "C Begin\n");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "read file=../geo/");
	fprintf(dataoutputfile, agename);
	fprintf(dataoutputfile, ".geo noecho\n\n");

	if (0 == strcmp(agename, "rpiam") || 0 == strcmp(agename, "rpiaf"))
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/rpiele.txt noecho\n");
	}
	else
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/elecommon.txt noecho\n");
		fprintf(dataoutputfile, "read file=../material/"); fprintf(dataoutputfile, agename); fprintf(dataoutputfile, "ele.txt noecho\n");
	}

	//计算中心线
	float xmedium, ymedium, zmedium;
	xmedium = mcnpgeo999.voxelsizex * (mcnpgeo999.dimxsup + mcnpgeo999.dimxinf) / 2;
	ymedium = mcnpgeo999.voxelsizey * (mcnpgeo999.dimysup + mcnpgeo999.dimyinf) / 2;
	zmedium = mcnpgeo999.voxelsizez * (mcnpgeo999.dimzsup + mcnpgeo999.dimzinf) / 2;

	float sourceBoxXMax = xmedium + 68 / 2;
	float sourceBoxXMin = xmedium - 68 / 2;
	float sourceBoxYMax = ymedium + 42 / 2;
	float sourceBoxYMin = ymedium - 42 / 2;
	float sourceBoxZMax = zmedium + 178 / 2;
	float sourceBoxZMin = zmedium - 178 / 2;

	fprintf(dataoutputfile, "C ------------------------source tally .... ------------------------\n");
	fprintf(dataoutputfile, "SDEF  VEC=d1 X=FVEC d2 Y=FVEC d3 Z=d4 POS=FVEC d5 DIR=-1 ");
	fprintf(dataoutputfile, "ERG="); fprintf(dataoutputfile, " %.2f ", PHOTONERG[ergindex]);
	fprintf(dataoutputfile, " PAR = 2 $ ROT\n");
	fprintf(dataoutputfile, "SI1   L 1 0 0   -1 0 0   0 1 0   0 -1 0\n");
	fprintf(dataoutputfile, "SP1       1        1       1        1\n");
	fprintf(dataoutputfile, "DS2   S   7        8       9        10\n");
	fprintf(dataoutputfile, "DS3   S   11        12       13        14\n");
	fprintf(dataoutputfile, "SI4   H  %.2f  %.2f\n", sourceBoxZMin, sourceBoxZMax);
	fprintf(dataoutputfile, "SP4       0  1\n");
	fprintf(dataoutputfile, "DS5   L %.2f 0 0  %.2f 0 0  0 %.2f 0  0 %.2f 0\n", sourceBoxXMax, sourceBoxXMin, sourceBoxYMax, sourceBoxYMax);
	fprintf(dataoutputfile, "c ------for DSi s-----\n");
	fprintf(dataoutputfile, "SI7   L  %.2f\n", sourceBoxXMax);
	fprintf(dataoutputfile, "SP7       1\n");
	fprintf(dataoutputfile, "SI8   L  %.2f\n", sourceBoxXMin);
	fprintf(dataoutputfile, "SP8       1\n");
	fprintf(dataoutputfile, "SI9   H  %.2f  %.2f\n", sourceBoxXMin, sourceBoxXMax);
	fprintf(dataoutputfile, "SP9       0   1\n");
	fprintf(dataoutputfile, "SI10  H  %.2f  %.2f\n", sourceBoxXMin, sourceBoxXMax);
	fprintf(dataoutputfile, "SP10      0   1\n");
	fprintf(dataoutputfile, "SI11  H  %.2f  %.2f\n", sourceBoxYMin, sourceBoxYMax);
	fprintf(dataoutputfile, "SP11      0   1\n");
	fprintf(dataoutputfile, "SI12  H  %.2f  %.2f\n", sourceBoxYMin, sourceBoxYMax);
	fprintf(dataoutputfile, "SP12      0   1\n");
	fprintf(dataoutputfile, "SI13  L  %.2f\n", sourceBoxYMax);
	fprintf(dataoutputfile, "SP13      1\n");
	fprintf(dataoutputfile, "SI14  L  %.2f\n", sourceBoxYMin);
	fprintf(dataoutputfile, "SP14      1\n");

	fprintf(dataoutputfile, "nps   1e7\nmode   p e\n");
	fprintf(dataoutputfile, "imp:p,e 0 1 144r   $ need change number JY\nphys:p,e 10\ncut:e j 0.0070 $ e- cutoff 7 keV corresponds to CSDA range (0.3/10) in lung w/ rho=0.25\n");
	fprintf(dataoutputfile, "dbcn   17j 1 $ ITS indexing\n");
	fprintf(dataoutputfile, "prdmp j -600 1 1  $ outp at the end, runtpe 10h, MCTAL, 1 dump\n");
	fprintf(dataoutputfile, "print -30 -85 -86 -128 110 $ -140\n");

	fprintf(dataoutputfile, "C  tally\n");
	fprintf(dataoutputfile, "*f1008:P,E\n      ");

	int organcount[200];
	for (int i = 0; i < 200; i++)
		organcount[i] = 0;

	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		organcount[*(mcnpgeo999.element + i)]++;
	}

	int entertmp = 0; // 输出三个元素后换行
	char linetmp[100]; linetmp[0] = '\0';
	for (int i = 0; i < 200; i++)
	{
		if (0 != organcount[i])
		{
			strcat(linetmp, "( ");
			sprintf(linetmp + strlen(linetmp), "%d", i);
			strcat(linetmp, "< 666 < 555)       ");
			entertmp++;
			if (entertmp == 3)
			{
				entertmp = 0;
				fprintf(dataoutputfile, linetmp);
				fprintf(dataoutputfile, "\n      ");
				linetmp[0] = '\0';
			}
		}
	}

	if (0 == entertmp)
	{
		fprintf(dataoutputfile, "$ the end of *f8\n");
	}
	else
	{
		fprintf(dataoutputfile, linetmp);
		fprintf(dataoutputfile, "\n");
	}

	fprintf(dataoutputfile, "f14:p   ( 14< 666 < 555)  $Humeri, upper half, spongiosa  \n");
	fprintf(dataoutputfile, "de14    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10\n");
	fprintf(dataoutputfile, "df14    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f24:p   ( 25< 666 < 555)  $Clavicles, spongiosa  \n");
	fprintf(dataoutputfile, "de24    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df24    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f34:p   ( 27< 666 < 555)   $Cranium, spongiosa  \n");
	fprintf(dataoutputfile, "de34    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df34    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f44:p   ( 29< 666 < 555)   $Femora, upper half, spongiosa\n");
	fprintf(dataoutputfile, "de44    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df44    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f54:p   ( 40< 666 < 555)   $Mandible, spongiosa  \n");
	fprintf(dataoutputfile, "de54    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df54    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f64:p   ( 42< 666 < 555)    $Pelvis, spongiosa   \n");
	fprintf(dataoutputfile, "de64    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df64    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f74:p   ( 44< 666 < 555)    $Ribs, spongiosa \n");
	fprintf(dataoutputfile, "de74    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df74    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f84:p   ( 46< 666 < 555)     $Scapulae, spongiosa  \n");
	fprintf(dataoutputfile, "de84    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df84    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f94:p   ( 48< 666 < 555)      $Cervical spine, spongiosa \n");
	fprintf(dataoutputfile, "de94    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df94    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f104:p  ( 50< 666 < 555)     $ Thoracic spine, spongiosa\n");
	fprintf(dataoutputfile, "de104    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df104    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f114:p  ( 52< 666 < 555)      $Lumbar spine, spongiosa  \n");
	fprintf(dataoutputfile, "de114    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df114    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f124:p  ( 54< 666 < 555)      $Sacrum, spongiosa\n");
	fprintf(dataoutputfile, "de124    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df124    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f134:p  ( 56< 666 < 555)      $Sternum, spongiosa\n");
	fprintf(dataoutputfile, "de134    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df134    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "c Tally For ALL\n");
	fprintf(dataoutputfile, "f144:p  ((14 25 27 29 40 42 44 46 48 50 52 54 56 )< 666 < 555)      $ALL \n");
	fprintf(dataoutputfile, "de144    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df144    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");

	fprintf(dataoutputfile, "\nC -------end of the file--------");

	fclose(dataoutputfile);
	return 1;
}

int datacardISO(McnpFillStruct mcnpgeo999, char* agename, int ergindex, char* dataoutputlocate)
{
	//计算中心点
	float xmedium, ymedium, zmedium;
	xmedium = mcnpgeo999.voxelsizex * (mcnpgeo999.dimxsup + mcnpgeo999.dimxinf) / 2;
	ymedium = mcnpgeo999.voxelsizey * (mcnpgeo999.dimysup + mcnpgeo999.dimyinf) / 2;
	zmedium = mcnpgeo999.voxelsizez * (mcnpgeo999.dimzsup + mcnpgeo999.dimzinf) / 2;

	float sourceBoxXMax = xmedium + 68 / 2;
	float sourceBoxXMin = xmedium - 68 / 2;
	float sourceBoxYMax = ymedium + 42 / 2;
	float sourceBoxYMin = ymedium - 42 / 2;
	float sourceBoxZMax = zmedium + 178 / 2;
	float sourceBoxZMin = zmedium - 178 / 2;
	float rafiumtmp = 95;

	FILE* dataoutputfile = fopen(dataoutputlocate, "w+");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "C Begin\n");
	fprintf(dataoutputfile, "C ******************************************************************************\n");
	fprintf(dataoutputfile, "read file=../geo/");
	fprintf(dataoutputfile, agename);
	fprintf(dataoutputfile, ".geo noecho\n");
	fprintf(dataoutputfile, "5     S ");
	fprintf(dataoutputfile, "%.2f %.2f %.2f %.2f $ For IOS source define, the centre of sphere\n\n", xmedium, ymedium, zmedium, rafiumtmp);

	if (0 == strcmp(agename, "rpiam") || 0 == strcmp(agename, "rpiaf"))
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/rpiele.txt noecho\n");
	}
	else
	{
		fprintf(dataoutputfile, "C ------------------------elemental composition------------------------\n");
		fprintf(dataoutputfile, "read file=../material/elecommon.txt noecho\n");
		fprintf(dataoutputfile, "read file=../material/"); fprintf(dataoutputfile, agename); fprintf(dataoutputfile, "ele.txt noecho\n");
	}

	fprintf(dataoutputfile, "C ------------------------source tally .... ------------------------\n");
	fprintf(dataoutputfile, "SDEF ERG="); fprintf(dataoutputfile, " %.2f ", PHOTONERG[ergindex]);
	fprintf(dataoutputfile, "POS= %.2f %.2f %.2f ", xmedium, ymedium, zmedium);
	fprintf(dataoutputfile, "SUR=5 nrm=-1\n");

	fprintf(dataoutputfile, "nps   1e7\nmode   p e\n");
	fprintf(dataoutputfile, "imp:p,e 0 1 144r   $ need change number JY\nphys:p,e 10\ncut:e j 0.0070 $ e- cutoff 7 keV corresponds to CSDA range (0.3/10) in lung w/ rho=0.25\n");
	fprintf(dataoutputfile, "dbcn   17j 1 $ ITS indexing\n");
	fprintf(dataoutputfile, "prdmp j -600 1 1  $ outp at the end, runtpe 10h, MCTAL, 1 dump\n");
	fprintf(dataoutputfile, "print -30 -85 -86 -128 110 $ -140\n");

	fprintf(dataoutputfile, "C  tally\n");
	fprintf(dataoutputfile, "*f1008:P,E\n      ");

	int organcount[200];
	for (int i = 0; i < 200; i++)
		organcount[i] = 0;

	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		organcount[*(mcnpgeo999.element + i)]++;
	}

	int entertmp = 0; // 输出三个元素后换行
	char linetmp[100]; linetmp[0] = '\0';
	for (int i = 0; i < 200; i++)
	{
		if (0 != organcount[i])
		{
			strcat(linetmp, "( ");
			sprintf(linetmp + strlen(linetmp), "%d", i);
			strcat(linetmp, "< 666 < 555)       ");
			entertmp++;
			if (entertmp == 3)
			{
				entertmp = 0;
				fprintf(dataoutputfile, linetmp);
				fprintf(dataoutputfile, "\n      ");
				linetmp[0] = '\0';
			}
		}
	}

	if (0 == entertmp)
	{
		fprintf(dataoutputfile, "$ the end of *f8\n");
	}
	else
	{
		fprintf(dataoutputfile, linetmp);
		fprintf(dataoutputfile, "\n");
	}

	fprintf(dataoutputfile, "f14:p   ( 14< 666 < 555)  $Humeri, upper half, spongiosa  \n");
	fprintf(dataoutputfile, "de14    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10\n");
	fprintf(dataoutputfile, "df14    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f24:p   ( 25< 666 < 555)  $Clavicles, spongiosa  \n");
	fprintf(dataoutputfile, "de24    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df24    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f34:p   ( 27< 666 < 555)   $Cranium, spongiosa  \n");
	fprintf(dataoutputfile, "de34    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df34    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f44:p   ( 29< 666 < 555)   $Femora, upper half, spongiosa\n");
	fprintf(dataoutputfile, "de44    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df44    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f54:p   ( 40< 666 < 555)   $Mandible, spongiosa  \n");
	fprintf(dataoutputfile, "de54    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df54    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f64:p   ( 42< 666 < 555)    $Pelvis, spongiosa   \n");
	fprintf(dataoutputfile, "de64    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df64    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f74:p   ( 44< 666 < 555)    $Ribs, spongiosa \n");
	fprintf(dataoutputfile, "de74    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df74    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f84:p   ( 46< 666 < 555)     $Scapulae, spongiosa  \n");
	fprintf(dataoutputfile, "de84    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df84    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f94:p   ( 48< 666 < 555)      $Cervical spine, spongiosa \n");
	fprintf(dataoutputfile, "de94    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03       &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df94    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226   &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f104:p  ( 50< 666 < 555)     $ Thoracic spine, spongiosa\n");
	fprintf(dataoutputfile, "de104    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df104    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f114:p  ( 52< 666 < 555)      $Lumbar spine, spongiosa  \n");
	fprintf(dataoutputfile, "de114    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df114    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f124:p  ( 54< 666 < 555)      $Sacrum, spongiosa\n");
	fprintf(dataoutputfile, "de124    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df124    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "f134:p  ( 56< 666 < 555)      $Sternum, spongiosa\n");
	fprintf(dataoutputfile, "de134    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df134    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");
	fprintf(dataoutputfile, "c Tally For ALL\n");
	fprintf(dataoutputfile, "f144:p  ((14 25 27 29 40 42 44 46 48 50 52 54 56 )< 666 < 555)      $ALL \n");
	fprintf(dataoutputfile, "de144    0.001  0.002  0.004  0.006  0.008  0.01  0.015  0.02  0.03      &\n");
	fprintf(dataoutputfile, "        0.04  0.05  0.06  0.08  0.1  0.12  0.14  0.15  0.2  0.3  0.4     &\n");
	fprintf(dataoutputfile, "        0.5  0.6  0.8  1  2  3  4  5  6  8  10                           \n");
	fprintf(dataoutputfile, "df144    3.087265918  0.915917603  0.247515605  0.108676654  0.06119226  &\n");
	fprintf(dataoutputfile, "        0.038308365  0.016111111  0.008689139  0.003876404  0.002521848  &\n");
	fprintf(dataoutputfile, "        0.002059925  0.001910112  0.002122347  0.002602996  0.003189763  &\n");
	fprintf(dataoutputfile, "        0.003801498  0.004138577  0.005942572  0.009606742  0.013158552  &\n");
	fprintf(dataoutputfile, "        0.016548065  0.019769039  0.025736579  0.03113608  0.052297129   &\n");
	fprintf(dataoutputfile, "        0.068489388  0.082440699  0.095262172  0.107434457  0.130667915  &\n");
	fprintf(dataoutputfile, "        0.153426966\n");

	fprintf(dataoutputfile, "\nC -------end of the file--------");

	fclose(dataoutputfile);
	return 1;
}