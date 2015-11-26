#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include "gaussian.h"

extern std::vector<float> coloredVolume;
extern const float transparency;
extern std::vector<float[3]> colorMap;

struct VolNode
{
	size_t x, y, z, r;
	float c;
	VolNode() :x(0), y(0), z(0), r(0), c(0) {};
};

extern int curDataIdx, curResLevel;
extern const size_t sr;
extern size_t sx, sy, sz;
extern size_t nsx, nsy, nsz;
extern const std::string cmpFileSet[];
extern const std::string volFilePath[];
extern const int numOfDataSet;
extern const int volSize[][3];

std::vector<float[3]> loadColorMap(int cmpIdx);
std::vector<uint8_t> loadDensity(int);
std::vector<float> loadVolume(int, int);
void saveVolume(int volIdx, int level, vec4(float) dictionVolume);

template<typename T, typename S>
T downSize(T x, S size)
{
	return T((x - 1) / size + 1);
}

template<typename T>
vec4(T) get4D(T e)
{
	vec4(T) output;
	return output;
}
