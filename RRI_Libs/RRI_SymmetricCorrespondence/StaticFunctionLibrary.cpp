// StaticFunctionLibrary.cpp: implementation of the StaticFunctionLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "RotateMFC.h"
//#include "RotateDlg.h"
//Note #include "engine.h" is called in RotateDlg.h and therefore
// we don't need to include it again in the header of StaticFunctionLibrary
#include "StaticFunctionLibrary.h"
#include "vtkObjectFactory.h"
#include "vtkPolyLine.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkLine.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkDoubleArray.h"
#include "vtkFeatureEdges.h"
#include "vtkPlane.h"
#include "vtkImageData.h"
#include "vtkPolyDataWriter.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkCleanPolyData.h"
#include "vtkStripper.h"
#include "vtkTriangle.h"
//#include "engine.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkLandmarkTransform.h"
#include "vtkSmartPointer.h"
#include "vtkClipPolyData.h"
#include "vtkCellLocator.h"
#include "vtkCutter.h"
//#include "MyWeightedLandmarkTransform.h"
#include "MyIterativeClosestPointTransform.h"
#include "OldSymmetricCorrespondence.h"
#include "MyMassProperties.h"
#include "BCDefinedMacro.h"
typedefT(vtkCutter);
typedefT(vtkPlane);
#include "vtkStripper.h"
typedefT(vtkStripper);
#include "vtkMyStripper.h"
typedefT(vtkMyStripper);
#include "vtkFeatureEdges.h"
typedefT(vtkFeatureEdges);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	//laspack linear solver package, calling with C standard
#include "f2c.h"
//#include "blaswrap.h"
#include "clapack.h"
#include ".\staticfunctionlibrary.h"

#ifdef __cplusplus
}
#endif


#define MRound(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#define AbsValue(x) (x<0?-x:x)
vtkStandardNewMacro(StaticFunctionLibrary);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const int VTK_YES_INTERSECTION=2;

//int StaticFunctionLibrary::FindAllIntersection(vtkPolyLine *pline, vtkPoints* outputP, vtkIdList* subIdArray, double startpoint[], double endpoint[], float tol)
//{
//	// outputP and subIdArray will be initialize here.
//	// check vtkPolyLine to be a closed curve
//
//	// known problem - Nov 14 2005:
//	// if the second point of one line (r = 1) has a perpendicular distance to pline of smaller than tol,
//	// and the angle between the next line and pline is less than 45 degree, and that the real intersection
//	// is somewhere in the next line, then both the real intersection and the projection from the second point
//	// of the previous line on pline are treated as intersection. Thus, the intersection is doubly 
//	// counted.
//
//	int n = pline->GetNumberOfPoints();
//	// if not, then make it closed.
//	if (pline->GetPointId(0) != pline->GetPointId(n-1)) 
//	{
//		pline->GetPointIds()->InsertId(n, pline->GetPointId(0));
//		pline->GetPoints()->InsertPoint(n, pline->GetPoints()->GetPoint(0));
//		n++;
//	}
//
//	double t, pcoords[3];
//	int subId;
//	int intersectionPoints = 0;
//	vtkPolyLine* tempPolyLine = vtkPolyLine::New();
//	tempPolyLine->DeepCopy(pline);
//	int npoints = n - 1; //number of non-duplicated point
//	int starti = 0;
//	double x1[3], tempPoint[3];
//
//	outputP->Initialize();
//	subIdArray->Initialize();
//
//	float minDist, dist;
//	int i;
//	double u, v;
//	int goToAddPoint;
//	int IsVtkYesIntersection;
//	vtkPoints* tempPoints = tempPolyLine->GetPoints();
//
//	double xtemp[3], ytemp[3];
//	
//	for (int step = 0; TRUE; step++) 
//	{
//
//		int success1 = tempPolyLine->IntersectWithLine(startpoint, endpoint, tol, t, x1, pcoords, subId);
//		starti += subId + 1;
//		
//		// Two consecutive intersections could not have a distance less than tolerance between them.
//
//		if (!success1) break; 
//
//		goToAddPoint = 0;
//		minDist = VTK_LARGE_FLOAT;
//
//		//throw away those point if there is not intersection, but success1 only because 
//		//one of the end point of the line is too close to the pline
//
//		tempPoints->GetPoint(subId, xtemp);
//		tempPoints->GetPoint(subId + 1, ytemp);
//
//		IsVtkYesIntersection = (vtkLine::Intersection(startpoint, endpoint, xtemp, ytemp, u, v) == VTK_YES_INTERSECTION);
// 
//		if (IsVtkYesIntersection)
//		{
//			if ((intersectionPoints == 0)) 
//			{
//				goToAddPoint = 1;
//			}
//			else
//			{
//				for (i = 0; i < outputP->GetNumberOfPoints(); i++)
//				{
//					dist = vtkMath::Distance2BetweenPoints(x1, outputP->GetPoint(i));
//					if (dist < minDist)
//					{
//						minDist = dist;
//					}
//				}
//					
//				if (minDist > tol*tol)
//				{
//					goToAddPoint = 1;
//				}
//			}
//		}
//	
//		if (goToAddPoint)
//		{
//			outputP->InsertNextPoint(x1);
//			subIdArray->InsertNextId(starti - 1);
//			intersectionPoints++;
//		}
//
//		if (starti < npoints) {
//			tempPolyLine->GetPoints()->Initialize();
//			tempPolyLine->GetPointIds()->Initialize();
//			tempPolyLine->GetPoints()->SetNumberOfPoints(n - starti);
//			tempPolyLine->GetPointIds()->SetNumberOfIds(n - starti);
//
//			for (int j = starti; j < n; j++) {
//				pline->GetPoints()->GetPoint(j, tempPoint);
//				tempPolyLine->GetPoints()->SetPoint(j-starti, tempPoint);
//				tempPolyLine->GetPointIds()->SetId(j-starti, pline->GetPointId(j));
//			}
//		
//		}
//		else {
//			break;
//		}
//	} 
//
//	tempPolyLine->Delete();
//	return intersectionPoints;
//}
//
////void StaticFunctionLibrary::ComputeMeanCurve(vtkPolyData **Inputs, int NumberOfPolyData, 
////											 vtkPolyData *Output, int IndexFirstSlice,
////											 int AttemptToFindAllIntersections)
////{	// Arguments:
////	// Inputs[0 to n-1] are n closed curves for which we want to find the mean
////	// Output is the resulting mean curve.
////	// Output->GetPointData() contains n scalars array and 1 vector array
////	//   The vector array represents the direction in which we sample at each vertex
////	//   The scalars array represents the locations of different curve at that vertex
////	// Inputs[IndexFirstSlice] has the lowest curvature.
////	// AttempToFindAllIntersection: If value = 0, The algorithm does not try again to find intersections
////	// if failed once. If value = 1, it tries an additional 10 times (0.1, -0.1, ..., 0.5, -0.5) if 
////	// intersections is not immediately found.
////
////	int i, j, k, u;
////	OldSymmetricCorrespondence** scs = new OldSymmetricCorrespondence*[NumberOfPolyData-1];
////	float normal1[3];
////
////	vtkPolyData* FirstContour = Inputs[IndexFirstSlice];
////	vtkPolyData* SecondContour;
////	int* SecondContourOrder = new int [NumberOfPolyData-1];
////
////	float referencePoint1[3], referencePointk[3];
////
////	//Output Points, Lines, Vectors, Scalars;
////	vtkPoints* outputPoints = vtkPoints::New();
////	Output->SetPoints(outputPoints);
////	outputPoints->Delete();
////
////	vtkCellArray* outputLines = vtkCellArray::New();
////	Output->SetLines(outputLines);
////	outputLines->Delete();
////
////	vtkFloatArray* outputVector = vtkFloatArray::New();
////	outputVector->SetNumberOfComponents(3);
////	Output->GetPointData()->SetVectors(outputVector);
////	outputVector->Delete();
////
////	vtkFloatArray** Data = new vtkFloatArray* [NumberOfPolyData];
////	float* tempData = new float [NumberOfPolyData]; 
////	float tempSum; 
////
////	for (i = 0; i < NumberOfPolyData; i++)
////	{
////		Data[i] = vtkFloatArray::New();
////		CString dataNum;
////		dataNum.Format("%s%i", "Data", i);
////		Data[i]->SetName((LPCSTR) dataNum);
////		Data[i]->SetNumberOfComponents(1);
////		if (i == 0)
////		{
////			Output->GetPointData()->SetScalars(Data[i]);
////		}
////		else
////		{
////			Output->GetPointData()->AddArray(Data[i]);
////		}
////		Data[i]->Delete();
////	}
////	//initialization of output completed
////
////	// Store angle
////	vtkFloatArray* AngleArray = vtkFloatArray::New();
////	AngleArray->SetName("Angle");
////	AngleArray->SetNumberOfComponents(1);
////	Output->GetPointData()->AddArray(AngleArray);
////	AngleArray->Delete();
////
////	for (i = 0; i < (NumberOfPolyData - 1); i++)
////	{
////		scs[i] = OldSymmetricCorrespondence::New();
////		scs[i]->SetContour1(FirstContour);
////				
////		if (i < IndexFirstSlice)
////		{
////			SecondContourOrder[i] = i;
////		}
////		else 
////		{
////			SecondContourOrder[i] = i+1;
////		}
////
////		SecondContour = Inputs[SecondContourOrder[i]];
////
////		if (!StaticFunctionLibrary::SameOrientation(SecondContour, FirstContour))
////		{
////			SecondContour->ReverseCell(SecondContour->GetNumberOfVerts());
////		}
////		scs[i]->SetContour2(SecondContour);
////		scs[i]->FindCorrespondence();
////
////		//test
////
////		vtkPolyData* PolyDataForTest = vtkPolyData::New();
////		vtkPoints* PointForTest = vtkPoints::New();
////		PolyDataForTest->SetPoints(PointForTest);
////		vtkCellArray* CellArrayForTest = vtkCellArray::New();
////		PolyDataForTest->SetLines(CellArrayForTest);
////		PointForTest->Delete();
////		CellArrayForTest->Delete();
////
////		for (int v = 0; v < scs[i]->GetNumberOfPoints(); v++) 
////		{
////			PointForTest->InsertNextPoint(scs[i]->GetPoints1()->GetPoint(v));
////			PointForTest->InsertNextPoint(scs[i]->GetPoints2()->GetPoint(v));
////			CellArrayForTest->InsertNextCell(2);
////			CellArrayForTest->InsertCellPoint(2*v);
////			CellArrayForTest->InsertCellPoint(2*v+1);
////		}
////
////		CString filename;
////		filename.Format("%s%i%s", "C:\\c\\scs_", i, ".vtk");
////		CRotateDlg::WritePolyData(PolyDataForTest, (LPCSTR) filename);
////		PolyDataForTest->Delete();
////		//test end
////
////
////	}
////
////	float NormalAtContour1[3];
////	//float tempPoint[3];
////	//float tempNormalNorm;
////	//float tempAngle;
////	//float NormalNorm;
////	float StartPoint[3], EndPoint[3];
////	//float LengthOfIntersectLine;
////	int MissedPoints = 0;
////	float jVector[3], tempPoint1[3];
////	float meanData, translatedData;
////	//test
////	int ptsId[2] = {0, 1};
////	vtkPolyData* PD = vtkPolyData::New();
////	vtkCellArray* CA = vtkCellArray::New();
////	CA->InsertNextCell(2, ptsId);
////	PD->SetLines(CA);
////	vtkPoints* PDPoints = vtkPoints::New();
////	PD->SetPoints(PDPoints);
////	CA->Delete();
////	PDPoints->Delete();
////
////	vtkPolyData* tempPolyData;
////	
////	vtkPoints* ptsi = vtkPoints::New();
////	vtkPoints* ptsiPlus1 = vtkPoints::New();
////	vtkPoints* ptsiMinus1 = vtkPoints::New();
////	vtkPoints* ptsInUse = vtkPoints::New();
////	int iPlus1, iMinus1;
////
////	outputLines->InsertNextCell(FirstContour->GetNumberOfPoints() + 1);
////
////	int maxIndex = (AttemptToFindAllIntersections)?10:0;
////	int numPts1 = FirstContour->GetNumberOfPoints();
////	float deltaR; //parametric value determining the point in use.
////	float tempPointInUse[3];
////	float tempPointi[3], tempPointi2[3];
////	int exitNoIntersection;
////	float startFactor = 1.0;
////	int IntersectionExist = 0;
////	int loopcount = 0;
////
////	for (i = 0; i < numPts1; i++) 
////	{
////
////		ptsi->Initialize();
////		ptsiPlus1->Initialize();
////		ptsiMinus1->Initialize();
////
////		iMinus1 = (i + numPts1 - 1)%numPts1;
////		iPlus1 = (i + 1)%numPts1;
////
////		ptsiMinus1->InsertPoint(IndexFirstSlice, scs[0]->GetPoints1()->GetPoint(iMinus1));
////		ptsi->InsertPoint(IndexFirstSlice, scs[0]->GetPoints1()->GetPoint(i));
////		ptsiPlus1->InsertPoint(IndexFirstSlice,	scs[0]->GetPoints1()->GetPoint(iPlus1));
////		
////		for (j = 0; j < NumberOfPolyData-1; j++)
////		{
////			ptsiMinus1->InsertPoint(SecondContourOrder[j], 
////				scs[j]->GetPoints2()->GetPoint(iMinus1));
////			ptsi->InsertPoint(SecondContourOrder[j], scs[j]->GetPoints2()->GetPoint(i));
////			ptsiPlus1->InsertPoint(SecondContourOrder[j], 
////				scs[j]->GetPoints2()->GetPoint(iPlus1));
////		}
////
////		StaticFunctionLibrary::DetermineOutwardNormal(FirstContour, i, NormalAtContour1);
////	
////		IntersectionExist = 0;
////		startFactor = 1.0;
////		loopcount = 0;
////
////		do 
////		{
////			loopcount++;
////			startFactor+=0.5;
////
////			for (j = 0; j < maxIndex + 1; j++)
////			{	
////				ptsInUse->Initialize();
////				if (j == 0)
////				{
////					ptsInUse->DeepCopy(ptsi);
////					deltaR = 0.0;
////				}
////				else 
////				{
////					deltaR = pow(-1.0, j - 1)*0.1*(1+(j - 1)/2);
////					
////					if (deltaR > 0.0)
////					{
////						for (k = 0; k < NumberOfPolyData; k++)
////						{
////							ptsi->GetPoint(k, tempPointi);
////							ptsiPlus1->GetPoint(k, tempPointi2);
////							tempPointInUse[0] = tempPointi[0]*(1.0 - deltaR) +
////								tempPointi2[0]*deltaR;
////							tempPointInUse[1] = tempPointi[1]*(1.0 - deltaR) +
////								tempPointi2[1]*deltaR;
////							tempPointInUse[2] = tempPointi[2]*(1.0 - deltaR) +
////								tempPointi2[2]*deltaR;
////							ptsInUse->InsertPoint(k, tempPointInUse);
////						}
////					}
////					else
////					{
////						deltaR = -deltaR;
////						for (k = 0; k < NumberOfPolyData; k++)
////						{
////							ptsi->GetPoint(k, tempPointi);
////							ptsiMinus1->GetPoint(k, tempPointi2);
////							tempPointInUse[0] = tempPointi[0]*(1.0 - deltaR) +
////								tempPointi2[0]*deltaR;
////							tempPointInUse[1] = tempPointi[1]*(1.0 - deltaR) +
////								tempPointi2[1]*deltaR;
////							tempPointInUse[2] = tempPointi[2]*(1.0 - deltaR) +
////								tempPointi2[2]*deltaR;
////							ptsInUse->InsertPoint(k, tempPointInUse);
////						}
////						
////					}
////				}
////				
////				StaticFunctionLibrary::FindLineFromCorrespondence(ptsInUse, NormalAtContour1, 
////					StartPoint, EndPoint, normal1, startFactor);
////				StaticFunctionLibrary::Centroid(ptsInUse, referencePoint1);
////				
////				//test
////				PDPoints->Initialize();
////				PDPoints->InsertPoint(0, StartPoint);
////				PDPoints->InsertPoint(1, EndPoint);
////				
////				//test: print line
////				
////				CString filename1;
////				if (i > 99) 
////				{
////					filename1.Format("%s%i%s", "C:\\c2\\intLine_", i, ".vtk");
////				}
////				else if (i > 9)
////				{
////					filename1.Format("%s%i%s", "C:\\c2\\intLine_0", i, ".vtk");
////				}
////				else
////				{
////					filename1.Format("%s%i%s", "C:\\c2\\intLine_00", i, ".vtk");
////				}
////				CRotateDlg::WritePolyData(PD, (LPCSTR) filename1);
////				
////				//end test
////				
////				exitNoIntersection = 0;
////				
////				for (k = 0; k < NumberOfPolyData; k++) 
////				{
////					tempPolyData = Inputs[k];
////					vtkPolyLine* pl = vtkPolyLine::SafeDownCast(
////						tempPolyData->GetCell(tempPolyData->GetNumberOfVerts()));
////					vtkPoints* inP = vtkPoints::New();
////					float tol = 0.001f;
////					int numpts;
////					VERIFY(pl);
////					vtkIdList* subIdArray = vtkIdList::New();
////					numpts = StaticFunctionLibrary::FindAllIntersection
////						(pl, inP, subIdArray, StartPoint, EndPoint, tol);
////					
////					if (numpts == 0) 
////					{
////						exitNoIntersection = 1;
////						break;
////					}
////					
////					subIdArray->Delete(); // not needed in this context
////					
////					float mindist = VTK_LARGE_FLOAT;
////					float tempdist; 
////					int chosenu = 0;
////					
////					if (numpts > 1)
////					{
////						ptsInUse->GetPoint(k, referencePointk);
////						
////						for (u = 0; u < numpts; u++) 
////						{
////							tempdist = vtkMath::Distance2BetweenPoints(inP->GetPoint(u), referencePointk);
////							tempdist = sqrt(tempdist);
////							if ( tempdist < mindist)
////							{
////								chosenu = u; 
////								mindist = tempdist;
////							}
////						}
////					}
////					
////					//float cp[3]; inP->GetPoint(chosenk, cp);
////					
////					jVector[0] = inP->GetPoint(chosenu)[0] - referencePoint1[0];
////					jVector[1] = inP->GetPoint(chosenu)[1] - referencePoint1[1];
////					jVector[2] = inP->GetPoint(chosenu)[2] - referencePoint1[2];
////					tempData[k] = vtkMath::Dot(jVector, normal1);
////					inP->Delete();
////					
////				}// for k
////				
////				if (exitNoIntersection) 
////				{
////					continue;
////				}
////				else
////				{
////					IntersectionExist = 1;
////					break;
////				}
////				
////			}// for j
////
////		}while (!IntersectionExist && AttemptToFindAllIntersections && loopcount <= 18);
////
////		if (!IntersectionExist)
////		{
////			MissedPoints++;
////			CString msg;
////			msg.Format("%s%i", "Missed Point# ", i);
////			//CRotateDlg::CustomMessageBox((LPCSTR) msg);
////			VERIFY(!AttemptToFindAllIntersections);
////			continue;
////		}
////		
////		tempSum = 0.0f;
////		
////		for (j = 0; j < NumberOfPolyData; j++) 
////		{
////			tempSum += tempData[j];
////		}
////		tempSum /= NumberOfPolyData;
////		meanData = tempSum;
////		
////		tempPoint1[0] = referencePoint1[0] + meanData*normal1[0];
////		tempPoint1[1] = referencePoint1[1] + meanData*normal1[1];
////		tempPoint1[2] = referencePoint1[2] + meanData*normal1[2];
////		
////		outputPoints->InsertPoint(i - MissedPoints, tempPoint1);
////		outputLines->InsertCellPoint(i - MissedPoints);
////		outputVector->InsertTuple(i - MissedPoints, normal1);
////		float angle = atan2(normal1[1], normal1[0]);
////		AngleArray->InsertTuple(i - MissedPoints, &angle);
////		
////		for (j = 0; j < NumberOfPolyData; j++)
////		{
////			translatedData = tempData[j] - meanData;
////			Data[j]->InsertTuple(i - MissedPoints, &translatedData);
////		}
////		
////	}
////
////	outputLines->InsertCellPoint(0);
////	outputLines->UpdateCellCount(Inputs[IndexFirstSlice]->GetNumberOfPoints() + 1 - MissedPoints);
////
////	//delete
////	ptsi->Delete();
////	ptsiPlus1->Delete();
////	ptsiMinus1->Delete();
////	ptsInUse->Delete();
////	for (i = 0; i < (NumberOfPolyData - 1); i++)
////	{
////		scs[i]->Delete();
////	}
////
////	delete [] scs;
////	delete [] Data;
////	delete [] tempData;
////	delete [] SecondContourOrder;
////
////}
//
//void StaticFunctionLibrary::FindLineFromCorrespondence(vtkPoints *Pts, float NormalAtContour1[], float StartPoint[], float EndPoint[], float normal1[], float LengthFactor)
//{
//	// Inputs: Pts and NormalAtContour1;
//	// Outputs: StartPoint, EndPoint, normal1;
//	// Assumption Pts lie on the plane z = constant
//	double referencePoint1[3];
//	StaticFunctionLibrary::Centroid(Pts, referencePoint1);
//		
//	normal1[0] = 0.0f;
//	normal1[1] = 0.0f;
//	normal1[2] = 0.0f;
//
//	double tempPoint[3], tempNormal[3];
//	float tempAngle, tempNormalNorm;
//
//	// Computing direction of intersecting line;
//	float maxDFromCorrespondence = -VTK_LARGE_FLOAT;
//	for (int j = 0; j < Pts->GetNumberOfPoints(); j++) 
//	{
//		Pts->GetPoint(j, tempPoint);
//		tempNormal[0] = tempPoint[0] - referencePoint1[0];
//		tempNormal[1] = tempPoint[1] - referencePoint1[1];
//		tempNormal[2] = tempPoint[2] - referencePoint1[2];
//		tempNormalNorm = vtkMath::Norm(tempNormal);
//		
//		if (tempNormalNorm > maxDFromCorrespondence)
//		{
//			maxDFromCorrespondence = tempNormalNorm;
//		}
//		
//		tempAngle = static_cast<float> (atan(tempNormal[1]/tempNormal[0]));
//		tempNormal[0] = tempNormalNorm*cos(2.0f*tempAngle);
//		tempNormal[1] = tempNormalNorm*sin(2.0f*tempAngle);
//		
//		normal1[0] += tempNormal[0];
//		normal1[1] += tempNormal[1];
//		normal1[2] += tempNormal[2];
//	}
//	
//	float NormalNorm = vtkMath::Norm(normal1);
//	tempAngle = static_cast<float> (atan2(normal1[1],normal1[0]));
//	normal1[0] = NormalNorm*cos(tempAngle/2.0f);
//	normal1[1] = NormalNorm*sin(tempAngle/2.0f);
//	normal1[2] = 0;
//	
//	vtkMath::Normalize(normal1);
//	if (vtkMath::Dot(normal1, NormalAtContour1) < 0) 
//	{
//		normal1[0] *= -1.0f;
//		normal1[1] *= -1.0f;
//		normal1[2] *= -1.0f;
//	}
//	// Done! normal1 is the intersecting line.
//	
//	float LengthOfIntersectLine = LengthFactor*maxDFromCorrespondence;
//	
//	// resample using reference point and normal1;
//	StartPoint[0] = referencePoint1[0] - LengthOfIntersectLine*normal1[0];
//	StartPoint[1] = referencePoint1[1] - LengthOfIntersectLine*normal1[1];
//	StartPoint[2] = referencePoint1[2] - LengthOfIntersectLine*normal1[2];
//	EndPoint[0] = referencePoint1[0] + LengthOfIntersectLine*normal1[0];
//	EndPoint[1] = referencePoint1[1] + LengthOfIntersectLine*normal1[1];
//	EndPoint[2] = referencePoint1[2] + LengthOfIntersectLine*normal1[2];
//
//}
//
void StaticFunctionLibrary::Centroid(vtkPoints *pts, double c[])
{
	int i;
	double tempPoint[3];
	int npts = pts->GetNumberOfPoints();

	c[0] = 0.0f;
	c[1] = 0.0f;
	c[2] = 0.0f;

	for (i = 0; i < npts; i++) 
	{
		pts->GetPoint(i, tempPoint);
		c[0] += tempPoint[0];
		c[1] += tempPoint[1];
		c[2] += tempPoint[2];
	}

	c[0] /= static_cast<double>(npts);
	c[1] /= static_cast<double>(npts);
	c[2] /= static_cast<double>(npts);
}

//void StaticFunctionLibrary::Centroid(vtkPolyData *pd, double c[])
//{
// 	//VERIFY(pd->GetNumberOfLines() == 1);
//
//	// only treat the first line
// 
// 	int i;
// 	int npts, *pts;
// 	double tempPoint[3];
// 	vtkCellArray* lines = pd->GetLines();
// 	lines->InitTraversal();
// 	VERIFY (lines->GetNextCell(npts, pts));
// 	if (pts[0] == pts[npts - 1]) npts--;
// 
// 	c[0] = 0.0f; c[1] = 0.0f; c[2] = 0.0f;
// 
// 	for (i = 0; i < npts; i++) 
//	{
// 		pd->GetPoints()->GetPoint(pts[i], tempPoint);
// 		c[0] += tempPoint[0];
// 		c[1] += tempPoint[1];
// 		c[2] += tempPoint[2];
// 	}
//
//	c[0] /= static_cast<double>(npts);
//	c[1] /= static_cast<double>(npts);
//	c[2] /= static_cast<double>(npts);
//
//}
//
//
void StaticFunctionLibrary::DetermineOutwardNormal(vtkPolyData * InputContour, const int PointNo, double OutputNormal[], int LineNumber)
{
	int i;
	// outward normat at InputContour->GetPoint(pts[i])
	vtkCellArray* line = InputContour->GetLines();
	int npts, *pts;
	line->InitTraversal();

	for (i = 0; i < LineNumber + 1; i++)
	{
		line->GetNextCell(npts, pts);
	}

	if (pts[npts-1] == pts[0]) npts--;

	// PointNo refers to the order of the vertices in the cell,
	// not the actual point number in the vtkPolyData file, which 
	// should be referred to as pts[PointNo]

	double lastPoint[3], currentPoint[3], nextPoint[3];

	InputContour->GetPoint(pts[(PointNo-1+npts)%npts], lastPoint);
	InputContour->GetPoint(pts[PointNo], currentPoint);
	InputContour->GetPoint(pts[(PointNo+1+npts)%npts], nextPoint);

	double di_1[3] = {currentPoint[0] - lastPoint[0],
		currentPoint[1] - lastPoint[1], currentPoint[2] - lastPoint[2]};
	double di[3] = {nextPoint[0] - currentPoint[0],
		nextPoint[1] - currentPoint[1], nextPoint[2] - currentPoint[2]};
	vtkMath::Normalize(di_1);
	vtkMath::Normalize(di);
	double tangent[3] ={di_1[0] + di[0], di_1[1] + di[1], di_1[2] + di[2]};

	vtkMath::Normalize(tangent);

	// Rotate 90 degrees
	double P1[3], P2[3];
	InputContour->GetPoint(pts[(PointNo+npts/3)%npts], P1);
	InputContour->GetPoint(pts[(PointNo+2*npts/3)%npts], P2);
	double omega[3]; 
	double P0P1[3] = {P1[0] - currentPoint[0], P1[1] - currentPoint[1],
		P1[2] - currentPoint[2]};
	double P0P2[3] = {P2[0] - currentPoint[0], P2[1] - currentPoint[1],
		P2[2] - currentPoint[2]};
	vtkMath::Cross(P0P1, P0P2, omega);
	vtkMath::Normalize(omega);
	
	vtkMath::Cross(tangent,omega,OutputNormal);
	vtkMath::Normalize(OutputNormal);

}

