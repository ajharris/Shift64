#ifndef RRI_SLICER_H_INCLUDED
#define RRI_SLICER_H_INCLUDED

#pragma once

#include "Visualizer.h"
#include "RRI_Graphics.h"
#include "Measure.h"

#include "RRIObserver.h"
#include "RRIObservable.h"
#include "RRIEvent.h"
#define VIDEO_AVAILABLE
#ifdef VIDEO_AVAILABLE
#define FRAME_COMPLETE              31001
#define FRAME_TIMEOUT               31002
#define FG_MOUSE_UP                 31003
#endif

#define VTK_AVAILABLE
#ifdef VTK_AVAILABLE
#include <vtkSmartPointer.h>
#include "vtkStructuredPoints.h"
#include "vtkImageImport.h"
#include "vtkMetaImageWriter.h"
#include "vtkMetaImageReader.h"
#include "vtkBMPReader.h"
#include "vtkUnsignedShortArray.h"
#include "vtkDICOMImageReader.h"
#include "vtkMath.h"
#include "vtkImageData.h"
#include "vtkImageShiftScale.h"
#include "vtkSphereSource.h"
#include "vtkCellArray.h"
#include "vtkMedicalImageReader2.h"
#include "vtkMedicalImageProperties.h"
#include "vtkGDCMImageReader.h"
#include "vtkThinPlateSplineTransform.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkGeneralTransform.h"
#include "vtkCellLocator.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkPlaneSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkDataArray.h"
#include "vtkLookupTable.h"
#include "vtkPolyDataWriter.h"
#include "vtkMassProperties.h"
#include "vtkPolyDataReader.h"
#include "vtkSelectEnclosedPoints.h"
#include "vtkLandmarkTransform.h"
#include "vtkShrinkPolyData.h"
#include "vtkSurfaceReconstructionFilter.h"
#include "vtkContourFilter.h"
#include "vtkReverseSense.h"
#include "vtkSTLReader.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyDataMapper.h"
#include "vtkMetaImageReader.h"
#include "vtkSmartPointer.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkSTLWriter.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"
#include "vtkFeatureEdges.h"
#include "vtkImageFlip.h"
#include "vtkDecimatePro.h"
#include "vtkVRMLExporter.h"
#endif

#include "gdcmImageWriter.h"
#include "gdcmFile.h"
#include "gdcmAttribute.h"
#include <gdcmStringFilter.h>
#include "gdcmUIDGenerator.h"
#include "gdcmAnonymizer.h"

#define SURFACE_OBJECT
#ifdef SURFACE_OBJECT
//struct for surface object with colour information
struct SurfaceObject
{
    TriangleVec triangles;
    Vector3Vec userPoints;
    std::string name;
    COLORREF colour;//default green
    long objectType;//0:freeform, 1:parallel, 3: radial, 4: needle
    float opacity;
	bool visible;
    float measurement;

};


typedef STL::vector<SurfaceObject> SurfaceList;//used to contain a list of SurfaceObjects
typedef STL::vector<float> FloatArray;
typedef STL::vector<float*> PhaseImageArray;
typedef STL::vector<unsigned char> CharArray;

#endif



// Action state/mode
const enum {    ST_ALREADY_SET
               ,ST_ROTATE
               ,ST_SLICE
               ,ST_SLICE_ROTATE
               ,ST_DOING_NOTHING
               ,ST_PAN
               ,ST_MEASURE
               ,ST_DEFINE_POINT
               ,ST_ZOOMING
               ,ST_DELETE}; 

//current mode of slicer
const enum {MODE_SLICER=1, MODE_ADD_POINT, MODE_DELETE_POINT, MODE_MOVE_POINT, MODE_MEASURE};

public class RRI_SlicerInterface
{
public:
	RRI_SlicerInterface();
	virtual ~RRI_SlicerInterface(void);

#ifdef VIDEO_AVAILABLE
	virtual bool Update( RRIObservable *, const RRIEvent &m);
#endif

//scanning functions
	long GetCurrentScanIndex(){ return m_currentScanningIndex; };
	void SetScanFrameIndex(long index){m_currentScanningIndex = index;}//this will set by the calling program that is running the scan protocol
	long InsertFrame(unsigned char* frame, long index, long width, long height, long channels);

	void StartScan();
	void StopScan();
	long m_currentScanningIndex;
	bool m_isScanning;
	long m_previousFrame;

//Cube functionality
	long Initialize(long width, long height, std::string label, bool fullInit = true);

