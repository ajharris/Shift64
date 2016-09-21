// Author: Bernard Chiu
// Date Created: July 06 2009

// vtk Class to correspond two surfaces as described in Papademetris 2002
#include "stdafx.h"
#include "Symmetric3DCorrespondence.h"
//#include "StaticFunctionLibrary.h"

//#include "RotateMFC.h"
//#include "RotateDlg.h"

#include "vtkCellArray.h"
//#include "vtkCellData.h"
//#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
//#include "vtkImageData.h"
//#include "vtkMatrix4x4.h"
//#include "vtkTransform.h"
//#include "vtkTransformPolyDataFilter.h"
//#include "vtkMarchingCubes.h"
//#include "vtkImageCast.h"
#include "vtkIdList.h"
//#include "vtkAppendPolyData.h"
#include "vtkExecutive.h"
#include "vtkCellLocator.h"
#include "vtkPointLocator.h"
#include "vtkGenericCell.h"
#include "vtkPolyDataNormals.h"
#include "vtkCleanPolyData.h"
#include <math.h>
#include <vector>
#include <map>

vtkStandardNewMacro(Symmetric3DCorrespondence);

Symmetric3DCorrespondence::Symmetric3DCorrespondence(void)
{
	this->Weight = 0.75;
    this->Tolerance = 5e-3;
	this->SignedThickness = 0;
	this->SetNumberOfInputPorts(2);
	this->SetNumberOfOutputPorts(2);

	this->GenerateCorrespondingLines = 0;
	vtkPolyData* CorrespondingLines = vtkPolyData::New();
	this->GetExecutive()->SetOutputData(1, CorrespondingLines);
	CorrespondingLines->Delete();
}

Symmetric3DCorrespondence::~Symmetric3DCorrespondence(void)
{
}