////int StaticFunctionLibrary::SameOrientation(vtkPolyData *p1, vtkPolyData *p2)
////{
////	// Test whether the two input contours are of the same orientation.
////	// Input: two contour slices, which means p1 and p2 only have lines, 
////	// which are an one-element cell array.
////
////	int *pts1, n1;
////	int *pts2, n2;
////
////	vtkCellArray* tempCellArray1 = p1->GetLines();
////	vtkCellArray* tempCellArray2 = p2->GetLines();
////	tempCellArray1->InitTraversal();
////	tempCellArray2->InitTraversal();
////	tempCellArray1->GetNextCell(n1, pts1);
////	tempCellArray2->GetNextCell(n2, pts2);
////	if (pts1[0] == pts1[n1 - 1]) n1--;
////	if (pts2[0] == pts2[n2 - 1]) n2--;
////
////	float AreaP1[3], AreaP2[3];
////	MyMassProperties::PolygonAreaVector(p1->GetPoints(), n1, pts1, AreaP1);
////	MyMassProperties::PolygonAreaVector(p2->GetPoints(), n2, pts2, AreaP2);
////
////	float dotProd;
////	if ((dotProd = vtkMath::Dot(AreaP1, AreaP2)) > 0) 
////		return 1;
////	else
////		return 0;
////
////}
//
//double StaticFunctionLibrary::ArcLength(vtkPolyData * p)
//{
//	//assume p has only one lines cell
//	vtkCellArray* tempCellArray = p->GetLines();
//	tempCellArray->InitTraversal();
//	int npts, *pts;
//	tempCellArray->GetNextCell(npts, pts);// Get the first and the only cell
//
//	if (pts[0] == pts[npts-1]){ 
//		npts--;}
//
//	int i = 0; 
//	double firstPoint[3], secondPoint[3];
//	double sum = 0;
//	for (i = 0; i < npts; i++) 
//	{
//		p->GetPoints()->GetPoint(pts[i], firstPoint);
//		p->GetPoints()->GetPoint(pts[(i+1)%npts], secondPoint);
//
//		sum += sqrt(vtkMath::Distance2BetweenPoints(firstPoint, secondPoint));
//	}
//
//	return sum;
//}
//
//void StaticFunctionLibrary::ResampleByAngle(vtkPolyData *input, vtkPolyData *output, 
//											double Centroid[3], int AngSample)
//{
//	// ASSUMPTIONS: (1) z-component of curve is constant;
//	// (2) Centroid is inside the curve input 
//	
//	//test
//	////CRotateDlg::WritePolyData(input, "C:\\input.vtk");
//
//	// March 7 2005
//	// The purpose of defining this function is to 
//	// sample the wall contour by angle before 
//	// the correspondent algorithm is executed.
//	
//	// input: vtkPolyData with one closed contour
//	// output: vtkPolyData with contour sampled in x->y direction
//		
//	vtkPoints* OutputPoints = vtkPoints::New();
//	vtkCellArray* OutputLines = vtkCellArray::New();
//	output->SetPoints(OutputPoints);
//	output->SetLines(OutputLines);
//	OutputPoints->Delete();
//	OutputLines->Delete();
//		
//	// STEP 1: Make sure input have one closed contour
//	VERIFY (StaticFunctionLibrary::CountNumberOfClosedCurve(input) == 1);
//	
//	// STEP 2: Find out the maximum radius of the curve
//	vtkCellArray* InputLine = input->GetLines();
//	InputLine->InitTraversal();
//	int npts, *pts;
//	InputLine->GetNextCell(npts, pts);
//	if (pts[0] == pts[npts-1]) npts--;
//	
//	double tempPoint[3];
//	double radius2;
//	float maxradius = -VTK_LARGE_FLOAT;
//
//	for (int i = 0; i < npts; i++) {
//		input->GetPoint(pts[i], tempPoint);
//		radius2 = vtkMath::Distance2BetweenPoints(Centroid, tempPoint);
//		if (sqrt(radius2) > maxradius){
//			maxradius = sqrt(radius2);
//		}
//	}
//	
//	// the ray then will have length 2*maxradius
//	// idx is offset by input->GetNumberOfVerts() because we are getting the first line cell.
//	vtkPolyLine* InputPolyLine = vtkPolyLine::SafeDownCast(input->GetCell(input->GetNumberOfVerts()));
//
//	double endPoint[3];
//	float theta;
//	float pi = vtkMath::Pi();
//	double tol = 0.01;
//	vtkPoints* outputP = vtkPoints::New();
//
//	int numIPts;
//
//	if (InputPolyLine) 
//	{
//		for (i = 0; i < AngSample; i++) 
//		{
//			outputP->Initialize();
//			theta = (float)(i)/(float)(AngSample)*2*pi;
//			endPoint[0] = Centroid[0] + 2*maxradius*cos(theta);
//			endPoint[1] = Centroid[1] + 2*maxradius*sin(theta);
//			endPoint[2] = Centroid[2];
//			vtkIdList* subIdArray = vtkIdList::New();
//			numIPts = StaticFunctionLibrary::FindAllIntersection\
//				(InputPolyLine, outputP, subIdArray, Centroid, endPoint, tol);
//
//			if (!(numIPts>0))
//			{
//				//kill the program
//				//CRotateDlg::CustomMessageBox("Got 0 intersection in StaticFunctionLibrary::ResampleByAngle()!");
//				VERIFY(0);
//			}
//
//			subIdArray->Delete();
//			OutputPoints->InsertPoint(i, outputP->GetPoint(0));
//		}
//	}
//	else 
//	{
//		VERIFY(FALSE);
//	}
//
//	int numpts = OutputPoints->GetNumberOfPoints();
//	OutputLines->InsertNextCell(numpts + 1);
//	
//	for (i = 0; i < numpts; i++) 
//	{
//		OutputLines->InsertCellPoint(i);
//	}
//	OutputLines->InsertCellPoint(0);
//
//	//delete
//	outputP->Delete();
//}
//
////void StaticFunctionLibrary::ResampleByAngle2(vtkPolyData *input, vtkPolyData *output, 
////											 float Centroid[], int AngSample)
////{
////	//Another implementation of the previous function
////	// input: vtkPolyData with one closed contour
////	// output: vtkPolyData with contour sampled in x->y direction
////		
////	vtkPoints* OutputPoints = vtkPoints::New();
////	vtkCellArray* OutputLines = vtkCellArray::New();
////	output->SetPoints(OutputPoints);
////	output->SetLines(OutputLines);
////	OutputPoints->Delete();
////	OutputLines->Delete();
////		
////	// STEP 1: Make sure input have one closed contour
////	VERIFY (StaticFunctionLibrary::CountNumberOfClosedCurve(input) == 1);
////	
////	// STEP 2: Find out the maximum radius of the curve
////	vtkCellArray* InputLine = input->GetLines();
////	InputLine->InitTraversal();
////	int npts, *pts;
////	InputLine->GetNextCell(npts, pts);
////	if (pts[0] == pts[npts-1]) npts--;
////	
////	float tempPoint[3];
////	float radius2;
////	float maxRadius = -VTK_LARGE_FLOAT;
////
////	for (int i = 0; i < npts; i++) 
////	{
////		input->GetPoint(pts[i], tempPoint);
////		radius2 = vtkMath::Distance2BetweenPoints(Centroid, tempPoint);
////		if (sqrt(radius2) > maxRadius)
////		{
////			maxRadius = sqrt(radius2);
////		}
////	}
////
////	vtkPolyData* Circle = vtkPolyData::New();
////	vtkPoints* CirclePoints = vtkPoints::New();
////	CirclePoints->SetNumberOfPoints(AngSample);
////	vtkCellArray* CircleLines = vtkCellArray::New();
////	Circle->SetPoints(CirclePoints);
////	Circle->SetLines(CircleLines);
////	CirclePoints->Delete();
////	CircleLines->Delete();
////
////	CircleLines->InsertNextCell(AngSample+1);
////
////	float xtemp, ytemp, ztemp;
////
////	float marginRadius = 1.5*maxRadius;
////
////	for (i = 0; i < AngSample; i++)
////	{
////		//xtemp = Centroid[0] + maxRadius*cos(2*vtkMath::Pi()*(float) i/(float) AngSample);
////		//ytemp = Centroid[1] + maxRadius*sin(2*vtkMath::Pi()*(float) i/(float) AngSample);
////		
////		xtemp = Centroid[0] + marginRadius*cos(2*vtkMath::Pi()*(float) i/(float) AngSample);
////		ytemp = Centroid[1] + marginRadius*sin(2*vtkMath::Pi()*(float) i/(float) AngSample);
////		
////		ztemp = Centroid[2]; 
////
////		CirclePoints->SetPoint(i, xtemp, ytemp, ztemp);
////		CircleLines->InsertCellPoint(i);
////	}
////	CircleLines->InsertCellPoint(0);
////
////
////
////	OldSymmetricCorrespondence* sc = OldSymmetricCorrespondence::New();
////	sc->SetContour1(Circle);
////	if (!StaticFunctionLibrary::SameOrientation(Circle, input))
////	{
////		// have verified above that it only contains one line.
////		input->GetLines()->ReverseCell(0);
////	}//CRotateDlg::WritePolyData(Circle, "C:\\Circle.vtk");
////	//CRotateDlg::WritePolyData(input, "C:\\input.vtk");
////	sc->SetContour2(input);
////	sc->FindCorrespondence();
////
////	//test
////	/*
////	vtkPolyData* pd = vtkPolyData::New();
////	vtkPoints* pdPoints = vtkPoints::New();
////	vtkCellArray* pdLines = vtkCellArray::New();
////	pd->SetPoints(pdPoints);
////	pd->SetLines(pdLines);
////	pdPoints->Delete();
////	pdLines->Delete();
////
////	int PointId;
////	float point1[3], point2[3];
////
////	for (i = 0; i < sc->GetPoints1()->GetNumberOfPoints(); i++)
////	{
////		pdLines->InsertNextCell(2);
////		sc->GetPoints1()->GetPoint(i, point1);
////		PointId = pdPoints->InsertNextPoint(point1);
////		pdLines->InsertCellPoint(PointId);
////		sc->GetPoints2()->GetPoint(i, point2);
////		PointId = pdPoints->InsertNextPoint(point2);
////		pdLines->InsertCellPoint(PointId);
////	}
////
////	//CRotateDlg::WritePolyData(pd, "C:\\pd.vtk");
////	//endtest*/
////
////	OutputPoints->DeepCopy(sc->GetPoints2());
////	OutputLines->DeepCopy(CircleLines);
////
////	CRotateDlg::WritePolyData(output, "C:\\output.vtk");
////
////	//delete
////	Circle->Delete();
////	sc->Delete();
////
////}
//
//void StaticFunctionLibrary::ResampleByArcLength(vtkPolyData *curvein, vtkPolyData *curveout, 
//												int NumberOfSamples)
//{
//	int i, j;
//	float d;
//	int npts, *pts;
//	curvein->GetLines()->InitTraversal();
//	curvein->GetLines()->GetNextCell(npts, pts);
//	if (pts[0] == pts[npts-1]) npts--;
//	
//	float residue = 0;
//	double firstPoint[3], secondPoint[3];
//	vtkPoints* ResampledPoints = vtkPoints::New();
//	vtkCellArray* ResampledCellArray = vtkCellArray::New();
//	ResampledPoints->InsertPoint(0,curvein->GetPoints()->GetPoint(pts[0]));
//	float resolution = StaticFunctionLibrary::ArcLength(curvein)/NumberOfSamples;
//	double uvecFirstToSecond[3], Point[3];
//	
//	int count = 1;
//	int NumberOfSegments;
//	
//	for (i = 0; i < npts; i++) 
//	{
//		curvein->GetPoints()->GetPoint(pts[i], firstPoint);
//		curvein->GetPoints()->GetPoint(pts[(i+1)%npts], secondPoint);
//		
//		uvecFirstToSecond[0] = secondPoint[0] - firstPoint[0];
//		uvecFirstToSecond[1] = secondPoint[1] - firstPoint[1];
//		uvecFirstToSecond[2] = secondPoint[2] - firstPoint[2];
//		d = vtkMath::Normalize(uvecFirstToSecond);
//		
//		NumberOfSegments = floor((d + residue)/resolution);
//		for (j = 1; j <= NumberOfSegments; j++)
//		{
//			Point[0] = firstPoint[0] + (j*resolution - residue)*uvecFirstToSecond[0];
//			Point[1] = firstPoint[1] + (j*resolution - residue)*uvecFirstToSecond[1];
//			Point[2] = firstPoint[2] + (j*resolution - residue)*uvecFirstToSecond[2];
//			if (sqrt(vtkMath::Distance2BetweenPoints(Point, ResampledPoints->GetPoint(0))) > resolution/100)
//			{
//				ResampledPoints->InsertPoint(count, Point);
//				count++;
//			}
//		}
//		
//		residue = (d + residue) - NumberOfSegments * resolution;
//	}
//	
//	ResampledCellArray->InsertNextCell(count + 1);
//	for (i = 0; i < count; i++) 
//	{
//		ResampledCellArray->InsertCellPoint(i);
//	}
//	
//	ResampledCellArray->InsertCellPoint(0);
//
//	VERIFY(count == NumberOfSamples);
//	
//	curveout->SetPoints(ResampledPoints);
//	curveout->SetLines(ResampledCellArray);
//	ResampledPoints->Delete();
//	ResampledCellArray->Delete();
//}
//
int StaticFunctionLibrary::CountNumberOfClosedCurve(vtkPolyData *pdin, vtkPolyData* pdout, int eliminateClosedCurveWithSmallArea)
{
	// input: pdin -- don't touch
	//        pdout -- pdin with non-closed curve removed. 
	// output: returns the number of closed curve
	
	vtkIdList* Ids = vtkIdList::New();
	vtkCellArray* pdline = pdin->GetLines();

	int npts, *pts, i;
	int NumberOfClosedCurve = 0;
	//pdline->InitTraversal(); 
	
	double distBPoint;
	double delta = 0.005;
	double closedCurveArea;
	double xtemp[3], ytemp[3];
	
	////CRotateDlg::WritePolyData(pd, "C:\\featureedges.vtk");

	for (i = 0, pdline->InitTraversal(); i < pdline->GetNumberOfCells(); i++) 
	{
		pdline->GetNextCell(npts, pts);

		//test whether curve is a closed curve
		pdin->GetPoints()->GetPoint(pts[0], xtemp);
		pdin->GetPoints()->GetPoint(pts[npts-1], ytemp);
		distBPoint = vtkMath::Distance2BetweenPoints(xtemp, ytemp);
		
		if ((distBPoint > delta*delta)||
			(eliminateClosedCurveWithSmallArea &&
			(closedCurveArea = StaticFunctionLibrary::PolygonArea(pdin->GetPoints(), npts, pts)) < delta)
			) 
		{
			Ids->InsertNextId(i);						
		}
		else
		{
			NumberOfClosedCurve++;
		}
	}

	int IntArrayTraversal = 0;
	vtkIdType s;

	pdout->DeepCopy(pdin);

	if (Ids->GetNumberOfIds() == 0) 
	{
		Ids->Delete();
		return NumberOfClosedCurve;
		// no change to pd, just report the number of closed curve in it.
	}
	else 
	{
		vtkCellArray* NewLines = vtkCellArray::New();
		pdline->InitTraversal();
		s = Ids->GetId(0);
		for (i = 0; i < pdline->GetNumberOfCells(); i++) {
			pdline->GetNextCell(npts, pts);
			if (i != s) 
			{
				NewLines->InsertNextCell(npts, pts);
			}
			else 
			{
				if (IntArrayTraversal + 1 < Ids->GetNumberOfIds()) 
				{
					IntArrayTraversal++;
					s = Ids->GetId(IntArrayTraversal);
				}
				//else do nothing, let the cellarray fills up
			}
		}
		pdout->SetLines(NewLines);
		NewLines->Delete();

		Ids->Delete();
		return NewLines->GetNumberOfCells();
	}
}
//
//float StaticFunctionLibrary::Mean(float *DataSet, int Size)
//{
//	float mean = 0;
//	
//	for (int i = 0; i < Size; i++)
//	{
//		mean += DataSet[i];
//	}
//
//	mean /= Size;
//
//	return mean;
//}
//
//float StaticFunctionLibrary::Variance(float *DataSet, int Size)
//{
//	float sum = 0, thisDiff;
//	
//	float mean = StaticFunctionLibrary::Mean(DataSet, Size);
//
//	for (int i = 0; i < Size; i++)
//	{
//		thisDiff = DataSet[i] - mean;
//		sum += thisDiff*thisDiff;
//	}
//
//	return sum/(Size - 1);
//
//}
//
//void StaticFunctionLibrary::DetermineReferenceFrame(float z[], float &phi, float &theta, float &psi)
//{
//	// input: z -- must be normalized
//	// output: theta, psi -- the angle we need to rotate the z axis of the original reference
//	// frame in order to align it to the z input. (see comment below)
//
//	// see 3Drotation.doc in math toolbox
//	// omega = [-sin(theta); cos(theta)sin(psi); cos(theta)cos(psi)]
//	// solve for theta and psi. Obviously, there are two set of solutions if 
//	// the domain of theta ranges from 0 to 2pi. Given a particular value of phi,
//	// these two solutions gives two different reference frames, in which the x and y 
//	// axis in one set point to the opposite direction of their correspondence in 
//	// another set. Thus, we can arbitrarily choose one solution. 
//
//	// for resamplebyangle(), we need the x and y reference to be consistent, which means
//	// there is a one-to-one mapping of the reference frame and the z to be aligned. 
//	// Therefore, we choose 
//	// phi = 0;
//	// theta = in the range of -pi/2 to pi/2.
//	// psi is unique after phi and theta are defined.
//
//	phi = 0;
//	double pi = vtkMath::Pi();
//	theta = asin(-z[0]);
//	if ((z[2] == 0) && (z[1] == 0)) 
//		psi = 0;
//	else if ((z[2] == 0) && (z[1] != 0))
//		psi = pi/2;
//	else 
//		psi = atan2(z[1],z[2]);
//
//}
//
//void StaticFunctionLibrary::DetermineReferenceFrame2(float z[3], float Pt[3], vtkMatrix4x4 *matrix)
//{
//	float x[3];
//	float y[3];
//
//	if (z[2] != 0)
//	{	
//		x[0] = 1; 
//		x[1] = 0;
//		x[2] = -z[0]/z[2];
//
//		if (z[0]*z[0] < z[2]*z[2])
//		{
//			x[0] *= -1;
//			x[1] *= -1;
//			x[2] *= -1;		
//		}
//	}
//	else
//	{
//		x[0] = 0;
//		x[1] = 0;
//		x[2] = -1;
//	}
//
//	vtkMath::Normalize(z);
//	vtkMath::Normalize(x);
//	vtkMath::Cross(z, x, y);
//
//	matrix->Identity();
//	
//	int j;
//	
//	for (j = 0; j < 3; j++)
//	{
//		matrix->SetElement(j, 0, x[j]);
//		matrix->SetElement(j, 1, y[j]);
//		matrix->SetElement(j, 2, z[j]);
//		matrix->SetElement(j, 3, Pt[j]);
//	}
//	
//}
//
//
//void StaticFunctionLibrary::TransformRotate(vtkPolyData *input, vtkPolyData *output, float phi, float theta, float psi, int forward)
//{
//	// also copy all the point attributes
//
//	float tempPoint[3];
//	float thisPoint[3];
//
//	output->DeepCopy(input);
//	
//	vtkPoints* outputP = vtkPoints::New();
//	output->SetPoints(outputP);
//	outputP->Delete();
//	int npts = input->GetPoints()->GetNumberOfPoints();
//	outputP->SetNumberOfPoints(npts);
//
//	for (int i = 0; i < npts; i++) {
//		input->GetPoints()->GetPoint(i, tempPoint);
//		StaticFunctionLibrary::HelperTransformRotate(tempPoint, thisPoint, phi, theta, psi, forward);
//		outputP->SetPoint(i, thisPoint);
//	}
//
//}
//
//void StaticFunctionLibrary::HelperTransformRotate(const float input[], float output[], float phi, float theta, float psi, int forward)
//{
//	float a11final = cos(theta)*cos(phi);
//	float a12 = cos(theta)*sin(phi);
//	float a13 = -sin(theta);
//	float a21 = sin(psi)*sin(theta)*cos(phi)-cos(psi)*sin(phi);
//	float a22final = sin(psi)*sin(theta)*sin(phi)+cos(psi)*cos(phi);
//	float a23 = cos(theta)*sin(psi);
//	float a31 = cos(psi)*sin(theta)*cos(phi)+sin(psi)*sin(phi);
//	float a32 = cos(psi)*sin(theta)*sin(phi)-sin(psi)*cos(phi);
//	float a33final = cos(theta)*cos(psi);
//
//	float a12final, a13final, a21final, a23final, a31final, a32final;
//	a12final = (forward)?a12:a21;
//	a13final = (forward)?a13:a31;
//	a21final = (forward)?a21:a12;
//	a23final = (forward)?a23:a32;
//	a31final = (forward)?a31:a13;
//	a32final = (forward)?a32:a23;
//
//	output[0] = a11final*input[0] + a12final*input[1] + a13final*input[2];
//	output[1] = a21final*input[0] + a22final*input[1] + a23final*input[2];
//	output[2] = a31final*input[0] + a32final*input[1] + a33final*input[2];
//
//}
//
//void StaticFunctionLibrary::FindNormal(vtkPolyData *OneSlice, float normal[])
//{
//	// OneSlice is assumed to have exactly one line;
//	// The direction of the normal is in the same direction as the curl of the contour
//
//	float C1_01[3] = {0, 0, 0};
//	float C1_02[3] = {0, 0, 0};
//
//	int *pts1, n1;
//
//	vtkCellArray* tempCellArray1 = OneSlice->GetLines();
//	vtkCellArray* tempCellArray2 = OneSlice->GetPolys();
//
//	vtkPolyData* IntermediatePolyData = vtkPolyData::New();
//
//	if (tempCellArray1->GetNumberOfCells() == 0 &&
//		tempCellArray2->GetNumberOfCells() > 0)
//	{
//		StaticFunctionLibrary::PolygonsToLines(OneSlice, IntermediatePolyData);	
//		tempCellArray1 = IntermediatePolyData->GetLines();
//	}
//
//	tempCellArray1->InitTraversal();
//	tempCellArray1->GetNextCell(n1, pts1);
//	if (pts1[0] == pts1[n1 - 1]) n1--;
//	
//	float C1point1[3], C1point2[3], C1point3[3]; 
//	
//	OneSlice->GetPoints()->GetPoint(pts1[0], C1point1);
//	OneSlice->GetPoints()->GetPoint(pts1[n1/3], C1point2);
//	OneSlice->GetPoints()->GetPoint(pts1[2*n1/3], C1point3);
//
//	C1_01[0] = C1point2[0] - C1point1[0];
//	C1_01[1] = C1point2[1] - C1point1[1];
//	C1_01[2] = C1point2[2] - C1point1[2];
//
//	C1_02[0] = C1point3[0] - C1point1[0];
//	C1_02[1] = C1point3[1] - C1point1[1];
//	C1_02[2] = C1point3[2] - C1point1[2];
//	
//	vtkMath::Cross(C1_01, C1_02, normal);
//
//	IntermediatePolyData->Delete();
//}
//
//int StaticFunctionLibrary::CurveWithLargestArea(vtkPolyData **Curves, vtkIdList* idList)
//{
//	int i;
//	float maxArea = -VTK_LARGE_FLOAT;
//	float thisArea;
//	int LargestAreaIndex = 0;
//	int npts, *pts;
//	int thisId;
//
//	vtkCellArray* CurveLines;
//	for (i = 0; i < idList->GetNumberOfIds(); i++)
//	{
//		thisId = idList->GetId(i);
//		CurveLines = Curves[thisId]->GetLines();
//		CurveLines->InitTraversal();
//		CurveLines->GetNextCell(npts, pts);
//
//		thisArea = MyMassProperties::PolygonArea(Curves[thisId]->GetPoints(), npts, pts);
//		
//		if (thisArea > maxArea)
//		{
//			maxArea = thisArea;
//			LargestAreaIndex = thisId;
//		}
//	}
//
//	return LargestAreaIndex;
//}
//
//// See SectionAnalysis
////DEL void StaticFunctionLibrary::ConvertOneCurveToTwoCurves(vtkPolyData *OneCurve, vtkPolyData *TwoCurves, vtkPoints* CutLine)
////DEL {
////DEL 	//assumption: OneCurve has only one closed line
////DEL 	// will initialize TwoCurves
////DEL 
////DEL 	int i, j;
////DEL 	float tol = 0.01;
////DEL 
////DEL 	float tempPoint[3];
////DEL 	float startPoint[3], endPoint[3];
////DEL 	CutLine->GetPoint(0, startPoint);
////DEL 	CutLine->GetPoint(1, endPoint);
////DEL 
////DEL 	vtkPoints* intPoints = vtkPoints::New();
////DEL 	vtkIdList* subIdList = vtkIdList::New();
////DEL 	vtkPolyLine* OneCurveLine = vtkPolyLine::SafeDownCast(OneCurve->GetCell(OneCurve->GetNumberOfVerts()));
////DEL 
////DEL 	int numInt = StaticFunctionLibrary::FindAllIntersection
////DEL 		(OneCurveLine, intPoints, subIdList, startPoint, endPoint, tol);
////DEL 
////DEL 	if (numInt != 2)
////DEL 	{
////DEL 		CRotateDlg::CustomMessageBox("Fatal Error: MeanSurfaceFilter::ConvertOneCurveToTwoCurves()");
////DEL 		VERIFY(0);
////DEL 	}
////DEL 
////DEL 	int subId1, subId2;
////DEL 	subId1 = subIdList->GetId(0);
////DEL 	subId2 = subIdList->GetId(1);
////DEL 	int npoints = OneCurveLine->GetPointIds()->GetNumberOfIds();
////DEL 	float x1[3], x2[3];
////DEL 	intPoints->GetPoint(0, x1);
////DEL 	intPoints->GetPoint(1, x2);
////DEL 
////DEL 	//check whether OneCurveLine has duplicated points
////DEL 	if (OneCurveLine->GetPointId(0) == OneCurveLine->GetPointId(npoints - 1))
////DEL 	{
////DEL 		npoints--;
////DEL 	}
////DEL 
////DEL 	TwoCurves->Initialize();
////DEL 	vtkCellArray* TwoCurvesCA = vtkCellArray::New();
////DEL 	vtkPoints* TwoCurvesP = vtkPoints::New();
////DEL 	TwoCurves->SetLines(TwoCurvesCA);
////DEL 	TwoCurves->SetPoints(TwoCurvesP);
////DEL 	TwoCurvesCA->Delete();
////DEL 	TwoCurvesP->Delete();
////DEL 
////DEL 	TwoCurvesCA->InsertNextCell(((subId1 - subId2 + npoints)%npoints)+3);
////DEL 	
////DEL 	for (i = 0; i < (subId1 - subId2 + npoints)%npoints ; i++) 
////DEL 	{
////DEL 		OneCurveLine->GetPoints()->GetPoint((subId2 + 1 + i)%npoints, tempPoint);
////DEL 		TwoCurvesP->InsertNextPoint(tempPoint);
////DEL 		TwoCurvesCA->InsertCellPoint(i);
////DEL 	}
////DEL 	
////DEL 	TwoCurvesP->InsertNextPoint(x1);
////DEL 	TwoCurvesCA->InsertCellPoint(i);
////DEL 	TwoCurvesP->InsertNextPoint(x2);
////DEL 	TwoCurvesCA->InsertCellPoint(i+1);
////DEL 	TwoCurvesCA->InsertCellPoint(0);
////DEL 
////DEL 	int countPoint = i + 2;
////DEL 
////DEL 	TwoCurvesCA->InsertNextCell(((subId2 - subId1 + npoints)%npoints)+3);
////DEL 	
////DEL 	for (j = 0; j < (subId2 - subId1 + npoints)%npoints; j++) 
////DEL 	{
////DEL 		OneCurveLine->GetPoints()->GetPoint((subId1 + 1 + j)%npoints, tempPoint);
////DEL 		TwoCurvesP->InsertNextPoint(tempPoint);
////DEL 		TwoCurvesCA->InsertCellPoint(countPoint + j);
////DEL 	}
////DEL 	
////DEL 	TwoCurvesP->InsertNextPoint(x2);
////DEL 	TwoCurvesCA->InsertCellPoint(j + countPoint);
////DEL 	TwoCurvesP->InsertNextPoint(x1);
////DEL 	TwoCurvesCA->InsertCellPoint(j + 1 + countPoint);
////DEL 	TwoCurvesCA->InsertCellPoint(countPoint);
////DEL 
////DEL 
////DEL 	//delete 
////DEL 	intPoints->Delete();
////DEL 	subIdList->Delete();
////DEL 
////DEL }
//
////DEL void StaticFunctionLibrary::PolygonAreaVector(vtkPoints *inPts, vtkIdType npts, vtkIdType *pts, float Area[])
////DEL {
////DEL 	// pts should contain the id of points according to the order of inPts
////DEL 	if (pts[0] == pts[npts - 1]) npts--;
////DEL 	//remove duplicate points
////DEL 
////DEL 	int i;
////DEL 	for (i = 0; i < 3; i++)
////DEL 	{
////DEL 		Area[i] = 0.0f;
////DEL 	}
////DEL 
////DEL 	float crossi[3];
////DEL 
////DEL 	for (i = 0; i < npts; i++) 
////DEL 	{
////DEL 		vtkMath::Cross(inPts->GetPoint(pts[i]), inPts->GetPoint(pts[(i+1)%npts]),crossi);
////DEL 		Area[0] += 0.5f*crossi[0];
////DEL 		Area[1] += 0.5f*crossi[1];
////DEL 		Area[2] += 0.5f*crossi[2];
////DEL 	}
////DEL }
//
//void StaticFunctionLibrary::FillFlattenedMap(vtkPolyData *ThicknessSlice, float z, 
// 	 int branchNumber, vtkPolyData* FlattenedMap)
//{
//	// common: branchNumber = 0;
// 	// x smaller: branchNumber = 1;
// 	// x larger: branchNumber = 2;
//
//	// General description: ThicknessSlice should be a contour with pointdata value. 
//	// This function fills ONE line of the flattened map with the pointdata value associated
//	// with this contour.
//	// branchNumber determines the angular shift (see the variable offset)
// 
// 	// Inputs: ThicknessSlice -- the slice generated using this->ComputeThicknessForOneSlice
// 	//         z -- the z level of the slice
// 	//		   branchNumber -- (see above)
// 	// Effect: Collect the information contained in ThicknessSlice and use it to fill the flattened map
// 
//	// (Feb 1 2006: Try to design a new algorithm to flatten the thickness map after
//	// journal club discussion)
//
//	// Inputs: (in addition to 
//
//
//
// 	int i, j;	
//	vtkPointData* ThicknessSlicePointData = ThicknessSlice->GetPointData();
// 	vtkPointData* FlattenedMapPointData = FlattenedMap->GetPointData();
//	int NumberOfFlattenedMapArrays = FlattenedMapPointData->GetNumberOfArrays();
//	int NumberOfThicknessSliceArrays = ThicknessSlicePointData->GetNumberOfArrays();
//	
//	//see comments below:
//	int NumberOfArrays = (NumberOfFlattenedMapArrays > 0)?\
//		NumberOfFlattenedMapArrays:NumberOfThicknessSliceArrays;
//
//	vtkPoints* FlattenedMapPoints = FlattenedMap->GetPoints();
// 	vtkCellArray* FlattenedMapPolys = FlattenedMap->GetPolys();
// 	vtkDataArray** FlattenedMapData = new vtkDataArray* [NumberOfArrays];
// 	vtkDataArray** ThicknessSliceData = new vtkDataArray* [NumberOfArrays];
// 	
//	int NumberOfPoints = ThicknessSlice->GetNumberOfPoints();
// 
// 	if (!FlattenedMapPoints)
// 	{
// 		FlattenedMapPoints = vtkPoints::New();
// 		FlattenedMap->SetPoints(FlattenedMapPoints);
// 		FlattenedMapPoints->Delete();
// 	}
// 
// 	//if (!FlattenedMapPolys)//FlattenedMapPolys = FlattenedMap->Dummy if Polys == NULL
// 	//if (!FlattenedMap->GetNumberOfPolys())
// 	//{
// 	//	FlattenedMapPolys = vtkCellArray::New();
// 	//	FlattenedMap->SetPolys(FlattenedMapPolys);
// 	//	FlattenedMapPolys->Delete();
// 	//}
// 	// changed positions
// 
// 	//int ArrayIndex;
//	const char* ArrayName;
//	vtkDataArray* ThicknessSliceDataArray;
// 
//	// Think of it this way:
//	// (1) if there is no array in FlattenedMapPointData, copy arrays from
//	// ThicknessMapPointData to FlattenedMapPointData. The order of the pointdata arrays
//	// follow that in ThicknessMapPointData
//	// (2) if there exist arrays in FlattenedMapPointData, go through the array name in
//	// FlattenedMapPointData. If the retrived name matches any array name in
//	// ThicknessMapPointData, assign ThicknessSliceData[i]
//
//	if (NumberOfFlattenedMapArrays == 0)
//	{
//		for (i = 0; i < NumberOfThicknessSliceArrays; i++)
//		{
//			ArrayName = ThicknessSlicePointData->GetArrayName(i);
//			FlattenedMapData[i] = ThicknessSlicePointData->GetArray(i)->NewInstance();
// 			FlattenedMapData[i]->SetName(ArrayName);
// 			FlattenedMapData[i]->SetNumberOfComponents(1);
//			if (ThicknessSlicePointData->IsArrayAnAttribute(i) == vtkPointData::SCALARS)
// 			{
// 				FlattenedMapPointData->SetScalars(FlattenedMapData[i]);
// 			}
// 			else
// 			{
// 				FlattenedMapPointData->AddArray(FlattenedMapData[i]);
// 			}
// 			FlattenedMapData[i]->Delete();
//			
//			ThicknessSliceData[i] = ThicknessSlicePointData->GetArray(ArrayName);
//			VERIFY(ThicknessSliceData[i]);
//		}
//	}
//	else
//	{
//		for (i = 0; i < NumberOfFlattenedMapArrays; i++)
//		{
//			ArrayName = FlattenedMapPointData->GetArrayName(i);
//			FlattenedMapData[i] = FlattenedMapPointData->GetArray(i);
//
//			if (ThicknessSliceDataArray = ThicknessSlicePointData->GetArray(ArrayName))
//			{
//				ThicknessSliceData[i] = ThicknessSliceDataArray;
//			}
//		}
//	}
//
// 	float radius = 4.0;
// 	float pi = vtkMath::Pi();
// 	float twoPiR = 2*pi*radius;
// 	float offset[3] = {-twoPiR/2.0, -twoPiR-1.0, 1.0};
// 	// offset = {offsetbranch0, offsetbranch1, offsetbranch2}
// 
// 	float pointi[3];
// 	int pointId;
// 	float tempScalar;
// 
// 	for (i = 0; i < NumberOfPoints; i++)
// 	{
// 		pointi[0] = offset[branchNumber] + 2*pi*radius*i/NumberOfPoints;
// 		pointi[1] = 0.0;
// 		pointi[2] = z;
// 		pointId = FlattenedMapPoints->InsertNextPoint(pointi);
// 		//point i at angle of 2*pi*i/NumberOfPoints;
// 
// 		for (j = 0; j < NumberOfArrays; j++)
// 		{
// 			tempScalar = ThicknessSliceData[j]->GetComponent(i, 0);
// 			FlattenedMapData[j]->InsertTuple(pointId, &tempScalar);
// 		}
// 	}
// 
// 	int npts = 4, pts[4];
// 	int initIndex = pointId - NumberOfPoints + 1;
// 
// 	if (initIndex >= NumberOfPoints)
// 	{
// 		//which means at least a row is inserted.
// 		// we need to connect the previous row with the current row.
// 		for (i = initIndex; i < pointId; i++)
// 		{
// 			pts[0] = i;
// 			pts[1] = i + 1;
// 			pts[2] = pts[1] - NumberOfPoints;
// 			pts[3] = pts[0] - NumberOfPoints;
// 			FlattenedMapPolys->InsertNextCell(npts, pts);
// 		}
// 	}
// 	else 
// 	{
// 		//first time into this function, initialize FlattenedMap->Polys
// 		FlattenedMapPolys = vtkCellArray::New();
// 		FlattenedMap->SetPolys(FlattenedMapPolys);
// 		FlattenedMapPolys->Delete();
// 	}
// 
// 	FlattenedMap->SetVerts(NULL);
// 	FlattenedMap->SetLines(NULL);
// 	FlattenedMap->SetStrips(NULL);
// 	//delete
// 	delete [] FlattenedMapData;
// 	delete [] ThicknessSliceData;
//}
//
//void StaticFunctionLibrary::ChangePolyLineToLine(vtkPolyData *in, vtkPolyData *out)
//{
//	int i, j;
//
//	out->DeepCopy(in);
//	vtkCellArray* outLines = vtkCellArray::New();
//	out->SetLines(outLines);
//	outLines->Delete();
//
//	vtkCellArray* Lines = in->GetLines();
//	Lines->InitTraversal();
//
//	int npts, *pts;
//	int insertPts[2];
//
//	for (i = 0; i < Lines->GetNumberOfCells(); i++)
//	{
//		Lines->GetNextCell(npts, pts);
//
//		if (npts > 2)
//		{
//			for (j = 0; j < (npts - 1); j++)
//			{
//				insertPts[0] = pts[j];
//				insertPts[1] = pts[j + 1];
//				outLines->InsertNextCell(2, insertPts);
//			}
//		}
//		else if (npts == 2)
//		{
//			outLines->InsertNextCell(npts, pts);
//		}
//		//otherwise do nothing
//
//	}
//
//}
//
//void StaticFunctionLibrary::ComputeAndInsertPointData(vtkPointData *inputPointData, vtkPointData *outputPointData, 
//													  int PtId1, int PtId2, float t, int Index)
//{
//	// This function retrieves data point values of PtId1 and PtId2 from inputPointData
//	// Calculate the interpolated value according to t
//	// insert the computed value into outputPointData(Index)
//
//	// Notice that this->CopyArraysFromInputPointData(InPointData, OutPointData) must have been called.
//
//	int i, j;
//	int numberOfArrays = outputPointData->GetNumberOfArrays();
//
//	if (numberOfArrays == 0) return;
//	
//	const char* ArrayName;
//	vtkDataArray* da, *Arrayi;
//	float* TuplePtId1, *TuplePtId2, *TupleIndex;
//	int numberOfComponentsda;
//
//	for (i = 0; i < numberOfArrays; i++)
//	{
//		Arrayi = outputPointData->GetArray(i);
//		ArrayName = outputPointData->GetArrayName(i);
//		da = inputPointData->GetArray(ArrayName);
//		numberOfComponentsda = da->GetNumberOfComponents();
//
//		TuplePtId1 = new float [numberOfComponentsda];
//		TuplePtId2 = new float [numberOfComponentsda];
//		TupleIndex = new float [numberOfComponentsda];
//
//		da->GetTuple(PtId1, TuplePtId1);
//		da->GetTuple(PtId2, TuplePtId2);
//
//		for (j = 0; j < numberOfComponentsda; j++)
//		{
//			TupleIndex[j] = (1 - t)*TuplePtId1[j] + t*TuplePtId2[j];
//		}
//
//		Arrayi->InsertTuple(Index, TupleIndex);
//
//		delete [] TuplePtId1;
//		delete [] TuplePtId2;
//		delete [] TupleIndex;
//	}
//}
//
//void StaticFunctionLibrary::CopyArraysFromInputPointData(vtkPointData *inputPointData, vtkPointData *outputPointData)
//{
//
//	// Notice that CopyArraysFromInputPointData(InPointData, OutPointData) must have been called.
//	// before calling StaticFunctionLibrary::ComputeAndInsertPointData
//	// This function only creates arrays that has the same name as inPointData, it will not 
//	// copy data.
//	
//	//  
//
//	int i;	
//	int numberOfArrays = inputPointData->GetNumberOfArrays();
//
//	if (numberOfArrays == 0) return;
//
//	vtkDataArray** outputDataArrays = new vtkDataArray*[numberOfArrays];
//	
//	const char* ArrayName;
//	vtkDataArray* da;
//	int whichAttribute, arrayIndex;
//
//	// Here we only want to match name
//
//	// Step 1: Remove those arrays that couldn't be found in inputPointData
//
//	for (i = 0; i < outputPointData->GetNumberOfArrays(); i++)
//	{
//		ArrayName = outputPointData->GetArrayName(i);
//		da = inputPointData->GetArray(ArrayName);
//		if (!da)
//		{
//			outputPointData->RemoveArray(ArrayName);
//		}
//	}
//
//	// Step 2: for each array at inputPointData -- either initialize it for outputPointData or not,
//	// depending on whether the array exists in outputPointData
//
//	vtkDataArray* da1;
//	//copy the arrays from inputPointData;
//	for (i = 0; i < numberOfArrays; i++)
//	{
//		ArrayName = inputPointData->GetArrayName(i);
//		da1 = outputPointData->GetArray(ArrayName, arrayIndex);
//
//		// if ArrayName is blank, there is no way that we can recognize the array later
//		// so don't add.
//
//		if (!da1 && ArrayName)
//		{
//			da = inputPointData->GetArray(i);
//			outputDataArrays[i] = da->NewInstance();
// 			outputDataArrays[i]->SetName(ArrayName);
// 			outputDataArrays[i]->SetNumberOfComponents(da->GetNumberOfComponents());
//
//			arrayIndex = outputPointData->AddArray(outputDataArrays[i]);
//			outputDataArrays[i]->Delete();
//		}
//
//		if ((whichAttribute = inputPointData->IsArrayAnAttribute(i)) >= 0)
//		{
//			outputPointData->SetActiveAttribute(arrayIndex, whichAttribute);
//		}
//	}
//
//	delete [] outputDataArrays;
//}
//
//void StaticFunctionLibrary::DetermineFittingLine(vtkPoints *DataPoints, float ReferencePoint[3], float Normal[3])
//
//{
//	// this function assumes DataPoints are 2D points, which means the function will
//	// ignore the third coordinate.
//	// Input: DataPoints -- the data points that we want to fit.
//	// Input: Length -- the length of the fitting line.
//	// Output: TwoPoints -- two points that define the best-of-fit line.
//
//	// The mathematical steps associated with this method is described in
//	// http://mathpages.com/home/kmath110.htm
//
//	int i;
//	int n = DataPoints->GetNumberOfPoints();
//	float Sumx_y = 0.0f;
//	float Sumxy = 0.0f;
//	float A;
//	float tempPoint[3];
//	float xi, yi;
//	float Xmean = 0.0f; 
//	float Ymean = 0.0f;
//	float Zmean = 0.0f;
//
//	for (i = 0; i < n; i++) {
//		DataPoints->GetPoint(i, tempPoint);
//		Xmean += tempPoint[0];
//		Ymean += tempPoint[1];
//		Zmean += tempPoint[2];
//	}
//
//	Xmean /= static_cast<float> (n);
//	Ymean /= static_cast<float> (n);
//	Zmean /= static_cast<float> (n);
//
//	for (i = 0; i < n; i++) {
//		DataPoints->GetPoint(i, tempPoint);
//		xi = tempPoint[0] - Xmean;
//		yi = tempPoint[1] - Ymean;
//		Sumx_y += xi*xi - yi*yi;
//		Sumxy += xi*yi;
//	}
//
//	A = Sumx_y/Sumxy;
//
//	// q is the rotation angle (anticlockwise) for which the line will be rotated.
//	// tan(q) is obtained by solving the following quadratic equation:
//	// tan(q)^2 + A tan(q) - 1 = 0;
//
//	float tan_q1, tan_q2;
//	tan_q1 = 0.5*(-A + sqrt(A*A + 4));
//	tan_q2 = 0.5*(-A - sqrt(A*A + 4));
//
//	double q1, q2;
//	q1 = atan(static_cast<double> (tan_q1));
//	q2 = atan(static_cast<double> (tan_q2));
//
//	float Sum_y_prime_2_q1 = 0.0f;
//	float Sum_y_prime_2_q2 = 0.0f;
//	float yi_prime_q1, yi_prime_q2;
//
//	for (i = 0; i < DataPoints->GetNumberOfPoints(); i++) {
//		DataPoints->GetPoint(i, tempPoint);
//		xi = tempPoint[0] - Xmean;
//		yi = tempPoint[1] - Ymean;
//		yi_prime_q1 = -xi*sin(q1) + yi*cos(q1);
//		yi_prime_q2 = -xi*sin(q2) + yi*cos(q2);
//		Sum_y_prime_2_q1 += yi_prime_q1*yi_prime_q1;
//		Sum_y_prime_2_q2 += yi_prime_q2*yi_prime_q2;
//	}
//
//	ReferencePoint[0] = Xmean;
//	ReferencePoint[1] = Ymean;
//	ReferencePoint[2] = Zmean;
//
//	Normal[2] = 0.0f;
//
//	if (Sum_y_prime_2_q1 < Sum_y_prime_2_q2) {
//		// then q1 is the solution
//		Normal[0] = cos(q1);
//		Normal[1] = sin(q1);
//	}
//	else {
//		// q2 is the solution
//		Normal[0] = cos(q2);
//		Normal[1] = sin(q2);
//	}
//}
//
//int StaticFunctionLibrary::NormalityTest(float *Data, int NumberOfSamples)
//{
//	//call matlab NormalityTest function inside the folder
//	// C:\\RotateMFC\\stat
//
//	Engine *ep;
//	if (!(ep = engOpen("\0")))
//	{
//		fprintf(stderr, "\nCan't start MATLAB engine\n");
//		return -1;
//	}
//
//	engEvalString(ep, "clear all");
//	
//	engEvalString(ep, "cd (fullfile('C:\\RotateMFC', 'stat'))");
//
//	double* Data_matlab = new double[NumberOfSamples];
//	for (int i = 0; i < NumberOfSamples; i++)
//	{
//		Data_matlab[i] = static_cast<double> (Data[i]);
//	}
//
//	mxArray* Data_matlab_mxArray = mxCreateDoubleMatrix(NumberOfSamples, 1, mxREAL);
//	// filling Data_matlab_mxArray column by column
//	memcpy((void*)mxGetPr(Data_matlab_mxArray), (void*)Data_matlab, NumberOfSamples*sizeof(double));
//	delete [] Data_matlab;
//
//	engPutVariable(ep, "Data_matlab_mxArray", Data_matlab_mxArray);
//
//	//mxArray* NumberOfSamples_mxArray = mxCreateDoubleMatrix(1, 1, mxREAL);
//	// The following line is a new discovery.
//	//mxGetPr(NumberOfSamples_mxArray)[0] = static_cast<double>(NumberOfSamples);
//	//engPutVariable(ep, "NumberOfSamples_mxArray", NumberOfSamples_mxArray);
//
//	engEvalString (ep, "NumberOfSamples = 1000");
//	engEvalString (ep, "bootstat = bootstrp(NumberOfSamples, 'mean', Data_matlab_mxArray)");
//	engEvalString (ep, "[h, p, lstat, cv] = Mylillietest(bootstat)");
//	//% h = 0 -- don't reject the normality hypothesis
//	engEvalString (ep, "IsNormal_mxArray = ~h");
//
//	//engEvalString(ep, "IsNormal_mxArray = NormalityTest(Data_matlab_mxArray)");
//	mxArray* IsNormal_mxArray = engGetVariable(ep, "IsNormal_mxArray");
//	
//	int IsNormal = static_cast<int>(mxGetPr(IsNormal_mxArray)[0]);
//
//	//delete
//	mxDestroyArray(Data_matlab_mxArray);
//	//mxDestroyArray(NumberOfSamples_mxArray);
//	mxDestroyArray(IsNormal_mxArray);
//
//	return IsNormal;
//
//}
//
//int StaticFunctionLibrary::BootstrapMeanTest(float *Data1, int size1, float *Data2, int size2, 
//											 float alpha, float &significance)
//{
//	// return 0 -- don't reject null hypothesis (difference not significant)
//	// return 1 -- reject null hypothesis (diffference significant)
//
//	Engine *ep;
//	if (!(ep = engOpen("\0")))
//	{
//		fprintf(stderr, "\nCan't start MATLAB engine\n");
//		return -1;
//	}
//
//	engEvalString(ep, "clear all");
//	
//	engEvalString(ep, "cd (fullfile('C:\\RotateMFC', 'stat'))");
//
//	// double array required when calling matlab
//	double* Data1_matlab = new double[size1];
//	double* Data2_matlab = new double[size2];
//
//	for (int i = 0; i < size1; i++)
//	{
//		Data1_matlab[i] = static_cast<double>(Data1[i]);
//	}
//	for (i = 0; i < size2; i++)
//	{
//		Data2_matlab[i] = static_cast<double>(Data2[i]);
//	}
//
//	mxArray* Data1_matlab_mxArray = mxCreateDoubleMatrix(size1, 1, mxREAL);
//	// filling Data1_matlab_mxArray column by column
//	memcpy((void*)mxGetPr(Data1_matlab_mxArray),(void*)Data1_matlab, size1*sizeof(double));
//	delete [] Data1_matlab;
//	engPutVariable(ep, "Data1_matlab_mxArray", Data1_matlab_mxArray);
//
//	mxArray* Data2_matlab_mxArray = mxCreateDoubleMatrix(size2, 1, mxREAL);
//	// filling Data2_matlab_mxArray column by column
//	memcpy((void*)mxGetPr(Data2_matlab_mxArray),(void*)Data2_matlab, size2*sizeof(double));
//	delete [] Data2_matlab;
//	engPutVariable(ep, "Data2_matlab_mxArray", Data2_matlab_mxArray);
//
//	mxArray* alpha_matlab_mxArray = mxCreateDoubleMatrix(1, 1, mxREAL);
//	mxGetPr(alpha_matlab_mxArray)[0] = alpha;
//	engPutVariable(ep, "alpha_matlab_mxArray", alpha_matlab_mxArray);
//
//	engEvalString(ep, "[h, significance] = bootstrapTestMean(Data1_matlab_mxArray, \
//		Data2_matlab_mxArray,alpha_matlab_mxArray)");
//
//	mxArray* h_mxArray = engGetVariable(ep, "h");
//	mxArray* significance_mxArray = engGetVariable(ep, "significance");
//
//    int	h = static_cast<int> (mxGetPr(h_mxArray)[0]);
//	significance = static_cast<float> (mxGetPr(significance_mxArray)[0]);
//
//	//delete
//	mxDestroyArray(Data1_matlab_mxArray);
//	mxDestroyArray(Data2_matlab_mxArray);
//	mxDestroyArray(alpha_matlab_mxArray);
//	mxDestroyArray(h_mxArray);
//	mxDestroyArray(significance_mxArray);
//
//	return h;
//
//}
//
//void StaticFunctionLibrary::OrderPolyData(vtkPolyData *InputPoly, vtkPolyData *OutputPoly, float* referenceDir)//, float* zCoorArray)
//{
//	// referenceDir is either NULL or the vector pointing to the desire direction which
//	// specifies the order of the polydata
//	// Note that any inputPoly could be order in 2 ways
//	// if referenceDir is not given (NULL) then inputPoly will be ordered in the +z direction
//	// otherwise inputPoly will be ordered according to the direction specified.
//
//	OutputPoly->DeepCopy(InputPoly);
//
//	float referenceDirReal[3] = {0, 0, 1};
//	
//	if (referenceDir)
//	{
//		// referenceDir is given, copy it to our local vars.
//		referenceDirReal[0] = referenceDir[0];
//		referenceDirReal[1] = referenceDir[1];
//		referenceDirReal[2] = referenceDir[2];
//	}
//
//	// assume the input polydata are made up of lines (not polygons)
//	int num = InputPoly->GetLines()->GetNumberOfCells();
//	int i = 0;
//	int npts, *pts;
//	float* zCoorArray = new float [num];
//	int* index = new int [num];
//	InputPoly->GetLines()->InitTraversal();
//	int* LocationArray = new int [num];
//	int sum = 0;
//
//	float *x0, *x1, *x2;
//	float x1x0[3], x2x0[3];
//	float CrossProduct[3];
//
//	LocationArray[0] = 0;
//
//	for (i = 0; i < num; i++) 
//	{
//		InputPoly->GetLines()->GetNextCell(npts, pts);
//		// use dot product:
//		if (i == 0) 
//		{
//			x0 = InputPoly->GetPoint(pts[0]);
//			x1 = InputPoly->GetPoint(pts[npts/3]);
//			x2 = InputPoly->GetPoint(pts[2*(npts/3)]);
//			x1x0[0] = x1[0] - x0[0];x1x0[1] = x1[1] - x0[1];x1x0[2] = x1[2] - x0[2];
//			x2x0[0] = x2[0] - x0[0];x2x0[1] = x2[1] - x0[1];x2x0[2] = x2[2] - x0[2];
//			vtkMath::Cross(x1x0, x2x0, CrossProduct);
//			vtkMath::Normalize(CrossProduct);	
//			
//			if (vtkMath::Dot(CrossProduct,referenceDirReal) < 0) 
//			{
//				CrossProduct[0] *= -1;
//				CrossProduct[1] *= -1;
//				CrossProduct[2] *= -1;
//			}	
//		}
//
//		// dot product with z unit vector
//		sum += npts + 1;
//		zCoorArray[i] = vtkMath::Dot(CrossProduct, InputPoly->GetPoints()->GetPoint(pts[0]));
//		// don't use zCoor, use dot products.
//		index[i] = i;
//		if (i != num - 1) 
//		{
//			LocationArray[i + 1] = sum;
//		}
//	}
//
//	StaticFunctionLibrary::SimpleSort(zCoorArray, index, num);
//
//	vtkCellArray* outputCellArray = vtkCellArray::New();
//	
//	outputCellArray->InitTraversal();
//
//	for (i = 0; i < num; i++) 
//	{
//		InputPoly->GetLines()->GetCell(LocationArray[index[i]], npts, pts);
//		outputCellArray->InsertNextCell(npts, pts);
//	}
//
//	OutputPoly->SetLines(outputCellArray);
//	outputCellArray->Delete();
//		
//	delete [] zCoorArray;
//	delete [] LocationArray;
//	delete [] index;
//}
//
void StaticFunctionLibrary::SimpleSort(float *sequence, int *index, int num)
{
	// arrange sequence of length num in ascending order
	int i, j, temp2;
	float temp1;
	
	//test
	//for (i = 0; i < num; i++) {
	//	TRACE ("%i:%f\n", i, sequence[i]);
	//}
	
	for (i = 0; i < num; i++)
	{
		for (j = i + 1; j < num; j++) 
		{
			if (sequence[j] < sequence[i]) 
			{
				temp1 = sequence[i];
				sequence[i] = sequence[j];
				sequence[j] = temp1;
				temp2 = index[i];
				index[i] = index[j];
				index[j] = temp2;
			}
		}
	}
}
//void StaticFunctionLibrary::DetermineBifurcation(vtkPolyData *input, float Bifurcation[3])
//{
//	float notused1, notused2, NormalOfInput[3];
//	StaticFunctionLibrary::ComputeNormalOfPlane(input, NormalOfInput, notused1, notused2);
//	vtkMath::Normalize(NormalOfInput);
//	
//	vtkPointData* inputPointData = input->GetPointData();
//
//	vtkDataArray* IsArch = inputPointData->GetArray("IsArch");
//	VERIFY (IsArch);
//
//	int IsArchi;
//	float ArchPointi[3];
//
//	int maxId;
//	float tempDoti, maxDot;
//	int EnteredIsArchCond = 0;
//
//	for (int i = 0; i < IsArch->GetNumberOfTuples(); i++)
//	{
//		IsArchi = static_cast<int>(IsArch->GetTuple1(i));
//		if (IsArchi)
//		{
//			input->GetPoint(i, ArchPointi);
//
//			tempDoti = vtkMath::Dot(ArchPointi, NormalOfInput);
//
//			if (!EnteredIsArchCond)
//			{
//				maxDot = tempDoti;
//				EnteredIsArchCond = 1;
//				maxId = i;
//			}
//			else
//			{
//				if (tempDoti > maxDot)
//				{
//					maxDot = tempDoti;
//					maxId = i;
//				}
//			}
//		}
//	}
//
//	input->GetPoint(maxId, Bifurcation);
//
//}
//
//int StaticFunctionLibrary::RandomInteger(int n)
//{
//// return a random integer from 0 to n-1 based on the uniform distribution
//
//	float temp;
//
//	temp = floor(n*vtkMath::Random());
//
//	return static_cast<int>(temp);
//}
//
//int StaticFunctionLibrary::BootstrapMeanTest2(float *Data1, int size1, float *Data2, int size2, 
//											  float alpha, int BootSamples, float &significance, int& tobsHigherThanMedian)
//{
//	int i;
//	float xbar, ybar, zbar, yvar, zvar, tobs;
//	
//	float* TwoSetsOfData = new float [size1+size2];
//	memcpy(TwoSetsOfData, Data1, sizeof(float)*size1);
//	memcpy(TwoSetsOfData+size1, Data2, sizeof(float)*size2);
//
//	/*
//	ofstream file("C:\\data1.txt");
//	for (i = 0; i < size1; i++)
//	{
//		file << Data1[i] << "\n";
//	}
//	file.close();
//
//	ofstream file2("C:\\data2.txt");
//	for (i = 0; i < size2; i++)
//	{
//		file2 << Data2[i] << "\n";
//	}
//	file2.close();
//	*/
//
//	xbar = StaticFunctionLibrary::Mean(TwoSetsOfData, size1+size2);
//	ybar = StaticFunctionLibrary::Mean(Data1, size1);
//	zbar = StaticFunctionLibrary::Mean(Data2, size2);
//	yvar = StaticFunctionLibrary::Variance(Data1, size1);
//	zvar = StaticFunctionLibrary::Variance(Data2, size2);
//	tobs = (zbar - ybar)/sqrt(yvar/size1 + zvar/size2);
//
//	//Step 1 and 2;
//
//	float* ytranslated = new float [size1];
//	float* ztranslated = new float [size2];
//
//	for (i = 0; i < size1; i++)
//	{
//		ytranslated[i] = Data1[i] - ybar + xbar;
//	}
//	for (i = 0; i < size2; i++)
//	{
//		ztranslated[i] = Data2[i] - zbar + xbar;
//	}
//
//	float* Meany = new float [BootSamples];
//	float* Meanz = new float [BootSamples];
//	float* Variancey = new float [BootSamples];
//	float* Variancez = new float [BootSamples];
//
//	StaticFunctionLibrary::bootstrp(ytranslated, size1, Meany, Variancey, BootSamples);
//	StaticFunctionLibrary::bootstrp(ztranslated, size2, Meanz, Variancez, BootSamples);
//
//	// Step 3
//
//	float* t = new float [BootSamples];
//
//	for (i = 0; i < BootSamples; i++)
//	{
//		t[i] = (Meanz[i] - Meany[i])/(sqrt(Variancey[i]/size1 + Variancez[i]/size2));
//	}
//
//	// Step 4
//
//	int count = 0;
//	float significanceTemp;
//
//	for (i = 0; i < BootSamples; i++)
//	{
//		if (tobs>t[i])
//		{
//			count++;
//		}
//	}
//
//	significanceTemp = static_cast<float>(count)/static_cast<float>(BootSamples);
//
//	tobsHigherThanMedian = 0;
//
//	if (significanceTemp > 0.5)
//	{
//		tobsHigherThanMedian = 1;
//		significanceTemp = 1 - significanceTemp;
//	}
//
//	significance = 2*significanceTemp;
//
//	int h = (significance <= alpha);
//	
//	//delete
//	delete [] t;
//	delete [] Meany;
//	delete [] Meanz;
//	delete [] Variancey;
//	delete [] Variancez;
//	delete [] ytranslated;
//	delete [] ztranslated;
//	delete [] TwoSetsOfData;
//
//	// return
//	return h;
//}
//
//void StaticFunctionLibrary::bootstrp(float *Data, int DataSize, float *Mean, float *Variance, int BootSamples)
//{
//	// Generate BootSamples of Data Samples
//	// Compute Mean and Variance of each bootstrap samples
//	// Thus:
//	// Data is and array of size DataSize
//	// Mean and Variance are arrays of size BootSamples
//
//	int i, j, randint;
//	float* BootStrapSamplei = new float [DataSize];
//
//	for (i = 0; i < BootSamples; i++)
//	{
//		for (j = 0; j < DataSize; j++)
//		{
//			//Fill RandomIndex for the ith iteration
//			randint = StaticFunctionLibrary::RandomInteger(DataSize);
//			BootStrapSamplei[j] = Data[randint];
//		}
//
//		Mean[i] = StaticFunctionLibrary::Mean(BootStrapSamplei, DataSize);
//		Variance[i] = StaticFunctionLibrary::Variance(BootStrapSamplei, DataSize);
//	}
//
//	//delete 
//	delete [] BootStrapSamplei;
//}
//
//void StaticFunctionLibrary::InitialMapping(vtkFloatArray *mu0, vtkFloatArray *mu1, 
//										   int mapSizeX, int mapSizeY, 
//										   float hx, float hy, 
//										   float A, float B, 
//										   vtkFloatArray* ux, vtkFloatArray* uy,
//										   int& uxMapSize, int& uyMapSize)
//{
//	int i,j;
//
//	uxMapSize = vtkMath::Round(A/hx) + 1;
//	uyMapSize = vtkMath::Round(B/hy) + 1;
//
//	float* f = new float [mapSizeX];
//	float* g = new float [mapSizeX];
//	float* f2 = new float [mapSizeY];
//	float* g2 = new float [mapSizeY];
//	
//	StaticFunctionLibrary::Computefg(mu0, mu1, mapSizeX, mapSizeY, A, B, f, g);
//
//	float delX = A/mapSizeX;
//	float delY = B/mapSizeY;
//
//	float tempux, tempuy, x, y, adiffx;
//	int index;
//
//	for (i = 0; i < uxMapSize; i++)
//	{
//		TRACE("%s%i%s", "i = ", i, "\n");
//
//		x = i*hx;
//		tempux = StaticFunctionLibrary::Computea(f, g, mapSizeX, x, delX, A);
//		
//		StaticFunctionLibrary::Computef2g2adiffx(mu0, mu1, mapSizeX, mapSizeY, delX, delY,
//			x, tempux, f2, g2, adiffx);
//
//		for (j = 0; j < uyMapSize; j++)
//		{
//			y = j*hy;
//			tempuy = StaticFunctionLibrary::Computeb(f2, g2, mapSizeY, y, delY, B, adiffx);
//
//			index = j*uxMapSize + i;
//
//			ux->InsertTuple1(index, tempux);
//			uy->InsertTuple1(index, tempuy);
//		}
//	}
//
//	//delete
//	delete [] f;
//	delete [] g;
//	delete [] f2;
//	delete [] g2;
//
//	/*
//	//1. Calculate ax
//
//	//mu0 and mu1 are 2D matrices -- filling x first then y;
//	//Assume mu0 and mu1 have domain [0,A]X[0,B]
//
//	double delX = A/static_cast<double>(mapSizeX);
//	double delY = B/static_cast<double>(mapSizeY);
//
//	double* mu0prime = new double[mapSizeX];
//	double* mu1prime = new double[mapSizeX];
//
//	double mu0sumj, mu1sumj;
//
//	for (i = 0; i < mapSizeX; i++)
//	{
//		mu0sumj = mu1sumj = 0.0;
//
//		for (j = i; j < mapSizeX*mapSizeY; j += mapSizeX)
//		{
//			mu0sumj += mu0->GetTuple1(j);
//			mu1sumj += mu1->GetTuple1(j);
//		}
//		
//		mu0prime[i] = mu0sumj*delY;
//		mu1prime[i] = mu1sumj*delY;
//	}
//
//	double* f = new double [mapSizeX];
//	double* g = new double [mapSizeX];
//	double fcum, gcum;
//	fcum = gcum = 0.0;
//
//	for (i = 0; i < mapSizeX; i++)
//	{
//		fcum += mu0prime[i];
//		gcum += mu1prime[i];
//
//		f[i] = fcum*delX;
//		g[i] = gcum*delX;
//	}
//
//	// scale f so that the mass preserving assumption: f(1) = g(1) is
//	// guaranteed.
//
//	double scale = g[mapSizeX-1]/f[mapSizeX-1];
//	
//	for (i = 0; i < mapSizeX; i++)
//	{
//		f[i] *= scale;
//	}
//
//	for (i = 0; i < mapSizeX*mapSizeY; i++)
//	{
//		mu0->SetTuple1(i, mu0->GetTuple1(i)*scale);
//	}
//
//	ux = StaticFunctionLibrary::Computea(f, g, mapSizeX, x, delX, A);
//
//	//2. Calculate bxy
//
//	double* f2 = new double [mapSizeY];
//	double* g2 = new double [mapSizeY];
//	double f2cum, g2cum;
//	f2cum = g2cum = 0.0;
//
//	double tempcol; 
//	int col;
//
//	// 0:delY:B --> f2(.) = [0 f2];
//	// 0:delY:B --> g2(.) = [0 g2];
//
//	tempcol = x/delX;
//	col = static_cast<int>(floor(tempcol));
//	col = (col>=mapSizeX)?(mapSizeX-1):col;
//
//	int index;
//
//	for (i = 0; i < mapSizeY; i++)
//	{
//		index = col + (mapSizeX*i);
//		f2cum += mu0->GetTuple1(index)*delY;
//		f2[i] = f2cum;
//	}
//
//	tempcol = ux/delX;
//	col = static_cast<int>(floor(tempcol));
//	col = (col>=mapSizeX)?(mapSizeX-1):col;
//
//	for (i = 0; i < mapSizeY; i++)
//	{
//		index = col + (mapSizeX*i);
//		g2cum += mu1->GetTuple1(index)*delY;
//		g2[i] = g2cum;
//	}
//
//	double adiffx = f2cum/g2cum;
//	
//	// now interpolate between f(i) and f(i + 1);
//
//	double tempindex = y/delY;
//	double f2lowi, f2highi;
//	int floortempindex = static_cast<int>(floor(tempindex));
//	int ceiltempindex = static_cast<int>(ceil(tempindex));
//	double remainder = tempindex - floortempindex;
//
//	if (floortempindex <= 0)
//	{
//		f2lowi = 0;
//	}
//	else
//	{
//		f2lowi = f2[floortempindex-1];
//	}
//
//	if (ceiltempindex <= 0)
//	{
//		f2highi = 0;
//	}
//	else
//	{
//		f2highi = f2[ceiltempindex-1];
//	}
//
//	double f2y = f2lowi + remainder*(f2highi - f2lowi);
//	double rhs = f2y/adiffx;
//	int highindex, lowindex;
//	double g2lowindex;
//	double portion;
//
//	// compute b(x,y) = g2^{-1}(rhs);
//
//	if (rhs >= g2[mapSizeY-1])
//	{
//		uy = B; return;
//	}
//	else
//	{
//		for (i = 0; i < mapSizeY; i++)
//		{
//			if (g2[i] > rhs)
//			{
//				highindex = i;
//				break;
//			}
//		}
//
//		lowindex = highindex - 1;
//    
//		if (lowindex < 0)
//		{
//			g2lowindex = 0;
//		}
//		else
//		{
//			g2lowindex = g2[lowindex];
//		}
//
//	    portion = (rhs - g2lowindex)/(g2[highindex] - g2lowindex);
//
//	    uy = (lowindex + portion + 1)*delY;
//	}
//
//	//delete
//	delete [] mu0prime;
//	delete [] mu1prime;
//	delete [] f;
//	delete [] g;
//	delete [] f2;
//	delete [] g2;
//	*/
//
//}
//
//float StaticFunctionLibrary::Computea(float *f, float *g, int mapSizeX, float x, float delX, float A)
//{
//	//test
//	//figure; plot(0:1/length(f):1, [0 f], 'b-');
//	//hold on; plot(0:1/length(g):1, [0 g], 'r-');
//	
//	// x' = 0:delX:A --> f(x') = [0 f];
//	// similar for g(x')
//	
//	// first find f(x) -- don't confuse with x' -- x is the input argument
//	
//	// now interpolate between f(i) and f(i + 1);
//
//	// Oct 25 2006: Instead of doing this, clamp the input
//	//if (x < 0 || x > A)
//	//{
//	//	CRotateDlg::CustomMessageBox("x must be within 0 and A");
//	//	VERIFY(0);
//	//}
//
//	if (x < 0) x = 0;
//	if (x > A) x = A;
//
//	float ax;
//	float i = x/delX;// index of x
//	float flowi, fhighi, fx;
//	int floori = static_cast<int>(floor(i));
//	int ceili = static_cast<int>(ceil(i));
//	float remainder = i - floori;
//
//	if (floori <= 0)
//	{
//		flowi = 0;
//	}
//	else
//	{
//		flowi = f[floori-1];
//	}
//
//	if (ceili <= 0)
//	{
//		fhighi = 0;
//	}
//	else
//	{
//		fhighi = f[ceili-1];
//	}
//	
//	fx = flowi + remainder*(fhighi-flowi);
//
//	//now find gx from f
//
//	int j;
//	float portion;
//	int highindex, lowindex;
//	float glowindex;
//
//	if (fx >= g[mapSizeX-1])
//	{
//		return A;
//	}
//	else
//	{
//		for (j = 0; j < mapSizeX; j++)
//		{
//			if (g[j] > fx)
//			{
//				highindex = j;
//				break;
//			}				
//		}
//		
//		lowindex = highindex - 1;
//    
//	    if (lowindex < 0)
//		{
//		    glowindex = 0;
//		}
//		else
//		{
//			glowindex = g[lowindex];
//		}
//
//	    portion = (fx - glowindex)/(g[highindex] - glowindex);
//
//	    ax = (lowindex + portion + 1)*delX;
//		return ax;
//	}
//}
//
//void StaticFunctionLibrary::Computefg(vtkFloatArray *mu0, vtkFloatArray *mu1, int mapSizeX, int mapSizeY, float A, float B, float *f, float *g)
//{
//	int i,j;
//
//	//1. Calculate ax
//
//	//mu0 and mu1 are 2D matrices -- filling x first then y;
//	//Assume mu0 and mu1 have domain [0,A]X[0,B]
//
//	float delX = A/static_cast<float>(mapSizeX);
//	float delY = B/static_cast<float>(mapSizeY);
//
//	float* mu0prime = new float[mapSizeX];
//	float* mu1prime = new float[mapSizeX];
//
//	float mu0sumj, mu1sumj;
//
//	for (i = 0; i < mapSizeX; i++)
//	{
//		mu0sumj = mu1sumj = 0.0;
//
//		for (j = i; j < mapSizeX*mapSizeY; j += mapSizeX)
//		{
//			mu0sumj += mu0->GetTuple1(j);
//			mu1sumj += mu1->GetTuple1(j);
//		}
//		
//		mu0prime[i] = mu0sumj*delY;
//		mu1prime[i] = mu1sumj*delY;
//	}
//
//	float fcum, gcum;
//	fcum = gcum = 0.0;
//
//	for (i = 0; i < mapSizeX; i++)
//	{
//		fcum += mu0prime[i];
//		gcum += mu1prime[i];
//
//		f[i] = fcum*delX;
//		g[i] = gcum*delX;
//	}
//
//	// scale f so that the mass preserving assumption: f(1) = g(1) is
//	// guaranteed.
//
//	float scale = g[mapSizeX-1]/f[mapSizeX-1];
//	
//	for (i = 0; i < mapSizeX; i++)
//	{
//		f[i] *= scale;
//	}
//
//	for (i = 0; i < mapSizeX*mapSizeY; i++)
//	{
//		mu0->SetTuple1(i, mu0->GetTuple1(i)*scale);
//	}
//
//	delete [] mu0prime;
//	delete [] mu1prime;
//}
//
//void StaticFunctionLibrary::Computef2g2adiffx(vtkFloatArray *mu0, vtkFloatArray *mu1, int mapSizeX, int mapSizeY, float delX, float delY, float x, float ux, float *f2, float *g2, float &adiffx)
//{
//	int i;
//	float f2cum, g2cum;
//	f2cum = g2cum = 0.0;
//
//	float tempcol; 
//	int col;
//
//	// 0:delY:B --> f2(.) = [0 f2];
//	// 0:delY:B --> g2(.) = [0 g2];
//
//	tempcol = x/delX;
//	col = static_cast<int>(floor(tempcol));
//	col = (col>=mapSizeX)?(mapSizeX-1):col;
//
//	int index;
//
//	for (i = 0; i < mapSizeY; i++)
//	{
//		index = col + (mapSizeX*i);
//		f2cum += mu0->GetTuple1(index)*delY;
//		f2[i] = f2cum;
//	}
//
//	tempcol = ux/delX;
//	col = static_cast<int>(floor(tempcol));
//	col = (col>=mapSizeX)?(mapSizeX-1):col;
//
//	for (i = 0; i < mapSizeY; i++)
//	{
//		index = col + (mapSizeX*i);
//		g2cum += mu1->GetTuple1(index)*delY;
//		g2[i] = g2cum;
//	}
//
//	adiffx = f2cum/g2cum;
//}
//
//float StaticFunctionLibrary::Computeb(float *f2, float *g2, int mapSizeY, float y, float delY, float B, float adiffx)
//{
//	/* //Oct 25 2006: Instead of doing this, clamp the input 
//	if (y < 0 || y > B)
//	{
//		CRotateDlg::CustomMessageBox("y must be within 0 and B");
//		VERIFY(0);
//	}
//	*/
//
//	if (y < 0) y = 0;
//	if (y > B) y = B;
//
//	int i;
//
//	float uy;
//	float tempindex = y/delY;
//	float f2lowi, f2highi;
//	int floortempindex = static_cast<int>(floor(tempindex));
//	int ceiltempindex = static_cast<int>(ceil(tempindex));
//	float remainder = tempindex - floortempindex;
//
//	if (floortempindex <= 0)
//	{
//		f2lowi = 0;
//	}
//	else
//	{
//		f2lowi = f2[floortempindex-1];
//	}
//
//	if (ceiltempindex <= 0)
//	{
//		f2highi = 0;
//	}
//	else
//	{
//		f2highi = f2[ceiltempindex-1];
//	}
//
//	float f2y = f2lowi + remainder*(f2highi - f2lowi);
//	float rhs = f2y/adiffx;
//	int highindex, lowindex;
//	float g2lowindex;
//	float portion;
//
//	// compute b(x,y) = g2^{-1}(rhs);
//
//	if (rhs >= g2[mapSizeY-1])
//	{
//		return B;
//	}
//	else
//	{
//		for (i = 0; i < mapSizeY; i++)
//		{
//			if (g2[i] > rhs)
//			{
//				highindex = i;
//				break;
//			}
//		}
//
//		lowindex = highindex - 1;
//    
//		if (lowindex < 0)
//		{
//			g2lowindex = 0;
//		}
//		else
//		{
//			g2lowindex = g2[lowindex];
//		}
//
//	    portion = (rhs - g2lowindex)/(g2[highindex] - g2lowindex);
//
//	    uy = (lowindex + portion + 1)*delY;
//		return uy;
//	}
//}
//
void StaticFunctionLibrary::ComputeMeanCurve2(vtkPolyData **Inputs, int NumberOfPolyData, vtkPolyData *Output, int IndexFirstSlice)
{
	// Arguments:
	// Inputs[0 to n-1] are n closed curves for which we want to find the mean
	// Output is the resulting mean curve.
	// In this function, we just calculate the mean curve. No PointData is attached to the point

	int i, j;
	OldSymmetricCorrespondence** scs = new OldSymmetricCorrespondence*[NumberOfPolyData-1];

	vtkPolyData* FirstContour = Inputs[IndexFirstSlice];
	vtkPolyData* SecondContour;
	int* SecondContourOrder = new int [NumberOfPolyData-1];

	// Grab the number of points in the first lines cell
	int NumberOfPoints, *pts;
	vtkCellArray* FirstContourLines = FirstContour->GetLines();
	FirstContourLines->InitTraversal();
	FirstContourLines->GetNextCell(NumberOfPoints, pts);
	if (pts[0] == pts[NumberOfPoints-1]) NumberOfPoints--;

	//Output Points, Lines, Vectors, Scalars;
	vtkPoints* outputPoints = vtkPoints::New();
	Output->SetPoints(outputPoints);
	outputPoints->Delete();

	vtkCellArray* outputLines = vtkCellArray::New();
	Output->SetLines(outputLines);
	outputLines->Delete();

	// Find correspondences
	for (i = 0; i < (NumberOfPolyData - 1); i++)
	{
		scs[i] = OldSymmetricCorrespondence::New();
		scs[i]->SetContour1(FirstContour);
				
		if (i < IndexFirstSlice)
		{
			SecondContourOrder[i] = i;
		}
		else 
		{
			SecondContourOrder[i] = i+1;
		}

		SecondContour = Inputs[SecondContourOrder[i]];

		if (!StaticFunctionLibrary::SameOrientation(SecondContour, FirstContour))
		{
			SecondContour->ReverseCell(SecondContour->GetNumberOfVerts());
			//StaticFunctionLibrary::WritePolyData(SecondContour, "C:\\testdata\\SecondContour.vtk");
		}
		scs[i]->SetContour2(SecondContour);
		scs[i]->FindCorrespondence();

		//test
		
		//vtkPolyData* PolyDataForTest = vtkPolyData::New();
		//vtkPoints* PointForTest = vtkPoints::New();
		//PolyDataForTest->SetPoints(PointForTest);
		//vtkCellArray* CellArrayForTest = vtkCellArray::New();
		//PolyDataForTest->SetLines(CellArrayForTest);
		//PointForTest->Delete();
		//CellArrayForTest->Delete();

		//for (int v = 0; v < scs[i]->GetNumberOfPoints(); v++) 
		//{
		//	PointForTest->InsertNextPoint(scs[i]->GetPoints1()->GetPoint(v));
		//	PointForTest->InsertNextPoint(scs[i]->GetPoints2()->GetPoint(v));
		//	CellArrayForTest->InsertNextCell(2);
		//	CellArrayForTest->InsertCellPoint(2*v);
		//	CellArrayForTest->InsertCellPoint(2*v+1);
		//}

		//CString filename;
		//filename.Format("%s%i%s", "C:\\testdata\\c\\scs_", i, ".vtk");
		//StaticFunctionLibrary::WritePolyData(PolyDataForTest, (LPCSTR) filename);
		//PolyDataForTest->Delete();
		//
		//test end
	}

	outputLines->InsertNextCell(NumberOfPoints+1);
	double currentPoint[3], tempPoint[3];

	for (i = 0; i < NumberOfPoints; i++)
	{
		// Find the centroid of each correspondence group
		scs[0]->GetPoints1()->GetPoint(i, currentPoint);

		for (j = 0; j < NumberOfPolyData-1; j++)
		{
			scs[j]->GetPoints2()->GetPoint(i, tempPoint);
			currentPoint[0] += tempPoint[0];
			currentPoint[1] += tempPoint[1];
			currentPoint[2] += tempPoint[2];
		}

		currentPoint[0] /= NumberOfPolyData;
		currentPoint[1] /= NumberOfPolyData;
		currentPoint[2] /= NumberOfPolyData;

		outputLines->InsertCellPoint(i);
		outputPoints->InsertNextPoint(currentPoint);
	}

	outputLines->InsertCellPoint(0);

	//delete
	for (i = 0; i < (NumberOfPolyData - 1); i++)
	{
		scs[i]->Delete();
	}

	delete [] scs;
	delete [] SecondContourOrder;

}
//
//void StaticFunctionLibrary::ReplaceExtension(CString inString, CString &outString, CString extension)
//{
//	int id = inString.ReverseFind('.');
//
//	if (id == -1)
//	{
//		outString = inString;
//		return;
//	}
//
//	outString = inString.Mid(0, id+1) + extension;
//}
//
void StaticFunctionLibrary::ComputeNormalOfPlane(vtkPolyData *input, 
												 double NormalOfPlane[], 
												 double &minZ, double &maxZ)
{
	// NormalOfPlane: the normal of the traverse slice of the input data (output)
	// minZ: The distance between the plane containing the origin (0, 0, 0) (with normal NormalOfPlane) and the lowest plane (lowest, of 
	// course, in the direction of the normal)
	// maxZ: The distance between the plane containing the origin (with normal NormalOfPlane) and the highest plane (in the direction
	// of normal)

	int i, numClosedCurve;	
	double tempNormal[3];

	vtkFeatureEdges* featureEdges = vtkFeatureEdges::New();
	featureEdges->SetInput(input);
	featureEdges->BoundaryEdgesOn();
	featureEdges->FeatureEdgesOff();
	featureEdges->NonManifoldEdgesOff();
	featureEdges->ManifoldEdgesOff();
	featureEdges->ColoringOff();

	vtkStripper* stripper = vtkStripper::New();
	stripper->SetInput(featureEdges->GetOutput());
	stripper->Update();

	StaticFunctionLibrary::WritePolyData(stripper->GetOutput(), "C:\\stripperOutput.vtk");

	vtkPolyData* pdinCount = vtkPolyData::New();
	pdinCount->DeepCopy(stripper->GetOutput());

	vtkPolyData* tempOutputStripper = vtkPolyData::New();

	numClosedCurve = StaticFunctionLibrary::CountNumberOfClosedCurve(pdinCount, tempOutputStripper,1);
	StaticFunctionLibrary::WritePolyData(tempOutputStripper, "C:\\tempOutputStripper.vtk");

	vtkPolyData* tempOutput2 = vtkPolyData::New();
	numClosedCurve = StaticFunctionLibrary::EliminateContoursWithPointsOnArch(tempOutputStripper, tempOutput2);

	vtkCellArray* lines = tempOutput2->GetLines();
	int npts, *pts;

	// numClosedCurve == 2 for phantom experiments
	if (numClosedCurve == 2)
	{
		double point1[3], point2[3];

		// compute normal, then
		StaticFunctionLibrary::FindNormal(tempOutput2, NormalOfPlane);
		// minZ, maxZ
		vtkMath::Normalize(NormalOfPlane);

		lines->InitTraversal();
		lines->GetNextCell(npts, pts);
		tempOutput2->GetPoint(pts[0], point1);
		
		lines->GetNextCell(npts, pts);
		tempOutput2->GetPoint(pts[0], point2);

		//for (i = 0; i < 3; i++)
		//{
		//	vec[i] = point2[i] - point1[i];
		//}

		//if (vtkMath::Dot(vec, NormalOfPlane) < 0)
		if (NormalOfPlane[2] < 0)
		{
			for (i = 0; i < 3; i++)
			{
				NormalOfPlane[i] *= -1;
			}
		}

		minZ = vtkMath::Dot(point1, NormalOfPlane);
		maxZ = vtkMath::Dot(point2, NormalOfPlane);

		float temp;
		if (maxZ < minZ)
		{
			temp = maxZ;
			maxZ = minZ;
			minZ = temp;
		}

		//delete
		pdinCount->Delete();
		tempOutputStripper->Delete();
		featureEdges->Delete();
		stripper->Delete();
		tempOutput2->Delete();

		return;
	}

	ASSERT (numClosedCurve == 3);

	//vtkCellArray* lines = tempOutputStripper->GetLines();
	
	vtkPolyData* OneSlice = vtkPolyData::New();
	//vtkCellArray* currentLines = vtkCellArray::New();
	//OneSlice->SetPoints(tempOutputStripper->GetPoints());
	//OneSlice->SetLines(currentLines);	
	//currentLines->Delete();

	NormalOfPlane[0] = 0.0;
	NormalOfPlane[1] = 0.0;
	NormalOfPlane[2] = 0.0;
	
	for (i = 0; i < numClosedCurve; i++)
	{
		OneSlice->Initialize();
		StaticFunctionLibrary::GrabOnlyOneLine(tempOutput2, OneSlice, i);
		StaticFunctionLibrary::FindNormal(OneSlice, tempNormal);
		//lines->GetNextCell(npts, pts);

		//currentLines->Reset();
		//currentLines->InsertNextCell(npts, pts);
		//StaticFunctionLibrary::FindNormal(OneSlice, tempNormal);
		vtkMath::Normalize(tempNormal);
		if (tempNormal[2] > 0)//dot with z-axis
		{
			//so that the normal points to the negative z direction
			//this decision is made arbitrarily -- as long as the direction is consistent, it's fine
			//NormalOfPlane will be corrected later (see next step --> Line (*))
			tempNormal[0] *= -1.0;
			tempNormal[1] *= -1.0;
			tempNormal[2] *= -1.0;
		}

		NormalOfPlane[0] += tempNormal[0];
		NormalOfPlane[1] += tempNormal[1];
		NormalOfPlane[2] += tempNormal[2];
	}

	vtkMath::Normalize(NormalOfPlane);

	// (*) -- ok, normal points to -ve z, but we want it to point from CCA to ica/eca
	// so which one is CCA
	
	double tol = 0.05;
	vtkPoints* ThreePoints = vtkPoints::New();
	ThreePoints->SetNumberOfPoints(3);
	double tempPoint[3];

	for (i = 0, lines->InitTraversal(); i < numClosedCurve; i++)
	{
		lines->GetNextCell(npts, pts);
		tempOutput2->GetPoint(pts[0], tempPoint);
		ThreePoints->SetPoint(i, tempPoint);
	}

	double tempVector[3], tempVector2[3];
	double origin[3];
	double firstPoint[3], secondPoint[3];
	double VectorFromCCA[3];
	
	ThreePoints->GetPoint(0, origin);
	ThreePoints->GetPoint(1, firstPoint);
	ThreePoints->GetPoint(2, secondPoint);

	tempVector[0] = firstPoint[0] - origin[0];
	tempVector[1] = firstPoint[1] - origin[1];
	tempVector[2] = firstPoint[2] - origin[2];

	tempVector2[0] = secondPoint[0] - origin[0];
	tempVector2[1] = secondPoint[1] - origin[1];
	tempVector2[2] = secondPoint[2] - origin[2];

	if (abs(vtkMath::Dot(tempVector, NormalOfPlane)) < tol)
	{
		//origin and firstPoint are points on the two branches --> Cell 2 is CCA
		VectorFromCCA[0] = origin[0] - secondPoint[0];
		VectorFromCCA[1] = origin[1] - secondPoint[1];
		VectorFromCCA[2] = origin[2] - secondPoint[2];
	}
	else if (abs(vtkMath::Dot(tempVector2, NormalOfPlane)) < tol)
	{
		//origin and secondPoint are points on the two branches --> Cell 1 is CCA
		VectorFromCCA[0] = origin[0] - firstPoint[0];
		VectorFromCCA[1] = origin[1] - firstPoint[1];
		VectorFromCCA[2] = origin[2] - firstPoint[2];
	}
	else
	{
		// firstPoint and secondPoint are points on the two branches --> Cell 0 is CCA
		VectorFromCCA[0] = firstPoint[0] - origin[0];
		VectorFromCCA[1] = firstPoint[1] - origin[1];
		VectorFromCCA[2] = firstPoint[2] - origin[2];
	}

	// Correct NormalOfPlane so that 
	if (vtkMath::Dot(NormalOfPlane, VectorFromCCA) < 0)
	{
		NormalOfPlane[0] *= -1.0;
		NormalOfPlane[1] *= -1.0;
		NormalOfPlane[2] *= -1.0;
	}

	double tempZ;
	maxZ = VTK_DOUBLE_MIN;
	minZ = VTK_DOUBLE_MAX;

	for (i = 0, lines->InitTraversal(); i < numClosedCurve; i++)
	{
		lines->GetNextCell(npts, pts);

		// get an arbitrary point from line i
		tempOutput2->GetPoint(pts[0], tempPoint);

		tempZ = vtkMath::Dot(tempPoint, NormalOfPlane);

		if (tempZ > maxZ)
		{
			maxZ = tempZ;
		}
		if (tempZ < minZ)
		{
			minZ = tempZ;
		}
	}

	//delete
	pdinCount->Delete();
	OneSlice->Delete();
	tempOutputStripper->Delete();
	featureEdges->Delete();
	stripper->Delete();
}

