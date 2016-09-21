#include "stdafx.h"
#include "ImageSlicing.h"
#include "vtkTransform.h"
#include "vtkImageReslice.h"
#include "vtkMetaImageWriter.h"
#include "vtkMath.h"
#include <cmath>
#include <omp.h>
#include "vtkResliceImageViewer.h"
#include "vtkPlane.h"
#include "vtkMetaImageWriter.h"
#include "vtkBMPWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkImageCast.h"
#include "vtkLandmarkTransform.h"
#include "vtkVector.h"
#include "vtkPoints.h"
#include "vtkBMPReader.h"
#include <stdlib.h>
#include "vtkSetGet.h"
#include "vtkRuledSurfaceFilter.h"
#include "vtkImageAppend.h"

#define SAVE_SLICED_IMAGES
using namespace std;

//cor - center of rotation

vtkSmartPointer<vtkImageData> ImageSlicing::SliceImage( vtkImageData* inImage3D, double* cor, double* axis, double degrees, int num2DImages, double* normal )
{

	std::vector<vtkSmartPointer<vtkImageData>> res; 
	vtkSmartPointer<vtkImageAppend> appedFilter = vtkSmartPointer<vtkImageAppend>::New();
	appedFilter->SetAppendAxis(2);
	appedFilter->GetOutput()->SetScalarTypeToFloat();
	double center[3];
	CalculateImageCenter(inImage3D, center);
 
 
	for(int i = 0 ;i<num2DImages;i++)
	{
		vtkSmartPointer<vtkImageReslice> reslicer = vtkSmartPointer<vtkImageReslice>::New();
		reslicer->SetInput(inImage3D);

		reslicer->SetOutputDimensionality(2);
		reslicer->SetInterpolationModeToLinear();
		double angle = CalculateAngle(degrees,num2DImages,i);
		vtkSmartPointer<vtkTransform> transform = CalculateTransform(angle, axis, cor, normal,center);
		reslicer->SetResliceTransform(transform);
		reslicer->Update();
		vtkSmartPointer<vtkImageData> resPointer = vtkSmartPointer<vtkImageData>::New(); 
		resPointer->ShallowCopy(reslicer->GetOutput());
		res.push_back(resPointer);
		appedFilter->AddInputConnection(reslicer->GetOutputPort());
		appedFilter->Update();
#ifdef SAVE_SLICED_IMAGES
		vtkSmartPointer<vtkImageCast> castFilter = vtkSmartPointer<vtkImageCast>::New();
		castFilter->SetInput(resPointer);
		castFilter->SetOutputScalarTypeToUnsignedChar();
		castFilter->Update();

		char filenameBuff[200];
		sprintf(filenameBuff,"c:\\temp\\sliced\\image%d.jpg",i);
		vtkSmartPointer<vtkJPEGWriter> writer2d = vtkSmartPointer<vtkJPEGWriter>::New();
		writer2d->SetInput(castFilter->GetOutput());
		writer2d->SetFileName(filenameBuff);
		writer2d->Write();
#endif
	}

	vtkSmartPointer<vtkImageCast> cast = vtkSmartPointer<vtkImageCast>::New();
	cast->SetOutputScalarTypeToFloat();
	cast->SetInput(appedFilter->GetOutput());
	cast->Update();
 	return cast->GetOutput();			
}




vtkSmartPointer<vtkTransform> ImageSlicing::CalculateTransform( float angle, double* axis, double* cor, double* normal, double* imageCenter )
{

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform> ::New();
	double normalizedAxis[3];
	for(int i=0;i<3;i++)
		normalizedAxis[i]=axis[i];
	vtkMath::Normalize(normalizedAxis);
	vtkSmartPointer<vtkTransform>  yTransform=CalculateAxisAllignemntTransform(normalizedAxis,cor, normal);
	double yAxis[3] = {0,1,0};
	
	transform->Concatenate(yTransform);
	transform->PostMultiply();
	transform->Translate(0,0,-cor[2]);
	transform->Translate(-cor[0],-cor[1],0);
	transform->RotateWXYZ(angle,yAxis);
	transform->Translate(cor[0],cor[1],0);
	transform->Translate(imageCenter[0]-cor[0],imageCenter[1]-cor[1],0);
	transform->Inverse();
	
	transform->Update();
	
	return transform;
}
 