	std::string GetSlicerLabel(){return m_slicerLabel;}
	std::string m_slicerLabel;

	long ImportVolume(const char* filePath, long stringLength);
	long ExportRaw(std::string filePath);
	long ImportRaw(std::string filePath);
    long ImportMHA(std::string filePath);
    long ExportMHA(std::string filePath);

    //-----------------------------------------------------------------------------------------------------------------------------
    //long ImportBMP(std::string filePath, float pixelX, float pixelY);
    void CreateVolumeFromBitmap(unsigned char* bitmapBuffer, long width, long height, long depth, float voxelX, float voxelY, float voxelZ, long windowMin, long windowMax, long channels);
    unsigned char* m_bitmapBuffer;
    unsigned char* m_bitmapBuffer_24;
    unsigned char* m_bitmapBuffer_32;
    long m_bytesPerPixel;
    long m_bitmapWidth;
    long m_bitmapHeight;
    unsigned char* GetBitmap8(){return m_bitmapBuffer;}
    //--------------------------------------------------------------------------------------------------------------------------------


    long ExportBinary(std::string filePath);
	
	bool readNFOfile(std::string filePath, long* width, long* height, long* count, float* xvoxel, float* yvoxel, float* zvoxel, 
		                                   float* linearExtent, float* angularExtent, long* scanGeometry, long* probeOrientation, long* probeReflection, long* scanDirection, float* axisOfRotation);
	long UpdateDisplay();
	unsigned char* GetBitmapBits();
	unsigned char* GetOverlayBits();

    //NOTE: binary buffer is used for texture based segmentation
	bool m_use3DBrush;
	bool Is3DBrushSelected(){ return m_use3DBrush; }
	void Set3DBrushSelected(bool selected){ m_use3DBrush = selected; }
    unsigned char* m_binaryBuffer;
    unsigned char* GetBinaryBuffer(){ return m_binaryBuffer; }
    void SetBinaryBuffer(unsigned char* buffer){m_binaryBuffer = buffer;}//must be the same size as the current buffer...DO NOT DELETE
	void PaintIntoBinaryCube(long x, long y, bool paint);
    long m_currentPaintMode;//1:X,Y 2:X,Z  3: Y,Z
    void SetCurrentPaintMode(long paintMode){ m_currentPaintMode = paintMode; }


	unsigned char* m_reconstructedBuffer;
	unsigned char* GetReconstructedBuffer(){return m_reconstructedBuffer;}


	void SetAlphaBlend(long blendOpacity);
	long GetAlphaBlend(){return m_blendOpacity;}
	void ClearAlphaBlend();
	void DoAlphaBlend();
	bool m_alphaBlendEnabled;
	long m_blendOpacity;

	long GetWidth();
	long GetHeight();

//strings associated with slicer volume

    //used when exporting surfaces to uniquely label them according to the volume label
	std::string GetVolumeLabel(){return m_volumeLabel;}
	void SetVolumeLabel(std::string label){m_volumeLabel = label;}

    //used to identify the folder containing the volume file
	std::string GetVolumeFolderPath(){return m_volumeFolderPath;}
	void SetVolumeFolderPath(std::string folderPath){m_volumeFolderPath = folderPath;}

    //this is the complete file path of the volume
	std::string GetVolumeFilePath(){return m_volumeFilePath;}
	void SetVolumeFilePath(std::string filePath){m_volumeFilePath = filePath;}

    //this is used to label which file this volume has been registered to
    std::string GetRegistrationLabel(){ return m_registrationLabel; }
    void SetRegistrationLabel(std::string regLabel){m_registrationLabel = regLabel;}
    

    //this functions sets the the volume label, folder path, and file path from the file path of the volume
    long UpdateVolumeInfo(std::string filepath);

//Get and Set cube state
	void SetState(long state);
	long GetCurrentState(){return m_currentState;}

	long SetActiveFace(long x, long y);//screen coordinates
	bool InsideCube(long x, long y);//x and y in screen coordinates
	bool InsideCube(float x, float y, float z);//x, y and z in volume coordinates
	void CenterCube();//put cube in the center of the viewing window