int Symmetric3DCorrespondence::RequestData(vtkInformation* request, 
										   vtkInformationVector** inputVector, 
										   vtkInformationVector* outputVector)
{
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *in2Info = inputVector[1]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	vtkPolyData* Surface1 = vtkPolyData::SafeDownCast(
		inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData* Surface2 = vtkPolyData::SafeDownCast(
		in2Info->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData* output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));	

	// Initialize vtkPointLocator
	vtkPointLocator* PointLocator1 = vtkPointLocator::New();
	PointLocator1->SetDataSet(Surface2);
	PointLocator1->BuildLocator();

	vtkPointLocator* PointLocator2 = vtkPointLocator::New();
	PointLocator2->SetDataSet(Surface1);
	PointLocator2->BuildLocator();

	double tempPoint1[3], tempPoint2[3];
	vtkIdType ptid2, ptid1;
	vtkIdList* PointIdWithCorr = vtkIdList::New();
	vtkIdList* PointIdWOCorr = vtkIdList::New();
	vtkPoints* CorrPoints = vtkPoints::New();
	CorrPoints->SetNumberOfPoints(Surface1->GetNumberOfPoints());
    
	int i,j;
	// Step 1: Find symmetric correspondence
	for (i = 0; i < Surface1->GetNumberOfPoints(); i++)
	{
        Surface1->GetPoint(i, tempPoint1);
		ptid2 = PointLocator1->FindClosestPoint(tempPoint1);
		Surface2->GetPoint(ptid2, tempPoint2);
		ptid1 = PointLocator2->FindClosestPoint(tempPoint2);

		if (i == ptid1)
		{
			PointIdWithCorr->InsertNextId(i);
			CorrPoints->SetPoint(i, tempPoint2);
		}
		else
		{
			PointIdWOCorr->InsertNextId(i);
		}
	}

	// Step 2: Symmetric Nearest Neighbour in Surfaces
	vtkIdList* PointIdNeighbours = vtkIdList::New();
	vtkIdType tempPointId;
	vtkIdType thispid;
	double tempDis[3], Dis[3];
	double tempPoint[3];
	double thispointtilde[3];

	// cell Locator to find closest point
	vtkCellLocator* cellLocator = vtkCellLocator::New();
	cellLocator->SetDataSet(Surface2);
	cellLocator->SetNumberOfCellsPerBucket(1);
	cellLocator->BuildLocator();

	vtkGenericCell* cell = vtkGenericCell::New();
	vtkIdType cellId, subId;
	double dist2;

	vtkIdList* PointIdGotCorrThisStep = vtkIdList::New();

	Surface1->BuildLinks();

	while (PointIdWOCorr->GetNumberOfIds() > 0)
	{
		PointIdGotCorrThisStep->Initialize();

		for (i = 0; i < PointIdWOCorr->GetNumberOfIds(); i++)
		{
			PointIdNeighbours->Initialize();

			thispid = PointIdWOCorr->GetId(i);
			this->GetFirstLevelNeighbour(Surface1, thispid, PointIdNeighbours);

			if (PointIdNeighbours->GetNumberOfIds() == 0)
			{
				continue;
			}

			int count = 0;
			tempDis[0] = 0.0;
			tempDis[1] = 0.0;
			tempDis[2] = 0.0;

			// Find Corresponding Points Of these neighbours;
			for (j = 0; j < PointIdNeighbours->GetNumberOfIds(); j++)
			{
				tempPointId = PointIdNeighbours->GetId(j);
				if (PointIdWithCorr->IsId(tempPointId)>=0)
				{
					// tempPointId has correspondence
					CorrPoints->GetPoint(tempPointId, tempPoint);
					Surface1->GetPoint(tempPointId, tempPoint1);

					Dis[0] = tempPoint[0] - tempPoint1[0];
					Dis[1] = tempPoint[1] - tempPoint1[1];
					Dis[2] = tempPoint[2] - tempPoint1[2];

					tempDis[0] += Dis[0];
					tempDis[1] += Dis[1];
					tempDis[2] += Dis[2];

					count++;
				}
			}

			if (count == 0)
			{
				continue;
			}

			tempDis[0] /= count;
			tempDis[1] /= count;
			tempDis[2] /= count;

			Surface1->GetPoint(thispid, thispointtilde);

			thispointtilde[0] += tempDis[0];
			thispointtilde[1] += tempDis[1];
			thispointtilde[2] += tempDis[2];

			cellLocator->FindClosestPoint(thispointtilde, tempPoint2, cell, cellId, subId, dist2);

			PointIdWithCorr->InsertNextId(thispid);
			CorrPoints->SetPoint(thispid, tempPoint2);
			PointIdGotCorrThisStep->InsertNextId(thispid);
		}

		for (j = 0; j < PointIdGotCorrThisStep->GetNumberOfIds(); j++)
		{
			PointIdWOCorr->DeleteId(PointIdGotCorrThisStep->GetId(j));
		}
	}

	// Step 3: Smoothing
	int NumNeighbour;
	double DeltaDist;
	double MaxDeltaDist;//, MaxDeltaDistPrev;
	double Ui[3];
	double CurrentCorrPointi[3];

	vtkCellLocator* cellLocatorSmooth = vtkCellLocator::New();
	cellLocatorSmooth->SetDataSet(Surface2);
	cellLocatorSmooth->SetNumberOfCellsPerBucket(1);
	cellLocatorSmooth->BuildLocator();

	//vtkPoints* CorrPointsPrev = vtkPoints::New();

	//int flagNormalExitWhile = 1;

	//int loopcount = 0;
	//while(loopcount < 2 || MaxDeltaDistPrev > MaxDeltaDist)
	while(1)
	{
		//CorrPointsPrev->Initialize();
		//CorrPointsPrev->DeepCopy(CorrPoints);

		/*if (loopcount != 0)
		{
			MaxDeltaDistPrev = MaxDeltaDist;
		}*/

		for (i = 0; i < Surface1->GetNumberOfPoints(); i++)
		{
			// Find Current Displacement
			CorrPoints->GetPoint(i, CurrentCorrPointi);
			Surface1->GetPoint(i, tempPoint1);

			Ui[0] = CurrentCorrPointi[0] - tempPoint1[0];
			Ui[1] = CurrentCorrPointi[1] - tempPoint1[1];
			Ui[2] = CurrentCorrPointi[2] - tempPoint1[2];

			// Find Average Displacement Vector From Its Neighbours
			PointIdNeighbours->Initialize();
			this->GetFirstLevelNeighbour(Surface1, i, PointIdNeighbours);
			NumNeighbour = PointIdNeighbours->GetNumberOfIds();

			tempDis[0] = 0.0;
			tempDis[1] = 0.0;
			tempDis[2] = 0.0;

			for (j = 0; j < NumNeighbour; j++)
			{
				tempPointId = PointIdNeighbours->GetId(j);
				CorrPoints->GetPoint(tempPointId, tempPoint);
				Surface1->GetPoint(tempPointId, tempPoint1);

				Dis[0] = tempPoint[0] - tempPoint1[0];
				Dis[1] = tempPoint[1] - tempPoint1[1];
				Dis[2] = tempPoint[2] - tempPoint1[2];

				tempDis[0] += Dis[0];
				tempDis[1] += Dis[1];
				tempDis[2] += Dis[2];
			}

			tempDis[0] /= NumNeighbour;
			tempDis[1] /= NumNeighbour;
			tempDis[2] /= NumNeighbour;

			Surface1->GetPoint(i, thispointtilde);

			thispointtilde[0] += this->Weight * Ui[0] + (1 - this->Weight) * tempDis[0];
			thispointtilde[1] += this->Weight * Ui[1] + (1 - this->Weight) * tempDis[1];
			thispointtilde[2] += this->Weight * Ui[2] + (1 - this->Weight) * tempDis[2];

			cellLocatorSmooth->FindClosestPoint(thispointtilde, tempPoint2, cell, cellId, subId, dist2);

			DeltaDist = sqrt(vtkMath::Distance2BetweenPoints(CurrentCorrPointi, tempPoint2));

			if (i == 0 || DeltaDist > MaxDeltaDist)
			{
				MaxDeltaDist = DeltaDist;
			}

			CorrPoints->SetPoint(i, tempPoint2);
		}
        
		if (MaxDeltaDist < this->Tolerance)
		{
			break;
		}
		/*if (MaxDeltaDist < this->Tolerance)
		{
			flagNormalExitWhile = 0;
			break;
		}*/


        //loopcount++;
	}

	// write out output
	// output add input array called Thickness as scalar
	output->DeepCopy(Surface1);

	vtkDoubleArray* Thickness = vtkDoubleArray::New();
	Thickness->SetName("Thickness");
	Thickness->SetNumberOfTuples(Surface1->GetNumberOfPoints());
	Thickness->SetNumberOfComponents(1);

	double* Thicknessptr = Thickness->GetPointer(0);

	output->GetPointData()->SetScalars(Thickness);
	Thickness->Delete();
    
	vtkPolyData* Surface1Copy = vtkPolyData::New();
	Surface1Copy->DeepCopy(Surface1);

	vtkPolyDataNormals* pdNormal = NULL;
	vtkCleanPolyData* pdClean = NULL;
	vtkDataArray* Surface1Normals = NULL;

	if (this->SignedThickness)
	{//Generate Normal for the input
		pdNormal = vtkPolyDataNormals::New();
		pdNormal->SetInput(Surface1Copy);
		pdClean = vtkCleanPolyData::New();
		pdClean->SetInputConnection(pdNormal->GetOutputPort());
		pdClean->Update();
		Surface1Normals = pdClean->GetOutput()->GetPointData()->GetNormals();
	}

	double Surface1Pointi[3], CorrPointsPointi[3], VectorDir[3];
	double Surface1Normalsi[3];

	for (i = 0; i < Surface1->GetNumberOfPoints(); i++)
	{
		//twoids[0] = 2*i;
		//twoids[1] = 2*i+1;
		Surface1->GetPoint(i, Surface1Pointi);
		CorrPoints->GetPoint(i, CorrPointsPointi);

		Thicknessptr[i] = sqrt(vtkMath::Distance2BetweenPoints(
			Surface1Pointi, CorrPointsPointi));

		if (this->SignedThickness)
		{
			VectorDir[0] = CorrPointsPointi[0] - Surface1Pointi[0];
			VectorDir[1] = CorrPointsPointi[1] - Surface1Pointi[1];
			VectorDir[2] = CorrPointsPointi[2] - Surface1Pointi[2];
            
			Surface1Normals->GetTuple(i, Surface1Normalsi);

			if (vtkMath::Dot(VectorDir, Surface1Normalsi) < 0)
			{
				Thicknessptr[i] *= -1;
			}
		}

		//outputPoints->SetPoint(twoids[0], Surface1->GetPoint(i));
		//if (flagNormalExitWhile)
		//{
		//	// Prev is better than current
		//	outputPoints->SetPoint(twoids[1], CorrPointsPrev->GetPoint(i));
		//}
		//else
		//{
		// current is the best
		//outputPoints->SetPoint(twoids[1], CorrPoints->GetPoint(i));
		//}
		//outputLines->InsertNextCell(2, twoids);		
	}

	// GenerateCorrespondingLines Here:
	if (this->GenerateCorrespondingLines)
	{
		vtkPolyData* CorrespondingLinesPtr = this->GetCorrespondingLines();
		CorrespondingLinesPtr->Initialize();

		vtkCellArray* outputLines = vtkCellArray::New();
		CorrespondingLinesPtr->SetLines(outputLines);
		outputLines->Delete();

		vtkPoints* outputPoints = vtkPoints::New();
		outputPoints->SetNumberOfPoints(2*Surface1->GetNumberOfPoints());
		CorrespondingLinesPtr->SetPoints(outputPoints);
		outputPoints->Delete();

		vtkIdType twoids[2];

		for (i = 0; i < Surface1->GetNumberOfPoints(); i++)
		{
			twoids[0] = 2*i;
			twoids[1] = 2*i+1;

			outputPoints->SetPoint(twoids[0], Surface1->GetPoint(i));
			outputPoints->SetPoint(twoids[1], CorrPoints->GetPoint(i));

			outputLines->InsertNextCell(2, twoids);		
		}
	}
	//StaticFunctionLibrary::WritePolyData(testcorrpd, "C:\\testcorr.vtk");

	//delete
	cellLocator->Delete();
	cellLocatorSmooth->Delete();
	PointLocator1->Delete();
	PointLocator2->Delete();
	PointIdWOCorr->Delete();
	PointIdGotCorrThisStep->Delete();
	PointIdNeighbours->Delete();
	cell->Delete();
	CorrPoints->Delete();
	if (pdNormal)
	{//Generate Normal for the input
		pdNormal->Delete();
	}
	if (pdClean)
	{
		pdClean->Delete();
	}

	return 1;
}