//void StaticFunctionLibrary::GrabOnlyOneLine(vtkPolyData *in, vtkPolyData *out, int LineNum)
//{
//	// assume in and out have already been allocated
//
//	int i;
//	vtkCellArray* inLines = in->GetLines();
//
//	if (!inLines->GetNumberOfCells())
//	{
//		return;
//	}
//	if (LineNum >= inLines->GetNumberOfCells())
//	{
//		return;
//	}
//
//	out->DeepCopy(in);
//
//	vtkCellArray* outLines = vtkCellArray::New();
//	out->SetLines(outLines);
//	outLines->Delete();
//
//	int npts, *pts;
//
//	inLines->InitTraversal();
//
//	for (i = 0; i <= LineNum; i++)
//	{
//		inLines->GetNextCell(npts, pts);		
//	}
//	
//	outLines->InsertNextCell(npts, pts);
//}
//
int StaticFunctionLibrary::CurveWithLargestCircularityRatio(vtkPolyData **Curves, vtkIdList *idList)
{
	int i;
	//float maxArea = -VTK_LARGE_FLOAT;
	float thisArea;
	float arclen, CircularityRatio;
	float maxCircularityRatio;
	int LargestCircularityRatioIndex;
	int npts, *pts;
	int thisId;

	vtkCellArray* CurveLines;
	for (i = 0; i < idList->GetNumberOfIds(); i++)
	{
		thisId = idList->GetId(i);
		CurveLines = Curves[thisId]->GetLines();
		CurveLines->InitTraversal();
		CurveLines->GetNextCell(npts, pts);

		thisArea = MyMassProperties::PolygonArea(Curves[thisId]->GetPoints(), npts, pts);
		arclen = StaticFunctionLibrary::ArcLength(Curves[thisId]);
		CircularityRatio = 4*vtkMath::Pi()*thisArea/(arclen*arclen);

		if (i == 0)
		{
			maxCircularityRatio = CircularityRatio;
			LargestCircularityRatioIndex = thisId;
		}
		else
		{			
			if (CircularityRatio > maxCircularityRatio)
			{
				maxCircularityRatio = CircularityRatio;
				LargestCircularityRatioIndex = thisId;
			}
		}
	}

	return LargestCircularityRatioIndex;
}

