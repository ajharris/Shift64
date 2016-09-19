#ifndef VISUALIZER_H_INCLUDED
#define VISUALIZER_H_INCLUDED

class VolumeClass;
class Voxel;

#include "geometry.h"
#include "Fixed.h"
#include "texture.h"
#include "Texture_MIP.h"
#include "TextureColour24.h"
#include "Model.h"
#include "VolumeClass.h"


#define VISUALIZER_AVAILABLE

class Visualizer 
{

public:

	  Visualizer();
	  virtual ~Visualizer();
#ifdef VISUALIZER_AVAILABLE

//----------------------------------------------------------------------
// Initialize
//
// Initialize Visualizer
//
// Input:  
// clientX: width of display window
// clientY: height of display window
// geometry: visualizer scan geometry
// width: width of volume image
// height: height of volume image
// numFrames: number of frames in the volume image
// channels: number of channels per each image
// sweepAngle: angular distance of travel of scanner (deg)
// sweepDistance: linear distance of travel of scanner (mm)
//
// Output:none
//----------------------------------------------------------------------
	 void Initialize(  long clientX//width of display area
					 , long clientY//height of display area
					 , eScanGeometry geometry//scan geometry of volume image
					 , long width//widht of volume image
					 , long height//height of volume image
					 , long numFrames//number of frames in the volume scan
					 , long channels//number of channels in each image (1, 2 and 3 channels supported)
					 , float xvox
					 , float yvox
					 , float zvox
					 , float sweepAngle = 0.0//angular distance of travel of scanner (deg)
					 , float sweepDistance = 0.0);//linear distance of travel of scanner (mm)

	 void Initialize(long clientX, long clientY);
	 
	 void Initialize(long clientX, long clientY, AcqParam acq, CalParam cal);
	 
     void SetTex(long tex, unsigned char* data, AcqParam acq, CalParam cal, long clientX, long clientY);	

	 long ImportVolume(string filePath);
	 					     			
     void Render();	

     void Render(long face);
 
	 void ActionModelView(long x, long y, long temporalMode, long actionMode);	
																				
	 long Slice(long faceID, float distMM);

	 void Zoom(float factor);
     float GetZoomFactor(void);
     void SetZoomFactor(float factor);//{m_model.SetZoomFactor(factor);}
				
     FilledBuff GetImBuff();												    																							// image buffer

	 tVoxel GetVoxValue(long x, long y);
	 tVoxel GetVoxValue(long x, long y, long z);
     tVoxel GetVoxValue(Vector3 point);
	 tVoxel GetVoxValue(long x, long y, long z, long n);
	 void SetVoxValue(long x, long y, long z, long n,unsigned char value);
	 void GetVoxValue(long x, long y, long z, tVoxel& lower, tVoxel& upper);

     //for colour images
     tVoxel GetVoxRValue(long x, long y, long z);
     tVoxel GetVoxGValue(long x, long y, long z);
     tVoxel GetVoxBValue(long x, long y, long z);

     Vector3 GetVoxPos(Vector3 modelPoint);


	 long GetFaceID(long x, long y);
	 void DeleteFace(long faceID);

	 void AlignFaceToScrn(long faceID);
	 void AlignFaceToScrn(Vector3 firstpoint, Vector3 secondpoint, Vector3 thirdpoint);
	 void SetDefaultModelView(long clientX, long clientY, eCubeRotation cubeRot);
     void SetModelView(long clientX, long clientY, long face, float rotX, float rotY, float rotZ, bool resetZoom);

		
	 void SetView(Vector3 normal, float offset, long alignMode, eCubeRotation cubeRot);

	 const viewORTHO& GetView();														
	 const viewORTHO& GetView(Vector3 normal, float offset, long alignMode, float rotation);


    //used to get and set the scaling factor of the cube display
     float GetScaleAbs(){return m_model->GetScaleAbs();}//***LG
     void SetScaleAbs(float scale){m_model->SetScaleAbs(scale);}

     float GetScaleRel(){return m_model->GetScaleRel();}
     void SetScaleRel(float scale){m_model->SetScaleRel(scale);}

	 Vector3 GetNormal(long faceID);

	 Vector3	GetCubeSize(); 
	 Vector3    GetCubeDim(); 
     long       GetModelSize();

	 long GetVoxelIndex(Vector3 point);//{return m_texture->GetVoxelIndex(point);}
	
	 bool ScrnPtToModelPt(long x, long y, Vector3* res);	
	 bool ScrnPtToViewPt(long x, long y, Vector3* res);
	 bool ModelPtToViewPt(Vector3 modelpt, Vector3* viewpt);
	 Vector3 GetArrayPoint(Vector3 viewPoint);

    //***NEW LG
     bool ScreenToModel(long face, long x, long y, Vector3* res);

	 mvState * GetStateModelView();
	 void SetStateModelView(mvState* state, long clientX, long clientY);

	 void SetPivotPoint(Vector3 point);
     void PivotRotationToggle(bool pivot);
	 bool GetRotationToggleState();

	 void RotateCubeOnAxis(short axis);
	 void RotateFaceOnAxis(short axis);
	 void SetConstrainAxis(Vector3 axis);
	 void SetRotationAngle(float angle);
     void RotateFaceOnAxis(short axis, long activeFace);//***LG

     Face GetFrame(Vector3 normal, float offset, long alignMode, float rotation);//new


	//Added by LG
     void PureRotate(Vector3 axis, float angle){m_model->PureRotate(axis,angle);}
	
//extra interface functions not used in BING
	 long GetActiveEdgeFace();	
	 long GetEdgeFace(long x, long y);	
																												
	 OrientMarker GetOrientMarker(long origin, long label);

     void PivotRotationToggle();

     FilledBuff GetImBuff(Vector3 normal, float offset, float rotation);
	 FilledBuff GetImBuff(Vector3 point1, Vector3 point2, Vector3 point3);

     void ResizeWindow(long x, long y);	

    bool m_usingTrilinear;

    //for 16-bit
     void SetLeftRight(long left, long right);
	 VolumeClass* GetVolume();// {return m_volume; }

private:
    float getD(long faceID);
    
	mvState m_viewState;

	VolumeClass *m_volume;
#endif

	Texture  *m_texture;   
	Model	 *m_model;

};

#endif