//assume vectors are normalized.
vtkSmartPointer<vtkTransform> ImageSlicing::CalculateAxisAllignemntTransform( double* axis,double* cor, double* normal)
{
	vtkSmartPointer<vtkTransform>  outTransform = vtkSmartPointer<vtkTransform> ::New();
	double angle = vtkMath::DegreesFromRadians(acos(axis[1]));
	outTransform->PostMultiply();
	outTransform->Translate(-cor[0],-cor[1],0);
	outTransform->RotateWXYZ(angle,normal);
	outTransform->Translate(+cor[0],+cor[1],0);

	outTransform->Inverse();
	return outTransform;

}
vtkPlane* ImageSlicing::BestFitPlane(vtkPoints *points)
{
	vtkIdType NumPoints = points->GetNumberOfPoints();

	//find the center of mass of the points
	double Center[3] = {0.0, 0.0, 0.0};

	for(vtkIdType i = 0; i < NumPoints; i++)
	{
		double point[3];
		points->GetPoint(i, point);

		Center[0] += point[0];
		Center[1] += point[1];
		Center[2] += point[2];
	}

	Center[0] = Center[0]/static_cast<double>(NumPoints);
	Center[1] = Center[1]/static_cast<double>(NumPoints);
	Center[2] = Center[2]/static_cast<double>(NumPoints);



	double *a[3], a0[3], a1[3], a2[3];
	a[0] = a0; a[1] = a1; a[2] = a2;
	for(unsigned int i = 0; i < 3; i++)
	{
		a0[i] = a1[i] = a2[i] = 0.0;
	}

	for(unsigned int pointId = 0; pointId < NumPoints; pointId++ )
	{
		double x[3], xp[3];
		points->GetPoint(pointId, x);
		xp[0] = x[0] - Center[0];
		xp[1] = x[1] - Center[1];
		xp[2] = x[2] - Center[2];
		for (unsigned int i = 0; i < 3; i++)
		{
			a0[i] += xp[0] * xp[i];
			a1[i] += xp[1] * xp[i];
			a2[i] += xp[2] * xp[i];
		}
	}

	for(unsigned int i = 0; i < 3; i++)
	{
		a0[i] /= static_cast<double>(NumPoints);
		a1[i] /= static_cast<double>(NumPoints);
		a2[i] /= static_cast<double>(NumPoints);
	}

	// Extract eigenvectors from covariance matrix
	double *v[3], v0[3], v1[3], v2[3];
	v[0] = v0; v[1] = v1; v[2] = v2;
	double eigval[3];
	vtkMath::Jacobi(a,eigval,v);


	vtkPlane* BestPlane = vtkPlane::New();
	//Set the plane normal to the smallest eigen vector
	BestPlane->SetNormal(v2[0], v2[1], v2[2]);

	//Set the plane origin to the center of mass
	BestPlane->SetOrigin(Center[0], Center[1], Center[2]);

	return BestPlane;
}


double ImageSlicing::CalculateAngle( double span,int numSlices,int sliceNum )
{
	float deltaRotDegress = span/(numSlices-1);
	return sliceNum*deltaRotDegress;
}

vtkSmartPointer<vtkImageData> ImageSlicing::BuildSlicedImage( char* dir, char* filePrefix )
{
	char buff[200];
	vtkSmartPointer<vtkBMPReader> image2DReader = vtkSmartPointer<vtkBMPReader>::New();


	std::vector<vtkSmartPointer<vtkImageData>> res; 
	for(int i = 0 ;i<30;i++)
	{
		vtkSmartPointer<vtkBMPReader> image2DReader = vtkSmartPointer<vtkBMPReader>::New();
		sprintf(buff,"%s%s%d%s",dir,filePrefix,i,".bmp");
		image2DReader->SetFileName(buff);
		image2DReader->Update();
		vtkSmartPointer<vtkImageData> resPointer = vtkSmartPointer<vtkImageData>::New(); 
		resPointer->ShallowCopy(image2DReader->GetOutput());
		res.push_back(resPointer);
	}
	vtkSmartPointer<vtkImageData> outImage = vtkSmartPointer<vtkImageData>::New();
	outImage->SetDimensions(res[0]->GetDimensions()[0],res[0]->GetDimensions()[1],30);
	outImage->SetSpacing(res[0]->GetSpacing()[0],res[0]->GetSpacing()[1],1);
	outImage->SetOrigin(res[0]->GetOrigin()[0],res[0]->GetOrigin()[1],0);
	outImage->SetScalarTypeToDouble();
	outImage->AllocateScalars();

	int* dims = outImage->GetDimensions();

	for(int z=0;z<dims[2];z++)
		for(int y =0; y<dims[1]; y++)
			for (int x=0; x<dims[0]; x++)
			{

				double val = res[z]->GetScalarComponentAsDouble(x,y,0,0);
				double *p = (double*)outImage->GetScalarPointer(x,y,z);
				*p=val;

			}
			return outImage;			
}
 

void ImageSlicing::CalculateImageCenter( vtkImageData* imageData, double* outCenter )
{
	double origin[3], spacing[3];
	int dimensions[3];

	imageData->GetOrigin(origin);
	imageData->GetSpacing(spacing);
	imageData->GetDimensions(dimensions);

	for(int i = 0; i < 3; i++)
	{
		outCenter[i] = origin[i] + ((dimensions[i] - 1 ) * spacing[i])/2;
	}
}
