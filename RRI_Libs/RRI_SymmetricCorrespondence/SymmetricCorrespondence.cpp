// SymmetricCorrespondence.cpp: implementation of the SymmetricCorrespondence class.
//
//////////////////////////////////////////////////////////////////////
// This version is adapted for the trianglation problem.

#include "stdafx.h"
//#include "RotateMFC.h"
//#include "RotateDlg.h"
#include "SymmetricCorrespondence.h"

#include "MinMax.h"
#include "vtkCellArray.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"

#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "StaticFunctionLibrary.h"
#include "vtkSmartPointer.h"
#include "vtkIdList.h"

//test
#include "vtkPolyDataWriter.h"
#include ".\symmetriccorrespondence.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SymmetricCorrespondence::SymmetricCorrespondence()
{
	this->MatchFirstPoint = 0;
	this->m_contour1 = vtkPolyData::New();
	this->m_contour2 = vtkPolyData::New();
	this->m_points1 = vtkPolyData::New();
	this->m_points2 = vtkPolyData::New();
	this->AnchorPoints = NULL;
}

SymmetricCorrespondence::SymmetricCorrespondence(vtkPolyData* contour1, vtkPolyData* contour2)
{
	this->m_contour1 = vtkPolyData::New();
	this->m_contour1->DeepCopy(contour1);
	this->m_contour2 = vtkPolyData::New();
	this->m_contour2->DeepCopy(contour2);
	this->m_points1 = vtkPolyData::New();
	this->m_points2 = vtkPolyData::New();
	this->AnchorPoints = NULL;
}

SymmetricCorrespondence::~SymmetricCorrespondence()
{
	if (this->m_contour1)
	{
		this->m_contour1->Delete();
	}

	if (this->m_contour2)
	{
		this->m_contour2->Delete();
	}

	if (this->m_points1)
	{
		this->m_points1->Delete();
	}

	if (this->m_points2)
	{
		this->m_points2->Delete();
	}
}

void SymmetricCorrespondence::SetContour1(vtkPolyData* contour1)
{
	this->m_contour1->Initialize();
	this->m_contour1->DeepCopy(contour1);
}

void SymmetricCorrespondence::SetContour2(vtkPolyData* contour2)
{
	this->m_contour2->Initialize();
	this->m_contour2->DeepCopy(contour2);
}

vtkPolyData* SymmetricCorrespondence::GetContour1()
{
	return this->m_contour1;	
}

vtkPolyData* SymmetricCorrespondence::GetContour2()
{
	return this->m_contour2;
}

