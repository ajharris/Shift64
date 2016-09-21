#include "stdafx.h"

#include "MaxFlowSegmentation.h"
#include "CoupledContinuousMaxFlow.h"
#include <vector>
#include <math.h>
#include "KDE.h"
#include "vtkMath.h"
#include "ImageSlicing.h"
#include "vtkImageGradientMagnitude.h"
#include "vtkImageGaussianSmooth.h"
#include "CoupledContinuousMaxFlowParams.h"
#include "vtkLine.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageThreshold.h"
#include "vtkImageShiftScale.h"
#include <iostream>
#include <fstream>
#include <vtkParametricSpline.h>
#include <vtkKochanekSpline.h>
#include "vtkImageCast.h"
#include "vtkExtractVOI.h"
#include "vtkImageTranslateExtent.h"
#include "vtkMarchingSquares.h"
#include "vtkPolydataWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkBoundingBox.h"
#include "vtkAppendPolyData.h"
#include "vtkRuledSurfaceFilter.h"
#include "vtkTriangleStrip.h"
#include "vtkCellArray.h"
#include "vtkPolyDataNormals.h"
#include "vtkContourFilter.h"
#include "vtkStripper.h"
#include "vtkSplineFilter.h"
#include <cmath>
#include "vtkCleanPolyData.h"
#include "vtkMergePoints.h"
 
using namespace std;
//#define WRITE_DATA
 
void MaxFlowSegmentation::BuildProbabilityImage(vtkSmartPointer<vtkImageData> image, vtkPoints* points, vtkSmartPointer<vtkImageData> Cs,vtkSmartPointer<vtkImageData> Ct)
{
	const double HARD_COSNTRAINT_VAL=10000;
	const double WIDTH=30; 
	const int NUM_BIN= 256;
	const double SIGMA =7;
	const double hardConstraintsInMm  = mHardConstraintIn*image->GetSpacing()[0];
	const double hardConstraintsOutMm  = mHardConstraintOut*image->GetSpacing()[0];

	double inPdf[NUM_BIN],outPdf[NUM_BIN];
	double inLUT[NUM_BIN],outLUT[NUM_BIN];

	int numPoints  = points->GetNumberOfPoints();
	std::vector<double> valsIn,valsOut;

	int Cextent[6];
	int CDim[3];
	double CSpacing[3],COrigin[3];
	image->GetDimensions(CDim);
	image->GetSpacing(CSpacing);
	image->GetOrigin(COrigin);
	image->GetExtent(Cextent);
	Cs->SetDimensions(CDim);
	Cs->SetOrigin(COrigin);
	Cs->SetSpacing(CSpacing);
	Cs->SetNumberOfScalarComponents(1);
	Cs->SetScalarTypeToFloat();
	Cs->AllocateScalars();

	Ct->SetDimensions(CDim);
	Ct->SetOrigin(COrigin);
	Ct->SetSpacing(CSpacing);
	Ct->SetScalarTypeToFloat();
	Ct->SetNumberOfScalarComponents(1);
	Ct->AllocateScalars();


	vtkSmartPointer<vtkPolygon> polygon =
		vtkSmartPointer<vtkPolygon>::New();
	polygon->GetPoints()->SetDataTypeToDouble();
	for(int i=0;i<points->GetNumberOfPoints();i++)
	{
		double pointIn[3];
		points->GetPoint(i,pointIn);
		polygon->GetPoints()->InsertNextPoint(pointIn[0],pointIn[1], 0.0);
	}


	double bounds[6];
	polygon->GetPoints()->GetBounds(bounds);

	for (int k=0;k<1;k++)
		for (int j=0;j<image->GetDimensions()[1];j++)
			for (int i=0;i<image->GetDimensions()[0];i++)
			{

				int ijk[3] = {i,j,k};
				double point[3],closest[3];
				double n[3] = {0,0,1};
				for (int q=0; q<3; q++)
				{
					point[q] = COrigin[q] + (ijk[q]+Cextent[q*2]) * CSpacing[q];
				}
				point[2]= 0;
				double val = *((float*)image->GetScalarPointer(i,j,k));
				int isIn = vtkPolygon::PointInPolygon(point,numPoints,static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)),bounds,n);
				double distance = MyDistanceToPolygon(point,numPoints,static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)),bounds,closest);
				if(isIn==1)
				{

					valsIn.push_back(val);
					if(abs(distance) > hardConstraintsInMm)
					{
						float* p = (float*)Ct->GetScalarPointer(i,j,k);
						*p = HARD_COSNTRAINT_VAL;
					}

				}
				else if(isIn==0)
				{

					valsOut.push_back(val);
					if(abs(distance)>hardConstraintsOutMm)
					{
						float* p = (float*)Cs->GetScalarPointer(i,j,k);
						*p = HARD_COSNTRAINT_VAL;
					}
				}
			}

			KDE::ComputeKDE(valsIn.data(),inPdf,valsIn.size(),NUM_BIN,SIGMA);
			KDE::ComputeKDE(valsOut.data(),outPdf,valsOut.size(),NUM_BIN,SIGMA);;

