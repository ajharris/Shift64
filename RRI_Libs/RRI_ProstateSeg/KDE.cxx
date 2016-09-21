#include "stdafx.h"
#include "KDE.h"
#include <cmath>



//////////////////////////////////////////////////////////////////////////
void KDE::ComputeKDE(double *buffer, double *de, long n, long numBins, double sigma)
{
	const float PI = 3.1415926f;
	const float F = 1 / sqrt(2*PI);
	double x;
	double k = 1.0 / (n *sigma)*F; 
	for(long i = 0; i < numBins; i++)
	{
		de[i] = 0.0f;
		for(long j = 0; j < n; j++)
		{
				x = (buffer[j] - (double)i) / sigma;
				de[i] +=  exp(-0.5 * (x *x));
		}

		de[i] =  k* de[i];
	}
}

