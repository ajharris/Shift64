#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkPlane.h"
#include <vector>
#pragma  once
// this class is used to slice image around a rotation axis.
class ImageSlicing
{
public:

		//************************************
		// Method:    SliceImage - main method use to slice a 3D image around a specified axis and rotation point.
		// FullName:  ImageSlicing::SliceImage
		// Access:    public static 
		// Returns:   vtkSmartPointer<vtkImageData> The set of 2D sliced stacked together as a 3D image.
		// Qualifier:
		// Parameter: vtkImageData * inImage3D - the imput 3D image.
		// Parameter: double * cor - the center of roation around which the images will be sliced. The geometrical center of the 2D images will be at this point.
		// Parameter: double * axis - the axis o rotation about which the image will be sliced.
		// Parameter: double degrees - total number of degrees from the initial slice to the last slice.
		// Parameter: int num2DImages - the total number of 2D images to create. degrees divided by this variable determines the angle between neighboring slices in the result.
		// Parameter: double * normal
		//************************************
		static vtkSmartPointer<vtkImageData> SliceImage(vtkImageData* inImage3D, double* cor, double* axis, double degrees, int num2DImages, double* normal);


		//************************************
		// Method:    CalculateTransform - calculate a transform for slicing an image tilted at a certain angle from initial plane. The gemometrical center of the 3D image will map to the center of rotation.
		// FullName:  ImageSlicing::CalculateTransform
		// Access:    public static 
		// Returns:   vtkSmartPointer<vtkTransform>  - the reslicing transform.
		// Qualifier:
		// Parameter: float angle - tilt angle from initial slice.
		// Parameter: double * axis - axis of rotation.
		// Parameter: double * cor - cener of rotation
		// Parameter: double * normal - the normal of the first plane.
		// Parameter: double * imageCenter =  geometrical center of the 3D image.
		//************************************
		static vtkSmartPointer<vtkTransform> CalculateTransform( float angle, double* axis, double* cor, double* normal, double* imageCenter );
		
		
		//************************************
		// Method:    CalculateAxisAllignemntTransform - Return a transform which aligns Y direction to the rotation axis about the center of rotation.
		// FullName:  ImageSlicing::CalculateAxisAllignemntTransform
		// Access:    public static 
		// Returns:   vtkSmartPointer<vtkTransform> - the alligning transform.
		// Qualifier:
		// Parameter: double * axis - thae axis of rotaiton.
		// Parameter: double * cor - center of rotation.
		// Parameter: double * normal
		//************************************
		static vtkSmartPointer<vtkTransform> CalculateAxisAllignemntTransform (double* axis,double* cor, double* normal);

		
		//************************************
		// Method:    CalculateAngle - Utility function that calculate the tilt angle of the Ith slice.
		// FullName:  ImageSlicing::CalculateAngle
		// Access:    public static 
		// Returns:   double - tilt angle of the slice.
		// Qualifier:
		// Parameter: double span - the total span angle.
		// Parameter: int numSlices - the total number of slices.
		// Parameter: int sliceNum - the index of the slice for which tilt is requested.
		//************************************
		static double CalculateAngle(double span,int numSlices,int sliceNum);

		//************************************
		// Method:    BestFitPlane - calculate the best plane  to fit a set of points.
		// FullName:  ImageSlicing::BestFitPlane
		// Access:    public static 
		// Returns:   vtkPlane* 
		// Qualifier:
		// Parameter: vtkPoints * points
		//************************************
		static vtkPlane* BestFitPlane(vtkPoints *points);

		
		//************************************
		// Method:    BuildSlicedImage - Build a 3D image which is actually a stacked 2D images from a set of sliced images on the disk. Allow to compare results to matlab program which provides sliced images only.
		// FullName:  ImageSlicing::BuildSlicedImage
		// Access:    public static 
		// Returns:   vtkSmartPointer<vtkImageData> - the stacked 3D image.
		// Qualifier:
		// Parameter: char * dir - directory of the image.
		// Parameter: char * filePrefix - prefix of the file name. for example if file are test1.jpg , test2.jpg .....test30.jpg then the prefix should be 'test'.
		//************************************
		static vtkSmartPointer<vtkImageData>  BuildSlicedImage(char* dir, char* filePrefix);



		//************************************
		// Method:    CalculateImageCenter - calculate the physical coordinates of image center - which is the center pixel.
		// FullName:  ImageSlicing::CalculateImageCenter
		// Access:    public static 
		// Returns:   void
		// Qualifier:
		// Parameter: vtkImageData * imageData - the image.
		// Parameter: double * outCenter - output paramters : the ceneter of the image.
		//************************************
		static void CalculateImageCenter(vtkImageData* imageData, double* outCenter);

 

}
;