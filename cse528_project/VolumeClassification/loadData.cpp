#include "loadData.h"
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <omp.h>
#include "gaussian.h"

std::vector<float> coloredVolume;
const float transparency = 0.5f;
std::vector<float[3]> colorMap;
const size_t sr = 256;
size_t sx = 1, sy = 1, sz = 1;
size_t nsx, nsy, nsz;
int curDataIdx, curResLevel;

//all the data path
const std::string cmpRootPath("../DataSet/MatFile/");
const std::string cmpFileSet[]=
{
	cmpRootPath + "parula.dat",						//0
	cmpRootPath + "viridis.dat"
};
const std::string volRootPath("../DataSet/RawFile/");
const std::string volFilePath[] =
{
	volRootPath + "aneurism/",				//0
	volRootPath + "my_ph_128/",
	volRootPath + "foot/",
	volRootPath + "head/",
	volRootPath + "my_ph_64/",
	volRootPath + "thigh/",					//5
	volRootPath + "skull/"
};
const int numOfDataSet = 7;
const int volSize[numOfDataSet][3] =
{
	256,256,256,
	128,128,128,
	256,256,256,
	256,256,109,
	64,64,64,
	64,64,1,
	256,256,256
};

float density2Trans(float density)
{
	if (curDataIdx == 6)
	{
		auto c = density*density*density / 65536.0f;
		return (c*transparency);
	}
	else
	{
		auto c = pow(density / 256.0f, 1.2f);
		return (c*transparency);
	}
}

std::vector<float[3]> loadColorMap(int cmpIdx)
{
	auto fileName = cmpFileSet[cmpIdx];
	std::ifstream input(fileName);
	std::vector<float[3]> cmp(sr);

	int i = 0;
	while (!input.eof())
	{
		input >> cmp[i][0] >> cmp[i][1] >> cmp[i][2];
		i++;
	}

	std::cout << "Color map read: " << i << "\n";
	input.close();

	return cmp;
}

std::vector<uint8_t> loadDensity(int volIdx)
{
	auto level = 0;
	auto fileName = volFilePath[volIdx] + std::to_string(level) + ".raw";
	std::ifstream input;
	std::vector<uint8_t> densityVolume(sx*sy*sz, 0);

	try
	{
		input.open(fileName, std::ios::in | std::ifstream::binary);

		sx = downSize(volSize[volIdx][0], level + 1);
		sy = downSize(volSize[volIdx][1], level + 1);
		sz = downSize(volSize[volIdx][2], level + 1);

		densityVolume.resize(sx*sy*sz);

		input.read(reinterpret_cast<char*>(&densityVolume[0]), densityVolume.size()*sizeof(uint8_t));
	}
	catch (std::ifstream::failure e)
	{
		std::cout << e.what() << "\n";
		printf("Error reading file %s\n", fileName.c_str());
		fprintf(stderr, "%s\t%d\n", __FILE__, __LINE__);
	}

	input.close();
	return densityVolume;
}

std::vector<float> loadVolume(int volIdx, int level)
{
	auto fileName = volFilePath[volIdx] + std::to_string(level) + ".raw";
	std::ifstream input;
	std::vector<float> coloredVolume(sx*sy*sz * 4, 0);

	try
	{
		if (level == 0)
		{
			auto densityVolume = loadDensity(volIdx);

			sx = volSize[volIdx][0];
			sy = volSize[volIdx][1];
			sz = volSize[volIdx][2];

			coloredVolume.clear();
			coloredVolume.resize(sx*sy*sz * 4);

#pragma omp parallel for
			for (int x = 0; x < sx; x++)
			{
				for (size_t y = 0; y < sy; y++)
				{
					for (size_t z = 0; z < sz; z++)
					{
						auto zmajor = x*(sy*sz) + y*sz + z;
						auto xmajor = z*(sx*sy) + y*sx + x;
						auto p = densityVolume[zmajor];
						auto i = xmajor;
						coloredVolume[4 * i + 0] = colorMap[p][0];
						coloredVolume[4 * i + 1] = colorMap[p][1];
						coloredVolume[4 * i + 2] = colorMap[p][2];
						coloredVolume[4 * i + 3] = density2Trans(p);
					}
				}
			}

			auto densityVolumeSize = densityVolume.size();
//#pragma omp parallel for
			/*for (int i = 0; i < densityVolumeSize; i++)
			{
				auto p = densityVolume[i];
				coloredVolume[4 * i + 0] = colorMap[p][0];
				coloredVolume[4 * i + 1] = colorMap[p][1];
				coloredVolume[4 * i + 2] = colorMap[p][2];
				coloredVolume[4 * i + 3] = density2Trans(p);
			}*/
		}
		else
		{
			input.open(fileName, std::ios::in | std::ifstream::binary);

			sx = downSize(volSize[volIdx][0], level + 1);
			sy = downSize(volSize[volIdx][1], level + 1);
			sz = downSize(volSize[volIdx][2], level + 1);

			coloredVolume.clear();
			coloredVolume.resize(sx*sy*sz * 4);

			input.read(reinterpret_cast<char*>(&coloredVolume[0]), coloredVolume.size()*sizeof(float));
		}

		curDataIdx = volIdx;
		curResLevel = level;
	}
	catch (std::ifstream::failure e)
	{
		std::cout << e.what() << "\n";
		printf("Error reading file %s\n", fileName.c_str());
		fprintf(stderr, "%s\t%d\n", __FILE__, __LINE__);
	}

	input.close();
	return coloredVolume;
}

void saveVolume(int volIdx, int level, vec4(float) pdfVolume)
{
	if (level<=0)
	{
		fprintf(stderr, "Cannot save with level <= 0.\n");
	}
	else
	{
		auto filename = volFilePath[volIdx] + std::to_string(level) + ".raw";
		std::ofstream output;

		try
		{
			output.open(filename, std::ofstream::out | std::ofstream::binary);

			auto sx = pdfVolume.size();
			auto sy = pdfVolume[0].size();
			auto sz = pdfVolume[0][0].size();
			int idx = 0;
			coloredVolume.resize(sx*sy*sz * 4);
			for (int z = 0; z < sz; z++)
			{
				for (int y = 0; y < sy; y++)
				{
					for (int x = 0; x < sx; x++)
					{
						normalize(pdfVolume[x][y][z]);
						float R = 0, G = 0, B = 0;
						auto density = pdfVolume[x][y][z][0] * 0;
						//#pragma omp parallel for
						// this parallel may effect result
						for (int r = 0; r < sr; r++)
						{
							auto p = pdfVolume[x][y][z][r];
							R += p*colorMap[r][0];
							G += p*colorMap[r][1];
							B += p*colorMap[r][2];
							density += p*r;
						}
						/*R = colorMap[density][0];
						G = colorMap[density][1];
						B = colorMap[density][2];*/
						coloredVolume[4 * idx + 0] = R;
						coloredVolume[4 * idx + 1] = G;
						coloredVolume[4 * idx + 2] = B;
						coloredVolume[4 * idx + 3] = density2Trans(density);
						idx++;
					}
				}
			}

			output.write(reinterpret_cast<char*>(&coloredVolume[0]), coloredVolume.size()*sizeof(float));
		}
		catch (std::ofstream::failure e)
		{
			std::cout << e.what() << "\n";
			printf("Error opening file %s\n", filename.c_str());
			fprintf(stderr, "%s\t%d\n", __FILE__, __LINE__);
		}

		output.close();
	}
}