#ifdef WRITE_DATA	
			ofstream myFile; 
			myFile.open("c:\\temp\\inPDF.txt");
			for(int i=0;i<256;i++)
				myFile << inPdf[i] << endl ;
			myFile.close();

			myFile.open("c:\\temp\\outPDF.txt");
			for(int i=0;i<256;i++)
				myFile << outPdf[i] << endl ;
			myFile.close();
#endif 

			for(int i=0;i<NUM_BIN;i++)
			{
				inLUT[i] = -log(inPdf[i])/WIDTH;
				outLUT[i] = -log(outPdf[i])/WIDTH;
			}
			for (int k=0;k<image->GetDimensions()[2];k++)
				for (int j=0;j<image->GetDimensions()[1];j++)
					for (int i=0;i<image->GetDimensions()[0];i++)
					{

						int ijk[3] = {i,j,k};

						float imgVal = *((float*)image->GetScalarPointer(i,j,k));
						float valCs=0;
						float valCt=0;

						float csFirstSlice = *((float*)Cs->GetScalarPointer(i,j,0));
						float ctFirstSlice = *((float*)Ct->GetScalarPointer(i,j,0));
						int ind = vtkMath::Round(imgVal);

						if(csFirstSlice==HARD_COSNTRAINT_VAL)
						{
							valCs = HARD_COSNTRAINT_VAL;
						}
						else
						{
							valCs = inLUT[ind];
						}
						if(ctFirstSlice == HARD_COSNTRAINT_VAL)
						{
							valCt = HARD_COSNTRAINT_VAL;
						}
						else
						{
							valCt = outLUT[ind];
						}
						float* pS = (float*)Cs->GetScalarPointer(i,j,k);
						*pS = valCs;
						float* pT = (float*)Ct->GetScalarPointer(i,j,k);
						*pT = valCt;
					}

					return;
}


