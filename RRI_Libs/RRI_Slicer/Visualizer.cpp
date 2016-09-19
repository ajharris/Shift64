//#include "stdafx.h"
#include "visualizer.h"

#include "Voxel.h"

//----------------------------------------------------------------------
// Visualizer constructor
//
//----------------------------------------------------------------------
Visualizer::Visualizer() 
:m_texture(0)
,m_model(0)
,m_volume(0)
{
}


//----------------------------------------------------------------------
// Visualizer destructor
//
//----------------------------------------------------------------------
Visualizer::~Visualizer()
{
	
	if (m_texture)
	{
		delete m_texture;
	}

	if (m_model)
	{
		delete m_model;
	}

	if (m_volume)
	{
		delete m_volume;
	}
	
}

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
/*
void Visualizer::Initialize(long clientX, long clientY, eScanGeometry geometry, long width, long height, long numFrames, long channels, float xvox, float yvox, float zvox, float sweepAngle, float sweepDistance)
{
//eScanGeometry geometry, long width, long height, long numFrames, long sweep
	if (m_volume) delete m_volume;
	m_volume = new VolumeClass();//default dimensions


	//InitVol sets up AcqParam and CalParam and creates a buffer of the size width*height*numFrames*channels
	m_volume->InitVol(geometry, width, height, numFrames, channels, xvox, yvox, zvox, sweepAngle, sweepDistance);//60 degrees, 25 mm

	AcqParam acq = m_volume->GetAcq();
	CalParam cal = m_volume->GetCal();

	unsigned char* data = m_volume->GetData();

	SetTex(TEX, data, acq, cal, clientX, clientY);//assume texture (not MIP) to start
	
	
	if (m_model)
	{
		delete m_model;
	}

	TmodelInit modelInit;
	modelInit.clientX   = clientX;
	modelInit.clientY   = clientY;
	modelInit.sizeX = m_texture->GetSizeXmm();
	modelInit.sizeY = m_texture->GetSizeYmm();
	modelInit.sizeZ = m_texture->GetSizeZmm();

	m_model = new Model(modelInit);
}
*/

VolumeClass* Visualizer::GetVolume()
{
	return m_volume;
}

void Visualizer::Initialize(long clientX, long clientY)
{
	m_texture->ResizeImBuff(clientX, clientY);

	if (m_model)
	{
		delete m_model;
	}

	TmodelInit modelInit;
	modelInit.clientX   = clientX;
	modelInit.clientY   = clientY;
	modelInit.sizeX = m_texture->GetSizeXmm();
	modelInit.sizeY = m_texture->GetSizeYmm();
	modelInit.sizeZ = m_texture->GetSizeZmm();

	//create new model class for visualizer
	m_model = new Model(modelInit);
}

void Visualizer::Initialize(long clientX, long clientY, AcqParam acq, CalParam cal)
{
//create new volume class

	if (m_volume) delete m_volume;
	m_volume = new VolumeClass();//default dimensions

	//initialize volume class with parameters passed in by caller
	m_volume->InitVol(acq, cal);

	//get a pointer to the data just created
	unsigned char* data = m_volume->GetData();

	//setup texture class
	SetTex(TEX, data, acq, cal, clientX, clientY);//assume texture (not MIP) to start
	
	if (m_model)
	{
		delete m_model;
	}

	TmodelInit modelInit;
	modelInit.clientX   = clientX;
	modelInit.clientY   = clientY;
	modelInit.sizeX = m_texture->GetSizeXmm();
	modelInit.sizeY = m_texture->GetSizeYmm();
	modelInit.sizeZ = m_texture->GetSizeZmm();

	//create new model class for visualizer
	m_model = new Model(modelInit);
}


/*
//----------------------------------------------------------------------
// SetTex
//
// Initialize texture
//
// Input:
// volume pixel data
// AcqParam structure filled with volume information
// CalParam structure filled with volume information
// width of the window
// height of the window
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::SetTex(long tex, const tVoxel *data, AcqParam* acq, CalParam* cal, long clientX, long clientY)
{

	
	if (m_texture)
	{
		delete m_texture;
	}

	TtexInit texInit;
	TvoxelInit voxelInit;

	voxelInit.data		= data;
	voxelInit.acq		= acq;
	voxelInit.cal		= cal;

	texInit.clientX		= clientX;
	texInit.clientY		= clientY;
	texInit.reconType	= acq.scanGeometry;

	if(tex == MIP)
	{
		m_texture = new TextureMIP(voxelInit, texInit);
	}
    else
    {
        if (texInit.reconType == SG_Linear24 || texInit.reconType == SG_Fan24 || texInit.reconType == SG_Hybrid24 || texInit.reconType == SG_Axial24)
        {
            m_texture = new TextureColour24(voxelInit, texInit);
        }
	    else
	    {
		    m_texture = new Texture(voxelInit, texInit);
	    }
    }

}
*/


