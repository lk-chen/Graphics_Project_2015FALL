#pragma once
#include <omp.h>
#include <vector>
#include <stdint.h>
#include "loadData.h"
#include "gaussian.h"

using namespace std;

template<typename T>
T findMax(const vec4(T)& input, VolNode& tuple
	, size_t startX, size_t endX, size_t startY, size_t endY, size_t startZ, size_t endZ
	)
{
	T largest = 0;

	for (int x = startX; x < endX; x++)
		for (int y = startY; y < endY; y++)
		{
			T tempMax;
			VolNode tempTuple;
#pragma omp parallel private(tempMax, tempTuple)
			{
				tempMax = input[x][y][0][0];
				tempTuple.x = x;
				tempTuple.y = y;
				tempTuple.z = 0;
				tempTuple.r = 0;
				tempTuple.c = tempMax;
#pragma omp for
				for (int z = startZ; z < endZ; z++)
					for (int r = 0; r<sr; r++)
					{
						// newresidual were initialized to zero, so skip 0
						if (input[x][y][z][r]>tempMax)
						{
							tempMax = input[x][y][z][r];
							tempTuple.x = x;
							tempTuple.y = y;
							tempTuple.z = z;
							tempTuple.r = r;
						}
					}
				if (tempMax > largest)
#pragma omp critical
				{
					if (tempMax > largest)
					{
						largest = tempMax;
						tuple.c = largest;
						tuple.x = tempTuple.x;
						tuple.y = tempTuple.y;
						tuple.z = tempTuple.z;
						tuple.r = tempTuple.r;
					}
				}
			}
		}

	return largest;
}

void updateResidual(vec4(float)&, vec4(float)&, const VolNode&);