// input/output protocol
int Symmetric3DCorrespondence::FillInputPortInformation(int port, vtkInformation* info)
{
	if (port == 0)
	{
		info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
		info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		return 1;
	}
	else if (port == 1)
	{
		info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
		info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		return 1;
	}
	return 0;
}

void Symmetric3DCorrespondence::SetSurface(int SurfaceId, int id, vtkPolyData* pd)
{
	if (SurfaceId < 0 || SurfaceId > 1)
	{
		//StaticFunctionLibrary::CustomMessageBox("Bad SurfaceId! SurfaceId = 0 or 1");
		return;
	}

	if (id < 0)
	{
		//StaticFunctionLibrary::CustomMessageBox("Bad id!");
		return;
	}

	int numConnections = this->GetNumberOfInputConnections(SurfaceId);

	vtkAlgorithmOutput* algOutput = 0;
	if (pd)
	{
		algOutput = pd->GetProducerPort();
	}
	else
	{
		//StaticFunctionLibrary::CustomMessageBox("Cannot Set NULL Source!");
		return;
	}

	if (algOutput)
	{
		if (id < numConnections)
		{
			this->SetNthInputConnection(SurfaceId, id, algOutput);
		}
		else 
		{
			this->AddInputConnection(SurfaceId, algOutput);
		}
	}

}

