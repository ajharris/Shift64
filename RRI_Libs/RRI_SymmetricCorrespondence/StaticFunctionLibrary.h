// StaticFunctionLibrary.h: Bernard's static function library
//
//////////////////////////////////////////////////////////////////////

#ifndef __StaticFunctionLibrary_h
#define __StaticFunctionLibrary_h

#include "vtkObject.h"

class vtkPolyLine;
class vtkIdList;
class vtkPoints;
class vtkPolyData;
class vtkImageData;
class vtkPointData;
class vtkFloatArray;
class vtkMatrix4x4;
class vtkPointSet;
class vtkPlane;
class vtkDoubleArray;
//class Engine;

class StaticFunctionLibrary : public vtkObject  
{
public:
	//Don't touch this:
	static StaticFunctionLibrary* New();
public:

	//static void PolygonsToLines(vtkPolyData *InputPoly, vtkPolyData *OutputPoly);
	//static void ResampleStackOfLines(vtkPolyData *input, vtkPolyData *output, float SamplingInterval);
	static int CurveWithLargestCircularityRatio(vtkPolyData** Curves, vtkIdList* idList);
	//static void GrabOnlyOneLine(vtkPolyData* in, vtkPolyData* out, int LineNum);
	static void ComputeNormalOfPlane(vtkPolyData* input, double NormalOfPlane[3], double& minZ, double& maxZ);
	//static void ReplaceExtension(CString inString, CString& outString, CString extension);
	//static void DetermineReferenceFrame2(float z[3], float Pt[3], vtkMatrix4x4* matrix);
	static void ComputeMeanCurve2(vtkPolyData** Inputs, int NumberOfPolyData, vtkPolyData* Output, int IndexFirstSlice);
	//static void InitialMapping(vtkFloatArray *mu0, vtkFloatArray *mu1, 
	//									   int mapSizeX, int mapSizeY, 
	//									   float hx, float hy, 
	//									   float A, float B, 
	//									   vtkFloatArray* ux, vtkFloatArray* uy,
	//									   int& uxMapSize, int& uyMapSize);
	//static void bootstrp(float* Data, int DataSize, float* Mean, float* Variance, int BootSamples);
	//static float Mean(float* DataSet, int Size);
	//static int BootstrapMeanTest2(float* Data1, int size1, float* Data2, int size2, float alpha, int BootSamples, float& significance, int& tobsHigherThanMedian);
	//static int RandomInteger(int n);
	static void DetermineBifurcation(vtkPolyData* input, double Bifurcation[3]);
	//static void RegisterPolyData(vtkPolyData* Source, vtkPolyData* Target, float BifurcationSource[3], float BifurcationTarget[3], vtkMatrix4x4* RegMatrix);
	//
	//// OrderPolyData uses SimpleSort
	//static void OrderPolyData(vtkPolyData *InputPoly, vtkPolyData *OutputPoly, float* referenceDir = NULL);
	static void SimpleSort(float *sequence, int *index, int num);

	//static int BootstrapMeanTest(float* Data1, int size1, float* Data2, int size2, float alpha, float& significance);
	//static int NormalityTest(float* Data, int NumberOfSamples);

	//static void DetermineFittingLine(vtkPoints *DataPoints, float ReferencePoint[3], float Normal[3]);
	//
	//// CopyArrays.. and Compute... are used together (see comments)
	//static void CopyArraysFromInputPointData(vtkPointData* inputPointData, vtkPointData* outputPointData);
	//static void ComputeAndInsertPointData(vtkPointData* inputPointData, vtkPointData *outputPointData, int PtId1, int PtId2, float t, int Index);
	//	
	//static void ChangePolyLineToLine(vtkPolyData* in, vtkPolyData* out);

	//static void FillFlattenedMap(vtkPolyData *ThicknessSlice, float z, int branchNumber, vtkPolyData* FlattenedMap);

	