	//slicing functionality
	void StartSlicing(long nFlags, long x, long y);
    void StopSlicing(long nFlags, long x, long y);
    void ContinueSlicing(long nFlags, long x, long y);
	void SetSlicing(bool slicing);
    bool IsSlicing();
	void Slice(long face, float distance){GetVisualizer()->Slice(face, distance);}
    void EnableSlicing(){ m_slicingEnabled = true; }
    void DisableSlicing(){ m_slicingEnabled = false; }
    bool IsSlicingEnabled(){ return m_slicingEnabled; }
    bool m_slicingEnabled;

#ifdef VTK_AVAILABLE
	//surface shifting and rotating
	bool IsShiftingSurfaces(){return m_isShiftingSurfaces;}
	bool IsRotatingSurfaces(){return m_isRotatingSurfaces;}
	void ShiftSurfacesEnable(std::string surfaceContains, bool set);//{m_isShiftingSurfaces = set;}
	std::string m_surfaceContains;
	void RotateSurfacesEnable(long mode, std::string surfaceContains, bool set, Vector3 center = Vector3(0,0,0));//(long mode, std::string surfaceContains, bool set){m_rotatingSurfacesMode = mode; m_surfaceContains = surfaceContains; m_isRotatingSurfaces = set;}
	void ShiftSurfaces(float xshift, float yshift, float zshift);
	void RotateSurfaces(long mode, float rotation);
	void ShowAllSurfaces();
	void HideAllSurfaces();
    void GenerateSurfaceMarchingCubes(vtkImageData* Volume, SurfaceObject* so);
    void GenerateSurfaceMarchingCubes(vtkImageData* Volume, SurfaceObject* so, vtkPolyData* polyData);

    void AddSegmentedNeedle(SurfaceObject* so, Vector3 tip, Vector3 entry);
    bool m_transformingNeedles;
#endif

	//selecting view functionality
	bool IsSelected(){return m_isSelected;}
	void SetSelected(bool selected){m_isSelected = selected;}

//adding, deleting, moving points
	void AddTarget(Vector3 point);//{m_userTargets.push_back(point);AddTargetSurface(point, 1.0f);}
	long AddTargetSurface(Vector3 target, float radius, long red=255, long green=0, long blue=0);
    float m_targetSurfaceRadius;
    void SetTargetSurfaceRadius(float radius){ m_targetSurfaceRadius = radius; }
	void ClearTargetSurfaces();
	void ClearTargets();
	//ADD
	void StartAddingPoints(){m_currentMode = MODE_ADD_POINT;}
	void StopAddingPoints(){m_currentMode = MODE_SLICER;}
	void AddPoint(long x, long y);
	//DELETE
	void StartDeletingPoints(){m_currentMode = MODE_DELETE_POINT;}
	void StopDeletingPoints(){m_currentMode = MODE_SLICER;}
	void DeletePoint(long x, long y);
	//MOVE
	void StartMovingPoint(){m_currentMode = MODE_MOVE_POINT;}
	void StopMovingPoint(){m_currentMode = MODE_SLICER;}
	void MovePoint(long x, long y);

	//FIND closest point
	long FindClosestPoint(Vector3Vec* points, long x, long y);//return index of closest point

	Vector3Vec GetUserTargets(){return m_userTargets;}

	void ShowWireframe(bool show){ m_showWireframe = show; }


	float HelperFindDistanceFromPointToLine(Vector3 startLine, Vector3 endLine, Vector3 point);
	

#ifdef VTK_AVAILABLE
	//Transform point through DICOM transform
	Vector3 TransformPointDICOM(Vector3 point){return helperTransformPointDICOM(point);}
	Vector3 TransformPointDICOMInverse(Vector3 point){return helperTransformPointDICOMInverse(point);}
	Vector3 HelperTransformPointLinear(vtkMatrix4x4* matrix, Vector3 point, Vector3 centroid = Vector3(0,0,0)){return helperTransformPointLinear(matrix, point, centroid);}
    Vector3 HelperTransformPointLinear(vtkTransform* trans, Vector3 point, Vector3 centroid = Vector3(0,0,0)){return helperTransformPointLinear(trans, point, centroid);}
	Vector3 HelperExtendLine(Vector3 ptBegin, Vector3 ptEnd, float factor){return extend_line(ptBegin, ptEnd, factor);}
	Vector3 HelperFindCentroid(TriangleVec* triangles){return helperFindCentroid(triangles);}
	void HelperConvertPolyDataToTriangles(vtkSmartPointer<vtkPolyData> polyData, TriangleVec* triangles, bool clear=true){helperConvertPolyDataToTriangles(polyData, triangles, clear);}
    void HelperConvertPolyDataToTriangles(vtkPolyData* polyData, TriangleVec* triangles, bool clear=true){helperConvertPolyDataToTriangles(polyData, triangles, clear);}
	void HelperConvertTrianglesToPolyData(TriangleVec* triangles, vtkPolyData* polyData, bool useDirectionCosines = true){helperConvertTrianglesToPolyData(triangles, polyData, useDirectionCosines);}

#endif


