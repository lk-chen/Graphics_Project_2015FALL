#include "parallel.h"
#include <omp.h>
#include <algorithm>
#include "loadData.h"

void updateResidual(vec4(float)& residual, vec4(float)& newresidual, const VolNode& tuple)
{
	auto x0 = tuple.x;
	auto y0 = tuple.y;
	auto z0 = tuple.z;
	auto r0 = tuple.r;
	auto c0 = tuple.c;

	auto kernelSizeS = getKernSize(sigma_s);
	auto kernelSizeR = getKernSize(sigma_r);

#pragma omp parallel for
	for (int kx = 1 - kernelSizeS; kx < kernelSizeS; kx++)
	{
		if (!(0 <= kx + x0 && kx + x0 < sx))continue;
		for (int ky = 1 - kernelSizeS; ky < kernelSizeS; ky++)
		{
			if (!(0 <= ky + y0 && ky + y0 < sy))continue;
			for (int kz = 1 - kernelSizeS; kz < kernelSizeS; kz++)
			{
				if (!(0 <= kz + z0 && kz + z0 < sz))continue;
				for (int kr = 1 - kernelSizeR; kr < kernelSizeR; kr++)
				{
					if (!(0 <= kr + r0 && kr + r0 < sr))continue;
					auto delta2 = c0* gaus4D2[abs(kx)][abs(ky)][abs(kz)][abs(kr)];
					residual[kx + x0][ky + y0][kz + z0][kr + r0] -= delta2;
					auto delta1 = c0* gaus4D[abs(kx)][abs(ky)][abs(kz)][abs(kr)];
					newresidual[kx + x0][ky + y0][kz + z0][kr + r0] += delta1;
				}
			}
		}
	}
}
