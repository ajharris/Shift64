// SymmetricCorrespondence.h: interface for the SymmetricCorrespondence class.
//
// This class is used to obtain the correspondence points between two contours
// Given two contours, find correspondence points for every point of the first contour
//////////////////////////////////////////////////////////////////////

#ifndef __SymmetricCorrespondence_h
#define __SymmetricCorrespondence_h

class vtkPolyData;

#include <vtkstd/map>

class SymmetricCorrespondence  
{
public:
	
	static float Arclength(vtkPolyData* pd, const int index1, const int index2);
	//static void QMatrix(const float q[4], float Q[3][3]);
	static void DetermineOutwardNormal(vtkPolyData* InputContour, const int PointNo, double OutputNormal[3]);
	static SymmetricCorrespondence* New();
	virtual void Delete();
	virtual int GetNumberOfPoints();
	virtual void Initialize();
	void FindCentroid(vtkPolyData* polydata, double centre[3]);
	void AlignCentre(vtkPolyData* aligned2);
	vtkPolyData* GetPoints2();
	vtkPolyData* GetPoints1();
	BOOL FindCorrespondence();
	vtkPolyData* GetContour2();
	vtkPolyData* GetContour1();
	void SetContour2(vtkPolyData* contour2);
	void SetContour1(vtkPolyData* contour1);
	SymmetricCorrespondence();
	SymmetricCorrespondence(vtkPolyData* contour1, vtkPolyData* contour2);
	virtual ~SymmetricCorrespondence();

protected:

	// in some application, we desire to match the first point in cell
	int MatchFirstPoint;

	BOOL CheckOrientation();
	virtual void Reset();
	vtkPolyData* m_contour1;
	vtkPolyData* m_contour2;
	vtkPolyData* m_points1;
	vtkPolyData* m_points2;

	vtkstd::map<int, int>* AnchorPoints;
	
	void ProduceFinalIntMap(vtkstd::map<int, int> IntMap, vtkstd::map<int, int> IntMap2, vtkstd::map<int, int> & FinalIntMap, int npts2);
	int DetermineIndexWithMaxValue(vtkstd::map<int, int> IntMap);
	int DetermineIndexWithMinValue(vtkstd::map<int, int> IntMap);
public:
	void SetAnchorPoints(vtkstd::map<int, int>* AnchorPointsIn);
};

#endif // !defined(AFX_SYMMETRICCORRESPONDENCE_H__49FA5B4A_1186_47CD_875F_F3BBC04D9E2B__INCLUDED_)