BOOL SymmetricCorrespondence::FindCorrespondence()
{
	// This class assumes this->m_contour1->GetPointData() have the same sets of arrays 
	// as this->m_contour2->GetPointData();
	// (i.e., NumberOfArrays of both vtkPointData objects must be the same and the
	// order of the data arrays must also be the same)

	int i, j, k;//, u;
	vtkCellArray* lines1; int maxnum1, maxnum2;
	lines1 = this->m_contour1->GetLines();
	maxnum1 = lines1->GetMaxCellSize();

	int npts1, npts2, *pts1, *pts2;

	this->m_points1->GetPointData()->Initialize();
	this->m_points2->GetPointData()->Initialize();

	StaticFunctionLibrary::CopyArraysFromInputPointData(this->m_contour1->GetPointData(),
		this->m_points1->GetPointData());
	StaticFunctionLibrary::CopyArraysFromInputPointData(this->m_contour2->GetPointData(),
		this->m_points2->GetPointData());

	/*BOOL treatscalar = FALSE;

	vtkDataArray* sourceArray, *sourceArray2;
	vtkDataArray *newArray1, *newArray2;
	int NumberOfArrays1, NumberOfArrays2;
	int arrayIndex1, arrayIndex2;
	int whichAttribute = -1;
	int NumberOfArrays = 0;
	
	//int attributesIndicesArray[vtkPointData::NUM_ATTRIBUTES]; 
	//int	activeScalarsIndex;

	if ((NumberOfArrays1 = this->m_contour1->GetPointData()->GetNumberOfArrays()) > 0 &&
		(NumberOfArrays2 = this->m_contour2->GetPointData()->GetNumberOfArrays()) > 0 &&
		(NumberOfArrays1 == NumberOfArrays2)) 
	{
		treatscalar = TRUE;
		NumberOfArrays = NumberOfArrays1;
		// get name and type of array in this->m_contour1 and this->m_contour2
		// initialize arrays in this->m_points1 and this->m_points2
		this->m_points1->GetPointData()->Initialize();
		this->m_points2->GetPointData()->Initialize();
		//this->m_contour1->GetPointData()->GetAttributeIndices(attributesIndicesArray);
		//activeScalarsIndex = attributesIndicesArray[vtkPointData::SCALARS];

		for (i = 0; i < NumberOfArrays1; i++) 
		{
			sourceArray = this->m_contour1->GetPointData()->GetArray(i);
			sourceArray2 = this->m_contour2->GetPointData()->GetArray(i);
			VERIFY(!strcmp(sourceArray->GetName(), sourceArray2->GetName()));
			newArray1 = sourceArray->NewInstance();
			newArray1->SetNumberOfComponents(sourceArray->GetNumberOfComponents());
			newArray1->SetName(sourceArray->GetName());
			newArray2 = sourceArray2->NewInstance();
			newArray2->SetNumberOfComponents(sourceArray2->GetNumberOfComponents());
			newArray2->SetName(sourceArray2->GetName());

			arrayIndex1 = this->m_points1->GetPointData()->AddArray(newArray1);
			arrayIndex2 = this->m_points2->GetPointData()->AddArray(newArray2);
			newArray1->Delete();
			newArray2->Delete();

			if ((whichAttribute = this->m_contour1->GetPointData()->IsArrayAnAttribute(i))>=0)
			{
				this->m_points1->GetPointData()->SetActiveAttribute(arrayIndex1, whichAttribute);
			}
			if ((whichAttribute = this->m_contour2->GetPointData()->IsArrayAnAttribute(i))>=0)
			{
				this->m_points2->GetPointData()->SetActiveAttribute(arrayIndex2, whichAttribute);
			}
		}
	}*/

	// Feb 14 2005: First I must check and if necessary align them in a consistent orientation
	//if (!this->CheckOrientation()) {
	//	this->m_contour2->ReverseCell(0);
	//}no. check before using this class

	for (lines1->InitTraversal(); lines1->GetNextCell(npts1, pts1); ) 
	{
		if (npts1 == maxnum1) break;
	}

	vtkCellArray* lines2;
	lines2 = this->m_contour2->GetLines();
	maxnum2 = lines2->GetMaxCellSize();

	for (lines2->InitTraversal(); lines2->GetNextCell(npts2, pts2); ) 
	{
		if (npts2 == maxnum2) break;
	}

	float min = 1000.0, distance;
	vtkstd::map<int, int> IntMap1, IntMap2, IntMap;

	// get rid of the duplicated poiints
	if(pts1[npts1-1] == pts1[0])  npts1--;
	if(pts2[npts2-1] == pts2[0])  npts2--;

	vtkPoints* pp1 = this->m_contour1->GetPoints();
	vtkPoints* pp2 = this->m_contour2->GetPoints();

	double x[3]; //*sum; 
	double sum[3];	
	//>>>>first contour vs second contour

	for( i = 0; i < npts1; i++ ) 
	{
		min = 1000.0;

		pp1->GetPoint(pts1[i], sum);
		
		for (j =0; j < npts2; j++)
		{
			pp2->GetPoint(pts2[j], x);
			distance = (float) sqrt((x[0]-sum[0])*(x[0]-sum[0]) + 
									(x[1]-sum[1])*(x[1]-sum[1]) +
									(x[2]-sum[2])*(x[2]-sum[2]));
				
			if ( distance < min )   
			{ 
				min = distance;
				IntMap1[i] = j; // this holds the mappings of point indices
			}				
		}
	}  //<<<<< first contour vs second contour

	//>>> second contour vs first contour
	for( i = 0; i < npts2; i++ ) 
	{
		min = 1000.0;

		pp2->GetPoint(pts2[i], sum);
			
		for (j =0; j < npts1; j++)
		{
			pp1->GetPoint(pts1[j], x);
			distance = (float) sqrt((x[0]-sum[0])*(x[0]-sum[0]) + 
									(x[1]-sum[1])*(x[1]-sum[1]) + 
									(x[2]-sum[2])*(x[2]-sum[2]));
				
			if ( distance < min )   
			{
				min = distance;
				IntMap2[i] = j;
			}
		}
	}//<<< second vs first contour

	///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	////check symmetric relationship
	//vtkstd::map<int, int>::iterator it1, it2;
	//IntMap.clear();

	//if (this->MatchFirstPoint)
	//{
	//	IntMap[0] = 0;
	//}

	//for(it1 = IntMap1.begin(); it1 != IntMap1.end(); it1++)
	//{
	//	if (this->MatchFirstPoint && it1->first == 0)
	//	{
	//		// if this->MatchFirstPoint, IntMap[0] has already been specified and
	//		// we don't want to get conflicting value.
	//		continue;
	//	}

	//	for(it2 = IntMap2.begin(); it2 != IntMap2.end(); it2++)	
	//	{
	//		if (this->MatchFirstPoint && it2->first == 0)
	//		{
	//			continue;
	//		}

	//		if( (*it1).first == (*it2).second &&  (*it1).second == (*it2).first )
	//		{
	//			IntMap[(*it1).first] = (*it1).second ;
	//		}
	//	}
	//}

	// IntMap is the correspondence set established.
	// Make N-1 instances of MinMax where N is the number of AnchorPoints
	// FinalIntMap

		//check symmetric relationship
	vtkstd::map<int, int>::iterator it1, it2;
	IntMap.clear();

	for(it1 = IntMap1.begin(); it1 != IntMap1.end(); it1++)
	{
		for(it2 = IntMap2.begin(); it2 != IntMap2.end(); it2++)	
		{
			if( (*it1).first == (*it2).second &&  (*it1).second == (*it2).first )
			{
				IntMap[(*it1).first] = (*it1).second ;
			}
		}
	}

	//test
	
	ofstream ofile("C:\\cpre.txt");
	vtkstd::map<int, int>::iterator it3;
	for(it3 = IntMap.begin(); it3 != IntMap.end(); it3++)
	{
		ofile << it3->first << " " << it3->second << "\n";
	}
	ofile.close();
	
	vtkstd::map<int, int> FinalIntMap;
	this->ProduceFinalIntMap(IntMap, IntMap2, FinalIntMap, npts2);
	
	ofstream ofile2("C:\\cpost.txt");
	for(it3 = FinalIntMap.begin(); it3 != FinalIntMap.end(); it3++)
	{
		ofile2 << it3->first << " " << it3->second << "\n";
	}
	ofile2.close();
	
	//endtest

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	//==> To determine the direction of the second contour
	//CMinMax<int> minmax;  // use the object to determin max and min values in the vector
	//first determin the allignment of directions in which the two contours are drawn	
					
	//TRACE(" Indices\n");
	//for (it1 = IntMap.begin(); it1 != IntMap.end(); it1++)
	//{
	//	minmax.AddValue((*it1).second);
		//TRACE(" %d, ", (*it1).second); 
	//}

	// will need to test arclength. Arclength has to be monotonically increasing.
	//float arclength2, arclength2last;
	//float DeltaArcLength;
	float flagAL = FALSE;
	double NormalOut1[3], NormalOut2[3];
	//int startindex1;
	//int startindex;
	vtkstd::map<int, int>::iterator it;
	int temp1, temp2;
	/*
 edited June 15 2011
	int temp1, temp2;

	//copy from OldSymmetricCorrespondence.cpp
	CMinMax<int> minmax;
//	minmax.SetOrient(1);
	vtkstd::map<int, int>::iterator it;
	
	int IndexWithMaxValue = 0;
	//int MaxValue = 0;

	for (it = IntMap.begin(); it != IntMap.end(); it++)
	{
		//TRACE("%i, ", it->second);
		minmax.AddValue(it->second);
		// only useful if this->MatchFirstPoint is true;
		//if (it->second > MaxValue)
		//{
		//	MaxValue = it->second;
		//	IndexWithMaxValue = minmax.NumberOfValues() - 1;
		//}
	}

	// only useful if this->MatchFirstPoint is true;
	// minmax does not allow user to set minI and maxI
	
	// strategy: find max local max (of course, there could be a number of local max)
	// if max local max is at the first half --> use end point;
	// if max local max is at the second half --> use max local max;
	// if max local max does not exist --> use end point;

	int maxlocalmax, maxlocalmaxIndex, maxlocalmaxExist = 0;

	for (i = 1; i < minmax.NumberOfValues() - 2; i++)
	{
		if ((minmax[i] > minmax[i-1]) && (minmax[i] > minmax[i+1]))
		{
			maxlocalmaxExist = 1;
			maxlocalmax = minmax[i];
			maxlocalmaxIndex = i;
		}
	}

	int maxlocalmaxIndexGreaterThanHalf;
	
	if (maxlocalmaxExist)
	{
		maxlocalmaxIndexGreaterThanHalf= (maxlocalmaxIndex > minmax.NumberOfValues()/2);
	}

	int last, secondlast;
	
	if ((!maxlocalmaxExist)||(maxlocalmaxExist && !maxlocalmaxIndexGreaterThanHalf))
	{//use end point
		for (i = minmax.NumberOfValues() - 1; i > 0; i--)
		{
			// handle case where
			// index   0 ......44 45
			//         0 ..... 45 1
			
			IndexWithMaxValue = i;
			
			// e.g., 1 > 45? no thus continue;
			
			if ((last = minmax[i]) > (secondlast = minmax[i-1]))
			{
				break;
			}
		}
	}
	else
	{//use maxlocalmax
		IndexWithMaxValue = maxlocalmaxIndex;
	}

	minmax.Set_setMinMaxIndex(this->MatchFirstPoint);
	// if this->MatchFirstPoint is false, set minI and maxI is prohibited by minmax;
	// thus the following two lines are ok.
	minmax.Set_minI(0);
	minmax.Set_maxI(IndexWithMaxValue);
	
	minmax.DetermineExceptionsInterface();

	for( minmax.ExceptionTraversalInitialize(); minmax.MoreException(); )
	{
		int eraseIndex = minmax.NextException();
		//TRACE("%i, ", minmax.NextException()); 
		IntMap.erase(IntMap2[eraseIndex]);
	}
end edited June 15 2011*/
	/*
	TRACE("\n");

	for (it = IntMap.begin(); it != IntMap.end(); it++)
	{
		TRACE("%i, ", it->second);
	}
	*/
	//end test

	vtkIdList* IndexToDelete = vtkIdList::New();

	//int enteredLoop = 0;
	for (it = FinalIntMap.begin(); it != FinalIntMap.end(); it++)
	{
		//if (this->MatchFirstPoint && !enteredLoop)
		//{
		//	// don't need to check the first point
		//	enteredLoop = 1;
		//	continue;		
		//}

		// Don't check anchor points
		
		temp1 = it->first;

		if (this->AnchorPoints->count(temp1) > 0)
		{
			continue;
		}
	
		temp2 = it->second;

		SymmetricCorrespondence::DetermineOutwardNormal(this->m_contour1, temp1, NormalOut1);
		//TRACE ("2:%i\n", (*itnow).second);
		SymmetricCorrespondence::DetermineOutwardNormal(this->m_contour2, temp2, NormalOut2);	

		if (vtkMath::Dot(NormalOut1, NormalOut2) < 0) 
		{
			//TRACE("NormalOut1: %f, %f, %f", NormalOut1[0], NormalOut1[1], NormalOut1[2]);
			//TRACE("NormalOut2: %f, %f, %f", NormalOut2[0], NormalOut2[1], NormalOut2[2]);
			TRACE ("Dot: %f", vtkMath::Dot(NormalOut1, NormalOut2));
			IndexToDelete->InsertUniqueId(temp1);
		}
	}

	/*
	// July 24 2006 -- not allowed to go more than one cycle on the second curve 
	// (enforced if this->MatchFirstPoint)
	float ArcLengthSecond = StaticFunctionLibrary::ArcLength(this->GetContour2());
	float ArcLengthTraversed = 0.0;

	vtkstd::map<int,int>::iterator itnow;
	std::vector<int> IndexToDelete;

	// Nov. 19 2004 -- we need to check the orientation of the outward normal.
	//TRACE("\n\nbefore correction:\n");
	for (k = 0, itnow = IntMap.begin() ; k < IntMap.size(); k++, itnow++) 
	{
		// monotonically increasing
	
		//TRACE ("%d,", itnow->second);

		vtkPolyData* towrite = vtkPolyData::New();
		vtkCellArray* carray = vtkCellArray::New();
		vtkPoints* cpoints = vtkPoints::New();
		cpoints->InsertNextPoint(this->m_contour1->GetPoint(pts1[itnow->first]));
		cpoints->InsertNextPoint(this->m_contour2->GetPoint(pts2[itnow->second]));
		carray->InsertNextCell(2);
		carray->InsertCellPoint(0); carray->InsertCellPoint(1);
		towrite->SetPoints(cpoints);
		towrite->SetLines(carray);
		
		vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
		char buffer[200];
		sprintf(buffer, "D:\\c\\pre_correspondence_%i.vtk", k); 
		writer->SetFileName(buffer);
		writer->SetInput(towrite);
		writer->Write(); writer->Delete();
		
		towrite->Delete();
		cpoints->Delete();
		carray->Delete();
		
		//test
		//float t1 = StaticFunctionLibrary::ArcLength(this->m_contour1);
		//float t2 = StaticFunctionLibrary::ArcLength(this->m_contour2);
		//float s1, s2;

		temp1 = itnow->first;
		temp2 = itnow->second;

		if (flagAL)
		{
			// arclength was calculated from the first correspondence point in the vtkpolydata
			// This ensure we have no discontinuity in the arclength parameterization.
			// assume orientation is the same, which is not a problem in our data
			//test

			
			//s1 = SymmetricCorrespondence::Arclength(this->m_contour1, startindex1, itnow->first);
			//s1 /= t1;
			//arclength2 = SymmetricCorrespondence::Arclength(this->m_contour2, startindex2, (*itnow).second);
			//s2 = arclength2/t2;
			DeltaArcLength = SymmetricCorrespondence::Arclength(this->m_contour2, startindex, itnow->second);
			ArcLengthTraversed += DeltaArcLength;

			if (DeltaArcLength < 0)
			{
				//TRACE("%s%i", "Deleted:", k);
				IndexToDelete.push_back(itnow->first);
				continue;
			}
			
			if (this->MatchFirstPoint && ArcLengthTraversed > ArcLengthSecond)
			{
				//IndexToDelete.push_back(itnow->first);
				//delete hereafter and break;
				for(;itnow!=IntMap.end(); itnow++)
				{
					TRACE("%d ", itnow->first);
					IndexToDelete.push_back(itnow->first);
				}
				break;
			}

		}
		
		// find normal -- magnitude/direction of the outward normal
		// (Very important! don't remove this test in the future -- Nov. 23, 2005)

		// don't even bother checking 
		if (k != 0 || !this->MatchFirstPoint)
		{
			//TRACE ("1:%i\n", (*itnow).first);
			SymmetricCorrespondence::DetermineOutwardNormal(this->m_contour1, (*itnow).first, NormalOut1);
			//TRACE ("2:%i\n", (*itnow).second);
			SymmetricCorrespondence::DetermineOutwardNormal(this->m_contour2, (*itnow).second, NormalOut2);	
			if (vtkMath::Dot(NormalOut1, NormalOut2) < 0) 
			{
				//TRACE ("Dot: %f %i", vtkMath::Dot(NormalOut1, NormalOut2),(*itnow).second );
				IndexToDelete.push_back((*itnow).first);
				continue;
			}
		}		

		startindex = itnow->second;

		if (!flagAL)
		{
			flagAL = 1;
		}
		/*
		if (flagAL)
		{
			startindex = itnow->second;
			//arclength2last = arclength2;
		}
		else 
		{
			//test
			//startindex1 = itnow->first;
			
			startindex = (*itnow).second;
			
			
			
			//arclength2last = 0;
			flagAL = TRUE;
		}
		
	}
	
	for (k = 0; k < IndexToDelete.size(); k++) 
	{
		//TRACE("%d, ", IndexToDelete[k]);
		IntMap.erase(IndexToDelete[k]);
	}
*/

//test
		vtkPolyData* PolyDataForTest = vtkPolyData::New();
		vtkPoints* PointForTest = vtkPoints::New();
		PolyDataForTest->SetPoints(PointForTest);
		vtkCellArray* CellArrayForTest = vtkCellArray::New();
		PolyDataForTest->SetLines(CellArrayForTest);

		vtkstd::map<int, int>::iterator itnow; int v;
	for (v = 0, itnow = FinalIntMap.begin() ; v < FinalIntMap.size(); v++, itnow++) 
	{
		// monotonically increasing
	
		//TRACE ("%d,", itnow->second);

			PointForTest->InsertNextPoint(this->m_contour1->GetPoint(pts1[itnow->first]));
			PointForTest->InsertNextPoint(this->m_contour2->GetPoint(pts2[itnow->second]));
			CellArrayForTest->InsertNextCell(2);
			CellArrayForTest->InsertCellPoint(2*v);
			CellArrayForTest->InsertCellPoint(2*v+1);
		}

		StaticFunctionLibrary::WritePolyData(PolyDataForTest, "C:\\polydatafortestqq.vtk");

	for (i = 0; i < IndexToDelete->GetNumberOfIds(); i++) 
	{
	//	TRACE ("Deleted:%i\n", IndexToDelete[k]);
		FinalIntMap.erase(IndexToDelete->GetId(i));
	}


	//TRACE("\nValue corrected 2\n");
	//for (it1 = IntMap.begin(); it1 != IntMap.end(); it1++)
	//{
	//	TRACE("%d,", (*it1).second); 
	//}

	//test
	ofstream ofile3("C:\\cpost2.txt");
	for(it3 = FinalIntMap.begin(); it3 != FinalIntMap.end(); it3++)
	{
		ofile3 << it3->first << " " << it3->second << "\n";
	}
	ofile3.close();
	//endtest

	//if( IntMap.size() == 0  )   
	//{
	//	//just find out IntMap1.Begin and connect according to arclength
	//	it1 = IntMap1.begin();
	//	IntMap.clear();
	//	TRACE(" %d, %d\n", (*it1).first, (*it1).second);  
	//	IntMap[(*it1).first] = (*it1).second;
	//	for (unsigned int jk = 0; jk < IntMap1.size()/2; jk++) {
	//		it1++;
	//	}
	//	TRACE(" %d, %d\n", (*it1).first, (*it1).second);
	//	IntMap[(*it1).first] = (*it1).second;
	//}
	//pp2 = this->m_contour2->GetPoints();
	//aligned2->Delete();
	//////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// - for those points that don't have symmetric nearest neighbour,
	// intepolate to find their neighbour
	// See X. Papademetris, A. J. Sinusas, D. P. Dione, R. T. Constable, J. S. Duncan, 
	// "Estimation of 3D Left Ventricular Deformation From Medical Images Using 
	// Biomechanical Models, " IEEE Trans. Medical Imaging, Vol. 21, No. 7, pp. 786-800, 
	// July 2002.

	// Problem: We want to connect each sample point of either contour to a sample point 
	// on another contour. Also, we don't want to add additional points

	// How can we solve it?
	// For each orphan point in contour1, we find another point with an arclength closest
	// to this. (lousy description, but I will have short-term memory on it. Need to rewrite).


	float r = 0.0f;
	int count = 0;
	//float firstPoint[3], secondPoint[3];
	int first1, first2, second1, second2;
	float tdis =0., tdis2=0., *dist1=NULL, *dist2=NULL;
	int n, m, maxmn, minmn;
	double thisPoint[3], nextPoint[3];
	// These parameters are used to indicate which contour was chosen
	// to be the first contour, which has more point in a particular segment.
	vtkPoints* firstvtkPoints, *secondvtkPoints;
	int* ptsfirstvtkPoints, *ptssecondvtkPoints;
	int firstvtkPoints1, firstvtkPoints2;
	int secondvtkPoints1, secondvtkPoints2;
	int nptsfirstvtkPoints, nptssecondvtkPoints;
	vtkPolyData* firstPoly, *secondPoly;
	vtkPolyData* firstPolyOut, *secondPolyOut;
	//float scalarout1i, scalarout2i;

	//int imax;
	float difference, mindiff = VTK_LARGE_FLOAT;
	int jmin;
	double* secondins;
	float dthis;
	BOOL flag;
	int GetPointIndex;
	//test
	int count1 = 0;
	int lastjmin, kstart;

	vtkPoints* Pt_m_points1 = vtkPoints::New();
	vtkPoints* Pt_m_points2 = vtkPoints::New();
	this->m_points1->SetPoints(Pt_m_points1);
	this->m_points2->SetPoints(Pt_m_points2);
	Pt_m_points1->Delete();
	Pt_m_points2->Delete();

	for (it1 = FinalIntMap.begin(); it1 != FinalIntMap.end(); it1++)
	{
		first1 = (*it1).first;
		second1 = (*it1).second;
		
		//test
		/*
		vtkPolyData* towrite = vtkPolyData::New();
		vtkCellArray* carray = vtkCellArray::New();
		vtkPoints* cpoints = vtkPoints::New();
		cpoints->InsertNextPoint(this->m_contour1->GetPoint(pts1[first1]));
		cpoints->InsertNextPoint(this->m_contour2->GetPoint(pts2[second1]));
		carray->InsertNextCell(2);
		carray->InsertCellPoint(0); carray->InsertCellPoint(1);
		towrite->SetPoints(cpoints);
		towrite->SetLines(carray);
		
		vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
		char buffer[200];
		sprintf(buffer, "D:\\c\\pre_correspondence_%i.vtk", first1); 
		writer->SetFileName(buffer);
		writer->SetInput(towrite);
		writer->Write(); writer->Delete();
		
		towrite->Delete();
		cpoints->Delete();
		carray->Delete();
		endtest
		*/

		if(first1 != -1)
		{// symmetric found
			it1++;
			if(it1 != FinalIntMap.end())
			{
				first2 = (*it1).first;
				second2 = (*it1).second;
			}
			else{
				first2 = (*(FinalIntMap.begin())).first;
				second2 = (*(FinalIntMap.begin())).second;
			}
			it1--;

			//float cpos1, cpos2, at, pos[3], pos2[3];
			double pos[3];
			tdis = 0; tdis2 = 0;

			//TRACE ("1[%i %i]: (%f %f %f, %f %f %f)\n",
			//first1,second1,
			//	this->m_contour1->GetPoint(pts1[first1])[0],
			//	this->m_contour1->GetPoint(pts1[first1])[1],
			//	this->m_contour1->GetPoint(pts1[first1])[2],
			//	this->m_contour2->GetPoint(pts2[second1])[0],
			//	this->m_contour2->GetPoint(pts2[second1])[1],
			//	this->m_contour2->GetPoint(pts2[second1])[2]);

			//TRACE ("2[%i %i]: (%f %f %f, %f %f %f)\n\n",
			//	first2,second2,
			//	this->m_contour1->GetPoint(pts1[first2])[0],
			//	this->m_contour1->GetPoint(pts1[first2])[1],
			//	this->m_contour1->GetPoint(pts1[first2])[2],
			//	this->m_contour2->GetPoint(pts2[second2])[0],
			//	this->m_contour2->GetPoint(pts2[second2])[1],
			//	this->m_contour2->GetPoint(pts2[second2])[2]);

			//test
/*			
			vtkPolyData* towrite = vtkPolyData::New();
			vtkCellArray* carray = vtkCellArray::New();
			vtkPoints* cpoints = vtkPoints::New();
			cpoints->InsertNextPoint(this->m_contour1->GetPoint(pts1[first1]));
			cpoints->InsertNextPoint(this->m_contour2->GetPoint(pts2[second1]));
			carray->InsertNextCell(2);
			carray->InsertCellPoint(0); carray->InsertCellPoint(1);
			towrite->SetPoints(cpoints);
			towrite->SetLines(carray);

			vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
			char buffer[200];
			sprintf(buffer, "D:\\c\\correspondence_%i.vtk", count1); 
			count1++;
			writer->SetFileName(buffer);
			writer->SetInput(towrite);
			writer->Write(); writer->Delete();

			towrite->Delete();
			cpoints->Delete();
			carray->Delete();
			//-----------------------------
*/			
			n = (first2-first1+npts1)%npts1;// number of points in the unmatched segment
			
			//m = (orient == 1)?((second2-second1+npts2)%npts2):((second1-second2+npts2)%npts2);
			m = (second2-second1+npts2)%npts2;
			// do not count the first point, count the last point.

			if ((first2 == first1) && (second2 == second1))
			{
				n = npts1; m = npts2;
			}

			if( (n > 1) || (m > 1) )
			{	
				maxmn = (n>=m)?n:m;
				dist1 = new float[maxmn];
				if (n >= m) {
					firstvtkPoints = pp1;
					secondvtkPoints = pp2;
					ptsfirstvtkPoints = pts1;
					ptssecondvtkPoints = pts2;
					firstvtkPoints1 = first1;
					secondvtkPoints1 = second1;
					firstvtkPoints2 = first2;
					secondvtkPoints2 = second2;	
					nptsfirstvtkPoints = npts1;
					nptssecondvtkPoints = npts2;
					firstPoly = this->m_contour1;
					secondPoly = this->m_contour2;
					firstPolyOut = this->m_points1;
					secondPolyOut = this->m_points2;
					flag = TRUE;
				}
				else{
					firstvtkPoints = pp2;
					secondvtkPoints = pp1;
					ptsfirstvtkPoints = pts2;
					ptssecondvtkPoints = pts1;
					firstvtkPoints1 = second1;
					secondvtkPoints1 = first1;
					firstvtkPoints2 = second2;
					secondvtkPoints2 = first2;
					nptsfirstvtkPoints = npts2;
					nptssecondvtkPoints = npts1;
					firstPoly = this->m_contour2;
					secondPoly = this->m_contour1;
					firstPolyOut = this->m_points2;
					secondPolyOut = this->m_points1;
					flag = FALSE;
				}
				
				// block for determining dist1
				// both contours are of the same orientation
				// as determined by the class minmax

				firstvtkPoints->GetPoint(ptsfirstvtkPoints[firstvtkPoints1], thisPoint);
				for (j=0; j< maxmn; j++)
				{
					GetPointIndex = (j+firstvtkPoints1+1)%nptsfirstvtkPoints;
					
					firstvtkPoints->GetPoint(ptsfirstvtkPoints[GetPointIndex], nextPoint);
					//this->m_points1->InsertPoint(count, x); // set m_points1
					//TRACE ("Inserted1 %i: (%f %f %f)\n", count, x[0],x[1],x[2]);//test
					//TRACE ("Inserted1 proof %i: (%f %f %f)\n", count, this->m_points1->GetPoint(count)[0],
					//this->m_points1->GetPoint(count)[1],this->m_points1->GetPoint(count)[2]);
					dthis = sqrt( (thisPoint[0]-nextPoint[0])*(thisPoint[0]-nextPoint[0]) + 
						(thisPoint[1]-nextPoint[1])*(thisPoint[1]-nextPoint[1]) + 
						(thisPoint[2]-nextPoint[2])*(thisPoint[2]-nextPoint[2]) );
					// e.g., dist1[0] = dist(pts[first1], pts[first1 + 1 mod npts] etc.
					dist1[j] = tdis + dthis;
					tdis += dthis; // total distance of the segment between 
					// pts[first1] and pts[first2]
					thisPoint[0] = nextPoint[0]; 
					thisPoint[1] = nextPoint[1]; 
					thisPoint[2] = nextPoint[2];
				}
					
				for (j = 0; j < maxmn; j++) 
				{
					dist1[j] = dist1[j]/tdis; // normalize by arclength
				}
					
				// so far we have chosen the segment with more points
				// and have entered the normalized arclength of points into dist1
					
				minmn = (n>=m)?m:n;
				dist2 = new float[minmn + 1]; dist2[0] = 0;
				
				secondvtkPoints->GetPoint(ptssecondvtkPoints[secondvtkPoints1], thisPoint);
					
				for (j = 1; j < minmn + 1; j++)
				{
					GetPointIndex = (j+secondvtkPoints1)%nptssecondvtkPoints;
					secondvtkPoints->GetPoint(ptssecondvtkPoints[GetPointIndex], nextPoint);
					dthis = (float) sqrt( (thisPoint[0]-nextPoint[0])*(thisPoint[0]-nextPoint[0]) + 
							(thisPoint[1]-nextPoint[1])*(thisPoint[1]-nextPoint[1]) + 
							(thisPoint[2]-nextPoint[2])*(thisPoint[2]-nextPoint[2]) );			
					dist2[j] = tdis2 + dthis;
					tdis2 += dthis; 
						// total distance between the segment pts[second1] and pts[second2]
					thisPoint[0] = nextPoint[0]; 
					thisPoint[1] = nextPoint[1]; 
					thisPoint[2] = nextPoint[2];
				}
					
				for (j = 0; j < minmn + 1; j++)
				{
					dist2[j] = dist2[j]/tdis2;
				}// Done finding dist2
					
				// see documentation 
				lastjmin = 0;

				for(i = 0; i < maxmn; i++)
				{
					mindiff = VTK_LARGE_FLOAT;
					firstvtkPoints->GetPoint(ptsfirstvtkPoints[(i+firstvtkPoints1+1)%nptsfirstvtkPoints], pos);

					for (j = 0; j < minmn + 1; j++) 
					{
						difference = dist1[i] - dist2[j];
						difference = (difference >= 0)?difference:-difference;
						// absolute difference.
						
						if (mindiff > difference){
							mindiff = difference;
							jmin = j;
						}
					}
					
					kstart = (jmin == lastjmin)?lastjmin:(lastjmin+1);
					
					for (k = kstart; k <= jmin; k++) 
					{
						secondins = secondvtkPoints->GetPoint(ptssecondvtkPoints[(k+secondvtkPoints1)%nptssecondvtkPoints]);
					
						// m_points1 is of type vtkPolyData* now
						// pos is the first correspondence
						// secondins is the second correspondence
					
						Pt_m_points1->InsertPoint(count,(flag?pos: secondins));
						Pt_m_points2->InsertPoint(count,(flag?secondins :pos));
					
						//if (treatscalar) 
						//{
							int inputIndex = ptsfirstvtkPoints[(i+firstvtkPoints1+1)%nptsfirstvtkPoints];
							StaticFunctionLibrary::ComputeAndInsertPointData(firstPoly->GetPointData(),
								firstPolyOut->GetPointData(), inputIndex, inputIndex, 0.0,
								count);
							inputIndex = ptssecondvtkPoints[(k+secondvtkPoints1)%nptssecondvtkPoints];
							StaticFunctionLibrary::ComputeAndInsertPointData(secondPoly->GetPointData(),
								secondPolyOut->GetPointData(), inputIndex, inputIndex, 0.0,
								count);

							//for (u = 0; u < NumberOfArrays; u++) 
							//{
							//scalarout1i = 
							//	firstPoly->GetPointData()->GetArray(u)->
							//	GetTuple(ptsfirstvtkPoints[(i+firstvtkPoints1+1)%nptsfirstvtkPoints])[0];
							//firstPolyOut->GetPointData()->GetArray(u)->InsertComponent(count, 0, scalarout1i);
							//scalarout2i = 
							//	secondPoly->GetPointData()->GetArray(u)->GetTuple(ptssecondvtkPoints[(k+secondvtkPoints1)%nptssecondvtkPoints])[0];
							//secondPolyOut->GetPointData()->GetArray(u)->InsertComponent(count, 0, scalarout2i);
							//}
						//}
						count++;
					} 
					lastjmin = jmin;
					
				}
				
				if (dist1) {
					delete [] dist1; dist1 = NULL;
				}
				if (dist2) {
					delete [] dist2; dist2 = NULL;
				}
		
			} //if(  (first2-first1+npts)%npts > 1 )
			else 
			{
				Pt_m_points1->InsertPoint(count, pp1->GetPoint(pts1[first2]) );
				//TRACE ("Inserted1 %i: (%f %f %f)\n", count, pp1->GetPoint(pts1[first2])[0],
				//	pp1->GetPoint(pts1[first2])[1],pp1->GetPoint(pts1[first2])[2]);
				//TRACE ("Inserted1 proof %i: (%f %f %f)\n", count, this->m_points1->GetPoint(count)[0],
				//	this->m_points1->GetPoint(count)[1],this->m_points1->GetPoint(count)[2]);
				//if (treatscalar) 
				//{
					StaticFunctionLibrary::ComputeAndInsertPointData(this->m_contour1->GetPointData(),
						this->m_points1->GetPointData(), pts1[first2], pts1[first2], 0.0,
						count);

					//for (i = 0; i < NumberOfArrays; i++)
					//{
					//	scalarout1i = 
					//		this->m_contour1->GetPointData()->GetArray(i)->GetTuple(pts1[first2])[0];
					//	this->m_points1->GetPointData()->GetArray(i)->InsertComponent(count, 0, scalarout1i);
					//}
				//}

				Pt_m_points2->InsertPoint(count, pp2->GetPoint(pts2[second2]) );
								
				//TRACE ("Inserted2 %i: (%f %f %f)\n", count2, pp2->GetPoint(pts2[second2])[0],
				//pp2->GetPoint(pts2[second2])[1],pp2->GetPoint(pts2[second2])[2]);
				//TRACE ("Inserted2 proof %i: (%f %f %f)\n", count2, this->m_points2->GetPoint(count2)[0],
				//this->m_points2->GetPoint(count2)[1],this->m_points2->GetPoint(count2)[2]);
				
				//if (treatscalar) 
				//{
					StaticFunctionLibrary::ComputeAndInsertPointData(this->m_contour2->GetPointData(),
						this->m_points2->GetPointData(), pts2[second2], pts2[second2], 0.0,
						count);

					//for (i = 0; i < NumberOfArrays; i++)
					//{
					//	scalarout2i = 
					//		this->m_contour2->GetPointData()->GetArray(i)->GetTuple(pts2[second2])[0];
					//	this->m_points2->GetPointData()->GetArray(i)->InsertComponent(count, 0, scalarout2i);
					//}
				//}
				count++;
			}
		}// if(first1 != -1)
	}// for (it1 = IntMap.begin(); it1 != IntMap.end(); it1++)

	VERIFY( this->m_points2->GetNumberOfPoints()== this->m_points1->GetNumberOfPoints()); 
	
	// for test
	//vtkCellArray* con1 = vtkCellArray::New();
	//vtkCellArray* con2 = vtkCellArray::New();
	//this->m_points1->SetLines(con1);
	//this->m_points2->SetLines(con2);

	//con1->InsertNextCell(this->m_points1->GetNumberOfPoints());
	//con2->InsertNextCell(this->m_points1->GetNumberOfPoints());

	//for (i = 0; i < this->m_points1->GetNumberOfPoints(); i++) {
	//	con1->InsertCellPoint(i);
	//	con2->InsertCellPoint(i);
	//}

	//con1->Delete();
	//con2->Delete();
	//--------------
	
	

	//TRACE ("%i %i %i\n", npts1, this->m_points1->GetNumberOfPoints(),
	//	this->m_points2->GetNumberOfPoints());
	return TRUE;
}