//----------------------------------------------------------------------
// SetTex
//
// Initialize texture
//
// Input:
// volume pixel data
// AcqParam structure filled with volume information
// CalParam structure filled with volume information
// width of the window
// height of the window
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::SetTex(long tex, unsigned char* data, AcqParam acq, CalParam cal, long width, long height)
{

	
	if (m_texture)
	{
		delete m_texture;
	}

	TtexInit texInit;
	TvoxelInit voxelInit;

	voxelInit.data		= data;
	voxelInit.acq		= acq;
	voxelInit.cal		= cal;

	texInit.clientX		= width;
	texInit.clientY		= height;
	texInit.reconType	= acq.scanGeometry;

	if(tex == MIP)
	{
		m_texture = new TextureMIP(voxelInit, texInit);
	}
    else
    {
        if (texInit.reconType == SG_Linear24 || texInit.reconType == SG_Fan24 || texInit.reconType == SG_Hybrid24 || texInit.reconType == SG_Axial24)
        {
            m_texture = new TextureColour24(voxelInit, texInit);
        }
	    else
	    {
		    m_texture = new Texture(voxelInit, texInit);
	    }
    }

}

//----------------------------------------------------------------------
// GetImBuff
//
// Get the image of current visualizer state for display
//
// Input:
// none
//
// Output:
// image buffer
//----------------------------------------------------------------------
FilledBuff Visualizer::GetImBuff()
{
	return m_texture->GetImBuff();
}


//----------------------------------------------------------------------
// GetImBuff
//
// Create a new face in the cube model. Align it with the screen and
// get the visualizer image for display. 
// 
// Input:
// normal of the new face
// distance from the origin of the cube of the new face in mm
//
// Output:
// image buffer
//----------------------------------------------------------------------
FilledBuff Visualizer::GetImBuff(Vector3 normal, float distFactor, float rotation)
{
	m_texture->Render(m_model->GetView(normal, distFactor, ALIGN_TO_SCRN, rotation));
	return m_texture->GetImBuff();
}


//----------------------------------------------------------------------
// GetOrientMarker
//
// Return the orientation marker struct given the cube model vertex ('origin') 
// and the opposite corner ('label') along which a text label will be placed 
//
// Input:
// the cube model vertex origin
// the opposite corner label
//
// Output:
// orientation marker struct
//----------------------------------------------------------------------
OrientMarker Visualizer::GetOrientMarker(long origin, long label)
{
	return m_model->GetOrientMarker(origin, label);
}


//----------------------------------------------------------------------
// ScreenToModel
//
// Convert a point from the screen to model coordinates
//
// Input:
// face active face
// x screen coordinate
// y screen coordinate
// res 
//
// Output:
// Vector3 structure pointer res contains the model coordinates of the point
//----------------------------------------------------------------------
bool Visualizer::ScreenToModel(long face, long x, long y, Vector3 *res)
{
	return m_model->ScreenToModel(face, x,y, res);
}




//----------------------------------------------------------------------
// ScrnPtToModelPt
//
// Convert a point from the screen to model coordinates
//
// Input:
// x screen coordinate
// y screen coordinate
//
// Output:
// Vector3 structure pointer res contains the model coordinates of the point
//----------------------------------------------------------------------
bool Visualizer::ScrnPtToModelPt(long x, long y, Vector3 *res)
{
	return m_model->ScrnPtToModelPt(x,y,res);
}


//----------------------------------------------------------------------
// ScrnPtToViewPt
//
// Convert a point from the screen to view coordinates
//
// Input:
// x screen coordinate
// y screen coordinate
//
// Output:
// Vector3 structure pointer res contains the view coordinates of the point
//----------------------------------------------------------------------
bool Visualizer::ScrnPtToViewPt(long x, long y, Vector3 *res)
{
	return m_model->ScrnPtToViewPt(x,y,res);
}