	//static int FindAllIntersection(vtkPolyLine *pline, vtkPoints *outputP, vtkIdList* subIdArray, double startpoint[], double endpoint[], float tol);
	///*static void ComputeMeanCurve(vtkPolyData **Inputs, int NumberOfPolyData, 
	//	vtkPolyData *Output, int IndexFirstSlice, int AttemptToFindAllIntersections = 0);*/
	//static void StaticFunctionLibrary::FindLineFromCorrespondence(vtkPoints *Pts, float NormalAtContour1[3], 
	//	float StartPoint[3], float EndPoint[3], float normal1[3], float LengthFactor = 1.5f);
	static void Centroid(vtkPoints*, double c[3]);
	static void Centroid(vtkPolyData *inpd, double c[3], int LineNum);
	//static void Centroid(vtkPolyData*, double c[3]);
	static void DetermineOutwardNormal(vtkPolyData*, const int, double OutputNormal[3], int LineNumber = 0);
	///*static int SameOrientation(vtkPolyData *p1, vtkPolyData *p2);*/
	//static double ArcLength(vtkPolyData * p);
	//static void ResampleByAngle(vtkPolyData* input, vtkPolyData* output, double Centroid[3], int AngSample);
	///*static void ResampleByAngle2(vtkPolyData *input, vtkPolyData *output, float Centroid[], int AngSample);*/
	//static void ResampleByArcLength(vtkPolyData*, vtkPolyData*, int);
	static int CountNumberOfClosedCurve(vtkPolyData *pdin, vtkPolyData* pdout, int eliminateClosedCurveWithSmallArea = 0);
	//static float Variance(float* DataSet, int Size);
	//static void HelperTransformRotate(const float input[3], float output[3], float, float, float, int);
	//static void TransformRotate(vtkPolyData*, vtkPolyData*, float phi, float theta, float psi, int);
	//static void DetermineReferenceFrame(float z[3], float& phi, float& theta, float& psi);
	//static void FindNormal(vtkPolyData *OneSlice, float normal[3]);
	//static int CurveWithLargestArea(vtkPolyData** Curves, vtkIdList*);
	static int CustomMessageBox(const char*, UINT = MB_ICONEXCLAMATION|MB_OK);
	
	// Helper to ResliceProstateNIFTI to determine whether InputFilename ends with "_Contour"
	static int IsContour(const char* InputFilename);

protected:
	/*static float Computeb(float* f2, float* g2, int mapSizeY, float y, float delY, float B, float adiffx);
	static void Computef2g2adiffx(vtkFloatArray* mu0, vtkFloatArray* mu1, int mapSizeX, int mapSizeY, float delX, float delY, float x, float ux, float* f2, float* g2, float& adiffx);
	static void Computefg(vtkFloatArray* mu0, vtkFloatArray* mu1,  int mapSizeX, int mapSizeY, float A, float B, float* f, float* g);
	static float Computea(float* f, float* g, int mapSizeX, float x, float delX, float A);
	*/StaticFunctionLibrary() {};
	~StaticFunctionLibrary() {};

private:
  StaticFunctionLibrary(const StaticFunctionLibrary&);  // Not implemented.
  void operator=(const StaticFunctionLibrary&);  // Not implemented.


public:
	static int ChoosePointWithSmallest(int XYZ, vtkPointSet* pd);
	static void DeleteExtension(CString inputString, CString& outputString);
	static void ReplaceExtension(CString inString, CString &outString, CString extension);
	static int ChoosePointWithLargest(int XYZ, vtkPointSet* pd);
	static int CenterlineIntersectionWithPlane(vtkPolyData* Centerline, double  Normal[3], double  Pt[3], double  IntersectionPoint[3]);
	static int DetermineReferenceFrame(double  z[3], double  Pt[3], vtkMatrix4x4* matrix);
	static void FindPlaneCoordinates(double  ImageCubeBounds[6], vtkMatrix4x4* matrix, double  ResliceOrigin[3], double  ReslicePoint1[3], double  ReslicePoint2[3]);
	static double sinc(double x);
	static BOOL IsInsideContour(vtkPolyData* InputContour, double  PointInQuestion[2]);
	static void GrabOnlyOneLine(vtkPolyData *in, vtkPolyData *out, int LineNum);
	static void InitialImage(vtkImageData* imdata, double Origin[3], double Spacing[3], int Extent[6], double InitialValue = 1.0, int DataType = VTK_FLOAT);
    static void FindNormal(vtkPolyData *OneSlice, double normal[3]);
	static int FindNormal(vtkPolyData *inpd, double normal[3], int LineNum);
	static void PolygonsToLines(vtkPolyData *InputPoly, vtkPolyData *OutputPoly);
	static void WritePolyData(vtkPolyData *input, const char* filename);
	static void WriteXMLImageData(vtkImageData* input, const char* filename);
	static void PolygonAreaVector(vtkPoints *inPts, vtkIdType npts, vtkIdType *pts, double Area[3]);
	static double PolygonArea(vtkPoints* Points , vtkIdType npts, vtkIdType* pts);
	static void ComputeAreaVectorForOneLine(vtkPolyData* pd, int LineNum, double Area[3]);
	static void CopyArraysFromInputPointData(vtkPointData* inputPointData, vtkPointData* outputPointData);
	static int SameOrientation(vtkPolyData *p1, vtkPolyData *p2);
	static void ComputeAndInsertPointData(vtkPointData* inputPointData, vtkPointData *outputPointData, 
		int PtId1, int PtId2, double t, int Index);
	static void ComputeAndInsertPointData(vtkPointData *inputPointData, vtkPointData *outputPointData, 
		vtkIdList* PtIds, double* weight, int Index);
	static double ArcLength(vtkPolyData * p, int closed = 1);