vtkPolyData* SymmetricCorrespondence::GetPoints1()
{
	return this->m_points1;
}

vtkPolyData* SymmetricCorrespondence::GetPoints2()
{
	return this->m_points2;
}

void SymmetricCorrespondence::AlignCentre(vtkPolyData *aligned2)
{
	double centroid1[3], centroid2[3];
	this->FindCentroid(this->m_contour1, centroid1);
	this->FindCentroid(this->m_contour2, centroid2);

	vtkTransform *aTransform = vtkTransform::New();
	aTransform->PostMultiply();
	aTransform->Translate(centroid1[0]-centroid2[0], centroid1[1]-centroid2[1], 
		centroid1[2]-centroid2[2]);
	aTransform->Update();

	/*TRACE("Translate: %f %f %f\n", target_centroid[0] - source_centroid[0],
														target_centroid[1] - source_centroid[1],
														target_centroid[2] - source_centroid[2]);
	*/
	
	vtkTransformPolyDataFilter *transFilter = vtkTransformPolyDataFilter::New();
    transFilter->SetInput(this->m_contour2);
	transFilter->SetTransform(aTransform);
	transFilter->Update();

	aligned2->SetPoints(transFilter->GetOutput()->GetPoints());

	aTransform->Delete();
	transFilter->Delete();
	
	return;
}