//----------------------------------------------------------------------
// ModelPtToViewPt
//
// Convert a point from the model to view coordinates
//
// Input:
// Vector3 modelp contains the point in the model(cube) coordinates
//
// Output:
// Vector3 structure pointer viewpt contains the view coordinates of the point
//----------------------------------------------------------------------
bool Visualizer::ModelPtToViewPt(Vector3 modelpt, Vector3 *viewpt)
{
	return m_model->ModelPtToViewPt(modelpt, viewpt);
}


//----------------------------------------------------------------------
// SetDefaultModelView
//
// Set the active state of the visualizer with one of the default states
//
// Input:
// width of the window
// height of the window
// one of the default states id
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::SetDefaultModelView(long clientX, long clientY, eCubeRotation cubeRot)
{
	m_model->SetDefaultModelView(clientX, clientY, cubeRot);

}

//----------------------------------------------------------------------
// SetModelView: added by LG
//
// Set the active state of the visualizer given face and three rotations
//
// Input:
// width of the window
// height of the window
// one of the default states id
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::SetModelView(long clientX, long clientY, long face, float rotX, float rotY, float rotZ, bool resetZoom)
{
    m_model->SetModelView(clientX, clientY, face, rotX, rotY, rotZ, resetZoom);
}


//----------------------------------------------------------------------
// GetFaceID
//
// Given a point on the screen return id of the cube model face  
//
// Input:
// x coordinate of the point
// y coordinate of the point
//
// Output:
// face id
//----------------------------------------------------------------------
long Visualizer::GetFaceID(long x, long y)
{
	return m_model->GetFaceID(x,y);
}


//----------------------------------------------------------------------
// Slice
//
// Move a face of the cube model by specified in mm distance
//
// Input:
// id of the face to move
// distance in mm to move 
//
// Output:
// id of the face which was moved otherwise -1
//----------------------------------------------------------------------
long Visualizer::Slice(long faceID, float distMM)
{
	long ret =  m_model->Slice(faceID, distMM);


	return ret;
}


//----------------------------------------------------------------------
// AlignFaceToScrn
//
// Rotate the cube model to align the face with parallel to the screen
//  
// Input:
// id of the face to align
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::AlignFaceToScrn(long faceID)
{
	m_model->AlignFaceToScrn(faceID);
}

//----------------------------------------------------------------------
// AlignFaceToScrn
//
// Given three points in 3D space,
// Rotate the cube model to align the face with given plane defined by the three points
//  
// Input:
// three points defined as Vector3
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::AlignFaceToScrn(Vector3 firstpoint, Vector3 secondpoint, Vector3 thirdpoint)
{
    m_model->AlignFaceToScrn(firstpoint, secondpoint, thirdpoint);
}
/*

//----------------------------------------------------------------------
// AlignFaceToScrn
//
// Rotate the cube model to align the face with given normal of the face
//
// Input:
// Vector3 structure contains the normal
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::AlignFaceToScrn(Vector3 normal)
{
	m_model->AlignFaceToScrn(normal);

}



//----------------------------------------------------------------------
// AlignFaceToScrn
//
// Given two points in 3D space,
// Rotate the cube model to align the face with given plane defined by the three points
//  
// Input:
// three points defined as Vector3
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::AlignFaceToScrn(Vector3 firstpoint, Vector3 secondpoint)
{
    m_model->AlignFaceToScrn(firstpoint, secondpoint);
}

*/



//----------------------------------------------------------------------
// DeleteFace
//
// Remove a face from the cube model
//
// Input:
// id of the face to be removed
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::DeleteFace(long faceID)
{
	m_model->DeleteFace(faceID);

}


//----------------------------------------------------------------------
// GetVoxValue
//
// Return the volume voxel value 
//
// Input:
// x coordinate of the point (screen coordinates)
// y coordinate of the point
//
// Output:
// voxel value
//----------------------------------------------------------------------
tVoxel Visualizer::GetVoxValue(long x, long y)
{
	Vector3 vwPt, arrPt;

	m_model->ScrnPtToViewPt(x,y, &vwPt);
	m_texture->ViewPtToArrayPt(vwPt, &arrPt);
	
	return m_texture->GetVoxValue(arrPt);
}