vtkSmartPointer<vtkPolyData> MaxFlowSegmentation::Segement(vtkSmartPointer<vtkImageData> image, vtkPoints* initialContour, double* centerOfRotation,double* axis, bool isUS)
{

	float *CsBuff,*CtBuff,*penaltyBuff,*segmentedImageBuff;
	vtkSmartPointer<vtkImageData> segmentedSliceImage = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> outImage;
	vtkSmartPointer<vtkImageData> sliced,retored,Ct,Cs,penalty;
	Cs = vtkSmartPointer<vtkImageData>::New();
	Ct = vtkSmartPointer<vtkImageData>::New();
	penalty = vtkSmartPointer<vtkImageData>::New();

	double normalizedAxis[3];
	for (int i=0;i<3;i++)
	{
		normalizedAxis[i] = axis[i];
	}


	vtkMath::Normalize(normalizedAxis);
		vtkPlane* plane = ImageSlicing::BestFitPlane(initialContour);
	double normal[3];
	plane->GetNormal(normal);
	sliced = ImageSlicing::SliceImage(image,centerOfRotation,normalizedAxis,180,30, normal);	
	
	vtkSmartPointer<vtkImageShiftScale> shiftScaleFilter = vtkSmartPointer<vtkImageShiftScale>::New();
	shiftScaleFilter->SetInput(sliced);
	shiftScaleFilter->SetShift(-1.0f * sliced->GetScalarRange()[0]); // brings the lower bound to 0
	float oldRange = sliced->GetScalarRange()[1] - sliced->GetScalarRange()[0];
	float newRange = 255; // We want the output [0,255]

	shiftScaleFilter->SetScale(newRange/oldRange);
	shiftScaleFilter->Update();
	double center[3];
	ImageSlicing::CalculateImageCenter(image, center);
	vtkSmartPointer<vtkTransform> transform = ImageSlicing::CalculateTransform(0, normalizedAxis, centerOfRotation, normal,center);
	transform->Inverse();
	vtkSmartPointer<vtkPoints> transformedPoints = vtkSmartPointer<vtkPoints>::New();
	transform->TransformPoints(initialContour,transformedPoints);
	vtkSmartPointer<vtkImageData>  croppedImage =  shiftScaleFilter->GetOutput();
	if(!isUS)
	{
		croppedImage = CropImage(shiftScaleFilter->GetOutput(),transformedPoints);
	}

	 
	BuildProbabilityImage(croppedImage,transformedPoints,Cs,Ct);
	penalty = BuildPenaltyImage(croppedImage, isUS);
	CoupledContinuousMaxFlowParams params;
	params.beta=mBeta;
	params.cc= 0.38;
	params.fError = 1e-4;
	params.NbIters = 300;
	params.nSlices = Cs->GetDimensions()[2];
	params.Nx  = Cs->GetDimensions()[0];
	params.Ny = Cs->GetDimensions()[1];
	params.steps =0.16;

	CsBuff = ConvertVTKImageDataToBuffer(Cs);
	CtBuff = ConvertVTKImageDataToBuffer(Ct);
	penaltyBuff = ConvertVTKImageDataToBuffer(penalty);
	CoupledContinuousMaxFlow::DoMaxFlow(CsBuff,CtBuff,penaltyBuff,params,segmentedImageBuff);
	segmentedSliceImage->SetDimensions(croppedImage->GetDimensions());
	segmentedSliceImage->SetOrigin(croppedImage->GetOrigin());
	segmentedSliceImage->SetSpacing(croppedImage->GetSpacing());
	segmentedSliceImage->SetScalarTypeToFloat();
	segmentedSliceImage->SetNumberOfScalarComponents(1);
	segmentedSliceImage->AllocateScalars();
	
	ConvertBufferToVTKImageData(segmentedImageBuff,segmentedSliceImage);

	vtkSmartPointer<vtkImageThreshold> thresholdFilter = vtkSmartPointer<vtkImageThreshold>::New();
	thresholdFilter->SetOutputScalarTypeToFloat();
	thresholdFilter->SetOutValue(0);
	thresholdFilter->SetInValue(1);
	thresholdFilter->ThresholdByLower(0.5);
	thresholdFilter->SetInput(segmentedSliceImage);
	thresholdFilter->Update();

	 
	
#ifdef WRITE_DATA





	vtkSmartPointer<vtkMetaImageWriter> writer =vtkSmartPointer<vtkMetaImageWriter>::New();
	writer->SetFileName("c:\\temp\\croppedImage.mha");
	writer->SetInput(croppedImage);
	writer->Write();


	writer->SetFileName("c:\\temp\\penalty.mha");
	writer->SetInput(penalty);
	writer->Write();

	writer->SetFileName("c:\\temp\\Cs.mha");
	writer->SetInput(Cs);
	writer->Write();

	writer->SetFileName("c:\\temp\\Ct.mha");
	writer->SetInput(Ct);
	writer->Write();


	writer->SetFileName("c:\\temp\\sliced.mha");
	writer->SetInput(croppedImage);
	writer->Write();



	writer->SetFileName("c:\\temp\\segmetnedSlicedImage.mha");
	writer->SetInput(thresholdFilter->GetOutput());
	writer->Write();

 
#endif

	vtkSmartPointer<vtkPolyData> contours = ReconstructSurface(thresholdFilter->GetOutput(),normalizedAxis,centerOfRotation,normal);
#ifdef WRITE_DATA
	vtkSmartPointer<vtkPolyDataWriter> pdWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
	pdWriter->SetFileName("c:\\temp\\contours.vtk");
	pdWriter->SetInput(contours);
	pdWriter->Write();
#endif

	delete[](CsBuff);
	delete[](CtBuff);
	delete[](penaltyBuff);
	plane->Delete();
	free(segmentedImageBuff);
	return contours;
}

