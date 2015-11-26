#pragma once
#include <vector>
#include <math.h>
#include <ctime>
#include <omp.h>
#include "timing.h"

#define vec4(T) std::vector<std::vector<std::vector<std::vector<T>>>>

extern const float sigma_s;
extern const float sigma_r;
extern vec4(float) gaus4D, gaus4D2;

int getKernSize(double);

template<typename T>
double gausPMF(T x, double mu, double sigma)
{
	return exp(-1 * (x - mu) * (x - mu) / (2 * sigma * sigma));
}

template<typename T>
void normalize(vec4(T)& input)
{
	double c = 0;

	// set size
	int sx = input.size();
	int sy = input[0].size();
	int sz = input[0][0].size();
	int sr = input[0][0][0].size();

	for (int x = 1 - sx; x <= sx - 1; x++)
		for (int y = 1 - sy; y <= sy - 1; y++)
			for (int z = 1 - sz; z <= sz - 1; z++)
				for (int r = 1 - sr; r <= sr - 1; r++)
				{
					c += input[abs(x)][abs(y)][abs(z)][abs(r)];
				}

	for (int x = 0; x < sx; x++)
		for (int y = 0; y < sy; y++)
			for (int z = 0; z < sz; z++)
				for (int r = 0; r < sr; r++)
				{
					input[x][y][z][r] /= c;
				}
}

template<typename T>
void normalize(std::vector<T>& input)
{
	double c = 0;

	for (int i = 0; i < input.size(); i++)
	{
		c += input[i];
	}

	for (int i = 0; i < input.size(); i++)
	{
		input[i] /= c;
	}
}

template<typename T>
vec4(T) normalizedGausKern4D
(const T sig_s, const T sig_r)
{
	const int kernel1DSizeS = getKernSize(sig_s);
	const int kernel1DSizeR = getKernSize(sig_r);
	std::vector<T> gau1DKernS(kernel1DSizeS, 0), gau1DKernR(kernel1DSizeR, 0);

	for (int i = 0; i < kernel1DSizeS; i++)
	{
		gau1DKernS[i] = gausPMF(i, 0, sig_s);
	}
	for (int i = 0; i < kernel1DSizeR; i++)
	{
		gau1DKernR[i] = gausPMF(i, 0, sig_r);
	}

	vec4(T) output(kernel1DSizeS);
	for (int x = 0; x < kernel1DSizeS; x++)
	{
		output[x].resize(kernel1DSizeS);
		for (int y = 0; y < kernel1DSizeS; y++)
		{
			output[x][y].resize(kernel1DSizeS);
			for (int z = 0; z < kernel1DSizeS; z++)
			{
				output[x][y][z].resize(kernel1DSizeR);
				for (int r = 0; r < kernel1DSizeR; r++)
					output[x][y][z][r] = gau1DKernS[x] * gau1DKernS[y] * gau1DKernS[z] * gau1DKernR[r];
			}
		}
	}

	normalize(output);

	return output;
}

template<typename T, typename S>
vec4(T) normalizedGausConv4D
(const vec4(T)& input,
	S sig_s, S sig_r)
{
	vec4(T) output;
	const double epsilon = 0.0001;

	auto gaus = normalizedGausKern4D(sig_s, sig_r);
	int kernel1DSizeS = (int)(gaus.size());
	int kernelSizeR = (int)(gaus[0][0][0].size());

	int sx = (int)(input.size());
	int sy = (int)(input[0].size());
	int sz = (int)(input[0][0].size());
	int sr = (int)(input[0][0][0].size());

	output.resize(sx);
	for (int x = 0; x < sx; x++)
	{
		output[x].resize(sy);
		for (int y = 0; y < sy; y++)
		{
			output[x][y].resize(sz);
			for (int z = 0; z < sz; z++)
			{
				output[x][y][z].resize(sr, 0);
			}
		}
	}

	startTiming();

	for (int x = 0; x < sx; x++)
		for (int y = 0; y < sy; y++)
			for (int z = 0; z < sz; z++)
			{
				for (int r = 0; r < sr; r++)
				{
					auto inVal = input[x][y][z][r];
					if (inVal < epsilon)
					{
						continue;
					}
					else
					{
#pragma omp parallel for
						for (int kx = 1 - kernel1DSizeS; kx <= kernel1DSizeS - 1; kx++)
						{
							if (!(kx + x >= 0 && kx + x < sx))continue;
							for (int ky = 1 - kernel1DSizeS; ky <= kernel1DSizeS - 1; ky++)
							{
								if (!(ky + y >= 0 && ky + y < sy))continue;

								for (int kz = 1 - kernel1DSizeS; kz <= kernel1DSizeS - 1; kz++)
								{
									if (!(kz + z >= 0 && kz + z < sz))continue;
									for (int kr = 1 - kernelSizeR; kr <= kernelSizeR - 1; kr++)
									{
										if (!(kr + r >= 0 && kr + r < sr))continue;
										output[kx + x][ky + y][kz + z][kr + r] +=
											gaus[abs(kx)][abs(ky)][abs(kz)][abs(kr)]
											* inVal;
									}
								}
							}
						}

						if (timesUp1s())
						{
							printf("\rWorking.. %d\t%d\t%d\t", x, y, z);
						}
					}
				}
			}
	std::cout << "\n";

	return output;
}
