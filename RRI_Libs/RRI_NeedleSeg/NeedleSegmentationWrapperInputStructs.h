#ifndef NeedleSegmentationWrapperInputStructs_h__
#define NeedleSegmentationWrapperInputStructs_h__



namespace NeedleSegmentationWrapperInputs
{

	class Point3D
	{
		public:
			double x,y,z;
	};

	class Image
	{
		public:	
			float spacing[3];
			float origin[3];
			int dimensions[3];
			short* buff;
	};

}

#endif // NeedleSegmentationWrapperInputStructs_h__