vtkSmartPointer<vtkImageData> MaxFlowSegmentation::BuildPenaltyImage( vtkSmartPointer<vtkImageData> slicedImage, bool isUS )
{
	
	double  sigma,alpha_1,alpha_2,alpha_3;
	sigma =2;
	if(isUS)
	{
		alpha_1 = 0.5; 
		alpha_2 = 1;
		alpha_3 = 30;
	}
	else
	{
		alpha_1 = 0.05; 
		alpha_2 = 0.7;
		alpha_3 = 3;
	}

	vtkSmartPointer<vtkImageData> outImage = vtkSmartPointer<vtkImageData>::New();
	outImage->SetDimensions(slicedImage->GetDimensions());
	outImage->SetSpacing(slicedImage->GetSpacing());
	outImage->SetOrigin(slicedImage->GetOrigin());
	outImage->SetExtent(slicedImage->GetExtent());
	outImage->SetScalarTypeToFloat();
	outImage->AllocateScalars();

	vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter = vtkSmartPointer<vtkImageGaussianSmooth> ::New();
	// if you set dimensionality of the Gaussian to 2 you only get the first slice . This in contrast to the gradient magnitude filter which works fine. 
	// Since we want a 2D kernel it is effectively achieved by setting a very tight sigma on the Z direction. 
	gaussianSmoothFilter->SetDimensionality(3);
	gaussianSmoothFilter->SetStandardDeviation(sigma,sigma);
	gaussianSmoothFilter->SetInput(slicedImage);
	gaussianSmoothFilter->Update();
	gaussianSmoothFilter->SetRadiusFactors(4,4,1);
	vtkSmartPointer<vtkImageGradientMagnitude> gradientMagnitudeFilter = vtkSmartPointer<vtkImageGradientMagnitude>::New();
	gradientMagnitudeFilter->SetDimensionality(2);
	gradientMagnitudeFilter->SetInput(gaussianSmoothFilter->GetOutput());
	gradientMagnitudeFilter->Update();

	int dim[3];
	slicedImage->GetDimensions(dim);
	int numSlices = dim[2];

	float* max = new float[numSlices];

	for (int k=0;k<dim[2];k++)
	{
		max[k] = -1;
		for(int j=0;j<dim[1];j++)
			for(int i=0;i<dim[0];i++)
			{
				float* p = (float*)gradientMagnitudeFilter->GetOutput()->GetScalarPointer(i,j,k);

				if(*p > max[k])
					max[k]=*p;
			}
	}


	for (int k=0;k<dim[2];k++)
	{
		for(int j=0;j<dim[1];j++)
			for(int i=0;i<dim[0];i++)
			{
				float gradientMag =*(float*)gradientMagnitudeFilter->GetOutput()->GetScalarPointer(i,j,k);
				float val = alpha_1+alpha_2*exp(-gradientMag/max[k]*alpha_3);

				float* p = (float*)outImage->GetScalarPointer(i,j,k);
				*p = val;
			}
	}
	delete(max);
	return outImage;
}

float* MaxFlowSegmentation::ConvertVTKImageDataToBuffer( vtkImageData* image )
{

	int dim[3];
	image->GetDimensions(dim);
	float* buffer = new float [dim[0]*dim[1]*dim[2]];
	int sliceSize = dim[0]*dim[1];
	for(int k=0;k<dim[2];k++)
		for(int j=0 ; j< dim[1];j++)
			for(int i=0;i<dim[0]; i++)
			{
				float* p = (float*)image->GetScalarPointer(i,j,k);
				buffer [k*sliceSize+i*dim[1]+j]= *p;
			}
			return buffer;
}

void MaxFlowSegmentation::ConvertBufferToVTKImageData( float* buffer, vtkImageData* image )
{
	int dim[3];
	image->GetDimensions(dim);
	int sliceSize = dim[0]*dim[1];
	for(int k=0;k<dim[2];k++)
		for(int j=0 ; j< dim[1];j++)
			for(int i=0;i<dim[0]; i++)
			{
				float val = buffer [k*sliceSize+i*dim[1]+j];
				float* p = (float*)image->GetScalarPointer(i,j,k);
				*p=val;
			}
}



