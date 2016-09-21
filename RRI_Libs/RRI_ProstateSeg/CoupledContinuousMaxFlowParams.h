#pragma once
struct CoupledContinuousMaxFlowParams 
{
	int Nx;
	int Ny;
	int NbIters;
	float fError;
	float cc;
	float steps;
	float beta;
	int nSlices;
};