int StaticFunctionLibrary::CurveWithLargestCircularityRatio(vtkPolyData** Curves, int NumOfCurves)
{
	vtkSmartPointer<vtkIdList> IdList = vtkSmartPointer<vtkIdList>::New();
	IdList->SetNumberOfIds(NumOfCurves);

	for (int i = 0; i < NumOfCurves; i++)
	{
		IdList->SetId(i, i);	
	}

	return StaticFunctionLibrary::CurveWithLargestCircularityRatio(Curves, IdList);
}

//
//void StaticFunctionLibrary::ResampleStackOfLines(vtkPolyData *input, vtkPolyData *output, float SamplingInterval)
//{
//	// This function resamples a stack of contour. Each contour is uniformly sampled into
//	// this->NumberOfSamples intervals
//
//	// First if the input does not contain lines and only polygons, convert ...
//
//	int i, j;
//	vtkPolyData* OutputTemp1Polys = vtkPolyData::New();
//
//	if (input->GetLines()->GetNumberOfCells() == 0) 
//	{
//		StaticFunctionLibrary::PolygonsToLines(input, OutputTemp1Polys);
//	}
//	else 
//	{
//		OutputTemp1Polys->DeepCopy(input);
//	}
//
//	vtkCellArray* inputLines = OutputTemp1Polys->GetLines();
//	int NumberOfLines = inputLines->GetNumberOfCells();
//	
//	int t = 0;
//	int npts, *pts;
//	int npts2, *pts2;
//	float tempPoint[3];
//	int offset = 0;
//
//	vtkPolyData* oneSlice = vtkPolyData::New();
//	vtkCellArray* oneSliceLines = vtkCellArray::New();
//	oneSlice->SetLines(oneSliceLines);
//	oneSliceLines->Delete();
//	vtkPoints* oneSlicePoints = vtkPoints::New();
//	oneSlicePoints->DeepCopy(input->GetPoints());
//	oneSlice->SetPoints(oneSlicePoints);
//	oneSlicePoints->Delete();
//
//	vtkPolyData* ResampledoneSlice = vtkPolyData::New();
//
//	// set up output
//	vtkCellArray* outputLines = vtkCellArray::New();
//	vtkPoints* outputPoints = vtkPoints::New();
//	output->SetLines(outputLines);
//	output->SetPoints(outputPoints);
//	outputLines->Delete();
//	outputPoints->Delete();
//	int NumberOfSamples;
//
//	for (i = 0; i < NumberOfLines; i++)
//	{
//		inputLines->SetTraversalLocation(t);
//		inputLines->GetNextCell(npts, pts);
//		t = inputLines->GetTraversalLocation();
//
//		oneSliceLines->Reset();
//		oneSliceLines->InsertNextCell(npts, pts);
//
//		ResampledoneSlice->Initialize();
//		NumberOfSamples = (int) ceil(StaticFunctionLibrary::ArcLength(oneSlice)/SamplingInterval);
//		StaticFunctionLibrary::ResampleByArcLength(oneSlice, ResampledoneSlice, NumberOfSamples);
//
//		ResampledoneSlice->GetLines()->InitTraversal();
//		ResampledoneSlice->GetLines()->GetNextCell(npts2, pts2);
//
//		outputLines->InsertNextCell(npts2 + 1);
//		for (j = 0; j < npts2; j++)
//		{
//			ResampledoneSlice->GetPoint(pts2[j], tempPoint);
//			outputPoints->InsertPoint(offset + j, tempPoint);
//			outputLines->InsertCellPoint(offset + j);
//		}
//		outputLines->InsertCellPoint(offset);
//
//		offset += npts2;
//	}
//
//	//delete
//	OutputTemp1Polys->Delete();
//	oneSlice->Delete();
//	ResampledoneSlice->Delete();
//}
//
//void StaticFunctionLibrary::PolygonsToLines(vtkPolyData *InputPoly, vtkPolyData *OutputPoly)
//{
//	// This function converts the polys cells to lines cells
//	OutputPoly->DeepCopy(InputPoly);
//
//	vtkCellArray* outputPolyCell = vtkCellArray::New();
//	
//	InputPoly->GetPolys()->InitTraversal();
//
//	int npts, *pts;
//	int i, j;
//
//	for (i = 0; i < InputPoly->GetPolys()->GetNumberOfCells(); i++) 
//	{
//		InputPoly->GetPolys()->GetNextCell(npts, pts);
//		if (pts[0] != pts[npts-1]) 
//		{// for each cell, if pts[npts-1] ! = pts[0], set so.
//			outputPolyCell->InsertNextCell(npts + 1);
//			for (j = 0; j < npts; j++){
//				outputPolyCell->InsertCellPoint(pts[j]);
//			}
//			outputPolyCell->InsertCellPoint(pts[0]);
//		}
//		else {
//			outputPolyCell->InsertNextCell(npts, pts);
//		}
//	}
//
//	OutputPoly->SetLines(outputPolyCell);
//	OutputPoly->SetPolys(NULL);
//	outputPolyCell->Delete();
//}