	static void OrderPolyData(vtkPolyData *InputPoly, vtkPolyData *OutputPoly, double* referenceDir = NULL);
	static void SimpleSort(double *sequence, int *index, int num);
	static int FindAllIntersection(vtkPolyLine *pline, vtkPoints *outputP, vtkIdList* subIdArray, double startpoint[], double endpoint[], double tol);

	static void StaticFunctionLibrary::Centroid(vtkPolyData *pd, double c[3]);

	static void ResampleByArcLength(vtkPolyData *curvein, vtkPolyData *curveout, int NumberOfSamples, int closed = 1);
	static void ResampleStackOfLines(vtkPolyData* input, vtkPolyData* output, double SamplingInterval);
	
	//static void LeastSquare3DLineMATLAB(vtkPoints* X, double x0[3], double a[3]);
	static void LeastSquare3DLine(vtkPoints* X, double x0[3], double a[3]);
	static void SVD(double* A , int m , int n , double* S, double* U, double* VT);
	// This function is only appropriate for surfaces recon based on parallel contours
	static void PickOutFirstCCASlice(vtkPolyData* input, vtkPolyData* output);
	// Helper function of ComputeNormalOfPlane. The purpose is to eliminate contours with points whose IsArch = 1
	static int EliminateContoursWithPointsOnArch(vtkPolyData* input, vtkPolyData* output);
	static void ExtractLineFromPolyData(vtkPolyData* input, vtkPolyData* output, 
		vtkIdType StartLine, vtkIdType NumberOfSlicesToExtract, vtkIdType LineIncrement = 1);
	static void ComplexMultiply(double x[2], double y[2], double xy[2]);
	static void ResampleByArcLength(vtkPolyData* curvein, vtkPolyData* curveout, double resolution, int closed = 1);
	static void PreprocessCenterline(vtkPolyData* InputPolyData , vtkPolyData* OutputPolyData , double Interval);
	static void RegisterPolyData(vtkPolyData *Source, vtkPolyData *Target, double BifurcationSource[], double BifurcationTarget[], vtkMatrix4x4 *RegMatrix);
	static int StaticFunctionLibrary::ReadTo4x4Matrix(CString inputfilename, vtkMatrix4x4* matrix, int Read3rowsIsEnough = 0);
	