//----------------------------------------------------------------------
// GetArrayPoint
//
// Return the array point that corresponds to the view point
//
// Input:
// x coordinate of the point (view coordinates)
// y coordinate of the point
//
// Output:
// array coordinate
//----------------------------------------------------------------------
Vector3 Visualizer::GetArrayPoint(Vector3 viewPoint)
{
	Vector3 arrayPoint;
	m_texture->ViewPtToArrayPt(viewPoint, &arrayPoint);

	return arrayPoint;
}
//----------------------------------------------------------------------
// GetVoxValue
//
// Return the volume voxel value  
//
// Input:
// x coordinate of the point (array coordinates in voxels)
// y coordinate of the point
// z coordinate of the point
//
// Output:
// voxel value
//----------------------------------------------------------------------
tVoxel Visualizer::GetVoxValue(long x, long y, long z)
{
	Vector3 arrPt(x,y,z);	
	return m_texture->GetVoxValue(arrPt);
}
tVoxel Visualizer::GetVoxValue(long x, long y, long z, long n)
{
	return m_texture->GetVoxValue(x,y,z,n);
}

void Visualizer::GetVoxValue(long x, long y, long z, tVoxel& lower, tVoxel& upper)
{
	return m_texture->GetVoxValue(x,y,z, lower, upper);
}

tVoxel Visualizer::GetVoxRValue(long x, long y, long z)
{
	Vector3 arrPt(x,y,z);	
	return m_texture->GetVoxRValue(arrPt);
}
tVoxel Visualizer::GetVoxGValue(long x, long y, long z)
{
	Vector3 arrPt(x,y,z);	
	return m_texture->GetVoxGValue(arrPt);
}
tVoxel Visualizer::GetVoxBValue(long x, long y, long z)
{
	Vector3 arrPt(x,y,z);	
	return m_texture->GetVoxBValue(arrPt);
}

void Visualizer::SetVoxValue(long x, long y, long z, long n, unsigned char value)
{
	m_texture->SetVoxValue(x,y,z,n, value);
}
//----------------------------------------------------------------------
// GetVoxValue
//
// Return the volume voxel value  
//
// Input:
// x coordinate of the point (model coordinates in voxels)
// y coordinate of the point
// z coordinate of the point
//
// Output:
// voxel value
//----------------------------------------------------------------------
tVoxel Visualizer::GetVoxValue(Vector3 point)
{
	return m_texture->GetVoxValue(point);
}


//-------------------------------------------------------------------------
Vector3 Visualizer::GetVoxPos(Vector3 point)
{
    return m_texture->GetVoxPos(point);
}


long Visualizer::GetVoxelIndex(Vector3 point)
{
	//get index into raw buffer
	long index = m_texture->GetVoxelIndex(point);
	return index;
}
//----------------------------------------------------------------------
// ActionModelView
//
// Slice, rotate one of the cube model faces or 
// move across the screen,rotate the cube model in the view coordinates  
//
// Input:
// x coordinate of the point (screen coordinates)
// y coordinate of the point
// temporalMode indicates phase of the action: BEGIN, CONTINUE, END
// actionMode indicates what operation to perform: SLICE, ROTATE...
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::ActionModelView(long x, long y, long temporalMode, long actionMode)
{
	m_model->ActionModelView(x,y,temporalMode, actionMode);
}


//----------------------------------------------------------------------
// Zoom
//
// Magnify the cube model in the view coordinates
//
// Input:
// magnify factor: above 1 specifies magnify in, below magnify out
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::Zoom(float factor)
{
	m_model->Zoom(factor);

}


//----------------------------------------------------------------------
// ResizeWindow
//
// Update the visualizer state when the view window size is altered
//
// Input:
// width of a new window size
// height of a new window size
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::ResizeWindow(long x, long y)
{
	m_model->SetScale(x, y);
	m_model->SetCenterAB(x/2, y/2);

	m_texture->ResizeImBuff(x, y);

}


//----------------------------------------------------------------------
// GetEdgeFace
//
// Given a point in the screen coordinates check whether this point is
// close to the active face edge if yes return active face id 
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// face id
//----------------------------------------------------------------------
long Visualizer::GetEdgeFace(long x, long y)
{
	return m_model->GetEdgeFace(x,y);
}


//----------------------------------------------------------------------
// GetActiveEdgeFace
//
// Return the active face id of the cube model 
//
// Input:
// none
//
// Output:
// face id
//----------------------------------------------------------------------
long Visualizer::GetActiveEdgeFace()
{
	return m_model->GetActiveEdgeFace();
}