int StaticFunctionLibrary::ChoosePointWithSmallest(int XYZ, vtkPointSet* pd)
{
	// XYZ = 0: ChoosePointWithSmallestX, 
	// XYZ = 1: ChoosePointWithSmallestY,
	// XYZ = 2: ChoosePointWithSmallestZ

	if (pd->GetNumberOfPoints() <= 0)
		return -1;

	double tempPoint[3];
	pd->GetPoint(0, tempPoint);
	double min = tempPoint[XYZ];
	int minindex = 0;

	for (int i = 1; i < pd->GetNumberOfPoints(); i++)
	{
		pd->GetPoint(i, tempPoint);
		if (tempPoint[XYZ] < min)
		{
			min = tempPoint[XYZ];
			minindex = i;
		}
	}

	return minindex;
}

void StaticFunctionLibrary::DeleteExtension(CString inputString, CString& outputString)
{
	int id = inputString.ReverseFind('.');

	if (id == -1)
	{
		outputString = inputString;
	}

	outputString = inputString.Mid(0, id);
}

void StaticFunctionLibrary::ReplaceExtension(CString inString, CString &outString, CString extension)
{
	int id = inString.ReverseFind('.');

	if (id == -1)
	{
		outString = inString + "." + extension;
		return;
	}

	outString = inString.Mid(0, id+1) + extension;
}

void StaticFunctionLibrary::RemoveExtension(CString inString, CString& outString)
{
	int id = inString.ReverseFind('.');

	if (id == -1)
	{
		outString = inString;
		return;
	}

	outString = inString.Mid(0, id);
}

int StaticFunctionLibrary::ChoosePointWithLargest(int XYZ, vtkPointSet* pd)
{
	// XYZ: See ChoosePointWithSmallest

	if (pd->GetNumberOfPoints() <= 0)
		return -1;

	double tempPoint[3];
	pd->GetPoint(0, tempPoint);
	double max = tempPoint[XYZ];
	int maxindex = 0;

	for (int i = 1; i < pd->GetNumberOfPoints(); i++)
	{
		pd->GetPoint(i, tempPoint);
		if (tempPoint[XYZ] > max)
		{
			max = tempPoint[XYZ];
			maxindex = i;
		}
	}

	return maxindex;
}

int StaticFunctionLibrary::CenterlineIntersectionWithPlane(vtkPolyData* Centerline, double  Normal[3], double  Pt[3], double  IntersectionPoint[3])
{
	//Assume only the part representing the ICA is included
	//Thus, Centerline should have only one line cell.
	//This function ignore all other than the first cell.

	int npts, *pts;

	Centerline->GetLines()->InitTraversal();
	Centerline->GetLines()->GetNextCell(npts, pts);
	
	double startPt[3], endPt[3];
	double t;

	for (int i = 0; i < npts-1; i++)
	{
		Centerline->GetPoint(pts[i], startPt);
		Centerline->GetPoint(pts[i+1], endPt);
		if (vtkPlane::IntersectWithLine(startPt, endPt, Normal, Pt, t, IntersectionPoint))
		{
			return 1;
		}
	}

	return 0;
}

int StaticFunctionLibrary::DetermineReferenceFrame(double  z[3], double  Pt[3], vtkMatrix4x4* matrix)
{
	// Determine XYZ

	double MaxZ = AbsValue(z[0]);
	int XYZ = 0;

	int i;
	for (i = 1; i < 3; i++)
	{
		if (AbsValue(z[i]) > MaxZ)
		{
			MaxZ = AbsValue(z[i]);
			XYZ = i;
		}
	}

	if (z[XYZ] < 0)
	{
		z[0] *= -1;
		z[1] *= -1;
		z[2] *= -1;
	}

	double xlocal[3] = {0, 0, 0};

	switch (XYZ)
	{
	case 0:
		if (z[1] == 0)
		{
            //xlocal[0] = xlocal[2] = 0;
			xlocal[1] = 1;
		}
		else
		{
			xlocal[0] = 1;
			xlocal[1] = -z[0]/z[1];
			//xlocal[2] = 0;

			if (xlocal[1] < 0)
			{
				xlocal[0] *= -1;
				xlocal[1] *= -1;
				xlocal[2] *= -1;
			}
		}
		break;
	case 1:
		if (z[2] == 0)
		{
            xlocal[2] = 1;
		}
		else
		{
			xlocal[1] = 1;
			xlocal[2] = -z[1]/z[2];

			if (xlocal[2] < 0)
			{
				xlocal[0] *= -1;
				xlocal[1] *= -1;
				xlocal[2] *= -1;
			}
		}
		break;
	case 2:
		xlocal[0] = 1;
		xlocal[2] = -z[0]/z[2];
		break;
	}

	double y[3];
	vtkMath::Normalize(xlocal);
	vtkMath::Normalize(z);
	vtkMath::Cross(z, xlocal, y);

	matrix->Identity();
	
	int j;
	
	for (j = 0; j < 3; j++)
	{
		matrix->SetElement(j, 0, xlocal[j]);
		matrix->SetElement(j, 1, y[j]);
		matrix->SetElement(j, 2, z[j]);
		matrix->SetElement(j, 3, Pt[j]);
	}

	return XYZ;
}

void StaticFunctionLibrary::FindPlaneCoordinates(double  ImageCubeBounds[6], vtkMatrix4x4* matrix, double  ResliceOrigin[3], double  ReslicePoint1[3], double  ReslicePoint2[3])
{
	int i;
	double diagonalLength2 = 0;
	double temp;
	double Length[3];
	double localx[3], localy[3], localz[3], Pt[3];

	for (i = 0; i < 3; i++)
	{
		localx[i] = matrix->GetElement(i, 0);
		localy[i] = matrix->GetElement(i, 1);
		localz[i] = matrix->GetElement(i, 2);
		Pt[i] = matrix->GetElement(i, 3);
	}

	for (i = 0; i < 3; i++)
	{
		Length[i] = ImageCubeBounds[2*i+1] - ImageCubeBounds[2*i];

		if (Length[i] < 0)
		{
			temp = ImageCubeBounds[2*i+1];
			ImageCubeBounds[2*i+1] = ImageCubeBounds[2*i];
			ImageCubeBounds[2*i] = temp;
			Length[i] *= -1;
		}

		diagonalLength2 += Length[i]*Length[i];
	}

	double MaxZ = AbsValue(localz[0]);
	int XYZ = 0;

	for (i = 1; i < 3; i++)
	{
		if (AbsValue(localz[i]) > MaxZ)
		{
			MaxZ = AbsValue(localz[i]);
			XYZ = i;
		}
	}

	double astart, aend;
	double p1[3], p2[3], p3[3], p4[3], p5[3], p6[3], p7[3], p8[3];

	switch (XYZ)
	{
	case 0:
		astart = ImageCubeBounds[0] - diagonalLength2;
		aend = ImageCubeBounds[1] + diagonalLength2;
        p1[0] = p3[0] = p5[0] = p7[0] = astart;
		p2[0] = p4[0] = p6[0] = p8[0] = aend;
		p1[1] = p2[1] = p5[1] = p6[1] = ImageCubeBounds[2];
		p3[1] = p4[1] = p7[1] = p8[1] = ImageCubeBounds[3];
		p1[2] = p2[2] = p3[2] = p4[2] = ImageCubeBounds[4];
		p5[2] = p6[2] = p7[2] = p8[2] = ImageCubeBounds[5];
		break;
	case 1:
		astart = ImageCubeBounds[2] - diagonalLength2;
		aend = ImageCubeBounds[3] + diagonalLength2;
		p1[1] = p3[1] = p5[1] = p7[1] = astart;
		p2[1] = p4[1] = p6[1] = p8[1] = aend;
        p1[0] = p2[0] = p3[0] = p4[0] = ImageCubeBounds[0];
		p5[0] = p6[0] = p7[0] = p8[0] = ImageCubeBounds[1];
		p1[2] = p2[2] = p5[2] = p6[2] = ImageCubeBounds[4];
		p3[2] = p4[2] = p7[2] = p8[2] = ImageCubeBounds[5];
		break;
	case 2:
		astart = ImageCubeBounds[4] - diagonalLength2;
		aend = ImageCubeBounds[5] + diagonalLength2;
		p1[2] = p3[2] = p5[2] = astart;
		p2[2] = p4[2] = p6[2] = aend;
        p1[0] = p2[0] = p5[0] = p6[0] = ImageCubeBounds[0];
		p3[0] = p4[0] = p7[0] = p8[0] = ImageCubeBounds[1];
		p1[1] = p2[1] = p3[1] = p4[1] = ImageCubeBounds[2];
		p5[1] = p6[1] = p7[1] = p8[1] = ImageCubeBounds[3];
		break;
	}

	double t;

	if (!vtkPlane::IntersectWithLine(p1, p2, localz, Pt, t, ResliceOrigin) ||
		!vtkPlane::IntersectWithLine(p3, p4, localz, Pt, t, ReslicePoint1) ||
		!vtkPlane::IntersectWithLine(p5, p6, localz, Pt, t, ReslicePoint2))
	{
		RotateDlg::CustomMessageBox("vtkPlane::IntersectWithLine failed");
	}

	//test
	double v[3];
	for (i = 0; i < 3; i++)
	{
		v[i] = ReslicePoint1[i] - ResliceOrigin[i];
	}
	vtkMath::Normalize(v);

	double test = vtkMath::Dot(v, localx);
}

double StaticFunctionLibrary::sinc(double x)
{
	if (x == 0)
		return 1;

	return sin(vtkMath::Pi()*x)/(vtkMath::Pi()*x);
}

BOOL StaticFunctionLibrary::IsInsideContour(vtkPolyData* InputContour, double  PointInQuestion[2])
{
	// Use Polygon Winding Number algorithm
	// see http://softsurfer.com/Archive/algorithm_0103/algorithm_0103.htm
	// Assumption: vertices[0] != vertices[N-1], N = Number of Vertices
	// 2D calculation, ignore z-component

	// See David G. Alciatore, "A Winding Number and Point-in-Polygon Algorithm", Fig. 3.

	vtkCellArray* line = InputContour->GetLines();
	int npts, *pts;
	// Assume line only has one cell.
	line->InitTraversal(); line->GetNextCell(npts, pts);
	if (pts[0] == pts[npts-1]) npts--;

	int i, iplus1; 
	bool isLeft; float isLeftCalculation;
	double P0P1[2], P0P2[2];
	float wn = 0; //Winding number counter
	double thisPoint[3], nextPoint[3];
	int N = npts;	
	vtkPoints* vertices = InputContour->GetPoints();
	
	for(i = 0; i < N; i++)
	{		
		iplus1 = (i+1) % N;  // next point index
		vertices->GetPoint(pts[i], thisPoint); // current point
		vertices->GetPoint(pts[iplus1], nextPoint); // next point

		// Note that a horizontal ray extending from PointInQuestion to its right is established.
		// (i.e., the ray is parallel to the positive x-axis)

		if ((thisPoint[1] <= PointInQuestion[1] && nextPoint[1] >= PointInQuestion[1]) ||
			(thisPoint[1] >= PointInQuestion[1] && nextPoint[1] <= PointInQuestion[1])) 
		{
			P0P1[0] = nextPoint[0] - thisPoint[0];
			P0P1[1] = nextPoint[1] - thisPoint[1];

			P0P2[0] = PointInQuestion[0] - thisPoint[0];
			P0P2[1] = PointInQuestion[1] - thisPoint[1];
			
			isLeftCalculation = P0P1[0] * P0P2[1] - P0P1[1] * P0P2[0];
			if (isLeftCalculation == 0) continue; //Case 5;
			
			isLeft = (isLeftCalculation > 0) ? TRUE: FALSE;
	
		}
		else continue;

		if (isLeft)
		{
            if (thisPoint[1] < PointInQuestion[1] && nextPoint[1] > PointInQuestion[1]) 
			{ // Detected an upward crossing and since isLeft is TRUE, the edge intersect the ray.
				wn = wn + 1.0; //Case 1;
			}
			else if ((thisPoint[1] < PointInQuestion[1] && nextPoint[1] == PointInQuestion[1])||
				     (thisPoint[1] == PointInQuestion[1] && nextPoint[1] > PointInQuestion[1]))
			{
				wn = wn + 0.5; //Case 3;
			}
		}
        else //!isLeft 
		{
			if (thisPoint[1] > PointInQuestion[1] && nextPoint[1] < PointInQuestion[1]) 
			{ 
				wn = wn - 1.0; //Case 2
			}
			else if ((thisPoint[1] > PointInQuestion[1] && nextPoint[1] == PointInQuestion[1]) ||
				     (thisPoint[1] == PointInQuestion[1] && nextPoint[1] < PointInQuestion[1]))
			{
				wn = wn - 0.5; //Case 4
			}
		}
	}

	wn = (wn > 0)?wn:-wn;

	if (wn > 0) return TRUE;
	else return FALSE;
}

void StaticFunctionLibrary::GrabOnlyOneLine(vtkPolyData *in, vtkPolyData *out, int LineNum)
{
	// assume in and out have already been allocated

	int i;
	vtkCellArray* inLines = in->GetLines();

	if (!inLines->GetNumberOfCells())
	{
		return;
	}
	if (LineNum >= inLines->GetNumberOfCells())
	{
		return;
	}

	vtkPolyData* outTemp = vtkPolyData::New();
	outTemp->DeepCopy(in);

	vtkCellArray* outLines = vtkCellArray::New();
	outTemp->SetLines(outLines);
	outLines->Delete();

	int npts, *pts;

	inLines->InitTraversal();

	for (i = 0; i <= LineNum; i++)
	{
		inLines->GetNextCell(npts, pts);		
	}
	
	outLines->InsertNextCell(npts, pts);

	// clean
	vtkCleanPolyData* clean = vtkCleanPolyData::New();
	clean->SetInput(outTemp);
	clean->ConvertLinesToPointsOff();
	clean->ConvertPolysToLinesOff();
	clean->ConvertStripsToPolysOff();
	clean->PointMergingOff();
	clean->Update();

	out->DeepCopy(clean->GetOutput());

	//delete
	outTemp->Delete();
	clean->Delete();	
}

void InitialImage(vtkImageData* imdata, double Origin[3], double Spacing[3], int Extent[6], float InitialValue)
{
	// Assume that imdata has already been initialized.

	int Dimensions[3];
	int TotalNumberOfPixels = 1;
	for (int i = 0; i < 3; i++)
	{
		Dimensions[i] = Extent[2*i+1]-Extent[2*i];
        TotalNumberOfPixels *= Dimensions[i];	
	}
	imdata->SetDimensions(Dimensions);
	imdata->SetExtent(Extent);
	imdata->SetOrigin(Origin);
	imdata->SetSpacing(Spacing);

	imdata->SetScalarTypeToFloat();
	imdata->SetNumberOfScalarComponents(1);
	imdata->AllocateScalars();

	float* ptr = (float*) imdata->GetScalarPointer();

	for (i = 0; i < TotalNumberOfPixels; i++)
	{
		*ptr++ = InitialValue; 
	}
}

void StaticFunctionLibrary::FindNormal(vtkPolyData *OneSlice, double normal[3])
{
	// OneSlice is assumed to have exactly one line;
	// The direction of the normal is in the same direction as the curl of the contour

	double C1_01[3] = {0, 0, 0};
	double C1_02[3] = {0, 0, 0};

	int *pts1, n1;

	vtkCellArray* tempCellArray1 = OneSlice->GetLines();
	vtkCellArray* tempCellArray2 = OneSlice->GetPolys();

	vtkPolyData* IntermediatePolyData = vtkPolyData::New();

	if (tempCellArray1->GetNumberOfCells() == 0 &&
		tempCellArray2->GetNumberOfCells() > 0)
	{
		StaticFunctionLibrary::PolygonsToLines(OneSlice, IntermediatePolyData);	
		tempCellArray1 = IntermediatePolyData->GetLines();
	}

	tempCellArray1->InitTraversal();
	tempCellArray1->GetNextCell(n1, pts1);
	if (pts1[0] == pts1[n1 - 1]) n1--;
	
	double C1point1[3], C1point2[3], C1point3[3]; 
	
	OneSlice->GetPoints()->GetPoint(pts1[0], C1point1);
	OneSlice->GetPoints()->GetPoint(pts1[n1/3], C1point2);
	OneSlice->GetPoints()->GetPoint(pts1[2*n1/3], C1point3);

	C1_01[0] = C1point2[0] - C1point1[0];
	C1_01[1] = C1point2[1] - C1point1[1];
	C1_01[2] = C1point2[2] - C1point1[2];

	C1_02[0] = C1point3[0] - C1point1[0];
	C1_02[1] = C1point3[1] - C1point1[1];
	C1_02[2] = C1point3[2] - C1point1[2];
	
	vtkMath::Cross(C1_01, C1_02, normal);

	IntermediatePolyData->Delete();
}

void StaticFunctionLibrary::PolygonsToLines(vtkPolyData *InputPoly, vtkPolyData *OutputPoly)
{
    // This function converts the polys cells to lines cells
	OutputPoly->DeepCopy(InputPoly);

	if (InputPoly->GetPolys()->GetNumberOfCells() == 0)
	{
		return;
	}

	vtkCellArray* outputPolyCell = vtkCellArray::New();
	outputPolyCell->DeepCopy(InputPoly->GetLines());
	
	InputPoly->GetPolys()->InitTraversal();

	int npts, *pts;
	int i, j;

	for (i = 0; i < InputPoly->GetPolys()->GetNumberOfCells(); i++) 
	{
		InputPoly->GetPolys()->GetNextCell(npts, pts);
		if (pts[0] != pts[npts-1]) 
		{// for each cell, if pts[npts-1] ! = pts[0], set so.
			outputPolyCell->InsertNextCell(npts + 1);
			for (j = 0; j < npts; j++){
				outputPolyCell->InsertCellPoint(pts[j]);
			}
			outputPolyCell->InsertCellPoint(pts[0]);
		}
		else {
			outputPolyCell->InsertNextCell(npts, pts);
		}
	}

	OutputPoly->SetLines(outputPolyCell);
	OutputPoly->SetPolys(NULL);
	outputPolyCell->Delete();
}

void StaticFunctionLibrary::InitialImage(vtkImageData* imdata, double Origin[3], double Spacing[3], int Extent[6], double InitialValue, int DataType)
{
	// Assume that imdata has already been initialized.

	int Dimensions[3];
	int TotalNumberOfPixels = 1;
	for (int i = 0; i < 3; i++)
	{
		Dimensions[i] = Extent[2*i+1]-Extent[2*i]+1;
        TotalNumberOfPixels *= Dimensions[i];	
	}
	imdata->SetDimensions(Dimensions);
	imdata->SetExtent(Extent);
	imdata->SetOrigin(Origin);
	imdata->SetSpacing(Spacing);

	imdata->SetScalarType(DataType);
	imdata->SetNumberOfScalarComponents(1);
	imdata->AllocateScalars();

	for (i = 0; i < TotalNumberOfPixels; i++)
	{
		imdata->GetPointData()->GetScalars()->SetTuple1(i, InitialValue);		 
	}
}

void StaticFunctionLibrary::WritePolyData(vtkPolyData *input, const char* filename)
{
	vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
	writer->SetInput(input);
	writer->SetFileName(filename);
	writer->Write();
	writer->Delete();
}

void StaticFunctionLibrary::WriteXMLImageData(vtkImageData* input, const char* filename)
{
	vtkXMLImageDataWriter* vtkxmlwriter = vtkXMLImageDataWriter::New();
	vtkxmlwriter->SetInput(input);
	vtkxmlwriter->SetFileName(filename);
	vtkxmlwriter->Write();
	vtkxmlwriter->Delete();
}

void StaticFunctionLibrary::PolygonAreaVector(vtkPoints *inPts, vtkIdType npts, vtkIdType *pts, double Area[3])
{
	// pts should contain the id of points according to the order of inPts
	if (pts[0] == pts[npts - 1]) npts--;
	//remove duplicate points
	
	double crossi[3];

	Area[0] = 0.0; Area[1] = 0.0; Area[2] = 0.0;

	double point1[3], point2[3];

	for (int i = 0; i < npts; i++) 
	{
		inPts->GetPoint(pts[i],point1);
		inPts->GetPoint(pts[(i+1)%npts],point2);
		vtkMath::Cross(point1, point2, crossi);
		Area[0] += 0.5f*crossi[0];
		Area[1] += 0.5f*crossi[1];
		Area[2] += 0.5f*crossi[2];
	}
}

double StaticFunctionLibrary::PolygonArea(vtkPoints* Points, vtkIdType npts, vtkIdType* pts)
{
	double Area[3];
	StaticFunctionLibrary::PolygonAreaVector(Points, npts, pts, Area);
	return vtkMath::Norm(Area);
}

void StaticFunctionLibrary::ComputeAreaVectorForOneLine(vtkPolyData* pd, int LineNum, double Area[3])
{
	vtkIdType npts, *pts;

	if (LineNum >= pd->GetNumberOfLines())
	{
		for (int i = 0; i < 3; i++)
		{
			Area[i] = 0.0;
		}
		return;
	}

	vtkIdType Offset = pd->GetNumberOfVerts();

	pd->BuildCells();
	pd->GetCellPoints(LineNum + Offset, npts, pts);

	StaticFunctionLibrary::PolygonAreaVector(pd->GetPoints(), npts, pts, Area);
}

void StaticFunctionLibrary::CopyArraysFromInputPointData(vtkPointData *inputPointData, vtkPointData *outputPointData)
{

	// Notice that CopyArraysFromInputPointData(InPointData, OutPointData) must have been called.
	// before calling StaticFunctionLibrary::ComputeAndInsertPointData
	// This function only creates arrays that has the same name as inPointData, it will not 
	// copy data.
	
	//  

	int i;	
	int numberOfArrays = inputPointData->GetNumberOfArrays();

	if (numberOfArrays == 0) return;

	vtkDataArray** outputDataArrays = new vtkDataArray*[numberOfArrays];
	
	const char* ArrayName;
	vtkDataArray* da;
	int whichAttribute, arrayIndex;

	// Here we only want to match name

	// Step 1: Remove those arrays that couldn't be found in inputPointData

	for (i = 0; i < outputPointData->GetNumberOfArrays(); i++)
	{
		ArrayName = outputPointData->GetArrayName(i);
		da = inputPointData->GetArray(ArrayName);
		if (!da)
		{
			outputPointData->RemoveArray(ArrayName);
		}
	}

	// Step 2: for each array at inputPointData -- either initialize it for outputPointData or not,
	// depending on whether the array exists in outputPointData

	vtkDataArray* da1;
	//copy the arrays from inputPointData;
	for (i = 0; i < numberOfArrays; i++)
	{
		ArrayName = inputPointData->GetArrayName(i);
		da1 = outputPointData->GetArray(ArrayName, arrayIndex);

		// if ArrayName is blank, there is no way that we can recognize the array later
		// so don't add.

		if (!da1 && ArrayName)
		{
			da = inputPointData->GetArray(i);
			outputDataArrays[i] = da->NewInstance();
 			outputDataArrays[i]->SetName(ArrayName);
 			outputDataArrays[i]->SetNumberOfComponents(da->GetNumberOfComponents());

			arrayIndex = outputPointData->AddArray(outputDataArrays[i]);
			outputDataArrays[i]->Delete();
		}

		if ((whichAttribute = inputPointData->IsArrayAnAttribute(i)) >= 0)
		{
			outputPointData->SetActiveAttribute(arrayIndex, whichAttribute);
		}
	}

	delete [] outputDataArrays;
}

int StaticFunctionLibrary::SameOrientation(vtkPolyData *p1, vtkPolyData *p2)
{
	// Test whether the two input contours are of the same orientation.
	// Input: two contour slices, which means p1 and p2 only have lines, 
	// which are an one-element cell array.

	int *pts1, n1;
	int *pts2, n2;

	vtkCellArray* tempCellArray1 = p1->GetLines();
	vtkCellArray* tempCellArray2 = p2->GetLines();
	tempCellArray1->InitTraversal();
	tempCellArray2->InitTraversal();
	tempCellArray1->GetNextCell(n1, pts1);
	tempCellArray2->GetNextCell(n2, pts2);
	if (pts1[0] == pts1[n1 - 1]) n1--;
	if (pts2[0] == pts2[n2 - 1]) n2--;

	double AreaP1[3], AreaP2[3];
	StaticFunctionLibrary::PolygonAreaVector(p1->GetPoints(), n1, pts1, AreaP1);
	StaticFunctionLibrary::PolygonAreaVector(p2->GetPoints(), n2, pts2, AreaP2);

	float dotProd;
	if ((dotProd = vtkMath::Dot(AreaP1, AreaP2)) > 0) 
		return 1;
	else
		return 0;

}