	//measurements-----------------------------------------------
	struct
	{
		unsigned Mode:8, polyMode:4;
		bool volDone, New, Done;
		unsigned Count:16; // keeps track of the number of measurements
	} measure;

	void AddMeasurePoint(long x, long y);
	void StartMeasurement(long type);
	bool DoMeasurements(long x, long y, bool end=false);
	void StoreMeasurements();
	void StopMeasurement();
	long m_measureFace;

    //---------------------------------------------------------------

	//Panning 
	void DoPan(long x, long y);
	void SetPanning(bool panning);
	bool IsPanning();

    //cursor functionality
	long m_currentCursorX;
	long m_currentCursorY;
    void SetCurrentCursor(long x, long y){m_currentCursorX = x; m_currentCursorY = y;}
	
	long m_extraCursorX;
	long m_extraCursorY;
    void SetExtraCursor(long x, long y){m_extraCursorX = x; m_extraCursorY = y;}

	//Zooming
	void Zoom(double zoomFactor){GetVisualizer()->Zoom(zoomFactor);}

	//mode and state functions
	void SetMode(long mode){m_currentMode = mode;}
	void UpdateState(long flags, long x, long y);

    //Painting funcitonality
    void SetBrushSize(double brushSize);
    void StartPainting();
    void StopPainting();
    bool IsPainting(){return m_isPainting;}
	bool IsErasing(){ return m_isErasing; }
    bool IsEditing(){ return m_isEditing; }
    void StartErasing();
    void StopErasing();
    void StartEditing();
    void StopEditing();

    double m_paintBrushSize;
    long   m_paintBrushRadiusPixels;
    void SetPaintingInside(bool value){ m_paintingInside = value; }
    bool m_paintingInside;
    

	//mouse functions
	void MouseDownEvent(UINT nFlags, long x, long y);
	void MouseUpEvent(UINT nFlags, long x, long y);
	void MouseMoveEvent(UINT nFlags, long x, long y);

	//helper functions
	bool ReplaceString(std::string& str, const std::string& from, const std::string& to);

     //buffer functions
    unsigned char* GetBuffer(){return GetVisualizer()->GetVolume()->GetData();}
	void ClearBuffer(unsigned char value);

	//volume functions
	long GetVolumeWidth(){return GetVisualizer()->GetVolume()->GetVolumeWidth();}
	long GetVolumeHeight(){return GetVisualizer()->GetVolume()->GetVolumeHeight();}
	long GetVolumeDepth(){return GetVisualizer()->GetVolume()->GetVolumeDepth();}

	double GetVoxelX(){return GetVisualizer()->GetVolume()->GetVoxelX();}
	double GetVoxelY(){return GetVisualizer()->GetVolume()->GetVoxelY();}
	double GetVoxelZ(){return GetVisualizer()->GetVolume()->GetVoxelZ();}

	//get parameters from visualizer
	eScanGeometry GetScanGeometry(){return GetVisualizer()->GetVolume()->GetScanGeometry();}
	eProbeOrientation GetProbeOrientation(){ return GetVisualizer()->GetVolume()->GetProbeOrientation(); }
	float GetSweepAngle(){ return GetVisualizer()->GetVolume()->m_Acq.sweepAngle; }
	float GetCenterOfRotation(){ return GetVisualizer()->GetVolume()->m_Cal.axisOfRotation; }
	eHorizontalImageReflection GetReflection(){ return GetVisualizer()->GetVolume()->GetReflection(); }
	eScanDirection GetScanDirection(){ return GetVisualizer()->GetVolume()->GetScanDirection(); }

    long GetNumberOfChannels();

	Vector3 GetVolumeToScreen(Vector3 point);
    Vector3 ModelToScreen(Vector3 point){return GetVolumeToScreen(point);}
    Vector3 ScreenToModel(long x, long y);