void SymmetricCorrespondence::FindCentroid(vtkPolyData *polydata, double centre[3])
{
	vtkPoints* tempPoints = polydata->GetPoints();
	double tempPoint[3];
	centre[0] = 0.0f; centre[1] = 0.0f; centre[2] = 0.0f;
	for (int i = 0; i < tempPoints->GetNumberOfPoints(); i++) {
		tempPoints->GetPoint(i, tempPoint);
		centre[0] += tempPoint[0];
		centre[1] += tempPoint[1];
		centre[2] += tempPoint[2];
	}

	centre[0] /= tempPoints->GetNumberOfPoints();
	centre[1] /= tempPoints->GetNumberOfPoints();
	centre[2] /= tempPoints->GetNumberOfPoints();

	return;
}

void SymmetricCorrespondence::Initialize()
{
	this->MatchFirstPoint = 0;
	this->m_contour1->Initialize();
	this->m_contour2->Initialize();
	this->m_points1->Initialize();
	this->m_points2->Initialize();
}

int SymmetricCorrespondence::GetNumberOfPoints()
{
	return this->m_points1->GetNumberOfPoints();
}

void SymmetricCorrespondence::Delete()
{
	this->Reset();		
	delete this;
}

SymmetricCorrespondence* SymmetricCorrespondence::New()
{
	return new SymmetricCorrespondence();
}