void StaticFunctionLibrary::ComputeAndInsertPointData(vtkPointData *inputPointData, vtkPointData *outputPointData, 
													  int PtId1, int PtId2, double t, int Index)
{
	// This function retrieves data point values of PtId1 and PtId2 from inputPointData
	// Calculate the interpolated value according to t
	// insert the computed value into outputPointData(Index)

	// Notice that this->CopyArraysFromInputPointData(InPointData, OutPointData) must have been called.

	int i, j;
	int numberOfArrays = outputPointData->GetNumberOfArrays();

	if (numberOfArrays == 0) return;
	
	const char* ArrayName;
	vtkDataArray* da, *Arrayi;
	double* TuplePtId1, *TuplePtId2, *TupleIndex;
	int numberOfComponentsda;

	for (i = 0; i < numberOfArrays; i++)
	{
		Arrayi = outputPointData->GetArray(i);
		ArrayName = outputPointData->GetArrayName(i);
		da = inputPointData->GetArray(ArrayName);
		
		if (!da) continue;

		numberOfComponentsda = da->GetNumberOfComponents();

		TuplePtId1 = new double [numberOfComponentsda];
		TuplePtId2 = new double [numberOfComponentsda];
		TupleIndex = new double [numberOfComponentsda];

		da->GetTuple(PtId1, TuplePtId1);
		da->GetTuple(PtId2, TuplePtId2);

		for (j = 0; j < numberOfComponentsda; j++)
		{
			TupleIndex[j] = (1 - t)*TuplePtId1[j] + t*TuplePtId2[j];
		}

		Arrayi->InsertTuple(Index, TupleIndex);

		delete [] TuplePtId1;
		delete [] TuplePtId2;
		delete [] TupleIndex;
	}
}

void StaticFunctionLibrary::ComputeAndInsertPointData(vtkPointData *inputPointData, vtkPointData *outputPointData, 
													  vtkIdList* PtIds, double* weight, int Index)
{
	// Jun 1 2009: An extension of the previous function
	// Interpolation of more than 2 points is allowed.

	// Notice that this->CopyArraysFromInputPointData(InPointData, OutPointData) must have been called.

	int i, j, k;
	int numberOfArrays = outputPointData->GetNumberOfArrays();

	if (numberOfArrays == 0) return;
	
	const char* ArrayName;
	vtkDataArray* da, *Arrayi;
	double* tempTuple, *TupleIndex;
	int numberOfComponentsda;

	for (i = 0; i < numberOfArrays; i++)
	{
		Arrayi = outputPointData->GetArray(i);
		ArrayName = outputPointData->GetArrayName(i);
		da = inputPointData->GetArray(ArrayName);
		if (!da) continue;
		numberOfComponentsda = da->GetNumberOfComponents();

//#define 	VTK_VOID   0
//#define 	VTK_BIT   1
//#define 	VTK_CHAR   2
//#define 	VTK_UNSIGNED_CHAR   3
//#define 	VTK_SHORT   4
//#define 	VTK_UNSIGNED_SHORT   5
//#define 	VTK_INT   6
//#define 	VTK_UNSIGNED_INT   7
//#define 	VTK_LONG   8
//#define 	VTK_UNSIGNED_LONG   9
//#define 	VTK_FLOAT   10
//#define 	VTK_DOUBLE   11
//#define 	VTK_ID_TYPE   12

        tempTuple = new double [numberOfComponentsda];
		TupleIndex = new double [numberOfComponentsda];
		
		for (k = 0; k < numberOfComponentsda; k++)
		{
			TupleIndex[k] = 0;
		}
	
		for (j = 0; j < PtIds->GetNumberOfIds(); j++)
		{
			da->GetTuple(PtIds->GetId(j), tempTuple);
			for (k = 0; k < numberOfComponentsda; k++)
			{
				TupleIndex[k] += weight[j]*tempTuple[k];
			}
		}

		int datatype = da->GetDataType();

		if (datatype < VTK_FLOAT || datatype > VTK_DOUBLE)
		{
			// not float or double, then round
			for (k = 0; k < numberOfComponentsda; k++)
			{
				TupleIndex[k] = MRound(TupleIndex[k]);
			}
		}

		Arrayi->InsertTuple(Index, TupleIndex);

		delete [] tempTuple;
		delete [] TupleIndex;
	}
}

double StaticFunctionLibrary::ArcLength(vtkPolyData * p, int closed)
{
	//assume p has only one lines cell
	vtkCellArray* tempCellArray = p->GetLines();
	tempCellArray->InitTraversal();
	int npts, *pts;
	tempCellArray->GetNextCell(npts, pts);// Get the first and the only cell

	if (pts[0] == pts[npts-1])
	{ 
		npts--;
	}

	int i = 0; 
	double firstPoint[3], secondPoint[3];
	double sum = 0;

	int nptsForLoop = (closed)?npts:npts-1;
	// if the curve is closed, the segment from pts[N-1] to pts[0] is counted. Otherwise not counted.

	for (i = 0; i < nptsForLoop; i++) 
	{
		p->GetPoints()->GetPoint(pts[i], firstPoint);
		p->GetPoints()->GetPoint(pts[(i+1)%npts], secondPoint);

		sum += sqrt(vtkMath::Distance2BetweenPoints(firstPoint, secondPoint));
	}

	return sum;
}

void StaticFunctionLibrary::OrderPolyData(vtkPolyData *InputPoly, vtkPolyData *OutputPoly, double* referenceDir)//, float* zCoorArray)
{
	// referenceDir is either NULL or the vector pointing to the desire direction which
	// specifies the order of the polydata
	// Note that any inputPoly could be order in 2 ways
	// if referenceDir is not given (NULL) then inputPoly will be ordered in the +z direction
	// otherwise inputPoly will be ordered according to the direction specified.

	OutputPoly->DeepCopy(InputPoly);

	double referenceDirReal[3] = {0, 0, 1};
	
	if (referenceDir)
	{
		// referenceDir is given, copy it to our local vars.
		referenceDirReal[0] = referenceDir[0];
		referenceDirReal[1] = referenceDir[1];
		referenceDirReal[2] = referenceDir[2];
	}

	// 

	InputPoly->BuildCells();

	// assume the input polydata are made up of lines (not polygons)
	int num = InputPoly->GetLines()->GetNumberOfCells();
	int i = 0;
	int npts, *pts;
	double* zCoorArray = new double [num];
	int* index = new int [num];
	InputPoly->GetLines()->InitTraversal();
	
	double FirstSliceNormal[3];

	StaticFunctionLibrary::FindNormal(InputPoly, FirstSliceNormal, 0);
	vtkMath::Normalize(FirstSliceNormal);

	if (vtkMath::Dot(FirstSliceNormal,referenceDirReal) < 0) 
	{
		FirstSliceNormal[0] *= -1;
		FirstSliceNormal[1] *= -1;
		FirstSliceNormal[2] *= -1;
	}	

	double tempPoint[3];
	for (i = 0; i < num; i++) 
	{
		InputPoly->GetCellPoints(i, npts, pts);
		// dot product with z unit vector
		InputPoly->GetPoint(pts[0], tempPoint);
		zCoorArray[i] = vtkMath::Dot(FirstSliceNormal, tempPoint);
		// don't use zCoor, use dot products.
		index[i] = i;
	}

	StaticFunctionLibrary::SimpleSort(zCoorArray, index, num);

	vtkCellArray* outputCellArray = vtkCellArray::New();
	
	outputCellArray->InitTraversal();

	for (i = 0; i < num; i++) 
	{
		InputPoly->GetCellPoints(index[i], npts, pts);
		outputCellArray->InsertNextCell(npts, pts);
	}

	OutputPoly->SetLines(outputCellArray);
	outputCellArray->Delete();
		
	delete [] zCoorArray;
	delete [] index;
}

void StaticFunctionLibrary::SimpleSort(double *sequence, int *index, int num)
{
	// arrange sequence of length num in ascending order
	int i, j, temp2;
	double temp1;
	
	//test
	//for (i = 0; i < num; i++) {
	//	TRACE ("%i:%f\n", i, sequence[i]);
	//}
	for (i = 0; i < num; i++)
	{
		index[i] = i;
	}
	
	for (i = 0; i < num; i++)
	{
		for (j = i + 1; j < num; j++) 
		{
			if (sequence[j] < sequence[i]) 
			{
				temp1 = sequence[i];
				sequence[i] = sequence[j];
				sequence[j] = temp1;
				temp2 = index[i];
				index[i] = index[j];
				index[j] = temp2;
			}
		}
	}
}

int StaticFunctionLibrary::FindAllIntersection(vtkPolyLine *pline, vtkPoints* outputP, vtkIdList* subIdArray, double startpoint[], double endpoint[], double tol)
{
	// outputP and subIdArray will be initialize here.
	// check vtkPolyLine to be a closed curve

	// known problem - Nov 14 2005:
	// if the second point of one line (r = 1) has a perpendicular distance to pline of smaller than tol,
	// and the angle between the next line and pline is less than 45 degree, and that the real intersection
	// is somewhere in the next line, then both the real intersection and the projection from the second point
	// of the previous line on pline are treated as intersection. Thus, the intersection is doubly 
	// counted.

	int n = pline->GetNumberOfPoints();
	// if not, then make it closed.
	if (pline->GetPointId(0) != pline->GetPointId(n-1)) 
	{
		pline->GetPointIds()->InsertId(n, pline->GetPointId(0));
		pline->GetPoints()->InsertPoint(n, pline->GetPoints()->GetPoint(0));
		n++;
	}

	double t, pcoords[3];
	int subId;
	int intersectionPoints = 0;
	vtkPolyLine* tempPolyLine = vtkPolyLine::New();
	tempPolyLine->DeepCopy(pline);
	int npoints = n - 1; //number of non-duplicated point
	int starti = 0;
	double x1[3], tempPoint[3];

	outputP->Initialize();
	subIdArray->Initialize();

	double minDist, dist;
	int i;
	double u, v;
	int goToAddPoint;
	int IsVtkYesIntersection;
	vtkPoints* tempPoints = tempPolyLine->GetPoints();

	double xtemp[3], ytemp[3];
	
	for (int step = 0; TRUE; step++) 
	{

		int success1 = tempPolyLine->IntersectWithLine(startpoint, endpoint, tol, t, x1, pcoords, subId);
		starti += subId + 1;
		
		// Two consecutive intersections could not have a distance less than tolerance between them.

		if (!success1) break; 

		goToAddPoint = 0;
		minDist = VTK_DOUBLE_MAX;

		//throw away those point if there is not intersection, but success1 only because 
		//one of the end point of the line is too close to the pline

		tempPoints->GetPoint(subId, xtemp);
		tempPoints->GetPoint(subId + 1, ytemp);

		IsVtkYesIntersection = (vtkLine::Intersection(startpoint, endpoint, xtemp, ytemp, u, v) == VTK_YES_INTERSECTION);
 
		if (IsVtkYesIntersection)
		{
			if ((intersectionPoints == 0)) 
			{
				goToAddPoint = 1;
			}
			else
			{
				for (i = 0; i < outputP->GetNumberOfPoints(); i++)
				{
					dist = vtkMath::Distance2BetweenPoints(x1, outputP->GetPoint(i));
					if (dist < minDist)
					{
						minDist = dist;
					}
				}
					
				if (minDist > tol*tol)
				{
					goToAddPoint = 1;
				}
			}
		}
	
		if (goToAddPoint)
		{
			outputP->InsertNextPoint(x1);
			subIdArray->InsertNextId(starti - 1);
			intersectionPoints++;
		}

		if (starti < npoints) {
			tempPolyLine->GetPoints()->Initialize();
			tempPolyLine->GetPointIds()->Initialize();
			tempPolyLine->GetPoints()->SetNumberOfPoints(n - starti);
			tempPolyLine->GetPointIds()->SetNumberOfIds(n - starti);

			for (int j = starti; j < n; j++) {
				pline->GetPoints()->GetPoint(j, tempPoint);
				tempPolyLine->GetPoints()->SetPoint(j-starti, tempPoint);
				tempPolyLine->GetPointIds()->SetId(j-starti, pline->GetPointId(j));
			}
		
		}
		else {
			break;
		}
	} 

	tempPolyLine->Delete();
	return intersectionPoints;
}

void StaticFunctionLibrary::Centroid(vtkPolyData *pd, double c[])
{
 	//VERIFY(pd->GetNumberOfLines() == 1);

	// only treat the first line
 
 	int i;
 	int npts, *pts;
 	double tempPoint[3];
 	vtkCellArray* lines = pd->GetLines();
 	lines->InitTraversal();
 	VERIFY (lines->GetNextCell(npts, pts));
 	if (pts[0] == pts[npts - 1]) npts--;
 
 	c[0] = 0.0f; c[1] = 0.0f; c[2] = 0.0f;
 
 	for (i = 0; i < npts; i++) 
	{
 		pd->GetPoints()->GetPoint(pts[i], tempPoint);
 		c[0] += tempPoint[0];
 		c[1] += tempPoint[1];
 		c[2] += tempPoint[2];
 	}

	c[0] /= static_cast<double>(npts);
	c[1] /= static_cast<double>(npts);
	c[2] /= static_cast<double>(npts);

}

void StaticFunctionLibrary::ResampleByArcLength(vtkPolyData *curvein, vtkPolyData *curveout, 
												int NumberOfSamples, int closed)
{
	double resolution = StaticFunctionLibrary::ArcLength(curvein, closed)/NumberOfSamples;

	StaticFunctionLibrary::ResampleByArcLength(curvein, curveout, resolution, closed);

}
void StaticFunctionLibrary::ResampleStackOfLines(vtkPolyData* input, vtkPolyData* output, double SamplingInterval)
{
	// This function resamples a stack of contour. Each contour is uniformly sampled into in SamplingInterval
	// interval.

	// First if the input does not contain lines and only polygons, convert ...

	int i, j;
	vtkPolyData* OutputTemp1Polys = vtkPolyData::New();

	if (input->GetLines()->GetNumberOfCells() == 0) 
	{
		StaticFunctionLibrary::PolygonsToLines(input, OutputTemp1Polys);
	}
	else 
	{
		OutputTemp1Polys->DeepCopy(input);
	}

	vtkCellArray* inputLines = OutputTemp1Polys->GetLines();
	int NumberOfLines = inputLines->GetNumberOfCells();
	
	int t = 0;
	int npts, *pts;
	int npts2, *pts2;
	double tempPoint[3];
	int offset = 0;

	vtkPolyData* oneSlice = vtkPolyData::New();
	vtkCellArray* oneSliceLines = vtkCellArray::New();
	oneSlice->SetLines(oneSliceLines);
	oneSliceLines->Delete();
	vtkPoints* oneSlicePoints = vtkPoints::New();
	oneSlicePoints->DeepCopy(input->GetPoints());
	oneSlice->SetPoints(oneSlicePoints);
	oneSlicePoints->Delete();

	vtkPolyData* ResampledoneSlice = vtkPolyData::New();

	// set up output
	vtkCellArray* outputLines = vtkCellArray::New();
	vtkPoints* outputPoints = vtkPoints::New();
	output->SetLines(outputLines);
	output->SetPoints(outputPoints);
	outputLines->Delete();
	outputPoints->Delete();
	int NumberOfSamples;

	for (i = 0; i < NumberOfLines; i++)
	{
		inputLines->SetTraversalLocation(t);
		inputLines->GetNextCell(npts, pts);
		t = inputLines->GetTraversalLocation();

		oneSliceLines->Reset();
		oneSliceLines->InsertNextCell(npts, pts);

		ResampledoneSlice->Initialize();
		NumberOfSamples = (int) ceil(StaticFunctionLibrary::ArcLength(oneSlice)/SamplingInterval);
		StaticFunctionLibrary::ResampleByArcLength(oneSlice, ResampledoneSlice, NumberOfSamples);

		ResampledoneSlice->GetLines()->InitTraversal();
		ResampledoneSlice->GetLines()->GetNextCell(npts2, pts2);

		outputLines->InsertNextCell(npts2 + 1);
		for (j = 0; j < npts2; j++)
		{
			ResampledoneSlice->GetPoint(pts2[j], tempPoint);
			outputPoints->InsertPoint(offset + j, tempPoint);
			outputLines->InsertCellPoint(offset + j);
		}
		outputLines->InsertCellPoint(offset);

		offset += npts2;
	}

	//delete
	OutputTemp1Polys->Delete();
	oneSlice->Delete();
	ResampledoneSlice->Delete();

}

//void StaticFunctionLibrary::LeastSquare3DLineMATLAB(vtkPoints* X, double x0[3], double a[3])
//{
//		// forced to call matlab to calculate the singular value decomposition.
//	Engine *ep;
//	mxArray *X_matlab_mxArray = NULL;
//	int retstatus;
//
//	if (!(ep = engOpenSingleUse("\0", NULL, &retstatus))) {
//		fprintf(stderr, "\nCan't start MATLAB engine\n");
//		return;
//	}
//	
//	double* CurrentPoint;
//	int N = X->GetNumberOfPoints();
//	
//	double* X_matlab = new double [3*N];
//	
//	for (int i = 0; i < N; i++) {
//		CurrentPoint = X->GetPoint(i);
//		X_matlab[3*i] = CurrentPoint[0];
//		X_matlab[3*i+1] = CurrentPoint[1];
//		X_matlab[3*i+2] = CurrentPoint[2];
//	}
//	
//	X_matlab_mxArray = mxCreateDoubleMatrix(3, N, mxREAL);
//	// filling 3*N array column by column
//	memcpy((void *)mxGetPr(X_matlab_mxArray), (void *)X_matlab, 3*N*sizeof(double));
//	engPutVariable(ep, "X_matlab_mxArray", X_matlab_mxArray);
//	engEvalString(ep, "x0_matlab_mxArray = mean(X_matlab_mxArray,2);");
//	
//	mxArray* x0_matlab_mxArray = engGetVariable(ep, "x0_matlab_mxArray");
//	double* x0_matlab_double = mxGetPr(x0_matlab_mxArray);
//	x0[0] = x0_matlab_double[0];
//	x0[1] = x0_matlab_double[1];
//	x0[2] = x0_matlab_double[2];	
//	
//	engEvalString(ep, "A = X_matlab_mxArray - repmat(x0_matlab_mxArray, 1, size(X_matlab_mxArray, 2));");
//	engEvalString(ep, "[U, S, V] = svd(A,0);");
//	engEvalString(ep, "[s, i] = max(diag(S));");
//	engEvalString(ep, "a_mxArray = U(:,i);");
//	mxArray* a_mxArray = engGetVariable(ep, "a_mxArray");
//	double* a_matlab_double = mxGetPr(a_mxArray);
//	a[0] = a_matlab_double[0];
//	a[1] = a_matlab_double[1];
//	a[2] = a_matlab_double[2];
//	
//	mxDestroyArray(X_matlab_mxArray);
//	mxDestroyArray(x0_matlab_mxArray);
//	mxDestroyArray(a_mxArray);
//	delete [] X_matlab;
//	engClose(ep);
//	
//}

void StaticFunctionLibrary::LeastSquare3DLine(vtkPoints* X, double x0[3], double a[3])
{
	// USE LAPACK to calculate svd.
	
    double CurrentPoint[3];
	int N = X->GetNumberOfPoints();
	
	double* X_MAT = new double [3*N];

	x0[0] = 0.0;
	x0[1] = 0.0;
	x0[2] = 0.0;
	
	for (int i = 0; i < N; i++) 
	{
		X->GetPoint(i, CurrentPoint);
		X_MAT[3*i] = CurrentPoint[0];
		X_MAT[3*i+1] = CurrentPoint[1];
		X_MAT[3*i+2] = CurrentPoint[2];
		x0[0] += CurrentPoint[0]/N;
		x0[1] += CurrentPoint[1]/N;
		x0[2] += CurrentPoint[2]/N;
	}

	double* A = new double [3*N];

	for (i = 0; i < 3*N; i++)
	{
		A[i] = X_MAT[i]- x0[i%3];
	}

	int m, n;
	m = 3;
	n = N;

    int	lda = m; // The leading dimension of matrix a;
	int ldu = m; // The leading dimension of u
	
	int maxmn = (m >= n)?m:n;
	int minmn = (m >= n)?n:m;

	// economy size; see MATLAB svd;
	double* S = new double [minmn];

    double* U = new double [ldu*minmn];

	int ldvt = minmn;
	double* VT = new double [ldvt*n];

	StaticFunctionLibrary::SVD(A, m, n, S, U, VT);

	// find m-length vector associated with the highest singular value;

	int mindex = 0;
	double maxSigma = S[0];

	for (i = 1; i < minmn; i++)
	{
		if (S[i] > maxSigma)
		{
			maxSigma = S[i];
			mindex = i;
			break;
		}
	}

	a[0] = U[mindex*m];
	a[1] = U[mindex*m+1];
	a[2] = U[mindex*m+2];

	//delete
	delete [] X_MAT;
	delete [] A;
	delete [] S;
	delete [] VT;
	delete [] U;
}

void StaticFunctionLibrary::SVD(double* A , int m , int n , double* S, double* U, double* VT)
{
	// input A is a mxn matrix, arranged col-wise
	// S is the list of singular value
	// U and VT are arranged col-wise

	char jobu, jobvt;
	//int lda, ldu, ldvt, lwork, info;
	integer lda, ldu, ldvt, lwork, info;
	double *work;

	// brute force conversion
	integer mint = static_cast<integer>(m);
	integer nint = static_cast<integer>(n);

	jobu = 'S'; jobvt = 'S';

	lda = mint; // The leading dimension of matrix a;
	ldu = mint; // The leading dimension of u
	
    integer maxmn = (mint >= nint)?mint:nint;
	integer minmn = (mint >= nint)?nint:mint;

	ldvt = minmn;

	lwork = 5*maxmn;
	work = new double [lwork];

	dgesvd_(&jobu, &jobvt, &mint, &nint, A, &lda, S, U, &ldu, VT, &ldvt, work, &lwork, &info);

	VERIFY (info == 0);
		
	delete [] work;
}
// This function is only appropriate for surfaces recon based on parallel contours
void StaticFunctionLibrary::PickOutFirstCCASlice(vtkPolyData* input, vtkPolyData* output)
{
	// NormalOfPlane: the normal of the traverse slice of the input data (output)
	// minZ: The distance between the plane containing the origin (0, 0, 0) (with normal NormalOfPlane) and the lowest plane (lowest, of 
	// course, in the direction of the normal)
	// maxZ: The distance between the plane containing the origin (with normal NormalOfPlane) and the highest plane (in the direction
	// of normal)

	int i, numClosedCurve;	
	double tempNormal[3];

	vtkFeatureEdges* featureEdges = vtkFeatureEdges::New();
	featureEdges->SetInput(input);
	featureEdges->BoundaryEdgesOn();
	featureEdges->FeatureEdgesOff();
	featureEdges->NonManifoldEdgesOff();
	featureEdges->ManifoldEdgesOff();
	featureEdges->ColoringOff();

	vtkStripper* stripper = vtkStripper::New();
	stripper->SetInputConnection(featureEdges->GetOutputPort());
	stripper->Update();

	StaticFunctionLibrary::WritePolyData(stripper->GetOutput(), "C:\\stripperOutput.vtk");

	vtkPolyData* pdinCount = vtkPolyData::New();
	pdinCount->DeepCopy(stripper->GetOutput());

	vtkPolyData* tempOutputStripper = vtkPolyData::New();

	numClosedCurve = StaticFunctionLibrary::CountNumberOfClosedCurve(pdinCount, tempOutputStripper,1);
	StaticFunctionLibrary::WritePolyData(tempOutputStripper, "C:\\tempOutputStripper.vtk");

	vtkCellArray* lines = tempOutputStripper->GetLines();
	int npts, *pts;

	ASSERT (numClosedCurve == 3);

	vtkPolyData* OneSlice = vtkPolyData::New();

	double NormalOfPlane[3];
	NormalOfPlane[0] = 0.0;
	NormalOfPlane[1] = 0.0;
	NormalOfPlane[2] = 0.0;
	
	for (i = 0; i < numClosedCurve; i++)
	{
		OneSlice->Initialize();
		StaticFunctionLibrary::GrabOnlyOneLine(tempOutputStripper, OneSlice, i);
		StaticFunctionLibrary::FindNormal(OneSlice, tempNormal);
		vtkMath::Normalize(tempNormal);
		if (tempNormal[2] > 0)//dot with z-axis
		{
			//so that the normal points to the negative z direction
			//this decision is made arbitrarily -- as long as the direction is consistent, it's fine
			//NormalOfPlane will be corrected later (see next step --> Line (*))
			tempNormal[0] *= -1.0;
			tempNormal[1] *= -1.0;
			tempNormal[2] *= -1.0;
		}

		NormalOfPlane[0] += tempNormal[0];
		NormalOfPlane[1] += tempNormal[1];
		NormalOfPlane[2] += tempNormal[2];
	}

	vtkMath::Normalize(NormalOfPlane);

	// (*) -- ok, normal points to -ve z, but we want it to point from CCA to ica/eca
	// so which one is CCA
	
	float tol = 0.05;
	vtkPoints* ThreePoints = vtkPoints::New();
	ThreePoints->SetNumberOfPoints(3);

	for (i = 0, lines->InitTraversal(); i < numClosedCurve; i++)
	{
		lines->GetNextCell(npts, pts);
		ThreePoints->SetPoint(i, tempOutputStripper->GetPoint(pts[0]));
	}

	double tempVector[3], tempVector2[3];
	double origin[3];
	double firstPoint[3], secondPoint[3];
	double VectorFromCCA[3];
	
	ThreePoints->GetPoint(0, origin);
	ThreePoints->GetPoint(1, firstPoint);
	ThreePoints->GetPoint(2, secondPoint);

	tempVector[0] = firstPoint[0] - origin[0];
	tempVector[1] = firstPoint[1] - origin[1];
	tempVector[2] = firstPoint[2] - origin[2];

	tempVector2[0] = secondPoint[0] - origin[0];
	tempVector2[1] = secondPoint[1] - origin[1];
	tempVector2[2] = secondPoint[2] - origin[2];

	int linenum;

	if (abs(vtkMath::Dot(tempVector, NormalOfPlane)) < tol)
	{
		//origin and firstPoint are points on the two branches --> Cell 2 is CCA
		VectorFromCCA[0] = origin[0] - secondPoint[0];
		VectorFromCCA[1] = origin[1] - secondPoint[1];
		VectorFromCCA[2] = origin[2] - secondPoint[2];
		linenum = 2;
	}
	else if (abs(vtkMath::Dot(tempVector2, NormalOfPlane)) < tol)
	{
		//origin and secondPoint are points on the two branches --> Cell 1 is CCA
		VectorFromCCA[0] = origin[0] - firstPoint[0];
		VectorFromCCA[1] = origin[1] - firstPoint[1];
		VectorFromCCA[2] = origin[2] - firstPoint[2];
		linenum = 1;
	}
	else
	{
		// firstPoint and secondPoint are points on the two branches --> Cell 0 is CCA
		VectorFromCCA[0] = firstPoint[0] - origin[0];
		VectorFromCCA[1] = firstPoint[1] - origin[1];
		VectorFromCCA[2] = firstPoint[2] - origin[2];
		linenum = 0;
	}

	StaticFunctionLibrary::GrabOnlyOneLine(tempOutputStripper, output, linenum);

	//delete
	pdinCount->Delete();
	tempOutputStripper->Delete();
	featureEdges->Delete();
	stripper->Delete();
	OneSlice->Delete();
}

//void StaticFunctionLibrary::MakeFirstPolygonFaceOutside(vtkPolyData* input)
//{
//	// This function makes the first polygon face outward
//	// The remaining will be done by vtkPolyDataNormals
//	vtkPolyData* FirstCCASlice = vtkPolyData::New();
//	StaticFunctionLibrary::PickOutFirstCCASlice(input, FirstCCASlice);
//	
//	double c[3];
//	StaticFunctionLibrary::Centroid(FirstCCASlice, c);
//
//    //Get First Polygon
//	vtkIdType npts, *pts;
//	vtkCellArray* polys = input->GetPolys();
//	polys->InitTraversal();
//	polys->GetNextCell(npts, pts);
//
//	if (npts != 3) return; //not triangle, bail out
//
//	double CellNormal[3];
//	double PointOne[3];
//
//	input->GetPoint(pts[0], PointOne);
//	
//	//test
//	double PointTwo[3];
//	double PointThree[3];
//
//	input->GetPoint(pts[1], PointTwo);
//	input->GetPoint(pts[2], PointThree);
//
//	vtkTriangle::ComputeNormal(PointOne, PointTwo, PointThree, CellNormal);
//
//	double CentroidToPointOne[3];
//
//	for (int i = 0; i < 3; i++)
//	{
//		CentroidToPointOne[i] = PointOne[i] - c[i];
//	}
//
//	double dotProd = vtkMath::Dot(CentroidToPointOne, CellNormal);
//
//	if (dotProd < 0)
//	{
//		polys->ReverseCell(0);
//	}
//
//	// delete
//	FirstCCASlice->Delete();
//}

int StaticFunctionLibrary::CustomMessageBox(const char *s, UINT nType)
{
	return AfxMessageBox(s, nType);
}

