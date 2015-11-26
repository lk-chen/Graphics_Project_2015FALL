#include "gaussian.h"

const float sigma_s = 0.333f;
const float sigma_r = 4.0f / 256;
const int kernExtSize = 8;
vec4(float) gaus4D, gaus4D2;

int getKernSize(double sig)
{
	return (int)(ceil(sig * kernExtSize) + 1);
}