double MaxFlowSegmentation::MyDistanceToPolygon(double x[3], int numPts, double *pts,
												double bounds[6], double closest[3])
{
	// Not inside, compute the distance of the point to the edges.
	double minDist2=VTK_LARGE_FLOAT;
	double *p0, *p1, dist2, t, c[3];
	for (int i=0; i<numPts; i++)
	{
		p0 = pts + 3*i;
		p1 = pts + 3*((i+1)%numPts);
		dist2 = vtkLine::DistanceToLine(x, p0, p1, t, c);
		if ( dist2 < minDist2 )
		{
			minDist2 = dist2;
			closest[0] = c[0];
			closest[1] = c[1];
			closest[2] = c[2];
		}
	}

	return sqrt(minDist2);
}

MaxFlowSegmentation::MaxFlowSegmentation()
{
	mHardConstraintIn = 20;
	mHardConstraintOut = 10;
	mBeta = 0.04;
}

vtkSmartPointer<vtkImageData> MaxFlowSegmentation::CropImage( vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkPoints> initialContour )
{
	const int cropMarginPixels = 50;
	int extent [6];
	 
	image->GetExtent(extent);
	double bounds[6],minPoint[3],maxPoint[3],pcoords[3];
	int maxInd[3],minInd[3],centerInd[3];
	
	initialContour->GetBounds(bounds);
	for(int i=0;i<3;i++)
	{
		minPoint[i] = bounds[2*i];
		maxPoint[i] = bounds[2*i+1];
	}
	double center[3];
	ImageSlicing::CalculateImageCenter(image,center);
	image->ComputeStructuredCoordinates(minPoint,minInd,pcoords);
	image->ComputeStructuredCoordinates(maxPoint,maxInd,pcoords);
	image->ComputeStructuredCoordinates(center,centerInd,pcoords);

	int startROI[2],endROI[2];
	for(int i=0;i<2;i++)
	{
		int minRadius= abs(centerInd[i]-(minInd[i]-cropMarginPixels));
		int maxRadius= abs(centerInd[i]-(maxInd[i]+cropMarginPixels));
        int radius = 0;
        if (minRadius < maxRadius)
            radius = maxRadius;
        else
            radius = minRadius;

		//int radius= max(minRadius,maxRadius);
		startROI[i] = centerInd[i]-radius< 0 ?0: centerInd[i]-radius;
		endROI[i] = centerInd[i]+radius> extent[2*i+1] ?extent[2*i+1]: centerInd[i]+radius;
	}
		
	vtkSmartPointer<vtkExtractVOI> extactVoi = vtkSmartPointer<vtkExtractVOI>::New();
	extactVoi->SetInput(image);
	extactVoi->SetVOI(startROI[0],endROI[0],startROI[1],endROI[1],extent[4],extent[5]);
	extactVoi->Update();

	vtkSmartPointer<vtkImageTranslateExtent> transExtentFilter = vtkSmartPointer<vtkImageTranslateExtent>::New();
	transExtentFilter->SetInputConnection(extactVoi->GetOutputPort());
	transExtentFilter->SetTranslation(-startROI[0],-startROI[1],0);
	transExtentFilter->Update();

	return transExtentFilter->GetOutput();

	
}

