#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "gaussian.h"
#include "parallel.h"
#include "timing.h"
#include "loadData.h"

using namespace std;

int main()
{
	//1:ph128 4:ph64 5:thigh 6:test
	curDataIdx = 4;
	curResLevel = 0;
	colorMap = loadColorMap(0);
	gaus4D = normalizedGausKern4D(sigma_s, sigma_r);
	gaus4D2 = normalizedGausConv4D(gaus4D, sigma_s, sigma_r);
	vec4(float) residual, newresidual;

	// read volume data, and sx sy sz.
	cout << "Loading volume data.\n";
	startTiming();
	auto densityVolume = loadDensity(curDataIdx);
	nsx = downSize(sx, 2);
	nsy = downSize(sy, 2);
	nsz = downSize(sz, 2);
	endTiming();

	// construct residual
	cout << "Constructing V0.\n";
	startTiming();
	residual.resize(sx);
	for (size_t x = 0; x < sx; x++)
	{
		residual[x].resize(sy);
		for (size_t y = 0; y < sy; y++)
		{
			residual[x][y].resize(sz);
			for (size_t z = 0; z < sz; z++)
			{
				residual[x][y][z].resize(sr, 0);			// initialize residual to be zero
				auto r = densityVolume[x*(sy*sz) + y*sz + z];
				residual[x][y][z][r] = 1;
			}
		}
	}
	cout << "Constructing residual.\n";
	auto tempRes = normalizedGausConv4D(residual, 2 * sigma_s, sigma_r);		// LPF
	
	residual.clear();
	residual.resize(nsx);
	newresidual.resize(nsx);
	for (size_t x = 0; x < nsx; x++)
	{
		residual[x].resize(nsy);
		newresidual[x].resize(nsy);
		for (size_t y = 0; y < nsy; y++)
		{
			residual[x][y].resize(nsz);
			newresidual[x][y].resize(nsz);
			for (size_t z = 0; z < nsz; z++)
			{
				residual[x][y][z] = tempRes[2 * x][2 * y][2 * z];
				newresidual[x][y][z].resize(sr, 0);
			}
		}
	}
	tempRes.clear();
	sx = nsx;
	sy = nsy;
	sz = nsz;
	endTiming();

	// pursuit V1
	cout << "Convolution before pursuit.\n";
	startTiming();
	residual = normalizedGausConv4D(residual, sigma_s, sigma_r);
	endTiming();
	cout << "Pursuiting V1.\n";
	auto i = nsx*nsy*nsz;
	
	std::vector<VolNode> V1(i);
	const int brickSize = 32;
	auto brickNumX = (nsx - 1) / brickSize + 1;
	auto brickNumY = (nsy - 1) / brickSize + 1;
	auto brickNumZ = (nsz - 1) / brickSize + 1;
	for (size_t brickIdxX = 0; brickIdxX < brickNumX; brickIdxX++)
	{
		auto startX = brickIdxX * brickSize;
		auto endX = min(startX + brickSize, sx);
		for (size_t brickIdxY = 0; brickIdxY < brickNumY; brickIdxY++)
		{
			auto startY = brickIdxY * brickSize;
			auto endY = min(startY + brickSize, sy);
			for (size_t brickIdxZ = 0; brickIdxZ < brickNumZ; brickIdxZ++)
			{
				auto startZ = brickIdxZ * brickSize;
				auto endZ = min(startZ + brickSize, sz);

				auto brickVoxelNum = (endX - startX)*(endY - startY)*(endZ - startZ);
				cout << "This brick has " << brickVoxelNum << "\n";
				startTiming();
				for (size_t k = 0; k < brickVoxelNum; k++)
				{
					i--;
					if (i % 16 == 0)
					{
						std::cout << "\rpursuit " << i << "\t";
					}

					// find max
					auto tempMax = findMax(residual, V1[i], startX, endX, startY, endY, startZ, endZ);

					//cout << "max found, recording.." << tempMax << endl;
					//tempMax /= kernMax2;
					//V1[i].c = tempMax;

					//cout << "updating residual..\n";
					updateResidual(residual, newresidual, V1[i]);
				}
				cout << "\n";
				endTiming();
			}
		}
	}
	
	// save result
	cout << "Saving pursuit result.\n";
	startTiming();
	saveVolume(curDataIdx, curResLevel + 1, newresidual);
	endTiming();

	// algorithm end
	std::cout << "Success.\n";
	getchar();
	return 0;
}