void SymmetricCorrespondence::Reset()
{
	if (this->m_contour1) {
		this->m_contour1->Delete();
		this->m_contour1 = NULL;
	}
	if (this->m_contour2) {
		this->m_contour2->Delete();
		this->m_contour2 = NULL;
	}
	if (this->m_points1) {
		this->m_points1->Delete();
		this->m_points1 = NULL;
	}
	if (this->m_points2) {
		this->m_points2->Delete();
		this->m_points2 = NULL;
	}
}

void SymmetricCorrespondence::DetermineOutwardNormal(vtkPolyData *InputContour, const int PointNo, double OutputNormal[3])
{
	vtkCellArray* line = InputContour->GetLines();
	int npts, *pts;
	// Assume line only has one cell.
	line->InitTraversal(); line->GetNextCell(npts, pts);
	
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
	
	/*float q[4];
	q[0] = 1/sqrt(2); //cos (pi/4);
	q[1] = (1/sqrt(2))*omega[0]; //sin(pi/4)*omega;
	q[2] = (1/sqrt(2))*omega[1];
	q[3] = (1/sqrt(2))*omega[2];

	float Q[3][3];
	SymmetricCorrespondence::QMatrix(q,Q);

	vtkMath::Multiply3x3(Q,tangent,OutputNormal);
*/
	vtkMath::Cross(tangent,omega,OutputNormal);
	vtkMath::Normalize(OutputNormal);

	float lambda = 0.1;
	double PointInQuestion[3] = {currentPoint[0] + lambda*OutputNormal[0],
		currentPoint[1] + lambda*OutputNormal[1], currentPoint[2] + lambda*OutputNormal[2]};

	//TRACE ("outwardNormal: %f %f %f \n", OutputNormal[0] + currentPoint[0], 
	//OutputNormal[1] + currentPoint[1], OutputNormal[2] + currentPoint[2]);

	if (StaticFunctionLibrary::IsInsideContour(InputContour, PointInQuestion)) {
	//	q[1] = -q[1]; //sin(pi/2)*omega;
	//	q[2] = -q[2];
	//	q[3] = -q[3];
		
	//	SymmetricCorrespondence::QMatrix(q,Q);
	//	vtkMath::Multiply3x3(Q,tangent,OutputNormal);
		vtkMath::Cross(omega,tangent,OutputNormal);
		vtkMath::Normalize(OutputNormal);
	}

	//TRACE ("currentPoint: %f %f %f \n", currentPoint[0], currentPoint[1], currentPoint[2]);
	//TRACE ("outwardNormal: %f %f %f \n", OutputNormal[0] + currentPoint[0], 
	//OutputNormal[1] + currentPoint[1], OutputNormal[2] + currentPoint[2]);
	//TRACE ("slicenormal: %f %f %f \n", omega[0] + currentPoint[0], 
	//omega[1] + currentPoint[1], omega[2] + currentPoint[2]);
	//TRACE("tangent %f %f %f \n", tangent[0] + currentPoint[0], 
	//tangent[1] + currentPoint[1], tangent[2] + currentPoint[2]);

}