Face	Visualizer::GetFrame(Vector3 normal, float offset, long alignMode, float rotation)
{

    return m_model->GetView(normal, offset, alignMode, rotation).face[0];

}


//----------------------------------------------------------------------
// GetView
//
// Return the view information of the current state of the cube model  
//
// Input:
// none
//
// Output:
// ViewORTHO structure - gives all the information of the current state
//----------------------------------------------------------------------
const viewORTHO& Visualizer::GetView()
{
	return m_model->GetView();
}


//----------------------------------------------------------------------
// GetView
//
// Create a new face in the cube model given a normal and distance,
// rotate to aligned the new face parallel with the screen, then
// return the view information of the new state of the cube model
//
// Input:
// alignMode indicates if one wants to aligned the new face parallel
//
// Output:
// ViewORTHO structure - gives all the information of the current state
//----------------------------------------------------------------------
const viewORTHO& Visualizer::GetView(Vector3 normal, float offset, long alignMode, float rotation)
{
	return m_model->GetView(normal, offset, alignMode, rotation);
}


//----------------------------------------------------------------------
// SetView
//
// Create a new face in the cube model given a normal and distance,
// rotate to aligned the new face parallel with the screen, then
// set the cube model with this new state 
//
// Input:
// normal of the new face
// distance from the origin of the cube of the new face in mm
// alignMode indicates if one wants to aligned the new face parallel
// one of the default states id
//
// Output:
// none
//----------------------------------------------------------------------
void  Visualizer::SetView(Vector3 normal, float offset, long alignMode, eCubeRotation cubeRot)
{
	m_model->SetView(normal, offset, alignMode, cubeRot);
	
}


//----------------------------------------------------------------------
// GetStateModelView
//
// Get the active state of the visualizer
//
// Input:
// none
//
// Output:
// mvState - gives all the information of the current state 
//----------------------------------------------------------------------

mvState * Visualizer::GetStateModelView()
{
	m_model->GetStateModelView(&m_viewState);
	return &m_viewState;
}

//vState *state, long clientX, long clientY, bool scaleAbs)
void Visualizer::SetStateModelView(mvState* state, long clientX, long clientY)
{
	m_model->SetStateModelView(state, clientX, clientY, true);
}
/*

void Visualizer::GetStateModelView(mvState* state)
{
    m_model->GetStateModelView(state);
}
*/

/*
//----------------------------------------------------------------------
// SetStateModelView
//
// Initialize the cube model given a state
//
// Input:
// pointer of the state structure
// width of a new window size
// height of a new window size
//
// Output:
// none 
//----------------------------------------------------------------------
void Visualizer::SetStateModelView(mvState *state, long clientX, long clientY, bool scaleAbs)
{
	m_model->SetStateModelView(state, clientX, clientY, scaleAbs);

}
*/

//----------------------------------------------------------------------
// GetNormal
//
// Return the normal of a given face in the cube model
//
// Input:
// face id
//
// Output:
// Vector3 structure contains the normal
//----------------------------------------------------------------------
Vector3 Visualizer::GetNormal(long faceID)
{
	return m_model->GetNormal(faceID); 		
}


//----------------------------------------------------------------------
// GetD
//
// Return the distance of a given face from the centre of the cube model 
// in mm 
//
// Input:
// face id
//
// Output:
// distance in mm
//----------------------------------------------------------------------
float Visualizer::getD(long faceID)
{
	return m_model->GetDmm(faceID); 		
}


//----------------------------------------------------------------------
// Render
//
// Get current state of the visualizer and retexture it 
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::Render()
{
	m_texture->Render(m_model->GetView());	
}

//----------------------------------------------------------------------
// Render
//
// Get current state of the visualizer and retexture it 
//
// Input:
// slice face to render
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::Render(long face)
{
    m_texture->Render(m_model->GetView(), face); 
}

//----------------------------------------------------------------------
// GetCubeSize
//
// Return the dimensions of the reconstructed volume in mm 
//
// Input:
// none
//
// Output:
// Vector3 structure contains x,y,z dimensions in mm
//----------------------------------------------------------------------
Vector3 Visualizer::GetCubeSize()
{
	Vector3 size;

	size.x = m_texture->GetSizeXmm();
	size.y = m_texture->GetSizeYmm();
	size.z = m_texture->GetSizeZmm();
	
	return(size);
}


