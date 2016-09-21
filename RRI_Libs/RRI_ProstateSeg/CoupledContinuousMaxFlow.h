#pragma  once;
#include "CoupledContinuousMaxFlowParams.h"
class CoupledContinuousMaxFlow 
{
	public:
		static bool DoMaxFlow(float* Cs,float* Ct,float* penalty,CoupledContinuousMaxFlowParams params,float*& outImage);
};