/*void SymmetricCorrespondence::QMatrix(const float q[4], float Q[3][3])
{
	Q[0][0] = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
	Q[0][1] = 2*(q[1]*q[2] - q[0]*q[3]);
	Q[0][2] = 2*(q[1]*q[3] + q[0]*q[2]);
	Q[1][0] = 2*(q[2]*q[1] + q[0]*q[3]);
	Q[1][1] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
	Q[1][2] = 2*(q[2]*q[3] - q[0]*q[1]);
	Q[2][0] = 2*(q[1]*q[3] - q[0]*q[2]);
	Q[2][1] = 2*(q[2]*q[3] + q[0]*q[1]);
	Q[2][2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
	return;
}*/

float SymmetricCorrespondence::Arclength(vtkPolyData *pd, const int index1, const int index2)
{
	// length of contour from index1 to index2
	// note: the vertex needs to be retrieved by pts[indexX]
	vtkCellArray* tempCellArray = pd->GetLines();
	tempCellArray->InitTraversal();
	int npts, *pts;
	
	float alength = StaticFunctionLibrary::ArcLength(pd);
	
	tempCellArray->InitTraversal();
	tempCellArray->GetNextCell(npts, pts);// Get the first and the only cell

	if (pts[0] == pts[npts-1])
	{ 
		npts--;
	}

	int i = 0; 
	double firstPoint[3], secondPoint[3];
	float sum = 0;
	//count circularly
	for (i = 0; i < (index2-index1+npts)%npts; i++) 
	{
		pd->GetPoints()->GetPoint(pts[(i+index1)%npts], firstPoint);
		pd->GetPoints()->GetPoint(pts[(i+index1+1)%npts], secondPoint);

		sum += sqrt(vtkMath::Distance2BetweenPoints(firstPoint, secondPoint));
	}

	if (sum >= alength/2) 
	{
		sum -= alength;
	}

	return sum;
}