vtkSmartPointer<vtkPolyData> MaxFlowSegmentation::ReconstructSurface( vtkSmartPointer<vtkImageData> inImage, double * axis, double* cor, double* normal)
{
	vtkSmartPointer<vtkPolyData> emptyPolydata = vtkSmartPointer<vtkPolyData>::New();
	try
	{

		const int NDIVISON =699;
		const int FIRST_DIVISON = 100;
		const int numPoints2D = NDIVISON+1;

		double center[3];
		ImageSlicing::CalculateImageCenter(inImage,center);
		vector<vtkSmartPointer<vtkPolyData>> res;
		vtkSmartPointer<vtkMarchingSquares> marchingSquares  = vtkSmartPointer<vtkMarchingSquares> ::New();
		int dim[3];
		inImage->GetDimensions(dim);
		marchingSquares->SetInput(inImage);
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
		std::vector<vtkSmartPointer<vtkPolyData>> contours;


		double spacing[3];
		inImage->GetSpacing(spacing);
		vtkIdType nPoints;
		vtkIdType *pointArr1,*pointArr2;

		for(int i=0;i<dim[2];i++)
		{
			//convert label maps to contours.
			marchingSquares->SetImageRange(0,dim[0],0,dim[1],i,i);
			marchingSquares->SetNumberOfContours(1);
			marchingSquares->SetValue(0,0.5);
			marchingSquares->Update();

			//all line fragments are connected to a single big line.
			vtkSmartPointer<vtkStripper> stripper = vtkSmartPointer<vtkStripper>::New();
			stripper->SetInput(marchingSquares->GetOutput());
			stripper->SetMaximumLength(10000);
			stripper->Update();

			///interpolate contours such that all contours have same number of samples.
			vtkSmartPointer<vtkSplineFilter> splineFilter = vtkSmartPointer<vtkSplineFilter>::New();
			splineFilter->SetNumberOfSubdivisions(FIRST_DIVISON);
			splineFilter->SetInput(stripper->GetOutput());
			splineFilter->Update();



			vtkSmartPointer<vtkPoints> points = splineFilter->GetOutput()->GetPoints();
			vtkSmartPointer<vtkCellArray> splineCell = splineFilter->GetOutput()->GetLines();


			splineCell->GetNextCell(nPoints,pointArr1);

			//find intersection point with rotational axis which is always Y in this coordinate system.
			int startingInd=-1,endingInd=-1;
			for(int p = 1;p<nPoints;p++)
			{
				double pbfr[3],pafr[3];
				points->GetPoint(pointArr1[p-1],pbfr);
				points->GetPoint(pointArr1[p],pafr);
				if((pbfr[0]-center[0])*(pafr[0]-center[0])<0 && (pbfr[1]-center[1])<0)
				{
					startingInd = p-1;
				}
				if((pbfr[0]-center[0])*(pafr[0]-center[0])<0 && (pbfr[1]-center[1])>0)
				{
					endingInd = p-1;
				}
			}
			if(startingInd<0  || endingInd<0)
				return emptyPolydata;
			vtkSmartPointer<vtkPolyData> firstSemiContour  = vtkSmartPointer<vtkPolyData>::New(); 
			vtkSmartPointer<vtkIdList> firstSemiContourIds =vtkSmartPointer<vtkIdList> ::New();
			vtkSmartPointer<vtkPoints> firstSemiContourPts = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkCellArray> firstSemiContourCellArr = vtkSmartPointer<vtkCellArray> ::New();
			int cnt=0;
			//splint the semi contour to two 'semi contours' each of which starting and ending at an intersection point
			for(int ind = startingInd;ind!=(endingInd+1)%nPoints;ind=(ind+1)%nPoints)
			{
				double pt[3];
				points->GetPoint(pointArr1[ind],pt);
				firstSemiContourPts->InsertNextPoint(pt);
				firstSemiContourIds->InsertNextId(cnt);
				cnt++;
			}

			firstSemiContour->SetPoints(firstSemiContourPts);
			firstSemiContourCellArr->InsertNextCell(firstSemiContourIds);
			firstSemiContour->SetLines(firstSemiContourCellArr);




			vtkSmartPointer<vtkPolyData> secondSemiContour  = vtkSmartPointer<vtkPolyData>::New(); 
			vtkSmartPointer<vtkIdList> secondSemiContourIds =vtkSmartPointer<vtkIdList> ::New();
			vtkSmartPointer<vtkPoints> secondSemiContourPts = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkCellArray> secondSemiContourCellArr = vtkSmartPointer<vtkCellArray> ::New();
			cnt=0;
			for(int ind = endingInd;ind!=(startingInd+1)%nPoints;ind=(ind+1)%nPoints)
			{
				double pt[3];
				points->GetPoint(pointArr1[ind],pt);
				secondSemiContourPts->InsertNextPoint(pt);
				secondSemiContourIds->InsertNextId(cnt);
				cnt++;
			}
			secondSemiContour->SetPoints(secondSemiContourPts);
			secondSemiContourCellArr->InsertNextCell(secondSemiContourIds);
			secondSemiContour->SetLines(secondSemiContourCellArr);


			// interpolate semi contours to equeal number of samples for the slices.
			vtkSmartPointer<vtkSplineFilter> firstSemiContourSpline = vtkSmartPointer<vtkSplineFilter>::New();
			firstSemiContourSpline->SetNumberOfSubdivisions(NDIVISON/2);
			firstSemiContourSpline->SetInput(firstSemiContour);
			firstSemiContourSpline->Update();

			vtkSmartPointer<vtkSplineFilter> secondSemiContourSpline = vtkSmartPointer<vtkSplineFilter>::New();
			secondSemiContourSpline->SetNumberOfSubdivisions(NDIVISON/2);
			secondSemiContourSpline->SetInput(secondSemiContour);
			secondSemiContourSpline->Update();

			//append the interpolated semi contours.
			vtkSmartPointer<vtkAppendPolyData> wholeCntApp = vtkSmartPointer<vtkAppendPolyData>::New();
			wholeCntApp->AddInputConnection(firstSemiContourSpline->GetOutputPort());
			wholeCntApp->AddInputConnection(secondSemiContourSpline->GetOutputPort());
			wholeCntApp->Update();

			//transform the contours to the 3D image coordinate system (inverse of the re slice transform).
			double angle = ImageSlicing::CalculateAngle(180,dim[2],i);	
			vtkSmartPointer<vtkTransform> transform  = vtkSmartPointer<vtkTransform> ::New();
			transform->Translate(0,0,-i*spacing[2]);
			vtkSmartPointer<vtkTransform> resliceTransform = ImageSlicing::CalculateTransform(angle,axis,cor,normal,center);
			transform->PostMultiply();
			transform->Concatenate(resliceTransform);
			transformFilter->SetInput(wholeCntApp->GetOutput());
			transformFilter->SetTransform(transform);
			transformFilter->Update();

			vtkSmartPointer<vtkPolyData> contour = vtkSmartPointer<vtkPolyData>::New();
			contour->ShallowCopy(transformFilter->GetOutput());
			contours.push_back(contour);

#ifdef WRITE_DATA
			char filename[40];
			vtkSmartPointer<vtkPolyDataWriter> polyDataWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
			sprintf(filename,"c:\\temp\\Contours%d.vtk",i);
			polyDataWriter->SetInput(contour);
			polyDataWriter->SetFileName(filename);
			polyDataWriter->Write();
#endif


		}
		//connect all the contours to form a surface.
		for(int i=1;i<dim[2];i++)
		{
			vtkPolyData* newPolyData = vtkPolyData::New();
			vtkPoints* combinedPoints = vtkPoints::New();
			vtkSmartPointer<vtkTriangleStrip> strip = vtkSmartPointer<vtkTriangleStrip>::New();

			//connect all contours sequentially
			int bfrContInd,afrContInd;
			bfrContInd = (i-1)%(dim[2]-1);
			afrContInd = i%(dim[2]-1);
			vtkPolyData* bfrCont = contours.at(bfrContInd);
			vtkPolyData* afrCont = contours.at(afrContInd);
			bfrCont->GetLines()->GetCell(0,nPoints,pointArr1);
			afrCont->GetLines()->GetCell(0,nPoints,pointArr2);

			bool flip =false;
			for(int j  = 0; j<numPoints2D ;j++)
			{

				int brtPtInd = pointArr1[j];
				int aftPtInd;
				// this condition detects that we need connect the last and the first contours. They are mirror images therefoe the array needs to be flipped.
				if(afrContInd==0)
					aftPtInd = pointArr2[numPoints2D - j];
				else
					aftPtInd = pointArr2[j];
				//we need to flip the order when reaching the intersection points when the contours cross because it confuses the rendering - the inside becomes out side.
				if(j ==  numPoints2D/2 )
					flip=true;
				if(flip)
				{
					combinedPoints->InsertNextPoint(bfrCont->GetPoint(brtPtInd));
					combinedPoints->InsertNextPoint(afrCont->GetPoint(aftPtInd));
				}
				else
				{
					combinedPoints->InsertNextPoint(afrCont->GetPoint(aftPtInd));
					combinedPoints->InsertNextPoint(bfrCont->GetPoint(brtPtInd));

				}
				strip->GetPointIds()->InsertId(2*j,2*j);
				strip->GetPointIds()->InsertId(2*j+1,2*j+1);
			}

			vtkCellArray* stripCellArr = vtkCellArray::New();
			stripCellArr->InsertNextCell(strip);
			newPolyData->SetPoints(combinedPoints);
			newPolyData->SetStrips(stripCellArr);
			appendPolyData->AddInput(newPolyData);
		}
		appendPolyData->Update();

		//renove duplicate points.
		vtkSmartPointer<vtkCleanPolyData> clean = vtkSmartPointer<vtkCleanPolyData>::New();
		clean->SetInput(appendPolyData->GetOutput());
		clean->SetTolerance(0);
		clean->SetAbsoluteTolerance(0);
		clean->PointMergingOn();
		clean->Update();
		vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
		normalGenerator->SetInput(clean->GetOutput());
		normalGenerator->Update();
		normalGenerator->ComputePointNormalsOn();
		return normalGenerator->GetOutput();
	}
	catch(exception e)
	{

		return emptyPolydata;
	}

}