	static void WriteTo4x4Matrix(CString FileName, vtkMatrix4x4* matrix);
	static void WriteBifurcation(CString FileName, double Bifurcation[3]);
	static int ReadBifurcation(CString FileName, double Bifurcation[3]);
	// This function searches corresponding ICA, ECA and CCA points. A landmark transform is used to register source to target using these three points plus bifurcation.
	static void InitialRegisterPolyData(vtkPolyData* Source, vtkPolyData* Target, double BifurcationSource[3], double BifurcationTarget[3], vtkMatrix4x4* RegMatrix);
	// This function is the helper function of InitialRegisterPolyData.
	static int BranchProjectedToNormal(vtkPolyData* pd , int BranchNumber, double Bifurcation[3], double& ProjectedLength);
	static int LocateBranchPoint(vtkPolyData* pd , int BranchNumber, double length, double Bifurcation[3], double OutputPoint[3], double tol = 0.01);
	// helper for LocateBranchPoint
	static int CutOneSlice(vtkPolyData* inpd, vtkPlane* inplane, vtkPolyData* slice);
	static void AdjustSourceBifurcation(vtkPolyData* Source, double Bifurcation[3], double Length, double BifurcationAdjusted[3]);
	static int CurveWithLargestCircularityRatio(vtkPolyData** Curves, int NumOfCurves);
	static void ComputeMoments(double Array[], int num, double& Mean, double& StandardDeviation, double& Skewness, double& Kurtosis);
	static void randperm(int numElements, int* Output);
	static void EliminateOutlierFromArray(double InputArray[], int numInputs, double OutputArray[], int& numOutputs, vtkIdList* IncludedIndex, int Iterations = 100);
	static double SmoothingFunction(double WholeSeq[], int NumElementsInSeq, vtkIdList* Indexj);
	static double Variance(double Array[], int num, int Type);
	static void ArcLengthParameterizationForEachPoint(vtkPolyData* InputCurve, vtkDoubleArray* s);
	static void DecomposeIntoBranches(vtkPolyData *Input, vtkPolyData **Outputs);
	static vtkIdType FindContoursInSameSlice(vtkPolyData* ContourSet1, vtkPolyData* ContourSet2, int LineNoOnContourSet1, double Tol=0.001);
	static int IntersectionOfTwoPolyLines(vtkPolyData* pd1, vtkPolyData* pd2, vtkPoints* OutputP, vtkIdList* subIdArray);
	static int ttest(double d1mean, double se1, double df1, double d2mean, double se2, double df2, double& t, double& p, double alpha);
	static double RoundInteger(double x);
	static double betainc(double a, double b, double x);
	static double gammaln(double xx);
	static double betacf(double a, double b, double x);
	static int ttest(double* Samples1, double ElementsInSamples1, double* Samples2, double ElementsInSamples2, double& t, double& p, double alpha);
	static double pInttest(double TValue, double df);
	static double DegreeOfFreedom(double SDWall, int WallSampleSize , double SDLumen, int LumenSampleSize);
	static void CutterFunction(vtkPolyData* Input, double Point[3], double Normal[3], vtkPolyData* Output);
	static void ComputeMedian(double* sequence, int NumberOfElements, double& Median);
	static int ReadPointsAndBuild4x4Matrix(CString inputfile, vtkMatrix4x4* ResliceAxes);
	static int ReadTovtkPoints(CString Filename, vtkPoints* Points);
	static int Build4x4Matrix(vtkPoints* Points, vtkMatrix4x4* Matrix);
	static void RemoveExtension(CString inString, CString& outString);
	// Check whether InputSurface is closed
	//static void ConvertPointIdToIndex(int PointId, int ijk[3], int Increments[3]);
	//Use this instead: copy from a later version of vtk vtkStructuredData.
	static void ComputePointStructuredCoords (const vtkIdType cellId, int dim[3], int ijk[3]);
	//) 		[inline, static]

	static int IsClosedSurface(vtkPolyData* InputSurface);
};

#endif // !defined(AFX_STATICFUNCTIONLIBRARY_H__FD9D2CFD_2D2E_4A48_A1FC_32607D805288__INCLUDED_)
