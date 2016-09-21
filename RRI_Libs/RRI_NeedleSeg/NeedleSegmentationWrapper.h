#ifndef NeedleSegmentationWrapper_h__
#define NeedleSegmentationWrapper_h__
#include "NeedleSegmentationWrapper_Export.h"
#include "NeedleSegmentationWrapperInputStructs.h"
#include <vector>
class  NeedleSegmentationWrapper
{
	public:
				static NEEDLESEGMENTATIONWRAPPER_EXPORT  int SegmentNeedle(NeedleSegmentationWrapperInputs::Image image,
																NeedleSegmentationWrapperInputs::Point3D entryPoint,
																NeedleSegmentationWrapperInputs::Point3D otherPoint,
																int numNeedles,
																double* outPierecingPoints,
																double* outTipPoints);

};


#endif // NeedleSegmentationWrapper_h__