BOOL SymmetricCorrespondence::CheckOrientation()
{
	// Test whether the two input contours are of the same orientation.
	float C1_01[3] = {0, 0, 0};
	float C1_02[3] = {0, 0, 0};

	float C2_01[3] = {0, 0, 0};
	float C2_02[3] = {0, 0, 0};

	int *pts1, n1;
	int *pts2, n2;

	vtkCellArray* tempCellArray1 = this->m_contour1->GetLines();
	vtkCellArray* tempCellArray2 = this->m_contour2->GetLines();
	tempCellArray1->InitTraversal();
	tempCellArray2->InitTraversal();
	tempCellArray1->GetNextCell(n1, pts1);
	tempCellArray2->GetNextCell(n2, pts2);

	double C1point1[3], C1point2[3], C1point3[3]; 
	double C2point1[3], C2point2[3], C2point3[3];
	
	this->m_contour1->GetPoints()->GetPoint(0, C1point1);
	this->m_contour1->GetPoints()->GetPoint(n1/3, C1point2);
	this->m_contour1->GetPoints()->GetPoint(2*n1/3, C1point3);

	this->m_contour2->GetPoints()->GetPoint(0, C2point1);
	this->m_contour2->GetPoints()->GetPoint(n2/3, C2point2);
	this->m_contour2->GetPoints()->GetPoint(2*n2/3, C2point3);

	C1_01[0] = C1point2[0] - C1point1[0];
	C1_01[1] = C1point2[1] - C1point1[1];
	C1_01[2] = C1point2[2] - C1point1[2];

	C1_02[0] = C1point3[0] - C1point1[0];
	C1_02[1] = C1point3[1] - C1point1[1];
	C1_02[2] = C1point3[2] - C1point1[2];

	C2_01[0] = C2point2[0] - C2point1[0];
	C2_01[1] = C2point2[1] - C2point1[1];
	C2_01[2] = C2point2[2] - C2point1[2];

	C2_02[0] = C2point3[0] - C2point1[0];
	C2_02[1] = C2point3[1] - C2point1[1];
	C2_02[2] = C2point3[2] - C2point1[2];

	float cross1[3] = {0, 0, 0};
	float cross2[3] = {0, 0, 0};

	vtkMath::Cross(C1_01, C1_02, cross1);
	vtkMath::Cross(C2_01, C2_02, cross2);

	if (vtkMath::Dot(cross1, cross2) > 0) 
		return TRUE;
	else
		return FALSE;

}

//void SymmetricCorrespondence::SetMatchFirstPoint(int m)
//{
//	if (m < 0) m = 0;
//	else if (m > 1) m = 1;
//
//	this->MatchFirstPoint = m;
//}
//
//void SymmetricCorrespondence::MatchFirstPointOn()
//{
//	this->SetMatchFirstPoint(1);
//}
//
//void SymmetricCorrespondence::MatchFirstPointOff()
//{
//	this->SetMatchFirstPoint(0);
//}