	//to be used for controlling a second cube using three points and AlignFaceToScreen function.
	Vector3 m_p1;
	Vector3 m_p2;
	Vector3 m_p3;
	Vector3 GetP1(){return m_p1;}
	Vector3 GetP2(){return m_p2;}
	Vector3 GetP3(){return m_p3;}
	void UpdateLinkingPoints();
	Vector3 m_currentPosition;
	Vector3 GetCurrentPosition(){return m_currentPosition;}


	//visualizer functions
	Visualizer* GetVisualizer(){return m_vis;}
	

	//set window level (for 8-bit data only)
	void SetWindowLevel(long window, long level);
	//for 16-bit data only
	void SetWindowMinMax(long windowMin, long windowMax);
	long GetWindowMin(){return m_windowMin;}
	long GetWindowMax(){return m_windowMax;}
	long GetWindow(){return m_window;}
	long GetLevel(){return m_level;}

	
    //MRPing functions-------------------------------------------------------------------------------------------
	//Functions for finding robot fiducials
	Vector3 FindRobotFiducial(Vector3 userPoint);
	void intensityWeightedCentroid(int **ptrin, int sizex, int sizey, bool autothresh, int manualthreshlevel,
                               double pixsizex, double pixsizey, double *xbar, double *ybar, int **ptrout);

    long GetPhaseEncodedDirection(){return m_phaseEncodedDirection;}
	long m_phaseEncodedDirection;//0:unknown, 1:ROW, 2:COL
    //------------------------------------------------------------------------------------------------------------

	//display functions
	void SetDisplayText(char* text, long textLength);
	std::string GetDisplayText();
	char* m_displayText;
	long m_displayTextSize;

	void SetWindowLevelText(char* text, long textLength);
	char* m_windowLevelText;
	long m_windowLevelTextSize;

	void SetCurrentPositionText(char* text, long textLength);
	char* m_currentPositionText;
	long m_currentPositionTextSize;
	
	long m_window;
	long m_level;
	long m_windowMin;
	long m_windowMax;

    //more volume functions
	void CleanUpForNewVolume();
	void ResetCube();
	void ResetCube(long mode);

    long CreateVolume(AcqParam acq, CalParam cal, long channels, tVoxel* replacementBuffer = 0);

#ifdef VTK_AVAILABLE
    //import DICOM functions
	long ImportDicom(const char* folderPath);
	long ImportDicomSingle(std::string folderPath);
    long ImportThermometry(const char* folderPath, long baseLineIndex, Vector3 thermalRegion, float radius);
    void UnwrapPhase(int StartScan);
    void UnwrapPhase1D(FloatArray* phaseArray);
    float UnwrapPhase(float pixelA, float pixelB);
    long UnwrapAndAddPhaseImage(float* phaseImage, long width, long height, long depth);
    void ClearPhaseImages();
    PhaseImageArray m_phaseImages; 

    long ScaleImage(vtkImageData* image);//called after importing DICOM image to scale window and level

    //export DICOM functions
    long ExportDicom(std::string folderPath, std::string fileLabel, std::string studyID, std::string patientID, long seriesNumber);
     void SetStringValueFromTag(const char *s, const gdcm::Tag& t, gdcm::Anonymizer & ano);//helper function for ExportDicom
    
    

    //create volume helper functions
	void CreateVolumeFromVtkImageData(vtkImageData *Img, long window, long level);//8-bit or 16-bit, channels extracted from Img data.
    void CreateVolumeForThermometry(long width, long height, long imageCount, double xSpacing, double ySpacing, double zSpacing, long channels);


    


    long RRI_SlicerInterface::HelperLoadBitmap(std::string filePath);

    void RRI_SlicerInterface::ImportColoursFromFile(std::string filePath);
    
    CharArray redArray;
    CharArray greenArray;
    CharArray blueArray;

	//manual or automatic registration matrix will be used to override the dicom matrix for linking volumes
	void GetRegMatrix(vtkMatrix4x4* matrix);//used to override the dicom matrix
	void SetRegMatrix(vtkMatrix4x4* matrix);
	vtkMatrix4x4* m_regMatrix;
	bool m_isRegMatrixSet;
	bool IsManuallyRegistered(){return m_isRegMatrixSet;}
    void SetManualRegistrationTrue(){ m_isRegMatrixSet = true; };//if user manually registers, clear dicom matrix flag, but not the dicom matrix.
    void SetManualRegistrationFalse(){ m_isRegMatrixSet = false; };