//----------------------------------------------------------------------
// GetCubeDimensions
//
// Return the size of the reconstructed volume in pixels
//
// Input:
// none
//
// Output:
// Vector3 structure contains x,y,z size in pixels
//----------------------------------------------------------------------
Vector3 Visualizer::GetCubeDim()
{
	Vector3 size;

	size.x = m_texture->GetDimX();
	size.y = m_texture->GetDimY();
	size.z = m_texture->GetDimZ();

	return(size);
}


//----------------------------------------------------------------------
// SetPivotPoint
//
// Set a rotation point around which to rotate all faces of the cube model 
//
// Input:
// Vector3 structure contains the new centroid point
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::SetPivotPoint(Vector3 point)
{
	m_model->SetPivotPoint(point);
}


//----------------------------------------------------------------------
// PivotRotationToggle
//
// Set/reset rotation around pivot point
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::PivotRotationToggle()
{
	m_model->PivotRotationToggle();
}

void Visualizer::PivotRotationToggle(bool pivot)
{
    m_model->PivotRotationToggle(pivot);
}


//----------------------------------------------------------------------
// GetRotationToggleState
//
// Return true if the faces of the cube rotate around a pivot point,
// if false then the faces rotate around the centroid of the cube model
//
// Input:
// none
//
// Output:
// true if the faces of the cube rotate around a pivot point,
// if false then the faces rotate around the centroid of the cube model
//----------------------------------------------------------------------
bool Visualizer::GetRotationToggleState()
{
	return m_model->GetRotationToggleState();
}


//----------------------------------------------------------------------
// RotateCubeOnAxis
//
// Rotate the cube model in the view coordinates around the constrain axis
//
// Input:
// short indicating on what axis to rotate: 1 is x axis, 2 is y axis 
// 3 is z axis
//
// Output:
// none
//----------------------------------------------------------------------
void  Visualizer::RotateCubeOnAxis(short axis)
{
	m_model->RotateCubeOnAxis(axis);

}


//----------------------------------------------------------------------
// RotateFaceOnAxis
//
// Rotate the active cube model face around the constrain axis
//
// Input:
// short indicating on what axis to rotate: 1 is x axis, 2 is y axis
//
// Output:
// none 
//----------------------------------------------------------------------
void  Visualizer::RotateFaceOnAxis(short axis)
{
	m_model->RotateFaceOnAxis(axis);

}


//----------------------------------------------------------------------
// SetConstrainAxis
//
// Initialize the constrain axis 
//
// Input:
// Vector3 structure contains the vector around which to rotate the cube
// face
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::SetConstrainAxis(Vector3 axis)
{
	m_model->SetConstrainAxis(axis);
}


//----------------------------------------------------------------------
// SetRotationAngle
//
// Set the angle of rotation
//
// Input:
// angle of rotation
//
// Output:
// none 
//----------------------------------------------------------------------
void Visualizer::SetRotationAngle(float angle)
{
	m_model->SetRotationAngle(angle);
}


//----------------------------------------------------------------------
// GetImBuff
//
// Create 
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
FilledBuff Visualizer::GetImBuff(Vector3 point0, Vector3 point1, Vector3 point2)
{
	Vector3 normal = !((point2 - point0) * (point1 - point0));
	float offset = normal % point0;

	m_texture->Render(m_model->GetView(normal, offset, ALIGN_TO_SCRN, 0));

	return m_texture->GetImBuff();
}


//----------------------------------------------------------------------
// GetModelSize
//
// Return the number of faces in the cube model 
//
// Input:
// none
//
// Output:
// number of faces in the cube model
//----------------------------------------------------------------------
long Visualizer::GetModelSize()
{
	long size =	m_model->GetModelSize();

	return size;
}

//----------------------------------------------------------------------
// setLeftRight
//
// Set the window for 16 bits data  
//
// Input:
// left, right extent of the window
//
// Output:
// none
//----------------------------------------------------------------------
void Visualizer::SetLeftRight(long left, long right)
{
	m_texture->setLeftRight(left, right);
}



void Visualizer::SetZoomFactor(float factor)
{
    
    m_model->SetZoomFactor(factor);

}


float Visualizer::GetZoomFactor(void)
{

	return m_model->GetZoomFactor();

}

//***LG ADDED
void Visualizer::RotateFaceOnAxis(short axis, long activeFace )
{
	m_model->RotateFaceOnAxis(axis, activeFace);
}
#endif