void Symmetric3DCorrespondence::SetSurfaceConnection(int SurfaceId, int id, vtkAlgorithmOutput* algOutput)
{
	if (SurfaceId < 0 || SurfaceId > 1)
	{
		//StaticFunctionLibrary::CustomMessageBox("Bad SurfaceId! SurfaceId = 0 or 1");
		return;
	}

	if (id < 0)
	{
		//StaticFunctionLibrary::CustomMessageBox("Bad id!");
		return;
	}

	int numConnections = this->GetNumberOfInputConnections(SurfaceId);

	if (algOutput)
	{
		if (id < numConnections)
		{
			this->SetNthInputConnection(SurfaceId, id, algOutput);
		}
		else 
		{
			this->AddInputConnection(SurfaceId, algOutput);
		}
	}
}

vtkPolyData* Symmetric3DCorrespondence::GetSurface(int SurfaceId, int id)
{
	if (id < 0 || id >= this->GetNumberOfInputConnections(SurfaceId))
	{
		return NULL;
	}

	return vtkPolyData::SafeDownCast(this->GetExecutive()->GetInputData(SurfaceId, id));
}
// input/output protocol ends here

void Symmetric3DCorrespondence::GetFirstLevelNeighbour(vtkPolyData* pd, vtkIdType ptidAtpd, 
													   vtkIdList* ptidFirstLevelNeighbour)
{
	// assume pd->BuildLinks() has been called;
	vtkIdList* cellIds = vtkIdList::New();
	pd->GetPointCells(ptidAtpd, cellIds);

	vtkIdType npts, *pts;

	for (int i = 0; i < cellIds->GetNumberOfIds(); i++)
	{
		pd->GetCellPoints(cellIds->GetId(i), npts, pts);
		for (int j = 0; j < npts; j++)
		{
			if (pts[j] != ptidAtpd)
			{
				ptidFirstLevelNeighbour->InsertUniqueId(pts[j]);
			}
		}
	}

	//delete
	cellIds->Delete();
}

vtkPolyData* Symmetric3DCorrespondence::GetCorrespondingLines()
{
	return vtkPolyData::SafeDownCast(this->GetExecutive()->GetOutputData(1));
}