	//DICOM matrix is derived from the direction cosines and offset read from the dicom file
    //transform user points using DICOM direction cosines
	void TransformUserPointsDICOM();
	void GetDicomMatrix(vtkMatrix4x4* matrix);
	void SetDicomMatrix(vtkMatrix4x4* matrix);
	vtkMatrix4x4* m_dicomMatrix;
    bool IsDicomMatrixSet(){return m_isDicomMatrixSet;}
    bool m_isDicomMatrixSet;
    void SetDicomMatrixTrue(){ m_isDicomMatrixSet = true;};
    void SetDicomMatrixFalse(){ m_isDicomMatrixSet = false; };


	Vector3 GetDicomOffset(){return m_dicomOffset;}
	void SetDicomOffset(Vector3 offset){m_dicomOffset = offset;}

    //dicom offset and origin used for generating 4x4 matrix
	Vector3 m_dicomOffset;
    Vector3 m_dicomOrigin;
	Vector3 m_dicomX, m_dicomY, m_dicomZ;

#endif
	
	
   
    //set state model view of visualizer from scratch
	void SetModelView(long face, float rotX, float rotY, float rotZ, bool resetZoom = true)
    {GetVisualizer()->SetModelView(GetWidth(), GetHeight(), face, rotX, rotY, rotZ, resetZoom);}

	//convert current scan geometry to linear
	void ConvertToLinear();//reconstruct current volume to linear format

    //reduce volume by specified factor
    void ReduceVolume(long factorX, long factorY, long factorZ);
    void ReduceVolume(unsigned char* buffer, long factorX, long factorY, long factorZ);

    void ReduceImage(unsigned char* source, unsigned char* destination, long width, long height, long reductionFactor);
    //extra functions
	long ExportMatrixToFile(std::string filePath, vtkMatrix4x4* matrix);
    long ImportMatrixFromFile(std::string filePath, vtkMatrix4x4* matrix);
	long ImportMetaData(std::string folderPath, std::string filePath);//matrix and window-level


#ifdef SURFACE_OBJECT
	//surface methods
	SurfaceList* GetSurfaceList(){return &m_surfaceList;}
	SurfaceList* GetTemplateList(){return &m_templateList;}
	Vector3 FindNearestTemplatePositionYZ(Vector3 point);

	void ClearSurfaces(){m_surfaceList.clear();}
	long ExportSurfaces(std::string folderPath, bool userDicomTransform);
	long ExportSurface(std::string folderPath, std::string contains);
	long ImportSurfaces(std::string folderPath, std::string filePath);
	long ExportVOI(std::string filePath, TriangleVec* triangles);
	long ImportSurface(std::string filePath, std::string surfaceName);//filepath of .vtk file

#ifdef VTK_AVAILABLE
	long GenerateSurfaceFromUserPoints();
	long GeneratePlaneSurface();
	long StoreUserPointsNoSurface();
	
#endif

    long ExportUserPoints(std::string filePath);

	void AddTemplateObject(std::string text, long type, long red, long green, long blue);
	void AddSurfaceObject(std::string text, long type, long red, long green, long blue);
	void DeleteSurfaceObject(long index);
	void ShowSurfaceObject(long index, bool show);
	void SetCurrentSurfaceObject(long index);

	long m_currentSurfaceObject;
	//----------------------------------------
	long AddPlanes();
	std::string GetCodeForPoint(Vector3 point);
	long GenerateCodesForUserPoints();
	void BuildCellLocatorsForPlanes();
	//----------------------------------------

    //helper functions
	float CalculateVolume(TriangleVec *triangles);
	Vector3 CalculateCentroid(long selectedObject);//given index to surface object
	Vector3 FindCentroid(TriangleVec* triangles);//given triangles

	//test code
	void Test();
	long ColourizePolydata(vtkPolyData* polyData, double radius);//LG TEST

    void StartPerformanceCounter();
    double EndPerformanceCounterGetDiff();
    LARGE_INTEGER freq;
    public: LARGE_INTEGER counterStart;
    public: LARGE_INTEGER counterEnd;
    public:void Test2();

#endif

	Visualizer* m_vis;
	RRI_Graphics* m_graphics;
	RRI_Graphics* m_alphaGraphics;

    //for displaying surface contour onto live video
    PlotVec* GetCurrentPlotVector(){return &m_currentPlotVector;}
    PlotVec m_currentPlotVector;


private:


	//private helper functions
#ifdef VTK_AVAILABLE
	long helperConvertPolyDataToTriangles(vtkSmartPointer<vtkPolyData> polyData, TriangleVec* triangles, bool clear=true);

    long helperConvertPolyDataToTriangles(vtkPolyData* polyData, TriangleVec* triangles, bool clear=true);

	long helperConvertTrianglesToPolyData(TriangleVec* triangles, vtkPolyData* polyData, bool useDirectionCosines);
	
	Vector3 helperTransformPointDICOM(Vector3 point);
	Vector3 helperTransformPointDICOMInverse(Vector3 point);

	Vector3 helperFindCentroid(TriangleVec* triangles);
	Vector3 helperTransformPointLinear(vtkMatrix4x4* matrix, Vector3 point, Vector3 centroid = Vector3(0,0,0));
    Vector3 helperTransformPointLinear(vtkTransform* trans, Vector3 point, Vector3 centroid = Vector3(0,0,0));

public:
	float helperDistanceBetweenTwoPoints(Vector3 vec1, Vector3 vec2);

	Vector3 extend_line(Vector3 ptBegin, Vector3 ptEnd, float factor);
private:

	//plane locators for dividing the prostate
	vtkCellLocator* MID_BASE_locator;
	vtkCellLocator* MID_APEX_locator;
	vtkCellLocator* CENTER_HORIZONTAL_locator;
	vtkCellLocator* CENTER_VERTICAL_locator;
	vtkCellLocator* BASE_LEFT_locator;
	vtkCellLocator* BASE_RIGHT_locator;
	vtkCellLocator* MID_LEFT_locator;
	vtkCellLocator* MID_RIGHT_locator;
	vtkCellLocator* APEX_RIGHT_locator;
	vtkCellLocator* APEX_LEFT_locator;

#endif

    //initialize device context
	long initHDC(long width, long height);

	long m_width, m_height;
	long m_activeFace;

	bool m_slicing;//are we slicing cube display
    bool m_panning;//are we panning cube display
	bool m_isShiftingSurfaces;
	bool m_isRotatingSurfaces;
	long m_rotatingSurfacesMode;//1:rotate about X, 2: rotate about Y, 3: rotate about z
	Vector3 m_centerOfRotation;
	bool m_isRendering;
    bool m_isPainting;//used for new segmentation functionality
    bool m_isErasing;//used to erase paint brush marks
    bool m_isEditing;//used to edit binary image
    bool m_isDroppingPoints;

	long  m_currentMode;//{MODE_SLICER=1, ...};
    long  m_currentState;//{ST_ALREADY_SET, ST_ROTATE, ST_SLICE, ST_SLICE_ROTATE, ST_DOING_NOTHING, ST_PAN, ST_MEASURE}

    //show and hide bool variables
	bool m_showWireframe;//do we want to show the cube wireframe
	bool m_showUserPoints;
	bool m_showSurfaces;
	bool m_showMeasurements;
	bool m_isSelected;

	//cursor members
	long m_oldPositionX;
	long m_oldPositionY;

	//used for window/level functionality
	long m_previousRightPointX;
	long m_previousRightPointY;
	long m_previousLeftPointX;
	long m_previousLeftPointY;
	long m_currentX, m_currentY;

    //points from mouse moves
public:
	Vector3Vec* GetPaintVector(){ return &m_paintVector; }
	Vector3Vec* GetEraseVector(){ return &m_eraseVector; }
private:
	Vector3Vec m_paintVector;
	Vector3Vec m_eraseVector;

	//NEW
	//user targets
	CMeasure* m_targets;//for display of targets within cube display
	Vector3Vec m_userTargets;//list of temporary user points
	TriangleVec m_target_surface;//surface object for user defined targets
	
	CMeasure* m_surfaces;//for display of surfaces within cube display
	CMeasure* m_template;//for display of template needle trajectories
	SurfaceList m_surfaceList;//list of segmented objects
	SurfaceList m_templateList;//list of needle positions in template

	//linear measurements
	CMeasure* m_measurements;
	long  m_measureMode;//MLINE, MANGLE...
	PointVec mPoly, mUndo;

	std::string m_volumeLabel;//label to attach to volume when exporting surfaces
	std::string m_volumeFolderPath;//label to attach to volume when exporting surfaces
	std::string m_volumeFilePath;
    std::string m_registrationLabel;//used to label which volume is belongs to stored transform
	

};

#endif