int StaticFunctionLibrary::FindNormal(vtkPolyData *inpd, double normal[3], int LineNum)
{
	//Assume inpd has many lines
	// Compute the normal of the line with LineNum
	vtkCellArray* Lines = inpd->GetLines();

	if (!Lines || Lines->GetNumberOfCells() == 0)
	{
		StaticFunctionLibrary::CustomMessageBox("StaticFunctionLibrary::FindNormal: inpd must contain lines!");
		return 0;
	}

	vtkPolyData* intermediatepd = vtkPolyData::New();

	StaticFunctionLibrary::GrabOnlyOneLine(inpd, intermediatepd, LineNum);
	StaticFunctionLibrary::FindNormal(intermediatepd, normal);

	//delete
	intermediatepd->Delete();

	return 1;
}

void StaticFunctionLibrary::Centroid(vtkPolyData *inpd, double c[3], int LineNum)
{
	//Assume inpd has many lines
	// Compute the normal of the line with LineNum
	vtkCellArray* Lines = inpd->GetLines();

	if (!Lines || Lines->GetNumberOfCells() == 0)
	{
		StaticFunctionLibrary::CustomMessageBox("StaticFunctionLibrary::FindNormal: inpd must contain lines!");
		return;
	}

	vtkPolyData* intermediatepd = vtkPolyData::New();

	StaticFunctionLibrary::GrabOnlyOneLine(inpd, intermediatepd, LineNum);
	StaticFunctionLibrary::Centroid(intermediatepd, c);

	//delete
	intermediatepd->Delete();
}


// Helper function of ComputeNormalOfPlane. The purpose is to eliminate contours with points whose IsArch = 1
int StaticFunctionLibrary::EliminateContoursWithPointsOnArch(vtkPolyData* input, vtkPolyData* output)
{
	// Return number of remaining curve
	int i, j;

	int numLines = input->GetNumberOfLines();

	vtkIntArray* IsArch = vtkIntArray::SafeDownCast(
		input->GetPointData()->GetArray("IsArch"));
	
	if (numLines == 0 || !IsArch)
	{
		output->DeepCopy(input);
		return numLines;
	}

	vtkPolyData* outputLocal = vtkPolyData::New();

	outputLocal->DeepCopy(input);

	vtkCellArray* outputLines = vtkCellArray::New();
	outputLocal->SetLines(outputLines);
	outputLines->Delete();
	
	int* IsArchPointer = IsArch->GetPointer(0);

	int npts, *pts;
	vtkIdType Offset = input->GetNumberOfVerts();

	input->BuildCells();
	int EliminateCelli;
	int returnnumLines = numLines;

	for (i = 0; i < numLines; i++)
	{
        input->GetCellPoints(i + Offset, npts, pts);

		EliminateCelli = 0;
		for (j = 0; j < npts; j++)
		{
			if (IsArch)
			{
				if (IsArchPointer[pts[j]] == 1)
				{
					EliminateCelli = 1;
					returnnumLines--;
					break;					
				}
			}
		}

		if (!EliminateCelli)
		{
			//copy cell over
			outputLines->InsertNextCell(npts, pts);
		}
	}

	vtkCleanPolyData* clean = vtkCleanPolyData::New();
	clean->PointMergingOff();
	clean->ConvertLinesToPointsOff();
	clean->ConvertPolysToLinesOff();
	clean->ConvertStripsToPolysOff();
	clean->SetInput(outputLocal);
	clean->Update();

	output->DeepCopy(clean->GetOutput());

	//delete
	outputLocal->Delete();
	clean->Delete();

	return returnnumLines;
}

void StaticFunctionLibrary::ExtractLineFromPolyData(vtkPolyData* input, vtkPolyData* output, 
													vtkIdType StartLine, vtkIdType NumberOfSlicesToExtract, 
													vtkIdType LineIncrement)
{
	if (StartLine < 0)
	{
		RotateDlg::CustomMessageBox("Error! StartSlice must be >= 0");
		return;
	}

	if (NumberOfSlicesToExtract <= 0)
	{
		RotateDlg::CustomMessageBox("Error! NumberOfSlicesToExtract must be > 0");
		return;
	}

	if (input->GetNumberOfLines() == 0)
	{
		RotateDlg::CustomMessageBox("Error! This PolyData does not have any line.");
		return;
	}

	vtkPolyData* outputCopy = vtkPolyData::New();
	outputCopy->DeepCopy(input);

	vtkCellArray* outputLines = vtkCellArray::New();
	outputCopy->SetLines(outputLines);
	outputLines->Delete();

	input->BuildCells();

	if (StartLine > input->GetNumberOfLines() - 1)
	{
		StartLine = input->GetNumberOfLines() - 1;
	}

    if (StartLine + (NumberOfSlicesToExtract-1)*LineIncrement >= input->GetNumberOfLines())
	{
		NumberOfSlicesToExtract = floor(static_cast<double>(input->GetNumberOfLines()+1-StartLine)/
			static_cast<double>(LineIncrement));
	}

	vtkIdType Offset = input->GetNumberOfVerts();
	vtkIdType npts, *pts;

	for (int i = 0; i < NumberOfSlicesToExtract; i++)
	{
		input->GetCellPoints(Offset + StartLine + LineIncrement*i, npts, pts);
		outputLines->InsertNextCell(npts, pts);
	}

	vtkCleanPolyData* clean = vtkCleanPolyData::New();
	clean->PointMergingOff();
	clean->ConvertLinesToPointsOff();
	clean->ConvertPolysToLinesOff();
	clean->ConvertStripsToPolysOff();
	clean->SetInput(outputCopy);
	clean->Update();

	output->DeepCopy(clean->GetOutput());

	//delete
	outputCopy->Delete();
	clean->Delete();
}

void StaticFunctionLibrary::ComplexMultiply(double x[2], double y[2], double xy[2])
{
	xy[0] = x[0]*y[0] - x[1]*y[1];
	xy[1] = x[0]*y[1] + x[1]*y[0];
}

void StaticFunctionLibrary::ResampleByArcLength(vtkPolyData* curvein, vtkPolyData* curveout, double resolution, int closed)
{
	int i, j;
	double d;
	int npts, *pts;
	curvein->GetLines()->InitTraversal();
	curvein->GetLines()->GetNextCell(npts, pts);
	if (pts[0] == pts[npts-1]) npts--;
	
	double residue = 0;
	double firstPoint[3], secondPoint[3];
	vtkPoints* ResampledPoints = vtkPoints::New();
	vtkCellArray* ResampledCellArray = vtkCellArray::New();
	ResampledPoints->InsertPoint(0,curvein->GetPoints()->GetPoint(pts[0]));
	double uvecFirstToSecond[3], Point[3];
	
	int count = 1;
	int NumberOfSegments;

	int nptsForLoop = (closed)?npts:npts-1;
	
	for (i = 0; i < nptsForLoop; i++) 
	{
		curvein->GetPoints()->GetPoint(pts[i], firstPoint);
		curvein->GetPoints()->GetPoint(pts[(i+1)%npts], secondPoint);
		
		uvecFirstToSecond[0] = secondPoint[0] - firstPoint[0];
		uvecFirstToSecond[1] = secondPoint[1] - firstPoint[1];
		uvecFirstToSecond[2] = secondPoint[2] - firstPoint[2];
		d = vtkMath::Normalize(uvecFirstToSecond);
		
		NumberOfSegments = floor((d + residue)/resolution);
		for (j = 1; j <= NumberOfSegments; j++)
		{
			Point[0] = firstPoint[0] + (j*resolution - residue)*uvecFirstToSecond[0];
			Point[1] = firstPoint[1] + (j*resolution - residue)*uvecFirstToSecond[1];
			Point[2] = firstPoint[2] + (j*resolution - residue)*uvecFirstToSecond[2];
			if (sqrt(vtkMath::Distance2BetweenPoints(Point, ResampledPoints->GetPoint(0))) > resolution/100)
			{
				ResampledPoints->InsertPoint(count, Point);
				count++;
			}
		}
		
		residue = (d + residue) - NumberOfSegments * resolution;
	}
	
	int countReal = (closed)?count+1:count;

	ResampledCellArray->InsertNextCell(countReal);
	for (i = 0; i < count; i++) 
	{
		ResampledCellArray->InsertCellPoint(i);
	}
	
	if (closed)
	{
		ResampledCellArray->InsertCellPoint(0);
	}

	//VERIFY(count == NumberOfSamples);
	
	curveout->SetPoints(ResampledPoints);
	curveout->SetLines(ResampledCellArray);
	ResampledPoints->Delete();
	ResampledCellArray->Delete();
}

void StaticFunctionLibrary::PreprocessCenterline(vtkPolyData* InputPolyData , vtkPolyData* OutputPolyData , double Interval)
{
	// used exclusively by CurvePlanarReformation, SplineSurface and RotateDlg::ResliceUsingCenterline

	StaticFunctionLibrary::ResampleByArcLength(InputPolyData, OutputPolyData, Interval, 0);

	// Get first point and last point, find the coordinate with the largest span, make sure
	// that coordinate is in ascending order

	vtkIdType npts, *pts;
	vtkCellArray* Lines = OutputPolyData->GetLines();
	Lines->InitTraversal();
	Lines->GetNextCell(npts, pts);

	double FirstPoint[3], LastPoint[3];
	int XYZ = 0;

	OutputPolyData->GetPoint(pts[0], FirstPoint);
	OutputPolyData->GetPoint(pts[npts-1], LastPoint);
	double temp = abs(LastPoint[0] - FirstPoint[0]);

	for (int i = 1; i < 3; i++)
	{
		if (abs(LastPoint[i] - FirstPoint[i]) > temp)
		{
			temp = abs(LastPoint[i] - FirstPoint[i]);
			XYZ = i;
		}
	}

	if (LastPoint[XYZ] < FirstPoint[XYZ])
	{
		Lines->ReverseCell(0);
	}
}

int StaticFunctionLibrary::ReadTo4x4Matrix(CString inputfilename, vtkMatrix4x4* matrix, int Read3rowsIsEnough)
{
	 //Make sure the number of components is 2;
	
    std::ifstream *fp;
	
	fp = new std::ifstream(inputfilename, ios::in);
	if(fp->fail()) 
	{
		CString filenamecomplete = "Could not open the specified file: " + inputfilename;
		StaticFunctionLibrary::CustomMessageBox(filenamecomplete);
		return 0; // is null
	}
	
	char coord[256];
	float irow[4];

	matrix->Identity();

	int NumberOfRowsToBeRead = Read3rowsIsEnough?3:4;
	
	for (int i = 0; i < NumberOfRowsToBeRead; i++)
	{
		fp->getline(coord, 255, '\n');
		
		if(strcmp(coord, ""))
		{
			sscanf(coord, "%f %f %f %f", &irow[0], &irow[1], &irow[2], &irow[3]);
			//TRACE("xyz: %f %f\n", xy[0], xy[1]);
			for (int j = 0; j < 4; j++)
			{
				matrix->SetElement(i, j, static_cast<double>(irow[j]));
			}
		}
	}
	
	fp->close();
	delete fp;

	return 1;
}

void StaticFunctionLibrary::DetermineBifurcation(vtkPolyData *input, double Bifurcation[3])
{
	// July 28: Change return type to int to indicate whether bif can be found.
	double notused1, notused2, NormalOfInput[3];
	StaticFunctionLibrary::ComputeNormalOfPlane(input, NormalOfInput, notused1, notused2);
	vtkMath::Normalize(NormalOfInput);
	
	vtkPointData* inputPointData = input->GetPointData();

	vtkDataArray* IsArch = inputPointData->GetArray("IsArch");
	VERIFY (IsArch);

	int IsArchi;
	double ArchPointi[3];

	int maxId;
	double tempDoti, maxDot;
	int EnteredIsArchCond = 0;

	for (int i = 0; i < IsArch->GetNumberOfTuples(); i++)
	{
		IsArchi = static_cast<int>(IsArch->GetTuple1(i));
		if (IsArchi)
		{
			input->GetPoint(i, ArchPointi);

			tempDoti = vtkMath::Dot(ArchPointi, NormalOfInput);

			if (!EnteredIsArchCond)
			{
				maxDot = tempDoti;
				EnteredIsArchCond = 1;
				maxId = i;
			}
			else
			{
				if (tempDoti > maxDot)
				{
					maxDot = tempDoti;
					maxId = i;
				}
			}
		}
	}

	input->GetPoint(maxId, Bifurcation);

}
void StaticFunctionLibrary::WriteTo4x4Matrix(CString FileName, vtkMatrix4x4* matrix)
{
	//Make sure the number of components is 2;
	
    std::ofstream *ofp;
	
	ofp = new std::ofstream((LPCSTR) FileName);
	if(!ofp) 
	{
		return; // is null
	}
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			*ofp << matrix->Element[i][j];

			if (j == 3)
			{
				*ofp << "\n";
			}
			else
			{
				*ofp << " ";
			}
		}
	}
	
	ofp->close();
	delete ofp;
}

void StaticFunctionLibrary::WriteBifurcation(CString FileName, double Bifurcation[3])
{
	std::ofstream *ofp;
	
	ofp = new std::ofstream((LPCSTR) FileName);
	if(!ofp) 
	{
		return; // is null
	}
	
	for (int i = 0; i < 3; i++)
	{
		*ofp << Bifurcation[i] << " ";
	}
	*ofp << "\n";
	
	ofp->close();
	delete ofp;
}

int StaticFunctionLibrary::ReadBifurcation(CString FileName, double Bifurcation[3])
{
    std::ifstream *fp;
	
	fp = new std::ifstream((LPCSTR) FileName, ios::in);
	if(!fp) 
	{
		return 0; // is null
	}
	
	char coord[256];
	float irow[3];
	
	fp->getline(coord, 255, '\n');
	
	int returnV;

	if(strcmp(coord, ""))
	{
		returnV = sscanf(coord, "%f %f %f", &irow[0], &irow[1], &irow[2]);

		if (returnV != 3)
		{
			return 0;
		}

		//TRACE("xyz: %f %f\n", xy[0], xy[1]);
		for (int j = 0; j < 3; j++)
		{
			Bifurcation[j] = static_cast<double>(irow[j]);
		}
	}
	else
	{
		return 0;
	}
	
	fp->close();
	delete fp;

	return 1;
}
// This function searches corresponding ICA, ECA and CCA points. A landmark transform is used to register source to target using these three points plus bifurcation.
void StaticFunctionLibrary::InitialRegisterPolyData(vtkPolyData* Source, vtkPolyData* Target, 
													double BifurcationSource[3], double BifurcationTarget[3], 
													vtkMatrix4x4* RegMatrix)
{
	// Prerequisite: Source and Target must have arrays Inputs0, Inputs1 and Inputs2.

	double SourceProjection[3];
	double TargetProjection[3];
	double DesiredProjection[3];

	int i;
	for (i = 0; i < 3; i++)
	{
		if (!StaticFunctionLibrary::BranchProjectedToNormal(Source, i, BifurcationSource, SourceProjection[i]))
		{
			return;
		}
		if (!StaticFunctionLibrary::BranchProjectedToNormal(Target, i, BifurcationTarget, TargetProjection[i]))
		{
			return;
		}
	}

	// Take the min of SourceProjection[0 or 1] and TargetProjection[0 or 1] 
	// Take the max of SourceProjection[2] and TargetProjection[2]

	DesiredProjection[0] = (SourceProjection[0] > TargetProjection[0])?TargetProjection[0]:SourceProjection[0];
	DesiredProjection[1] = (SourceProjection[1] > TargetProjection[1])?TargetProjection[1]:SourceProjection[1];
	DesiredProjection[2] = (SourceProjection[2] < TargetProjection[2])?TargetProjection[2]:SourceProjection[2];

	vtkSmartPointer<vtkPoints> SourcePoints = vtkSmartPointer<vtkPoints>::New();
	SourcePoints->SetNumberOfPoints(3);
	SourcePoints->SetPoint(0, BifurcationSource);

	vtkSmartPointer<vtkPoints> TargetPoints = vtkSmartPointer<vtkPoints>::New();
	TargetPoints->SetNumberOfPoints(3);
	TargetPoints->SetPoint(0, BifurcationTarget);

	double tempPoint[3];
	int BranchNumber;
	for (BranchNumber = 0; BranchNumber < 2; BranchNumber++)
	{
		if (!StaticFunctionLibrary::LocateBranchPoint(Source, BranchNumber, DesiredProjection[BranchNumber], BifurcationSource, tempPoint))
		{
			return;
		}
		SourcePoints->SetPoint(BranchNumber+1, tempPoint);

		if (!StaticFunctionLibrary::LocateBranchPoint(Target, BranchNumber, DesiredProjection[BranchNumber], BifurcationTarget, tempPoint))
		{
			return;
		}
		TargetPoints->SetPoint(BranchNumber+1, tempPoint);
	}

	vtkSmartPointer<vtkLandmarkTransform> lmtrans = vtkSmartPointer<vtkLandmarkTransform>::New();
	lmtrans->SetModeToRigidBody();
	//lmtrans->SetTranslationRefSource(BifurcationSource);
	//lmtrans->SetTranslationRefTarget(BifurcationTarget);
	lmtrans->SetSourceLandmarks(SourcePoints);
	lmtrans->SetTargetLandmarks(TargetPoints);
	lmtrans->Update();

	RegMatrix->DeepCopy(lmtrans->GetMatrix());
}

// This function is the helper function of InitialRegisterPolyData.
int StaticFunctionLibrary::BranchProjectedToNormal(vtkPolyData* pd , int BranchNumber, double Bifurcation[3], double& ProjectedLength)
{
	// Prerequisite: Source and Target must have arrays Inputs0, Inputs1 and Inputs2.
	// If ECA and CCA (i.e., BranchNumber = 0 or 1), ProjectedLength = max_{i=1}^N dot(p_i-Bifurcation, normal);
	// where p_i 1<i<N is the ith point of pd.

	CString InputNumberString;

	switch (BranchNumber)
	{
	case 0:
		InputNumberString = "IsInputs0";
		break;
	case 1:
		InputNumberString = "IsInputs1";
		break;
	case 2: 
		InputNumberString = "IsInputs2";
		break;
	default:
		return 0;
	}
	
	vtkIntArray* BranchArray = vtkIntArray::SafeDownCast(pd->GetPointData()->GetArray((LPCSTR) InputNumberString));
	if (!BranchArray)
	{
		return 0;
	}

	int* IsInputs = BranchArray->GetPointer(0);

	double minZ, maxZ;
	double NormalOfPlane[3];
	StaticFunctionLibrary::ComputeNormalOfPlane(pd, NormalOfPlane, minZ, maxZ);
	
	int i;

	// if BranchNumber = 0, 1, get the max - most positive
	// if BranchNumber = 2, get the min - most negative

	double tempPoint[3];
	double e;
	int Flag = 0;

	for (i = 0; i < pd->GetNumberOfPoints(); i++)
	{
		if (IsInputs[i])
		{
			pd->GetPoint(i, tempPoint);
			tempPoint[0] -= Bifurcation[0];
			tempPoint[1] -= Bifurcation[1];
			tempPoint[2] -= Bifurcation[2];

			e = vtkMath::Dot(NormalOfPlane, tempPoint);
            // dot product with NormalOfPlane
            if (!Flag)
			{
				ProjectedLength = e;
				Flag = 1;
				continue;
			}

			if (BranchNumber != 2)
			{
				// find max;
				if (e > ProjectedLength)
				{
					ProjectedLength = e;
				}
			}
			else
			{
				// find min
				if (e < ProjectedLength)
				{
					ProjectedLength = e;
				}
			}
		}
	}

	return 1;
}

int StaticFunctionLibrary::LocateBranchPoint(vtkPolyData* pd , int BranchNumber, double length, double Bifurcation[3], double OutputPoint[3], double tol)
{
	// Prerequisite: Source and Target must have arrays Inputs0, Inputs1 and Inputs2.
	// Find a point in BranchNumber s.t. of all points in pd, it is closest to the point 
	// Bifurcation + length*Normal (Note: length could be positive or negative)

	CString InputNumberString;

	switch (BranchNumber)
	{
	case 0:
		InputNumberString = "IsInputs0";
		tol = (tol>0)?-tol:tol;
		break;
	case 1:
		InputNumberString = "IsInputs1";
		tol = (tol>0)?-tol:tol;
		break;
	case 2: 
		InputNumberString = "IsInputs2";
		tol = (tol>0)?tol:-tol;
		break;
	default:
		return 0;
	}
	
	//vtkIntArray* BranchArray = vtkIntArray::SafeDownCast(pd->GetPointData()->GetArray((LPCSTR) InputNumberString));
	//if (!BranchArray)
	//{
	//	return 0;
	//}

	//int* IsInputs = BranchArray->GetPointer(0);

	double minZ, maxZ;
	double NormalOfPlane[3];
	StaticFunctionLibrary::ComputeNormalOfPlane(pd, NormalOfPlane, minZ, maxZ);

	double StandardPoint[3];
	int i;
	for (i = 0; i < 3; i++)
	{
		StandardPoint[i] = Bifurcation[i] + length*NormalOfPlane[i]; 
	}

	int OldAttribute[vtkDataSetAttributes::NUM_ATTRIBUTES];
	pd->GetPointData()->GetAttributeIndices(OldAttribute);
	int OldScalarIndex = OldAttribute[vtkDataSetAttributes::SCALARS];

	if (pd->GetPointData()->SetActiveScalars((LPCSTR) InputNumberString)<0)
	{
		return 0;
	}

	vtkSmartPointer<vtkClipPolyData> clip = vtkSmartPointer<vtkClipPolyData>::New();
	clip->SetInput(pd);
	clip->SetValue(1.0-1.0e-12);
	clip->Update();

	//test
	StaticFunctionLibrary::WritePolyData(clip->GetOutput(), "C:\\testclip.vtk");

	vtkPolyData* pdNew = clip->GetOutput();

	//vtkSmartPointer<vtkCellLocator> cellLoc = vtkSmartPointer<vtkCellLocator>::New();
	//cellLoc->SetDataSet(clip->GetOutput());
	//cellLoc->SetNumberOfCellsPerBucket(1);
	//cellLoc->BuildLocator();

	//vtkIdType cellId;
	//int subId;
	//double dist2;
	//
	//cellLoc->FindClosestPoint(StandardPoint, OutputPoint, cellId, subId, dist2);
	vtkSmartPointer<vtkPlane> inplane = vtkSmartPointer<vtkPlane>::New();
	double origini[3];
	for (i = 0; i < 3; i++)
	{
		origini[i] = StandardPoint[i] + tol*NormalOfPlane[i]; 
	}
	inplane->SetOrigin(origini);
	inplane->SetNormal(NormalOfPlane);

	vtkSmartPointer<vtkPolyData> slice = vtkSmartPointer<vtkPolyData>::New();
	if (StaticFunctionLibrary::CutOneSlice(pdNew, inplane, slice)<1)
	{
		return 0;
	}

	StaticFunctionLibrary::WritePolyData(slice, "C:\\slice.vtk");

	//Pick the furthest point

	double tempPoint[3];
	double e, emax;
	vtkIdType maxIndex;

	for (i = 0; i < slice->GetNumberOfPoints(); i++)
	{
		slice->GetPoint(i, tempPoint);
		e = vtkMath::Distance2BetweenPoints(tempPoint, StandardPoint);

		if (i == 0 || e > emax)
		{
			emax = e;
			maxIndex = i;
		}
	}

	slice->GetPoint(maxIndex, OutputPoint);

	//double tempPoint[3];
	//double e, emin;
	//int Flag = 0;
	//vtkIdType minIndex;

	//for (i = 0; i < pd->GetNumberOfPoints(); i++)
	//{
	//	if (IsInputs[i])
	//	{
	//		pd->GetPoint(i, tempPoint);
	//		e = vtkMath::Distance2BetweenPoints(tempPoint, StandardPoint);
	//		
	//		if (!Flag)
	//		{
	//			emin = e;
	//			minIndex = i;
	//			Flag = 1;
	//			continue;
	//		}

	//		if (e < emin)
	//		{
	//			emin = e;
	//			minIndex = i;
	//		}
	//	}
	//}

	//pd->GetPoint(minIndex, OutputPoint);

	// reset original active scalar.
	pd->GetPointData()->SetActiveAttribute(OldScalarIndex, vtkDataSetAttributes::SCALARS);

	return 1;
}

// helper for LocateBranchPoint
int StaticFunctionLibrary::CutOneSlice(vtkPolyData* inpd, vtkPlane* inplane, vtkPolyData* slice)
{
	vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
	vtkSmartPointer<vtkStripper> stripper = vtkSmartPointer<vtkStripper>::New();

	cutter->SetInput(inpd);
	cutter->SetCutFunction(inplane);
	cutter->SetValue(0, 0);

	stripper->SetInputConnection(cutter->GetOutputPort());
	stripper->Update();

	slice->DeepCopy(stripper->GetOutput());
    
	int numlines = slice->GetNumberOfLines();

	return numlines;
}

void StaticFunctionLibrary::AdjustSourceBifurcation(vtkPolyData* Source, double Bifurcation[3], double Length, double BifurcationAdjusted[3])
{
	double minZ, maxZ;
	double NormalOfPlane[3];
	StaticFunctionLibrary::ComputeNormalOfPlane(Source, NormalOfPlane, minZ, maxZ);

	for (int i = 0; i < 3; i++)
	{
        BifurcationAdjusted[i] = Bifurcation[i] + Length*NormalOfPlane[i];
	}
}



void StaticFunctionLibrary::ComputeMoments(double Array[], int num, double& Mean, double& StandardDeviation, double& Skewness, double& Kurtosis)
{
	Mean = 0;
	int i;
	for (i = 0; i < num; i++)
	{
        Mean += Array[i]/num;        		
	}

	Skewness = 0;
	double secondmoment = 0;
	double ep;
	double thirdmoment = 0;
	double fourthmoment = 0;
	double dnum = static_cast<double>(num);

	for (i = 0; i < num; i++)
	{
        ep = Array[i] - Mean;	
        secondmoment += ep*ep;
		thirdmoment += ep*ep*ep;
        fourthmoment += ep*ep*ep*ep;
	}

	StandardDeviation = sqrt(secondmoment/(num-1));

	double factorSkewness = sqrt(dnum*dnum*(dnum-1))/(dnum-2);
	Skewness = factorSkewness*thirdmoment/pow(secondmoment, 1.5); //unbiased

	double G2 = dnum*fourthmoment/(secondmoment*secondmoment) - 3; //biased
	double factorKurtosis = (dnum-1)/((dnum-2)*(dnum-3));
	Kurtosis = factorKurtosis*((dnum+1)*G2+6);
}

void StaticFunctionLibrary::randperm(int numElements, int* Output)
{
	// Randomly permute [0 .... numElements-1]

	double* Elements = new double [numElements];

	int i;
    for (i = 0; i < numElements; i++)
	{
		Elements[i] = vtkMath::Random();
	}

	StaticFunctionLibrary::SimpleSort(Elements, Output, numElements);

	delete [] Elements;
}

void StaticFunctionLibrary::EliminateOutlierFromArray(double InputArray[], int numInputs, double OutputArray[], int& numOutputs, vtkIdList* IncludedIndex, int Iterations)
{
	int i, j;
	int* Index = new int [numInputs];
	double* RearrangeInputArray = new double [numInputs];
	double dj, djmin;
	double VIj, VIj_1;
	int minIndex;
	int k, kk;
	double d1, d2;

	vtkSmartPointer<vtkIdList> EIndexInput = vtkSmartPointer<vtkIdList>::New();
	vtkSmartPointer<vtkIdList> EIndexInputMax = vtkSmartPointer<vtkIdList>::New();

	double* Group1;
	double SF, SFMax;

	for (i = 0; i < Iterations; i++)
	{
		StaticFunctionLibrary::randperm(numInputs, Index);
		for (j = 0; j < numInputs; j++)
		{
			RearrangeInputArray[j] = InputArray[Index[j]];
		}

		djmin = 0;

		// find the max dissimilarity value which is >= 0
		VIj_1 = 0;
		for (j = 1; j < numInputs; j++)
		{
			VIj = StaticFunctionLibrary::Variance(RearrangeInputArray, j+1, 1);
            dj = VIj - VIj_1;
			if (dj > djmin)
			{
				djmin = dj;
				minIndex = j;
			}   
            VIj_1 = VIj;
		}

		EIndexInput->Initialize();
		EIndexInput->InsertNextId(Index[minIndex]);

		//    Ixi = rIndex(minIndex);
		//    for k = minIndex+1:length(Seq)
		//        Group1 = [Seq(1:minIndex-1) Seq(k)];
		//        Group2 = Seq(1:minIndex);
		//        d1 = Dissimilarity(Group1);
		//        d2 = Dissimilarity(Group2);
		//        if (d1 >= d2)
		//            Ixi = [Ixi rIndex(k)];
		//        end
		//    end

		for (k = minIndex + 1; k < numInputs; k++)
		{
			Group1 = new double [minIndex+1];
			for (kk = 0; kk < minIndex; kk++)
			{
				Group1[kk] = RearrangeInputArray[kk];
			}
			Group1[minIndex] = RearrangeInputArray[k];
			d1 = StaticFunctionLibrary::Variance(Group1, minIndex+1, 1);	
			// d2
			d2 = StaticFunctionLibrary::Variance(RearrangeInputArray, minIndex+1, 1);
			if (d1 >= d2)
			{
				EIndexInput->InsertNextId(Index[k]);
			}
			delete [] Group1;
		}

		 SF = SmoothingFunction(InputArray, numInputs, EIndexInput);
		 
		 if (i == 0)
		 {
			SFMax = SF;
			EIndexInputMax->DeepCopy(EIndexInput);
		 }
		 else
		 {
			 if (SF > SFMax)
			 {
				 SFMax = SF;
				 EIndexInputMax->DeepCopy(EIndexInput);
			 }
		 }
	}

	numOutputs = 0;
	for (i = 0; i < numInputs; i++)
	{
		if (EIndexInputMax->IsId(i) < 0)
		{
			OutputArray[numOutputs++] = InputArray[i];
			IncludedIndex->InsertNextId(i);
		}
	}
    
	delete [] Index;
	delete [] RearrangeInputArray;
}

