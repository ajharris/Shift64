#pragma once
#include "vtkPolygon.h"
#include "vtkPoints.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"


/// This class implements the continuous max flow algroithm described in :
///	Qiu W et al , "Prostate segmentation: an efficient convex optimization approach with axial symmetry using 3-D TRUS and MR images.," IEEE Trans Med Imaging, pp. 947-60, 2014. 

class MaxFlowSegmentation 
{

public: 
	///
	//************************************
	// Method:    Segement
	// FullName:  MaxFlowSegmentation::Segement
	// Access:    public 
	// Returns:   vtkSmartPointer<vtkPolyData> - the reconstructed surface.
	// Qualifier:
	// Parameter: vtkSmartPointer<vtkImageData> image - the input image.
	// Parameter: vtkPoints * initialContour - the contour at the first resliced image selected manual by the user.
	// Parameter: double * centerOfRotation - the center of the prostate coordinates in the first slice
	// Parameter: double * axis - the rotation axis
	// Parameter: bool isUS - if the image is US true , if MR false.
	//************************************
	vtkSmartPointer<vtkPolyData> Segement(vtkSmartPointer<vtkImageData> image, vtkPoints* initialContour, double* centerOfRotation,double* axis, bool isUS);



	//************************************
	// Method:    SegementSlicedImage - Segment image that were already sliced
	// FullName:  MaxFlowSegmentation::SegementSlicedImage
	// Access:    public 
	// Returns:   vtkSmartPointer<vtkImageData> - Bit map 
	// Qualifier:
	// Parameter: vtkSmartPointer<vtkImageData> image - the sliced image
	// Parameter: vtkPoints * initialContour - 
	// Parameter: bool isUS
	//************************************
	vtkSmartPointer<vtkImageData> SegementSlicedImage(vtkSmartPointer<vtkImageData> image, vtkPoints* initialContour, bool isUS);


	//************************************
	// Method:    CropImage - cropped a sliced image such that it is just enough to include the ROI in the contour plus some offset.
	// FullName:  MaxFlowSegmentation::CropImage
	// Access:    public static 
	// Returns:   vtkSmartPointer<vtkImageData>
	// Qualifier:
	// Parameter: vtkSmartPointer<vtkImageData> image - the sliced image
	// Parameter: vtkSmartPointer<vtkPoints> initialContour - coordinates of the contour in slice 0.
	//************************************
	static vtkSmartPointer<vtkImageData> CropImage( vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkPoints> initialContour );

	MaxFlowSegmentation();

	void SetHardConstraitnIn(double val ) 
	{
		mHardConstraintIn = val;
	}
	void SetHardConstraintOut(double val)
	{
		mHardConstraintOut = val;
	}

	void SetBeta(double val)
	{
		mBeta = val;
	}
private:

	//************************************
	// Method:    BuildProbabilityImage - Builds the Ct and Cs terms according to the algorithm document.
	// FullName:  MaxFlowSegmentation::BuildProbabilityImage
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: vtkSmartPointer<vtkImageData> image - sliced image.
	// Parameter: vtkPoints * points - contour of the prostate in slice 0. Used to detemine PDF inside and outside the prostate.
	// Parameter: vtkSmartPointer<vtkImageData> Cs - Output parameter. Image containing Cs term for each of pixels in the sliced image.
	// Parameter: vtkSmartPointer<vtkImageData> Ct - Output parameter. Image containing Ct term for each of pixels in the sliced image.
	//************************************
	void BuildProbabilityImage(vtkSmartPointer<vtkImageData> image, vtkPoints* points, vtkSmartPointer<vtkImageData> Cs,vtkSmartPointer<vtkImageData> Ct);


	//************************************
	// Method:    BuildPenaltyImage - build the geodesic distance term g(x)  from the papaer.
	// FullName:  MaxFlowSegmentation::BuildPenaltyImage
	// Access:    private 
	// Returns:   vtkSmartPointer<vtkImageData> - Image containing Cs term for each of pixels in the sliced image.
	// Qualifier:
	// Parameter: vtkSmartPointer<vtkImageData> slicedImage -  sliced image.
	// Parameter: bool isUS - true if the image is US false if the image is MR.
	//************************************
	vtkSmartPointer<vtkImageData> BuildPenaltyImage(vtkSmartPointer<vtkImageData> slicedImage, bool isUS);


	//************************************
	// Method:    ConvertVTKImageDataToBuffer - utility function that converts the vtk image to a buffer for max flow optimization module.
	// FullName:  MaxFlowSegmentation::ConvertVTKImageDataToBuffer
	// Access:    private static 
	// Returns:   float*
	// Qualifier:
	// Parameter: vtkImageData * image
	//************************************
	static float* ConvertVTKImageDataToBuffer(vtkImageData* image);


	//************************************
	// Method:    ConvertBufferToVTKImageData - utility function that converts the buffer output of maxflow to a  vtk image.
	// FullName:  MaxFlowSegmentation::ConvertBufferToVTKImageData
	// Access:    private static 
	// Returns:   void
	// Qualifier:
	// Parameter: float * buffer
	// Parameter: vtkImageData * image
	//************************************
	static void ConvertBufferToVTKImageData(float* buffer, vtkImageData* image);


	//************************************
	// Method:    MyDistanceToPolygon - check the distance of a point from polygon.
	// FullName:  MaxFlowSegmentation::MyDistanceToPolygon
	// Access:    private static 
	// Returns:   double - the distance from the polygon. inside distances are negative and outside distance are positive.
	// Qualifier:
	// Parameter: double x[3] - the point whose distance is queried. 
	// Parameter: int numPts - number of points in the polygon. 
	// Parameter: double * pts - buffer with the point data. the bufferr is aranged such that point coordiantes are serialzed linearly in (x,y,z) order linearly.
	// Parameter: double bounds[6] - two points the form the bounding box of the image. serialized the same as pts.
	// Parameter: double closest[3] - output parameter for the closest point on the contour.
	//************************************
	static double MyDistanceToPolygon(double x[3], int numPts, double *pts, double bounds[6], double closest[3]);


	//************************************
	// Method:    ReconstructSurface - Build the prostate surface from the segmented sliced image. The reconstruction needs to know all the paramerts that were used to sliced the image.
	// FullName:  MaxFlowSegmentation::ReconstructSurface
	// Access:    private 
	// Returns:   vtkSmartPointer<vtkPolyData> - mesh of the reconstructed surface.
	// Qualifier:
	// Parameter: vtkSmartPointer<vtkImageData> inImage
	// Parameter: double * axis - axis of rotation used to slice the image (in original image coordinates).
	// Parameter: double * cor - center  of rotation used to slice the image (in original image coordinates).
	// Parameter: double * normal - the nomral to the plane containing the inial contour.
	//************************************
	vtkSmartPointer<vtkPolyData> ReconstructSurface(vtkSmartPointer<vtkImageData> inImage, double * axis, double* cor, double* normal);



	double mHardConstraintIn ;
	double mHardConstraintOut;
	double mBeta;

};






































































































































































































































































