void SymmetricCorrespondence::ProduceFinalIntMap(vtkstd::map<int, int> IntMap, vtkstd::map<int, int> IntMap2, vtkstd::map<int, int> & FinalIntMap, int npts2)
{
	// June 14 2011
	// IntMap is the symmetric correspondence established
	// first     1 2 3
	// second    4 7 6 <- Put into MinMax; MinMax rejects 7
	// i.e., need to erase the second item
	// IntMap2 (inverse map of IntMap plus more) is needed when erasing things
	// first  7
	// second 2
	// IntMap.erase(IntMap2[7]);
    // IntMap does not included manually specified anchor points.

	// Assumption: IntMap->first and AnchorPoints->first are sorted.

	int i;
	vtkstd::map<int, int>::iterator it, it2;

	FinalIntMap.clear();

	if (!this->AnchorPoints) 
	{
		// Case 1: If MatchFirstPoint = 0 && AnchorPoints.size() == 0
		// Don't need to specify MaxI and MinI, just do the usual thing.
		CMinMax<int> minmax;

		for (it = IntMap.begin(); it != IntMap.end(); it++)
		{
			minmax.AddValue(it->second);
			// just copy all from InputMap to FinalIntMap;
			FinalIntMap[it->first] = it->second;
		}

		minmax.Setnpts2(npts2);
		minmax.DetermineExceptionsInterface();

		// Copy all from IntMap to FinalIntMap and then erase
		for (minmax.ExceptionTraversalInitialize(); minmax.MoreException();)
		{
			int eraseIndex = minmax.NextException();
			FinalIntMap.erase(IntMap2[eraseIndex]);
		}

		// done;   
		return;
	}

	vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();

	// Before doing anything, fill up FinalIntMap appropriately.
	for (it2 = this->AnchorPoints->begin(); it2 != this->AnchorPoints->end(); it2++)
	{
		if (IntMap.count(it2->first) > 0)
		{
			idList->InsertUniqueId(it2->first);
		}
		for (it = IntMap.begin(); it != IntMap.end(); it++)
		{
			if (it2->second == it->second)
			{
				idList->InsertUniqueId(it->first);
				break;
			}
		}
	}

	for (i = 0; i < idList->GetNumberOfIds(); i++)
	{
		IntMap.erase(idList->GetId(i));
	}

	//test
	//TRACE("IntMap:\n");
	//for (it = IntMap.begin(); it != IntMap.end(); it++)
	//{
	//	TRACE("%d %d \n", it->first, it->second);
	//}


 //   if (this->MatchFirstPoint)
	//{
	//	FinalIntMap[0] = 0;
	//	if (IntMap.count(0) > 0)
	//	{
	//		IntMap.erase(0);
	//	}
	//}

	// Make sure there is no deplicate.

	// Merge IntMap and Anchor Points put results in FinalIntMap 
	it  = IntMap.begin();
	it2 = this->AnchorPoints->begin();
	// either throw in points in AnchorPoints or copy from IntMap
	CMinMax<int> MinMax;

	// June 17 2011
	// Appropriate only if the index of one of the anchor points (in the second curve) is small
	// e.g., If MatchedFirstPoints is asserted, Index 0 is matched to index 0. (second = 0)
	//MinMax.Set_setMinMaxIndex(1);

	int minI, minIndex;
	int Flag = 0;

	while(it != IntMap.end())
	{
		if (it->first < it2->first || it2 == this->AnchorPoints->end())
		{
			FinalIntMap[it->first] = it->second;
			it++;
		}
		else
		{
			MinMax.AddAnchorPointsIndex(FinalIntMap.size());
            if (!Flag || it2->second < minI)
			{
				minI = it2->second; // Whatever smallest from this->AnchorPoints
				minIndex = FinalIntMap.size(); // The index in FinalIntMap
				Flag = 1;
			}
			FinalIntMap[it2->first] = it2->second;
			it2++;
		}
	}

	//int n_Final = FinalIntMap.size();
	//int maxIndex = (minIndex - 1 + n_Final)%n_Final;
	//MinMax.Set_minI(minIndex);
	//MinMax.Set_maxI(maxIndex);

//	TRACE("FinalIntMap:\n");
	for (it = FinalIntMap.begin(); it != FinalIntMap.end(); it++)
	{
		//TRACE("%d %d \n", it->first, it->second);
		MinMax.AddValue(it->second);
	}

	MinMax.Setnpts2(npts2);
	MinMax.DetermineExceptionsInterface();
	for (MinMax.ExceptionTraversalInitialize(); MinMax.MoreException();)
	{
		int eraseIndex = MinMax.NextException();
		FinalIntMap.erase(IntMap2[eraseIndex]);
	}
}

int SymmetricCorrespondence::DetermineIndexWithMaxValue(vtkstd::map<int, int> IntMap)
{
	// only useful if this->MatchFirstPoint is true;
	// minmax does not allow user to set minI and maxI
	
	// strategy: find max local max (of course, there could be a number of local max)
	// if max local max is at the first half --> use end point;
	// if max local max is at the second half --> use max local max;
	// if max local max does not exist --> use end point;

	int i;
	int IndexWithMaxValue = 0;
	int maxlocalmax, maxlocalmaxIndex, maxlocalmaxExist = 0;

	// Insert IntMap into array
	int N = IntMap.size();
	int* minmax = new int [N];
	vtkstd::map<int, int>::iterator it;

	for (i = 0, it = IntMap.begin(); it != IntMap.end(); it++, i++)
	{
		minmax[i] = it->second;
	}

	for (i = 1; i < N - 2; i++)
	{
		if ((minmax[i] > minmax[i-1]) && (minmax[i] > minmax[i+1]))
		{
			maxlocalmaxExist = 1;
			maxlocalmax = minmax[i];
			maxlocalmaxIndex = i;
		}
	}

	int maxlocalmaxIndexGreaterThanHalf;
	
	if (maxlocalmaxExist)
	{
		maxlocalmaxIndexGreaterThanHalf= (maxlocalmaxIndex > N/2);
	}

	int last, secondlast;
	
	if ((!maxlocalmaxExist)||(maxlocalmaxExist && !maxlocalmaxIndexGreaterThanHalf))
	{//use end point
		for (i = N - 1; i > 0; i--)
		{
			// handle case where
			// index   0 ......44 45
			//         0 ..... 45 1
			
			IndexWithMaxValue = minmax[i];
			
			// e.g., 1 > 45? no thus continue;
			
			if ((last = minmax[i]) > (secondlast = minmax[i-1]))
			{
				break;
			}
		}
	}
	else
	{//use maxlocalmax
		IndexWithMaxValue = maxlocalmaxIndex;
	}

	delete [] minmax;
	return IndexWithMaxValue;
}

int SymmetricCorrespondence::DetermineIndexWithMinValue(vtkstd::map<int, int> IntMap)
{
	// just take the minimum value among it->second

	vtkstd::map<int, int>::iterator it;

	int MinValue;
	int EnteredLoop = 0;

	for (it = IntMap.begin(); it != IntMap.end(); it++)
	{
		if (!EnteredLoop)
		{
			MinValue = it->second;
			EnteredLoop = 1;
			continue;
		}
		if (it->second < MinValue)
		{
			MinValue = it->second;
		}
	}

	return MinValue;
}

void SymmetricCorrespondence::SetAnchorPoints(vtkstd::map<int, int>* AnchorPointsIn)
{
	this->AnchorPoints = AnchorPointsIn;
}