double StaticFunctionLibrary::SmoothingFunction(double WholeSeq[], int NumElementsInSeq, vtkIdList* Indexj)
{
	// Helper function for EliminateOutlierFromArray

	//tf = ismember([1:length(Seq)],Indexj);
	//I_Ij = find(tf == 0);

	//SF = Cardinality(I_Ij)*(Dissimilarity(Seq) - Dissimilarity(Seq(I_Ij)));

	// Assumption: Items in Indexj are unique;

	//Construct Array {WholeSeq[i]: Indexj->IsId(i) == -1}
	double* ArrayI_Ij = new double [NumElementsInSeq - Indexj->GetNumberOfIds()];

	int i;
	int countI_Ij = 0;
	for (i = 0; i < NumElementsInSeq; i++)
	{
		if (Indexj->IsId(i) < 0)
		{
			ArrayI_Ij[countI_Ij++] = WholeSeq[i];
		}
	}

	double SF, DI, DI_Ij;
	DI = StaticFunctionLibrary::Variance(WholeSeq, NumElementsInSeq, 1);
	DI_Ij = StaticFunctionLibrary::Variance(ArrayI_Ij, countI_Ij, 1);

	SF = countI_Ij*(DI-DI_Ij);

	return SF;	
}

double StaticFunctionLibrary::Variance(double Array[], int num, int Type)
{
	// Type 1 = second moment is normalized by num
	// Type 0 = second moment is normalized by num-1

	double Mean = 0;
	int i;
	for (i = 0; i < num; i++)
	{
        Mean += Array[i]/num;        		
	}

	double secondmoment = 0;
	double ep;
	double dnum = static_cast<double>(num);

	for (i = 0; i < num; i++)
	{
        ep = Array[i] - Mean;	
        secondmoment += ep*ep;
	}

	double Var;
	if (Type)
	{
        Var = secondmoment/dnum;
	}
	else
	{
		Var = secondmoment/(dnum-1);
	}

	return Var;
}

void StaticFunctionLibrary::ArcLengthParameterizationForEachPoint(vtkPolyData* InputCurve, vtkDoubleArray* s)
{
	//Only care about first line; don't care about other lines

	vtkIdType npts, *pts;

	vtkCellArray* Lines = InputCurve->GetLines();
	Lines->InitTraversal();
	Lines->GetNextCell(npts, pts);

	if (pts[0] == pts[npts-1]) npts--;

	s->SetNumberOfTuples(npts);
	s->SetNumberOfComponents(1);
	
	int i;
	double si = 0;
	s->SetTuple(0, &si);
	double tempPoint1[3], tempPoint2[3];

	for (i = 1; i < npts; i++)
	{
		InputCurve->GetPoint(pts[i-1], tempPoint1);
		InputCurve->GetPoint(pts[i],   tempPoint2);

		si += sqrt(vtkMath::Distance2BetweenPoints(tempPoint1, tempPoint2));

		s->SetTuple(i, &si);
	}
}

void StaticFunctionLibrary::DecomposeIntoBranches(vtkPolyData *Input, vtkPolyData **Outputs)
{
	// Input: the contours of the whole carotid artery (ica, eca, cca)
	// Outputs: the contours of the ica, eca and cca are decomposed into three vtkPolyData objects.
	
	int i;
	
	vtkPointData* InputPointData = Input->GetPointData();
	vtkCellArray* InputLines = Input->GetLines();
	
	// Access the IsInputs arrays 
	vtkDataArray* IsInputs0 = InputPointData->GetArray("IsInputs0");
	VERIFY (IsInputs0);
	vtkDataArray* IsInputs1 = InputPointData->GetArray("IsInputs1");
	VERIFY (IsInputs1);
	vtkDataArray* IsInputs2 = InputPointData->GetArray("IsInputs2");
	VERIFY (IsInputs2);

	// Prepare the outputs: 
	// Outputs should copy everything from Inputs, except verts, lines, polys and strips

	vtkPolyData* tempPolyData;
	vtkCellArray* tempLines;
	
	for (i = 0; i < 3; i++)
	{
		// make sure tempPolyData points to a vtkPolyData object
		tempPolyData = vtkPolyData::SafeDownCast( Outputs[i] );
		VERIFY (tempPolyData);

		Outputs[i]->DeepCopy(Input);
		tempLines = vtkCellArray::New();
		Outputs[i]->SetLines(tempLines);
		tempLines->Delete();

		Outputs[i]->SetVerts(NULL);
		Outputs[i]->SetPolys(NULL);
		Outputs[i]->SetStrips(NULL);
	}


	int npts, *pts;
	int IsInputs0i, IsInputs1i, IsInputs2i;
	vtkCellArray* Outputs0Lines = Outputs[0]->GetLines();
	vtkCellArray* Outputs1Lines = Outputs[1]->GetLines();
	vtkCellArray* Outputs2Lines = Outputs[2]->GetLines();
	
	InputLines->InitTraversal();
	
	for (i = 0; i < InputLines->GetNumberOfCells(); i++)
	{
		InputLines->GetNextCell(npts, pts);
		IsInputs0i = IsInputs0->GetComponent(pts[0], 0);
		IsInputs1i = IsInputs1->GetComponent(pts[0], 0);
		IsInputs2i = IsInputs2->GetComponent(pts[0], 0);

		if (IsInputs0i && !IsInputs1i && !IsInputs2i)
		{
			Outputs0Lines->InsertNextCell(npts, pts);			
		}
		else if (!IsInputs0i && IsInputs1i && !IsInputs2i)
		{
			Outputs1Lines->InsertNextCell(npts, pts);
		}
		else if (!IsInputs0i && !IsInputs1i && IsInputs2i)
		{
			Outputs2Lines->InsertNextCell(npts, pts);
		}
		else
		{
			VERIFY (0);
		}	

	}

}
vtkIdType StaticFunctionLibrary::FindContoursInSameSlice(vtkPolyData* ContourSet1, 
														 vtkPolyData* ContourSet2, 
														 int LineNoOnContourSet1, double Tol)
{
	// Given LineNoOnContourSet1 in ContourSet1, return the corresponding line in ContourSet2
	// in the same line. return -1 if not found.

	// Find normal of the cell in GoldStandard
	double normal[3];

	StaticFunctionLibrary::FindNormal(ContourSet1, normal, LineNoOnContourSet1);
	vtkMath::Normalize(normal);

	vtkIdType npts, *pts;
	ContourSet1->BuildCells();
	vtkIdType Offset = ContourSet1->GetNumberOfVerts();
	ContourSet1->GetCellPoints(LineNoOnContourSet1+Offset, npts, pts);

	if (pts[0] == pts[npts-1]) npts--;

	double PointThisCell[3];
	ContourSet1->GetPoint(pts[0], PointThisCell);

	double disvec[3], tempPoint[3];
	double dotN;

	vtkIdType numLines = ContourSet2->GetNumberOfLines();
	ContourSet2->BuildCells();
	vtkIdType Offseti = ContourSet2->GetNumberOfVerts();
	
	for (vtkIdType i = 0; i < numLines; i++)
	{
		ContourSet2->GetCellPoints(i+Offseti, npts, pts);
		ContourSet2->GetPoint(pts[0], tempPoint);
		
		disvec[0] = tempPoint[0] - PointThisCell[0];
		disvec[1] = tempPoint[1] - PointThisCell[1];
		disvec[2] = tempPoint[2] - PointThisCell[2];

		// Compute dot product
		dotN = vtkMath::Dot(disvec, normal);
		if (dotN < Tol && dotN > -Tol)
		{
            return i;
		}
	}

	return -1;
}

int StaticFunctionLibrary::IntersectionOfTwoPolyLines(vtkPolyData* pd1, vtkPolyData* pd2, vtkPoints* OutputP, vtkIdList* subIdArray)
{
	// This function finds the intersection between two lines pd1 and pd2 and return the number of intersections.
	// This function only considers the first line.

	OutputP->Reset();
	subIdArray->Reset();

	int numint = 0;

	vtkPolyDataPointer pd1Line = vtkPolyDataPointer::New();
	vtkPolyDataPointer pd2Line = vtkPolyDataPointer::New();
	
	StaticFunctionLibrary::GrabOnlyOneLine(pd1, pd1Line, 0);
	StaticFunctionLibrary::GrabOnlyOneLine(pd2, pd2Line, 0);

	//OffsetOf(pd2Line);
	//vtkPolyLine* pd2PolyLine = vtkPolyLine::SafeDownCast(pd2Line->GetCell(Offset));

    //int n = pd2PolyLine->GetNumberOfPoints();
	// if not, then make it closed.
	//if (pd2PolyLine->GetPointId(0) != pd2PolyLine->GetPointId(n-1)) 
	//{
	//	pd2PolyLine->GetPointIds()->InsertId(n, pd2PolyLine->GetPointId(0));
	//	pd2PolyLine->GetPoints()->InsertPoint(n, pd2PolyLine->GetPoints()->GetPoint(0));
	//	n++;
	//}

	vtkIdType npts, *pts;
	vtkCellArray* pd1LineLines = pd1Line->GetLines();
	pd1LineLines->InitTraversal();
	pd1LineLines->GetNextCell(npts, pts);

	if (pts[npts-1] == pts[0]) npts--;

    double startPoint[3], endPoint[3];

	//vtkPoints* tempPoints = pd2PolyLine->GetPoints();

	//double t, pcoords[3];
	//int subId;

	vtkIdType npts2, *pts2;
	vtkCellArray* pd2LineLines = pd2Line->GetLines();
	pd2LineLines->InitTraversal();
	pd2LineLines->GetNextCell(npts2, pts2);

	if (pts2[npts2-1] == pts2[0]) npts2--;

	// BC: May 17 2012 -- We don't even need the tolerance parameter because we accept all points with condition IsYesIntersection.
	double x1[3];

	double xtemp[3], ytemp[3];
	double u, v;
	int IsVtkYesIntersection;

	int i, j;
	for (i = 0; i < npts; i++)
	{
		pd1Line->GetPoint(pts[i], startPoint);
		pd1Line->GetPoint(pts[(i+1)%npts], endPoint);

		for (j = 0; j < npts2; j++)
		{
			pd2Line->GetPoint(pts2[j], xtemp);
			pd2Line->GetPoint(pts2[(j+1)%npts2], ytemp);

			IsVtkYesIntersection = (vtkLine::Intersection(startPoint, endPoint, xtemp, ytemp, u, v) == VTK_YES_INTERSECTION);

			if (IsVtkYesIntersection)
			{
				x1[0] = startPoint[0] + u*(endPoint[0]-startPoint[0]);
				x1[1] = startPoint[1] + u*(endPoint[1]-startPoint[1]);
				x1[2] = startPoint[2] + u*(endPoint[2]-startPoint[2]);

				OutputP->InsertNextPoint(x1);
				subIdArray->InsertNextId(j);
				numint++;
				break;
			}

		}
		//if (!pd2PolyLine->IntersectWithLine(startPoint, endPoint, tol, t, x1, pcoords, subId))
		//{
		//	continue;
		//}

		//// Check whether intersection really occurs.
		//tempPoints->GetPoint(subId, xtemp);
		//tempPoints->GetPoint(subId + 1, ytemp);

		//IsVtkYesIntersection = (vtkLine::Intersection(startPoint, endPoint, xtemp, ytemp, u, v) == VTK_YES_INTERSECTION);

		//if (IsVtkYesIntersection)
		//{
		//	OutputP->InsertNextPoint(x1);
		//	subIdArray->InsertNextId(subId);
		//	numint++;
		//}
	}

	return numint;
}

#define EPS 3.0e-7
#define FPMIN 1.0e-30
#define MAXIT 1000

int StaticFunctionLibrary::ttest(double d1mean, double se1, double df1, double d2mean, double se2, double df2, \
								 double& t, double& p, double alpha)
{
	t = (d1mean - d2mean)/sqrt(se1*se1 + se2*se2);
	//t = (t > 0)?t:-t;

	double dfdenom = se1*se1*se1*se1/df1 + se2*se2*se2*se2/df2;
	double df = (se1*se1 + se2*se2);
	df *= df;
	df /= dfdenom;
	
	df = StaticFunctionLibrary::RoundInteger(df);

	double xx = df/(df + t*t);

	p = StaticFunctionLibrary::betainc(df/2, 0.5, xx);

	return (p < alpha);

}

double StaticFunctionLibrary::RoundInteger(double x)
{
	// round df
	double floorx = floor(x);
	double ceilx = ceil(x);

	if (floorx != ceilx)
	{
		//decimal
		if ((x - floorx)<(ceilx - x))
		{
			return floorx;
		}
		else
		{
			return ceilx;
		}
	}
	else 
	{// x is an integer
		return x;
	}
}

double StaticFunctionLibrary::betainc(double a, double b, double x)
{
	// x is in the range of [0, 1]
	//incomplete beta function
	// numerical recipe in C pp. 227

	if (x < 0.0 || x > 1.0)
	{
		AfxMessageBox("Fatal Error: StaticFunctionLibrary::betainc(), x must be inside 0 to 1", MB_ICONEXCLAMATION|MB_OK);
		ASSERT(0);
	}

	double bt;

	if (x == 0.0 || x == 1.0) bt = 0.0;
	else
	{
		bt = exp(gammaln(a+b)-gammaln(a)-gammaln(b) + a*log(x) + b*log(1-x));	
	}

	if (x < (a + 1.0)/(a + b + 2.0))
		return bt*StaticFunctionLibrary::betacf(a,b,x)/a;
	else
		return 1.0 - bt*StaticFunctionLibrary::betacf(b,a,1.0-x)/b;
}

double StaticFunctionLibrary::gammaln(double xx)
{
	//Returns the value ln[gamma(xx)] for xx > 0.
	//Internal arithmetic will be done in double precision, a nicety that you can omit if five-figure
	//accuracy is good enough.

	// copy from numerical recipe in C. Section 6.2
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,
						24.01409824083091,-1.231739572450155,
						0.1208650973866179e-2,-0.5395239384953e-5};
	
	int j;
	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}

double StaticFunctionLibrary::betacf(double a, double b, double x)
{
	// copied from Numerical Recipe in C Section 6.4

	// used by betainc: Evalutes fraction for incomplete beta function by modified Lentz's
	//method

	int m, m2;
	double aa, c, d, del, h, qab, qam, qap;
	double dabs, cabs, delabs;

	qab = a + b;
	qap = a + 1.0;
	qam = a - 1.0;
	c = 1.0; //C2;
	d = 1.0 - qab*x/qap; // 1+d1;
	dabs = (d < 0)? -d: d;
	if (dabs < FPMIN) d = FPMIN;
	d = 1.0/d; //D2;
	h = d; //fj

	for (m = 1; m <= MAXIT; m++)
	{
		m2 = 2*m;
		// One step of recurrence for odd m
		aa = m*(b-m)*x/((qam+m2)*(a+m2));
		d = 1.0 + aa*d; // since bj is always 1
		dabs = (d < 0)?-d:d;
		if (dabs < FPMIN) d = FPMIN;
		c = 1.0 + aa/c; // 
		cabs = (c < 0)?-c:c;
		if (cabs < FPMIN) c = FPMIN;
		d = 1.0/d;
		// Dj = 1/(bj + aj*Dj-1) = 1/(1+aa*Dj-1) 
		// Cj = bj + aj/Cj-1 = 1 + aa/Cj-1
		// since bj = 1 and aj = aa;
		h *= c*d; // fj
		
		// One step of recurrence for even m
		aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
		d = 1.0 + aa*d;
		dabs = (d < 0)?-d:d;
		if (dabs < FPMIN) d = FPMIN;
		c = 1.0 + aa/c; // 
		cabs = (c < 0)?-c:c;
		if (cabs < FPMIN) c = FPMIN;
		d = 1.0/d;
		del = d*c;
		h *= del;

		delabs = del - 1.0;
		delabs = (delabs < 0)?-delabs:delabs;
		if (delabs < EPS) break;
	}
	if (m > MAXIT)
	{
		AfxMessageBox("Fatal Error: StaticFunctionLibrary::betacf() doesn't converge", MB_ICONEXCLAMATION|MB_OK);
		ASSERT(0);
	}
	return h;

}
int StaticFunctionLibrary::ttest(double* Samples1, double ElementsInSamples1, double* Samples2, double ElementsInSamples2, double& t, double& p, double alpha)
{
	double d1mean, sd1, se1, df1, notuseds, notusedk;
	double d2mean, sd2, se2, df2;

	StaticFunctionLibrary::ComputeMoments(Samples1, ElementsInSamples1, d1mean, sd1, notuseds, notusedk);

	se1 = sd1/sqrt(ElementsInSamples1);
	df1 = ElementsInSamples1-1;

	StaticFunctionLibrary::ComputeMoments(Samples2, ElementsInSamples2, d2mean, sd2, notuseds, notusedk);

	se2 = sd2/sqrt(ElementsInSamples2);
	df2 = ElementsInSamples2-1;

	return StaticFunctionLibrary::ttest(d1mean, se1, df1, d2mean, se2, df2, t, p, alpha);
}

double StaticFunctionLibrary::pInttest(double TValue, double df)
{
	df = StaticFunctionLibrary::RoundInteger(df);

	double xx = df/(df + TValue*TValue);

	return StaticFunctionLibrary::betainc(df/2, 0.5, xx);
}

double StaticFunctionLibrary::DegreeOfFreedom(double SDWall, int WallSampleSize , double SDLumen, int LumenSampleSize)
{
	double se1, se2;
	se1 = SDWall*SDWall/WallSampleSize;
	se2 = SDLumen*SDLumen/LumenSampleSize;

	double dof;
	dof = (se1 + se2)*(se1 + se2);

	dof /= se1*se1/(WallSampleSize-1) + se2*se2/(LumenSampleSize-1);
	
	return dof;
}

void StaticFunctionLibrary::RegisterPolyData(vtkPolyData *Source, vtkPolyData *Target, 
											 double BifurcationSource[], double BifurcationTarget[], 
											 vtkMatrix4x4 *RegMatrix)
{
		//int i;
	vtkTransformPolyDataFilter* TransformPolyDataSource = vtkTransformPolyDataFilter::New(); 
	vtkTransform* TransformSource = vtkTransform::New();
	TransformPolyDataSource->SetTransform(TransformSource);
	TransformSource->Delete();

	//TransformSource->PostMultiply();

	//double transCoordinate[3];

	//for (i = 0; i < 3; i++)
	//{
	//	transCoordinate[i] = BifurcationTarget[i] - BifurcationSource[i];
	//}

	//// estimate
	//transCoordinate[2] -= 0.5;

	//TransformSource->Translate(transCoordinate);

	vtkSmartPointer<vtkMatrix4x4> initMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	StaticFunctionLibrary::InitialRegisterPolyData(Source, Target, BifurcationSource, 
		BifurcationTarget, initMatrix);

	TransformSource->SetMatrix(initMatrix);

	TransformPolyDataSource->SetInput(Source);

	TransformPolyDataSource->Update();

	//StaticFunctionLibrary::WritePolyData(TransformPolyDataSource->GetOutput(), "C:\\t.vtk");

	// Adjust Bifurcation;
	double BifurcationSourceAdjusted[3];

	double BifurcationSource4[4] = {0, 0, 0, 1};
	for (int i = 0; i < 3; i++)
	{
		BifurcationSource4[i] = BifurcationSource[i];
	}
	double BifurcationSourceTransformed[4];

	initMatrix->MultiplyPoint(BifurcationSource4, BifurcationSourceTransformed);

	StaticFunctionLibrary::AdjustSourceBifurcation(TransformPolyDataSource->GetOutput(), BifurcationSourceTransformed, 0.0, BifurcationSourceAdjusted);

	MyIterativeClosestPointTransform* trans = MyIterativeClosestPointTransform::New();
	trans->SetSource(TransformPolyDataSource->GetOutput());
	trans->SetTarget(Target);
	trans->SetTranslationRefSource(BifurcationSourceAdjusted); // correct bifurcation has been shifted by initial registration.
	trans->SetTranslationRefTarget(BifurcationTarget);
	trans->StartByMatchingCentroidsOff();
	trans->SetCheckMeanDistance( 1 );
	trans->SetMaximumMeanDistance(0.0001);
	trans->SetMaximumNumberOfIterations( 500 );
	trans->SetMaximumNumberOfLandmarks( 1000 );
	trans->StartByMatchingCentroidsOff();
	trans->SetMeanDistanceMode(1);
	trans->GetLandmarkTransform()->SetModeToRigidBody(); 
	trans->Update();
	vtkMatrix4x4* m = trans->GetMatrix();
/*
	TRACE("ICP Matrix: %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n", 
		m->Element[0][0], m->Element[0][1], m->Element[0][2], m->Element[0][3], 
		m->Element[1][0], m->Element[1][1], m->Element[1][2], m->Element[1][3],
		m->Element[2][0], m->Element[2][1], m->Element[2][2], m->Element[2][3],
		m->Element[3][0], m->Element[3][1], m->Element[3][2], m->Element[3][3]);
*/
	//vtkMatrix4x4* translateMatrix1 = vtkMatrix4x4::New();
	//translateMatrix1->Identity();
	//translateMatrix1->SetElement(0,3, transCoordinate[0]);
	//translateMatrix1->SetElement(1,3, transCoordinate[1]);
	//translateMatrix1->SetElement(2,3, transCoordinate[2]);
	//
	vtkMatrix4x4::Multiply4x4(m, initMatrix, RegMatrix); 

	//delete
	TransformPolyDataSource->Delete();
	trans->Delete();
	//translateMatrix1->Delete();
}
void StaticFunctionLibrary::CutterFunction(vtkPolyData* Input, double Point[3], double Normal[3], vtkPolyData* Output)
{
	vtkCutterPointer cutter = vtkCutterPointer::New();
	cutter->SetInput( Input );
	
	vtkPlanePointer plane = vtkPlanePointer::New();
	plane->SetOrigin(Point);
	plane->SetNormal(Normal);
	plane->Modified();		

	cutter->SetCutFunction( plane );
	cutter->SetValue(0, 0);

	vtkMyStripperPointer stripper = vtkMyStripperPointer::New();
	stripper->SetInputConnection(cutter->GetOutputPort());
	stripper->Update();

	Output->DeepCopy(stripper->GetOutput());

}

void StaticFunctionLibrary::ComputeMedian(double* sequence, int NumberOfElements, double& Median)
{
	// Copy over sequence
	double* sequenceCopy = new double [NumberOfElements];
	
	int* index = new int [NumberOfElements];

	int i;

	for (i = 0; i < NumberOfElements; i++)
	{
		sequenceCopy[i] = sequence[i];
		index[i] = i;
	}

	StaticFunctionLibrary::SimpleSort(sequenceCopy, index, NumberOfElements);

	if (NumberOfElements % 2)
	{
        Median = sequenceCopy[(NumberOfElements-1)/2];
	}
	else
	{
		Median = 0.5 * sequenceCopy[NumberOfElements/2-1] + 0.5 * sequenceCopy[NumberOfElements/2];
	}

	delete [] sequenceCopy;
	delete [] index;
}

int StaticFunctionLibrary::ReadPointsAndBuild4x4Matrix(CString inputfile, vtkMatrix4x4* ResliceAxes)
{
	vtkPointsPointer Points = vtkPointsPointer::New();
	if (!StaticFunctionLibrary::ReadTovtkPoints(inputfile, Points))
	{
		return 0;
	}
	StaticFunctionLibrary::Build4x4Matrix(Points, ResliceAxes);
	return 1;
}

int StaticFunctionLibrary::ReadTovtkPoints(CString Filename, vtkPoints* Points)
{
	std::ifstream *fp;
	
	fp = new std::ifstream((LPCSTR) Filename, ios::in);
	if(fp->fail()) 
	{
		CString filenamecomplete = "Could not open the specified file: " + Filename;
		StaticFunctionLibrary::CustomMessageBox(filenamecomplete);
		return 0; // is null
	}
	
	char coord[256];
	float irow[3];
	
	unsigned int count = 0;
	
	while(1)
	{
		fp->getline(coord, 255, '\n');

		if (strcmp(coord, ""))
		{
			sscanf(coord, "%f %f %f", &irow[0], &irow[1], &irow[2]);
			//TRACE("xyz: %f %f\n", xy[0], xy[1]);
			Points->InsertPoint(count, irow);
			count++;
		}
		else
		{
			break;
		}
	}
	
	fp->close();
	delete fp;

	return 1;
}

int StaticFunctionLibrary::Build4x4Matrix(vtkPoints* Points, vtkMatrix4x4* Matrix)
{
	// Big change on June 17 2014. 
	// Input: Points contains four points, which is four corner of an image:
	//        Point 1 has index (i1,j1), Point 2 has index (j1, i1), Point 3 has index (i2, j2) and Point 4 has index (i1, j2)
	// Output: 4x4 matrix built to reslice image.

	// find origin: Origin points to its row neighbour (same j) with a vector that has x-coor positive. 
	// It points to its column neighbour (same i) with a vector that has y-coor positive.

	int i = 0;

	double Pointi[3], RowNeighbouri[3], ColumnNeighbouri[3];

	// Lookup table looking up the index of row neighbour and column neighbour of each point i;
	vtkIdType RowNeighbourIndex[4] = {1, 0, 3, 2};
	vtkIdType ColumnNeighbourIndex[4] = {3, 2, 1, 0};

	double VecToRowNeighbour[3], VecToColumnNeighbour[3];

	int OriginIndex = -1;

	double Origin[3], xaxis[3], yaxis[3], zaxis[3];

	for (i = 0; i < 4; i++)
	{
		Points->GetPoint(i, Pointi);
		Points->GetPoint(RowNeighbourIndex[i], RowNeighbouri);
		Points->GetPoint(ColumnNeighbourIndex[i], ColumnNeighbouri);

		Vector(RowNeighbouri, Pointi, VecToRowNeighbour);
		Vector(ColumnNeighbouri, Pointi, VecToColumnNeighbour);

		if (VecToRowNeighbour[0] > 0 && VecToColumnNeighbour[1] > 0)
		{
			OriginIndex = i;
			CopyPoint(Origin, Pointi);
			CopyPoint(xaxis, VecToRowNeighbour);
			vtkMath::Normalize(xaxis);
            vtkMath::Cross(xaxis, VecToColumnNeighbour, zaxis);
			vtkMath::Normalize(zaxis);
			vtkMath::Cross(zaxis, xaxis, yaxis);
			break;
		}
	}

	if (OriginIndex < 0)
	{
		StaticFunctionLibrary::CustomMessageBox("Could not find origin!");
		return 0;
	}

	Matrix->Identity();
	for (i = 0; i < 3; i++)
	{
		Matrix->Element[i][0] = xaxis[i];
		Matrix->Element[i][1] = yaxis[i];
		Matrix->Element[i][2] = zaxis[i];
		Matrix->Element[i][3] = Origin[i];
	}

	return 1;
}



// vtkImageData. A pixel with point id PointId has index (i,j,k). Increments can be obtained by imdata->GetIncrements().
//void StaticFunctionLibrary::ConvertPointIdToIndex(int PointId, int ijk[3], int Increments[3])
//{
//	ijk[2] = PointId/Increments[2];
//	ijk[1] = (PointId%Increments[2])/Increments[1];
//	ijk[0] = ((PointId%Increments[2])%Increments[1])/Increments[0];
//}

void StaticFunctionLibrary::ComputePointStructuredCoords(const vtkIdType cellId, int dim[3], int ijk[3])
{
	 int Ni  = dim[0];
     int Nj  = dim[1];
 
     int Nij = Ni*Nj;
 
     int k = cellId/Nij + 1;
     int j = (cellId - (k-1)*Nij)/Ni + 1;
     int i = cellId - (k-1)*Nij - (j-1)*Ni + 1;
     ijk[0] = i-1;
     ijk[1] = j-1;
     ijk[2] = k-1;
}

// Check whether InputSurface is closed
int StaticFunctionLibrary::IsClosedSurface(vtkPolyData* InputSurface)
{
  vtkFeatureEdgesPointer featureEdges = vtkFeatureEdgesPointer::New();
  featureEdges->FeatureEdgesOff();
  featureEdges->BoundaryEdgesOn();
  featureEdges->NonManifoldEdgesOn();
  featureEdges->SetInput(InputSurface);
  featureEdges->Update();
 
  int numberOfOpenEdges = featureEdges->GetOutput()->GetNumberOfCells();
 
  int closed = (numberOfOpenEdges > 0)?0:1;

  return closed;
}

int StaticFunctionLibrary::IsContour(const char* InputFilename)
{
	// Helper to button ResliceProstateNIFTI
	// The reason we need to identify images that are contours is that contours should be interpolated using nearest neighbour instead of cubic spline. 
	// If ImageNames[i] has _Contour at the end of the filename, it is a contour image. 
	CString inString (InputFilename);

	inString.Trim();

	CString OutputString;

	int id = inString.ReverseFind('_');

	if (id == -1)
	{
		return 0;
	}
	else 
	{
		OutputString = inString.Right(7);

		if (!OutputString.Compare("Contour"))
		{
			// filename ends with _Contour
			return 1;
		}
		else
		{
			return 0;
		}
	}

}