vtkSmartPointer<vtkImageData> MaxFlowSegmentation::SegementSlicedImage( vtkSmartPointer<vtkImageData> image, vtkPoints* initialContour, bool isUS )
{
	float *CsBuff,*CtBuff,*penaltyBuff,*segmentedImageBuff;
	vtkSmartPointer<vtkImageData> segmentedSliceImage = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> Ct,Cs,penalty;
	Cs = vtkSmartPointer<vtkImageData>::New();
	Ct = vtkSmartPointer<vtkImageData>::New();
	penalty = vtkSmartPointer<vtkImageData>::New();



	BuildProbabilityImage(image,initialContour,Cs,Ct);
	penalty = BuildPenaltyImage(image, isUS);


	CoupledContinuousMaxFlowParams params;
	params.beta=mBeta;
	params.cc= 0.38;
	params.fError = 1e-4;
	params.NbIters = 300;
	params.nSlices = Cs->GetDimensions()[2];
	params.Nx  = Cs->GetDimensions()[0];
	params.Ny = Cs->GetDimensions()[1];
	params.steps =0.16;

	CsBuff = ConvertVTKImageDataToBuffer(Cs);
	CtBuff = ConvertVTKImageDataToBuffer(Ct);
	penaltyBuff = ConvertVTKImageDataToBuffer(penalty);
	CoupledContinuousMaxFlow::DoMaxFlow(CsBuff,CtBuff,penaltyBuff,params,segmentedImageBuff);
	segmentedSliceImage->SetDimensions(image->GetDimensions());
	segmentedSliceImage->SetOrigin(image->GetOrigin());
	segmentedSliceImage->SetSpacing(image->GetSpacing());
	segmentedSliceImage->SetScalarTypeToDouble();
	segmentedSliceImage->SetNumberOfScalarComponents(1);
	segmentedSliceImage->AllocateScalars();


	ConvertBufferToVTKImageData(segmentedImageBuff,segmentedSliceImage);

	vtkSmartPointer<vtkImageThreshold> thresholdFilter = vtkSmartPointer<vtkImageThreshold>::New();
	thresholdFilter->SetOutputScalarTypeToDouble();
	thresholdFilter->SetOutValue(0);
	thresholdFilter->SetInValue(1);
	thresholdFilter->ThresholdByLower(0.5);
	thresholdFilter->SetInput(segmentedSliceImage);
	thresholdFilter->Update();



#ifdef WRITE_DATA
	vtkSmartPointer<vtkMetaImageWriter> writer =vtkSmartPointer<vtkMetaImageWriter>::New();

	writer->SetFileName("c:\\temp\\penalty.mha");
	writer->SetInput(penalty);
	writer->Write();

	writer->SetFileName("c:\\temp\\Cs.mha");
	writer->SetInput(Cs);
	writer->Write();

	writer->SetFileName("c:\\temp\\Ct.mha");
	writer->SetInput(Ct);
	writer->Write();

	writer->SetFileName("c:\\temp\\segmetnedSlicedImage.mha");
	writer->SetInput(thresholdFilter->GetOutput());
	writer->Write();

	writer->SetFileName("c:\\temp\\outImage.mha");
	writer->SetInput(segmentedSliceImage);
	writer->Write();
#endif

	delete[](CsBuff);
	delete[](CtBuff);
	delete[](penaltyBuff);
	free(segmentedImageBuff);
	return thresholdFilter->GetOutput();
}
