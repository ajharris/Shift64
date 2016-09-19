//#if !defined( MAKE_DLL )
//#include "stdafx.h"
//#endif



#include "model.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


//----------------------------------------------------------------------
// Model constructor
//
// 
//----------------------------------------------------------------------
Model::Model(TmodelInit modelInit):
	 m_creatingFace(false),
	 m_actionDone(false),
	 m_activeFace(-1),
	 m_actionMode(DOING_NOTHING)
{

	m_model = FaceVec(ORIGINAL_FACES_NUM);

	m_originalModel = m_model;

	m_AB.viewCenter = Vector3((long)modelInit.clientX >> 1, (long)modelInit.clientY >> 1, 0);
	m_AB.radius = ~m_AB.viewCenter + 1;
	m_shiftAbs = m_AB.viewCenter;
	m_shiftRel = Vector3(0,0,0);
	m_scaleAbs = 1.0f;


	float minX = -modelInit.sizeX / 2.0f;
	float minY = -modelInit.sizeY / 2.0f;
	float minZ = -modelInit.sizeZ / 2.0f;

	float maxX = minX + modelInit.sizeX;
	float maxY = minY + modelInit.sizeY;
	float maxZ = minZ + modelInit.sizeZ;
	
	minX *= FUDGE; 
	minY *= FUDGE;
	minZ *= FUDGE;

	m_diag = 2 * sqrtf(maxX * maxX + maxY * maxY + maxZ * maxZ); 
	
	m_modelVerts[tlf].x = m_modelVerts[blf].x = m_modelVerts[tlb].x = m_modelVerts[blb].x = minX;
	m_modelVerts[trf].x = m_modelVerts[brf].x = m_modelVerts[trb].x = m_modelVerts[brb].x = maxX;
	m_modelVerts[tlf].y = m_modelVerts[trf].y = m_modelVerts[tlb].y = m_modelVerts[trb].y = minY;
	m_modelVerts[blf].y = m_modelVerts[brf].y = m_modelVerts[blb].y = m_modelVerts[brb].y = maxY;
	m_modelVerts[tlf].z = m_modelVerts[trf].z = m_modelVerts[blf].z = m_modelVerts[brf].z = minZ;
	m_modelVerts[tlb].z = m_modelVerts[trb].z = m_modelVerts[blb].z = m_modelVerts[brb].z = maxZ;
		
	// constrain rotation
	m_constrain = false;
	m_constrainAxis = Vector3(0.0, -1.0, 0.0);
	SetRotationAngle(3);

	m_pivotPoint.x = m_pivotPoint.y = m_pivotPoint.z = 0.0;
	m_pointRotationMode = false;

	SetDefaultModelView(modelInit.clientX, modelInit.clientY );
}


//----------------------------------------------------------------------
// Model destructor
//
// 
//----------------------------------------------------------------------
Model::~Model()
{
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
OrientMarker Model::GetOrientMarker(long origin, long label)
{
    //***EIGEN
	//const float OFFSET = 0.05f; // amount the vertex pushed out from the model cuboid
	//const float LENGTH = 0.3f;  // length of lines as a percentage of edge lengths
	//const float TEXT = 0.05f;   // offset for text label

    const float OFFSET = 0.07f; // amount the vertex pushed out from the model cuboid
	const float LENGTH = 0.2f;  // length of lines as a percentage of edge lengths
	const float TEXT = 0.05f;   // offset for text label

//	OrientMarker m;
	
	Vector3 offset = m_modelVerts[origin] * OFFSET;
	m_orentationMarker.start = m_modelVerts[origin] + offset;

	m_orentationMarker.endX = m_modelVerts[origin] * (1.0f - LENGTH) + m_modelVerts[orientID[origin].x] * LENGTH + offset;
	m_orentationMarker.endY = m_modelVerts[origin] * (1.0f - LENGTH) + m_modelVerts[orientID[origin].y] * LENGTH + offset;
	m_orentationMarker.endZ = m_modelVerts[origin] * (1.0f - LENGTH) + m_modelVerts[orientID[origin].z] * LENGTH + offset;

	m_orentationMarker.label = m_orentationMarker.start * (1.0f - TEXT) + m_modelVerts[label] * TEXT; 
	
	return m_orentationMarker;
//	return m;
}


//----------------------------------------------------------------------
// Slice
//
// Move a plane of the cube model by specified in mm distance
//
// Input:
// id of the face to move
// distance in mm to move
//
// Output:
// id of the moved face, -1 if face not found
//----------------------------------------------------------------------
long Model::Slice(long faceID, float distMM)
{
	if (faceID < ORIGINAL_FACES_NUM)
	{
		faceID = newFace(faceID, m_model); 
		m_creatingFace = true;
	}

	Plane savedFace = m_model[faceID].plane;	

	m_model[faceID].plane.D += distMM;

	computeVertices(m_model);
	transformView(m_view, m_model);

	// if the change would make the slice plane nearly disappear, undo it
	if (m_model[faceID].vert.size() < 3 || maxEdgeLength(faceID) < m_diag / 50.0f )
	{
		if (m_creatingFace)
		{
			DeleteFace(faceID);
			m_creatingFace = false;
		}
		else
		{

			m_model[faceID].plane = savedFace;
		}
	
		computeVertices(m_model);
		transformView(m_view, m_model);

		return -1 ;	
	}

	m_creatingFace = false;

	return faceID;
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
long Model::GetFaceID(long x, long y)
{
	for (long i = m_view.face.size() -1; i >= 0; i--)
	{
		if (m_view.face[i].vert.size() > 2 && geom::PtInPoly(m_view.face[i].vert, x, y)) 
		{
			return i;
		}
	}

	return -1;
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
bool Model::ScrnPtToViewPt(long x, long y, Vector3* vec)
{
	long face = GetFaceID( x,y);


	if (face == -1)
	{
		return false;
	}


	*vec = Vector3(x,y,0);

	float D = m_view.face[face].plane.D;
	Vector3 norm = m_view.face[face].plane.normal;

	if(norm.z)
	{
		vec->z = (D - norm.x * vec->x - norm.y * vec->y) / norm.z;
	}
	else
	{
		vec->z = D;
	}

	return true;
}



//=========================================================================
//LG: added these two functions to
//----------------------------------------------------------------------
// ScrnPtToViewPt
//
// Convert a point from the screen to view coordinates
//
// Input:
// face (if you know the face)
// x screen coordinate
// y screen coordinate
// 
//
// Output:
// Vector3 structure pointer res contains the view coordinates of the point
//----------------------------------------------------------------------
bool Model::ScrnPtToViewPt(long face, long x, long y, Vector3* vec)
{

	*vec = Vector3(x,y,0);

	float D = m_view.face[face].plane.D;
	Vector3 norm = m_view.face[face].plane.normal;

	if(norm.z)
	{
		vec->z = (D - norm.x * vec->x - norm.y * vec->y) / norm.z;
	}
	else
	{
		vec->z = D;
	}

	return true;
}


//----------------------------------------------------------------------
// ScrnPtToModelPt
//
// Convert a point from the screen to model coordinates
//
// Input:
// face
// x screen coordinate
// y screen coordinate
//
// Output:
// Vector3 structure pointer res contains the model coordinates of the point
//----------------------------------------------------------------------
bool Model::ScreenToModel(long face, long x, long y, Vector3* vec)
{
	Vector3 vwPt;


	ScrnPtToViewPt(face, x, y, &vwPt);//convert to view coordinates
	


	*vec = vwPt * m_view2Model;//calculate model coordinates

	return true;
}
//================================================================================


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
bool Model::ScrnPtToModelPt(long x, long y, Vector3* vec)
{
	Vector3 vwPt;

    
	if (!ScrnPtToViewPt(x, y, &vwPt))
	{
		return false;
	}
    

	*vec = vwPt * m_view2Model;

	return true;
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
bool Model::ModelPtToViewPt(Vector3 modelpt, Vector3* viewpt)
{
	*viewpt = modelpt * m_model2View;

	return true;
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
long Model::GetActiveEdgeFace()
{
	if (m_actionMode == SLICE_ROTATE)
	{
		return m_activeFace;
	}
	else
	{
		return -1;
	}
}


//----------------------------------------------------------------------
// GetView
//
// Create a new face in the cube model given a normal and distance,
// rotate to aligned the new face parallel with the screen, then
// return the view information of the new state of the cube model
//
// Input:
// normal of the new face
// distance from the origin of the cube model of the new face
// alignMode indicates if one wants to aligned the new face parallel
// to the screen
//
// Output:
// ViewORTHO structure - gives all the information of the current state
//----------------------------------------------------------------------
const viewORTHO&  Model::GetView(Vector3 normal, float distFactor, long alignMode, float rotation)
{
	FaceVec clipModel = m_originalModel;

	float D = distFactor;
	Plane plane(normal, D);

	long faceID = newFace(plane, clipModel);

	Quaternion qRot;

	if (alignMode == ALIGN_TO_SCRN)
	{
		Vector3 Scrn(0,0, -1);
		Vector3 axis = !(normal * Scrn);

		float cosTheta	= normal % Scrn;
		cosTheta		= sqrtf( (1.0 + cosTheta) / 2.0 );
		float sinTheta  = sqrtf(1.0 - cosTheta * cosTheta);

		qRot = Quaternion(0,0,rotation) * Quaternion(cosTheta, sinTheta * axis);
	}
	else
	{
		qRot = 	Quaternion(0,0,rotation) * Quaternion(10.0f, 20.0f, 3.4f);
	}

	Matrix4x4 rot(qRot.s, qRot.V);

	Matrix4x4 M2VRot = m_TcenterInv * rot * m_Tcenter;
	Matrix4x4 M2V = M2VRot * m_S * m_T;

	Matrix4x4 V2MRot = m_TcenterInv * ~rot * m_Tcenter;
	Matrix4x4 V2M = m_Tinv * m_Sinv * V2MRot;

	transformView(m_view, clipModel, M2V);

	m_view.Model2View = M2V;
	m_view.View2Model = V2M;

	Face face = m_view.face[faceID];

	m_view.face.erase(m_view.face.begin(), m_view.face.end() );
	m_view.face.push_back(face);

	return m_view;
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
void Model::GetStateModelView(mvState *state)
{
	state->model = m_model;

	state->Model2ViewRot = m_model2ViewRot;
	state->View2ModelRot = m_view2ModelRot;

	state->scaleAbs = m_scaleAbs;

	state->modelShift[0] = m_shiftRel.x / m_scaleRel;
	state->modelShift[1] = m_shiftRel.y / m_scaleRel;
}


//----------------------------------------------------------------------
// SetStateModelView
//
// Initialize the cube model
//
// Input:
// pointer of the state structure
// width of a new window size
// height of a new window size
//
// Output:
// none 
//----------------------------------------------------------------------
void Model::SetStateModelView(mvState *state, long clientX, long clientY, bool scaleAbs)
{
	m_model = state->model;

	m_shiftAbs = Vector3(clientX >> 1, clientY >> 1, 0);

	m_shiftRel = Vector3(state->modelShift[0] * m_scaleRel, state->modelShift[1] * m_scaleRel, 0);

	m_AB.viewCenter = m_shiftAbs;
	m_AB.radius = ~m_AB.viewCenter + 1.0;

	m_model2ViewRot = state->Model2ViewRot;
	m_view2ModelRot = state->View2ModelRot;


	if (state->scaleAbs && scaleAbs)
	{
		m_scaleAbs = state->scaleAbs;
	}



	m_T = Matrix4x4(m_shiftAbs + m_shiftRel);
	m_Tinv = Matrix4x4(-1.0 * (m_shiftAbs + m_shiftRel));

	SetScale(clientX, clientY);

	centroidCorrect();
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
void Model::SetDefaultModelView(long clientX, long clientY, eCubeRotation cubeRot)
{
	m_AB.viewCenter = m_shiftAbs;

	m_AB.radius = sqrtf(clientX * clientX/4 + clientY * clientY/4) + 1.0;

	m_shiftRel = Vector3(0,0,0);

	long minWinDim = (clientX < clientY) ? clientX : clientY;

	m_scaleAbs = 1.55f;//1.35f
//***	m_scaleRel =  0.95 * minWinDim / m_diag;
    //***LG modified this so default scaling matches ultrasound image more closely in BING application
    m_scaleRel =  1.0 * minWinDim / m_diag;

	m_AB.scaleFactor = m_scaleRel * m_scaleAbs;

	m_S = Matrix4x4(m_AB.scaleFactor);
	m_Sinv = Matrix4x4(1.0 / m_AB.scaleFactor);	

	m_T = Matrix4x4(m_shiftAbs);
	m_Tinv = Matrix4x4(-1.0 * m_shiftAbs);

	m_Tcenter = identity4x4;
	m_TcenterInv = identity4x4;

	m_model = FaceVec(ORIGINAL_FACES_NUM); 

	for (long i = 0; i < ORIGINAL_FACES_NUM; ++i)
	{
		m_model[i].vert.erase(m_model[i].vert.begin(), m_model[i].vert.end());

		for (long j = 0; j < 4; ++j)
		{
			m_model[i].vert.push_back(m_modelVerts[CVID[i][j]]);
		}

		m_model[i].plane.normal = !( (m_model[i].vert[2] - m_model[i].vert[0]) * (m_model[i].vert[1] - m_model[i].vert[0]) );

		m_model[i].plane.D = m_model[i].plane.normal % m_model[i].vert[0];
	}


	m_model2ViewRot = identity4x4;
	m_view2ModelRot = identity4x4;

	long sliceFace = -1;

	switch(cubeRot)
	{
		default:
		case rotNone:
			m_AB.qDrag = Quaternion(10.0f, 20.0f, 3.4f);
            //m_AB.qDrag = Quaternion(90.0f, 0.0f, 0.0f);
			break;

		case rotUpDown:
			m_AB.qDrag = Quaternion(0.0, 90.0f, 0.0f);
            sliceFace = 3;
			break;

		case rotFrontBack:
			//m_AB.qDrag = Quaternion(-10.0f, -160.0f, 3.4f);
            m_AB.qDrag = Quaternion(10, 10,  0);
            sliceFace = 0;
			break;

       case rotSagittal:
			m_AB.qDrag = Quaternion(-90.0f, -90.0f, 180.0f);
			sliceFace = 2;
			break;

		case rotSagittal2:
			m_AB.qDrag = Quaternion(0.0f, 0.0f, 90.0f);
			sliceFace = 2;
			break;

        case rotSagittal3:
			m_AB.qDrag = Quaternion(90.0f, 0.0f, 0.0f);
			sliceFace = 3;
			break;

		case rotTransverse:
			//m_AB.qDrag = Quaternion(0.0f, 90.0f, 0.0f);
            m_AB.qDrag = Quaternion(90.0f, 90.0f, 0.0f);//modified for BING
			sliceFace = 4;
			break;

		case rotCoronal:
			//m_AB.qDrag = Quaternion(90.0f, 90.0f, 0.0f);//modified for BING
            m_AB.qDrag = Quaternion(90.0f, 90.0f, 0.0f);//modified for BING
            sliceFace = 4;
			break;

		case rotTransverseNoSlice:
			//m_AB.qDrag = Quaternion(0.0f, 90.0f, 0.0f)*Quaternion(-3.4f,-10.0f,  20.0f);
            m_AB.qDrag = Quaternion(90.0f, 90.0f, 0.0f);//modified for BING
			break;

		case rotSagittalNoSlice:
			//m_AB.qDrag = Quaternion(-90.0f, -90.0f, 180.0f);
            m_AB.qDrag = Quaternion(-90.0f, 0.0f, 180.0f);
            //m_AB.qDrag = Quaternion(10.0f, -20.0f, -3.4f);
			break;

		case rotCoronalNoSlice:
			//m_AB.qDrag = Quaternion(100.0f, -10.0f, 0.0f);
            m_AB.qDrag = Quaternion(0.0f, 0.0f, 0.0f);//modified for BING
			break;

		case rotCardiac:
			m_AB.qDrag = Quaternion(0.0f, -90.0f, 0.0f) * Quaternion(-90.0f, 0.0f, 0.0f);
			sliceFace = 2;
			break;

        //***LG: added these
        case rotAxialNoOffsets:
            m_AB.qDrag = Quaternion(90.0f, 90.0f, 0.0f);
            sliceFace = 3;
		    break;
        case rotSagittalNoOffsets:
            m_AB.qDrag = Quaternion(90.0f, 90.0f, 0.0f);
            sliceFace = 4;

		    break;
        case rotSideFired:
            m_AB.qDrag = Quaternion(0.0f, 90.0f, 0.0f);
		    sliceFace = 3;
		    break;
	}

	m_AB.qRotSess = qid;

	updateModelView();

	// do the slicing if needed
	if (sliceFace >=0 )
	{
		Slice(sliceFace, -m_model[sliceFace].plane.D);
	}
}

//***NEW: added by LG
void Model::SetModelView(long clientX, long clientY, long face, float rotX, float rotY, float rotZ, bool resetZoom)
{
	m_AB.viewCenter = m_shiftAbs;

	m_AB.radius = sqrtf(clientX * clientX/4 + clientY * clientY/4) + 1.0;

    
	m_shiftRel = Vector3(0,0,0);

	long minWinDim = (clientX < clientY) ? clientX : clientY;

    if (resetZoom)
	    m_scaleAbs = 1.55f;//1.35f

    m_scaleRel =  1.0 * minWinDim / m_diag;

	m_AB.scaleFactor = m_scaleRel * m_scaleAbs;

	m_S = Matrix4x4(m_AB.scaleFactor);
	m_Sinv = Matrix4x4(1.0 / m_AB.scaleFactor);	

	m_T = Matrix4x4(m_shiftAbs);
	m_Tinv = Matrix4x4(-1.0 * m_shiftAbs);

	m_Tcenter = identity4x4;
	m_TcenterInv = identity4x4;

	m_model = FaceVec(ORIGINAL_FACES_NUM); 

	for (long i = 0; i < ORIGINAL_FACES_NUM; ++i)
	{
		m_model[i].vert.erase(m_model[i].vert.begin(), m_model[i].vert.end());

		for (long j = 0; j < 4; ++j)
		{
			m_model[i].vert.push_back(m_modelVerts[CVID[i][j]]);
		}

		m_model[i].plane.normal = !( (m_model[i].vert[2] - m_model[i].vert[0]) * (m_model[i].vert[1] - m_model[i].vert[0]) );

		m_model[i].plane.D = m_model[i].plane.normal % m_model[i].vert[0];
	}

    //if (resetZoom)
    {

	    m_model2ViewRot = identity4x4;
	    m_view2ModelRot = identity4x4;
    }



    m_AB.qDrag = Quaternion(rotX, rotY, rotZ);


	m_AB.qRotSess = qid;

	updateModelView();



	// do the slicing if needed
	if (face >=0 )
	{
		Slice(face, -m_model[face].plane.D);
	}



}



//----------------------------------------------------------------------
// SetView
//
// Set the active state of the visualizer with one of the default states 
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void  Model::SetView(Vector3 normal, float distFactor, long alignMode, eCubeRotation cubeRot)
{
	// reset model
	m_model = FaceVec(ORIGINAL_FACES_NUM); 

	for (long i = 0; i < ORIGINAL_FACES_NUM; ++i)
	{
		m_model[i].vert.erase(m_model[i].vert.begin(), m_model[i].vert.end());

		for (long j = 0; j < 4; ++j)
		{
			m_model[i].vert.push_back(m_modelVerts[CVID[i][j]]);
		}

		m_model[i].plane.normal = !((m_model[i].vert[2] - m_model[i].vert[0]) * (m_model[i].vert[1] - m_model[i].vert[0]));

		m_model[i].plane.D = m_model[i].plane.normal % m_model[i].vert[0];
	}

	m_model2ViewRot = identity4x4;
	m_view2ModelRot = identity4x4;

	long sliceFace = 0;

	switch(cubeRot)
	{
		default:
		case rotNone:
		case rotSagittal:
			m_AB.qDrag = Quaternion(0,0,0);
			sliceFace = 0;
			break;
		
		case rotTransverse:
			m_AB.qDrag = Quaternion(0.0f, 90.0f, 0.0f);
			sliceFace = 3;
			break;
		
		case rotCoronal:
			m_AB.qDrag = Quaternion(0.0f, 90.0f, 0.0f) * Quaternion(0.0f, 0.0f, 90.0f);
			sliceFace = 4;
			break;
	}

	m_AB.qRotSess = qid;

	updateModelView();

	Slice(sliceFace, distFactor - m_model[sliceFace].plane.D);
}


//----------------------------------------------------------------------
// ActionModelView
//
// Slice, rotate one of the cube model faces or move across the screen,
// rotate the cube model in the view coordinates 
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
void Model::ActionModelView(long x, long y, long temporalMode, long action)
{
	long dx = x - m_AB.viewCenter.x;
	long dy = y - m_AB.viewCenter.y;

	switch(temporalMode)
	{
	
		case BEGIN:

			m_actionMode = action;

			switch (m_actionMode)
			{
				case PAN:
					 panInit(x, y);
					 break;

				case ROTATE:
					rotateInit(dx, dy);
					break;

				case SLICE:
 				    sliceInit(dx, dy, GetFaceID(x,y) );
					break;

				case SLICE_ROTATE :
					sliceRotateInit(dx, dy, GetFaceID(x,y));
					break;

				default:
					break;
			}
			break;


		case CONTINUE:

			switch (m_actionMode)
			{

				case PAN:
					 pan(x, y);
					 break;

				case ROTATE:
					rotate(dx, dy);
					break;

				case SLICE:
 				    slice(dx, dy);
					break;

				case SLICE_ROTATE :
					sliceRotate(dx, dy);
					break;

				default:
					break;
			}
			break;


		case END:

			if (m_creatingFace && !m_actionDone)
			{
				DeleteFace(m_activeFace);
				m_creatingFace = false;
			}

			if (m_actionDone)
			{
				if (m_actionMode == SLICE || m_actionMode == SLICE_ROTATE)
				{
					centroidCorrect();
				}

				m_actionDone = false;
			}

			m_activeFace = -1;
			m_actionMode = DOING_NOTHING;

			m_creatingFace = false;
			break;
	}
}


//----------------------------------------------------------------------
// GetView
//
// Return current view information
//
// Input:
// none
//
// Output:
// ViewORTHO structure - gives all the information of the current state
//----------------------------------------------------------------------
const viewORTHO& Model::GetView()
{
	return m_view;
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
long Model::GetEdgeFace(long x, long y)
{
	long activeFace =  GetFaceID(x, y);
		
	if (activeFace == -1 || ptInNonEdgePoly(m_view.face[activeFace].vert, x, y))
	{
		return -1;
	}
	else 
	{
	   return activeFace;
	}
}


//----------------------------------------------------------------------
// AlignFaceToScrn
//
// Rotate the cube model to align the face with given id parallel to
// the screen
//
// Input:
// id of the face to align
//
// Output:
// none
//----------------------------------------------------------------------
void Model::AlignFaceToScrn(long faceID)
{
	AlignFaceToScrn(m_view.face[faceID].plane.normal);
}


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
void Model::AlignFaceToScrn(Vector3 norm)
{
	Vector3 Scrn(0.0,0.0, -1.0);

	Vector3 axis = !(norm * Scrn);

	float cosTheta = norm % Scrn;

	cosTheta = sqrtf( (1.0 + cosTheta) / 2.0f);//rounding error
	float sinTheta = sqrtf(1.0 - cosTheta * cosTheta);

	m_AB.qDrag.s = cosTheta;
	m_AB.qDrag.V = sinTheta * axis;
	m_AB.qRotSess = m_AB.qDrag * m_AB.qRotSess;

	updateModelView();

}



//----------------------------------------------------------------------
// AlignFaceToScrn
//
// Rotate the cube model to align the face perpendicular with given axis
// and then align the face parallel with the screen
//
// Input:
// first point of the axis
// second point of the axis
//
// Output:
// none
//----------------------------------------------------------------------
void Model::AlignFaceToScrn(Vector3 firstpoint, Vector3 secondpoint)
{
	Vector3 normal = !(firstpoint - secondpoint);

	// first point
	Plane plane(normal, 0);
	long faceID = newFace(plane, m_model);

	DeleteFace(6);
	AlignFaceToScrn(6);

	if(!m_pointRotationMode)
	{
		SetPivotPoint(firstpoint);	
		PivotRotationToggle();
		
		sliceRotateInit(0, 0, 6);
		sliceRotate(0, 1);
		sliceRotate(0, 0);
		
		PivotRotationToggle();
		SetPivotPoint(Vector3(0,0,0));	
	}
	else
	{
		SetPivotPoint(firstpoint);	
		
		sliceRotateInit(0, 0, 6);
		sliceRotate(0, 1);
		sliceRotate(0, 0);
		
		PivotRotationToggle();
		SetPivotPoint(Vector3(0,0,0));	
	}	

	AlignFaceToScrn(6);
}

//Align plane to normal
void Model::AlignPlaneToNormal(Vector3 newNormal)
{
    Vector3 planeNormal = GetNormal(6);
  
	Vector3 axis = !(newNormal * planeNormal);

	float cosTheta = newNormal % planeNormal;

	cosTheta = sqrtf( (1.0 + cosTheta) / 2.0f);//rounding error
	float sinTheta = sqrtf(1.0 - cosTheta * cosTheta);

	m_AB.qDrag.s = cosTheta;
	m_AB.qDrag.V = sinTheta * axis;
	m_AB.qRotSess = m_AB.qDrag * m_AB.qRotSess;

	updateModelView();
}



//Takes three points and aligns face to screen
void Model::AlignFaceToScrn(Vector3 firstpoint, Vector3 secondpoint, Vector3 thirdpoint)
{


	Vector3 planenormal = !((thirdpoint - firstpoint) * (secondpoint - firstpoint));
	float D = planenormal % firstpoint;

	Plane plane(planenormal, D);
	long faceID = newFace(plane, m_model);

	if (faceID > -1)
	{
		DeleteFace(6);
        AlignFaceToScrn(6);
	}
}

//----------------------------------------------------------------------
// SetCenterAB
//
// Initialize Arcball centre (see Arcball in Graphics gems 4)
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none
//----------------------------------------------------------------------
void Model::SetCenterAB(long x, long y)
{
	m_AB.viewCenter = Vector3(x,y,0);
	m_shiftAbs = m_AB.viewCenter;

	updateModelViewTranslate(0,0);
}


//----------------------------------------------------------------------
// SetScale
//
// Resize the cube model in the view coordinates
//
// Input:
// width of a new window size
// height of a new window size
//
// Output:
// none
//----------------------------------------------------------------------
void Model::SetScale(long x, long y)
{
	m_scaleRel =  0.95 * ((x < y) ? x : y) / m_diag;
	
	m_AB.scaleFactor = m_scaleRel * m_scaleAbs;

	m_AB.radius = (sqrtf(x * x + y * y)) / 2.0 + 1.0;

	m_S    = Matrix4x4(m_AB.scaleFactor);
	m_Sinv = Matrix4x4(1.0 / m_AB.scaleFactor);

	m_model2View = m_model2ViewRot * m_S * m_T;
	m_view2Model = m_Tinv * m_Sinv * m_view2ModelRot;

	transformView(m_view, m_model);
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
void Model::Zoom(float s)
{
	float scale = s * m_scaleAbs;

	if (scale > 40 || scale < 0.025)
	{
		return;
	}

	m_scaleAbs *= s;

	m_AB.scaleFactor *= s;

	m_S    = Matrix4x4(m_AB.scaleFactor);
	m_Sinv = Matrix4x4(1.0 / m_AB.scaleFactor);

	m_model2View = m_model2ViewRot * m_S * m_T;
	m_view2Model = m_Tinv * m_Sinv * m_view2ModelRot;

	transformView(m_view, m_model);
}


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
void Model::DeleteFace(long dFace)
{
	if (dFace >= ORIGINAL_FACES_NUM && dFace < m_model.size())
	{
		m_model.erase(m_model.begin() + dFace);

		computeVertices(m_model);
		transformView(m_view, m_model);

		centroidCorrect();
	}
}


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
Vector3 Model::GetNormal(long faceID)
{
	Vector3 norm(0,0,0);

	if (faceID >= 0 && faceID < m_model.size())
	{
		norm = m_model[faceID].plane.normal;
	}
	
	return norm;  		
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
float Model::GetDmm(long faceID)
{
	float D = 0.0;
	
	if (faceID >= 0 && faceID < m_model.size())
	{
		D = m_model[faceID].plane.D;
	}
	
	return D;  		
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
void Model::SetPivotPoint(Vector3 point)
{
	m_pivotPoint = point;
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
long Model::GetModelSize()
{
	long size =	m_model.size();

	return size;
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
void Model::SetConstrainAxis(Vector3 axis)
{

	m_constrainAxis = !axis;

	m_modifiedConstrainAxis = m_constrainAxis % m_model2ViewRot;
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
void Model::RotateCubeOnAxis(short axis)
{

	m_constrain = true;

	switch (axis)
	{
		case 1:
			rotateInit(0, 0);
			rotate(0, 100);
			break;	

		case 2:
			rotateInit(0, 0);
			rotate(100, 0);
			break;

		case 3:
			rotateInit(100, 0);
			rotate(100, 100);
			break;

		default:
			break;
	}
	
	m_constrain = false;

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
void Model::RotateFaceOnAxis(short axis)
{
	m_constrain = true;

	switch (axis)
	{
		case 1:
			sliceRotateInit(0, 0, 6);
			sliceRotate(0, 100);
			break;	

		case 2:
			sliceRotateInit(0, 0, 6);
			sliceRotate(100, 0);
			break;

		default:
			break;
	}
	
	m_constrain = false;
}


//----------------------------------------------------------------------
// panInit
//
// Prepare to move the cube model across the screen
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none
//----------------------------------------------------------------------
void Model::panInit(long x, long y)
{
	m_AB.vDown.x = (float)x;
	m_AB.vDown.y = (float)y;
}


//----------------------------------------------------------------------
// pan
//
// Move the cube model in the view coordinates
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none
//----------------------------------------------------------------------
void Model::pan(long x, long y)
{
	float delX = (float)x - m_AB.vDown.x;
	float delY = (float)y - m_AB.vDown.y;

	updateModelViewTranslate(delX, delY);

	m_AB.vDown.x = (float)x;
	m_AB.vDown.y = (float)y;
}


//----------------------------------------------------------------------
// sliceRotateInit
//
// Prepare to the rotation of the active cube model face
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
// id of the active face in the cube model
//
// Output:
// none
//----------------------------------------------------------------------
void Model::sliceRotateInit(long x, long y, long activeFace)
{	
	mouseOnSphere(x, y, &m_AB.vFrom);

	m_actionDone = true;

	m_AB.qRotSess = qid;

	if (activeFace < ORIGINAL_FACES_NUM)
	{
		m_activeFace = newFace(activeFace, m_model); // generates new internal number 
		transformView(m_view, m_model);
		m_creatingFace = true;
	}
	else
	{
		m_activeFace = activeFace;
	}
}


//----------------------------------------------------------------------
// sliceInit
//
// Prepare to move the active cube model face 
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
// id of the active face in the cube model
//
// Output:
// none
//----------------------------------------------------------------------
void Model::sliceInit(long x, long y, long activeFace)
{	
	m_actionDone = true;

	m_AB.vDown.x = (float)x;
	m_AB.vDown.y = (float)y;

	if (activeFace < 6)
	{
		m_activeFace = newFace(activeFace, m_model); // generates new internal number 
		transformView(m_view, m_model);
		m_creatingFace = true;
	}
	else
	{
		m_activeFace = activeFace;
	}
}


//----------------------------------------------------------------------
// rotateInit
//
// Prepare to the rotation of the  cube model 
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none
//----------------------------------------------------------------------
void Model::rotateInit(long x, long y)
{
	m_actionDone = true;

	mouseOnSphere(x, y, &m_AB.vFrom);
}


//----------------------------------------------------------------------
// sliceRotate
//
// Rotate the active cube model face around the cube centroid
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none
//----------------------------------------------------------------------
void Model::sliceRotate(long x, long y)
{
	Plane MostRecentPlaneLoc = m_model[m_activeFace].plane; 

	mouseOnSphere(x, y, &m_AB.vTo);
	abRot();

	Matrix4x4 Rot(m_AB.qDrag.s, m_AB.qDrag.V);


	m_model[m_activeFace].plane.normal = (m_view.face[m_activeFace].plane.normal % Rot) % m_view2ModelRot;
	
	// calculate centroid
	Vector3 centroid(0,0,0);
	for (Vector3Vec::iterator i = m_model[m_activeFace].vert.begin(); i != m_model[m_activeFace].vert.end(); ++i)
	{
		centroid += *i;
	}

	centroid /= m_model[m_activeFace].vert.size();

	if(m_pointRotationMode)
	{
		centroid.x = m_pivotPoint.x; 	
		centroid.y = m_pivotPoint.y; 	
		centroid.z = m_pivotPoint.z;
	}

	m_model[m_activeFace].plane.D =  m_model[m_activeFace].plane.normal % centroid;
	
	computeVertices(m_model);
	
	// if the plane would nearly disappear, undo all changes
	if (m_model[m_activeFace].vert.size() < 3 || maxEdgeLength(m_activeFace) < m_diag / 50.0f )
	{
		m_model[m_activeFace].plane = MostRecentPlaneLoc;

		computeVertices(m_model);

		return;
	}

	m_creatingFace = false;

	m_actionDone = true;

	transformView(m_view, m_model);
}


//----------------------------------------------------------------------
// slice
//
// Move the active cube model face
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none 
//----------------------------------------------------------------------
void Model::slice(long x, long y)
{
	long delX = x - (long)m_AB.vDown.x;
	long delY = y - (long)m_AB.vDown.y;

	long tryCount = 3;

	float length = sqrtf(delX * delX + delY * delY);

	long sign = geom::sgn(m_view.face[m_activeFace].plane.normal.x * delX + m_view.face[m_activeFace].plane.normal.y * delY);

	float disp = sign * length / m_AB.scaleFactor;

	
	Plane savedFace = m_model[m_activeFace].plane;	

	while (tryCount)
	{
		m_model[m_activeFace].plane.D += disp;

		computeVertices(m_model);

		// if the change would make the slice plane nearly disappear, undo it
		if (m_model[m_activeFace].vert.size() < 3 || maxEdgeLength(m_activeFace) < m_diag / 50.0f)
		{
		
			m_model[m_activeFace].plane = savedFace;
		
			computeVertices(m_model);

			transformView(m_view, m_model);

			if (!tryCount)
			{
				return;
			}

			tryCount--;

			disp /= 1.5;

		}
		else
		{
			break;
		}

	}

	m_actionDone = true;
	m_creatingFace = false;

	transformView(m_view, m_model);

	m_AB.vDown.x = (float)x;
	m_AB.vDown.y = (float)y;
}


//----------------------------------------------------------------------
// rotate
//
// Rotate the cube model in the view coordinates
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none
//----------------------------------------------------------------------
void Model::rotate(long x, long y)
{
	mouseOnSphere(x, y, &m_AB.vTo);
	abRot();

	updateModelView();	
}


//----------------------------------------------------------------------
// updateModelViewTranslate
//
// Update the cube model in the view coordinates
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// none 
//----------------------------------------------------------------------
void Model::updateModelViewTranslate(float x, float y)
{
	m_shiftRel.x += x;
	m_shiftRel.y += y;

	m_T = Matrix4x4(m_shiftAbs + m_shiftRel);
	m_Tinv = Matrix4x4(-1 * (m_shiftAbs + m_shiftRel));

	m_model2View = m_model2ViewRot * m_S * m_T;
	m_view2Model = m_Tinv * m_Sinv * m_view2ModelRot;

	transformView(m_view, m_model);
}


//----------------------------------------------------------------------
// updateModelView
//
// Update the cube model in the view coordinates
//
// Input:
// none
//
// Output:
// none 
//----------------------------------------------------------------------
void Model::updateModelView()
{
	Matrix4x4 rot(m_AB.qDrag.s, m_AB.qDrag.V);

	m_model2ViewRot = m_model2ViewRot * m_TcenterInv * rot * m_Tcenter;
	m_model2View = m_model2ViewRot * m_S * m_T;

	m_view2ModelRot = m_TcenterInv * ~rot * m_Tcenter * m_view2ModelRot;
	m_view2Model = m_Tinv * m_Sinv * m_view2ModelRot;

	m_modifiedConstrainAxis = m_constrainAxis % m_model2ViewRot;

	transformView(m_view, m_model);
}


//http://snippets.dzone.com/posts/show/4309
//pure quaternion rotation about a normal vector
void Model::PureRotate(Vector3 axis, float angle)
{

    float length = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    float omega = -0.5 * (angle*PI/180.0f);
    float s = sin(omega)/length;


    m_AB.qDrag.V.x = s * axis.x;
    m_AB.qDrag.V.y = s * axis.y;
    m_AB.qDrag.V.z = s * axis.z;
    m_AB.qDrag.s = cos(omega); 

    m_AB.qRotSess = m_AB.qDrag * m_AB.qRotSess;;//m_AB.qRotSess = qid;

    updateModelView();	

}


//----------------------------------------------------------------------
// abRot
//
// Calculate arcball rotation quaternion (see Arcball in Graphics gems 4)
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void Model::abRot()
{		
	if(!m_constrain)
	{
		m_AB.qDrag = Quaternion(m_AB.vFrom %  m_AB.vTo, m_AB.vFrom * m_AB.vTo);
	}
	else
	{
		Vector3 from = constrainToAxis(m_AB.vFrom);
		Vector3 to = constrainToAxis(m_AB.vTo);
		Vector3 v = from * to;

		m_AB.qDrag = Quaternion(cos(m_rotationAngle / 180.0 * PI), v / sin(acos(from % to)) * sin(m_rotationAngle / 180.0 * PI));
	}

	m_AB.qRotSess = m_AB.qDrag * m_AB.qRotSess;
	m_AB.vFrom = m_AB.vTo;
}


//----------------------------------------------------------------------
// mouseOnSphere
//
// Convert window coordinates to unit-sphere coordinates 
// (see Arcball in Graphics gems 4)
//
// Input:
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// Vector3 res structure contains the point in the unit-sphere coordinates
//---------------------------------------------------------------------- 
void Model::mouseOnSphere(long x, long y, Vector3 *res)
{
	if (m_actionMode == ROTATE)
	{
		float scale = 1.0f / m_AB.radius;

		*res = Vector3(scale * x, scale * y, 0);

		double zsq = 1.0 - (res->x * res->x) - (res->y * res->y);
		
		if (zsq <= 0)
		{
			zsq = 0;
		}

		res->z = -sqrtf(zsq);
	}
	else
	{
		float scale = 2.0 * m_AB.radius/ (x * x + y * y + m_AB.radius * m_AB.radius);

		*res = Vector3(x * scale, y * scale, 1.0 - scale * m_AB.radius);
	}
}


//----------------------------------------------------------------------
// maxEdgeLength
//
// Find the longest edge in a given face of the cube model
//
// Input:
// id of a face in the cube model
//
// Output:
// lenth of the longest edge in mm
//----------------------------------------------------------------------
float Model::maxEdgeLength(long face)
{
	float maxEdgeLen = 0;

	int start = m_model[face].vert.size() - 1;

	if (start < 0 || start >= 20)
	{
		return (0.0);
	}

	for (int end = 0; end < m_model[face].vert.size(); end++)
	{
		float edgeLen = ~(m_model[face].vert[end] - m_model[face].vert[start]);
	
		if (edgeLen > maxEdgeLen) 
		{
			maxEdgeLen = edgeLen;
		}

		start = end;
	}

	return maxEdgeLen;
}


//----------------------------------------------------------------------
// newFace
//
// Add a new face to the cube model which is simillar to some other face
//
// Input:
// id of the face to replicate
// FaceVec model is the input model 
//
// Output:
// FaceVec model is the input model with extra face in it
//----------------------------------------------------------------------
long Model::newFace(long coincidentFace, FaceVec &model)
{
	assert( coincidentFace < model.size()  && coincidentFace>= 0); 

	long newFaceID = model.size();
	model.push_back(model[coincidentFace]);

	computeVertices(model);

	return newFaceID;
}


//----------------------------------------------------------------------
// newFace
//
// Create a new face in a cube model given a plane 
//
// Input:
// Plane plane structure contains the plane parameters
// FaceVec model is the input model 
//
// Output:
// FaceVec model is the input model with extra face in it
//----------------------------------------------------------------------
long Model::newFace(Plane plane, FaceVec &model)
{
	long newFaceID = model.size();

	Face face(plane);
	model.push_back(face);

	computeVertices(model);

	return newFaceID; 
}


//----------------------------------------------------------------------
// transformView
//
// Update the cube model in the view coordinates
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void Model::transformView(viewORTHO &view, FaceVec &model, Matrix4x4 M2V)
{	
	FaceVec::iterator currFace, viewFace;

	view.face = model;

	// transform face planes
	for (currFace=model.begin(), viewFace=view.face.begin(); currFace != model.end(); currFace++, viewFace++)
	{
		viewFace->plane.normal = !(currFace->plane.normal % M2V);
		viewFace->plane.D = viewFace->plane.normal % (( currFace->plane.D * currFace->plane.normal) * M2V);
 	}

	// transform face vertices
	for (currFace=model.begin(), viewFace=view.face.begin(); currFace != model.end(); currFace++, viewFace++)
	{
		for (long i=0; i < currFace->vert.size(); i++)
		{
			viewFace->vert[i] = currFace->vert[i] * M2V;
		}
	}
}


//----------------------------------------------------------------------
// transformView
//
// Update the cube model in the view coordinates 
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void Model::transformView(viewORTHO &view, FaceVec &model)
{	
	transformView(view, model, m_model2View);

	view.Model2View = m_model2View;
	view.View2Model = m_view2Model;
}


//----------------------------------------------------------------------
// computeVertices
//
// Generate the vertices of all cube model faces
//
// Input:
// FaceVec model contains an input cube model
//
// Output:
// none
//----------------------------------------------------------------------
void Model::computeVertices(FaceVec &model)
{
	for (long i = 0; i < ORIGINAL_FACES_NUM; ++i)
	{
		model[i].vert = Vector3Vec(4);
	
		for (long j = 0; j < 4; ++j)
		{
			model[i].vert[j] = m_modelVerts[CVID[i][j]];
		}
	}

	Vector3Vec intersections;

	// assemble the current model cut state by clipping all slice planes
	// sequentially with current model clip state.
	for(long iFace = ORIGINAL_FACES_NUM; iFace < model.size(); iFace++) 
	{											
		model[iFace].vert.erase( model[iFace].vert.begin(), model[iFace].vert.end());
		intersections.erase( intersections.begin(),  intersections.end() );

		for (long doneFace = 0; doneFace < iFace; doneFace++)
		{
			clipFace(&model[doneFace], &model[iFace], intersections);
		}
		
		geom::SortVertices(intersections, &model[iFace]);
	}
}


//----------------------------------------------------------------------
// clipFace
//
// Clip a cube model face aginst another face in the model
//
// Input:
// Face face is current face to clip (up to next face clip) 
// Face clip is the next face in the model to clip against
//
// Output:
// Vector3Vec intersections contains intersection vertices as result of
// clipping
//----------------------------------------------------------------------
void Model::clipFace(Face* face, Face* clip, Vector3Vec& intersections)
{
	Vector3 i;
	Vector3Vec clipped;	

	if (face->vert.empty())
	{
		return;	
	}
	
	Vector3* s = &face->vert.back(); 

	for (long p = 0; p < face->vert.size(); p++)
	{
		Vector3* point = &face->vert[p];

		if (geom::inside(point, &clip->plane)) // cases 1 and 4 in Foley/Van Dam
		{									
			if (geom::inside(s, &clip->plane)) // case 1: output p
			{
				clipped.push_back(*point);
			}
			else
			{
				// case 4: output intersection of line seg s--p, then p
				geom::intersect(s, point, &clip->plane, &i);
				clipped.push_back(i);
				clipped.push_back(*point);
				intersections.push_back(i);
			}
		}
		else 
		{	// cases 2 and 3
			if (geom::inside(s, &clip->plane))
			{
				// case 2: output intersection of line seg s--p only
				geom::intersect(s, point, &clip->plane, &i);
				clipped.push_back(i);
				intersections.push_back(i);
			}
			// else case 3: no action
		}

		s = point;	// advance to next pair of vertices
	}

	// done: copy clipped set of vertices to face
	face->vert = clipped;
}


//----------------------------------------------------------------------
// centroidCorrect
//
// Recalculate the centre of the cube model
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void Model::centroidCorrect()
{
	Vector3Vec vertList;

	bool include = true;

	long i;

	for (i = 0; i < m_model.size(); ++i)
	{
		long n = m_model[i].vert.size();

		if (n < 3)
		{
			continue;
		}

		for (long j = 0; j < n; ++j)
		{
			Vector3 mv = m_model[i].vert[j];

			for (long k = 0; k < vertList.size(); ++k)
			{
				if (mv.x == vertList[k].x && mv.y == vertList[k].y && mv.z == vertList[k].z)
				{
					include = false;
				}
			}

			if (include)
			{
				vertList.push_back(m_model[i].vert[j]);
			}
			else
			{
				include = true;
			}

		}
	}

	Vector3 centroid(0,0,0);

	for (i = 0; i < vertList.size(); ++i)
	{
		centroid += vertList[i];
	}

	centroid /= vertList.size();

	Vector3 rotatedCentroid = centroid * m_model2ViewRot;

	m_Tcenter = Matrix4x4(rotatedCentroid);
	m_TcenterInv = Matrix4x4(-1 * rotatedCentroid);
}


//----------------------------------------------------------------------
// constrainToAxis
//
// Constrain rotation of the cube model face around the given axis
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
Vector3 Model::constrainToAxis(Vector3 loose)
{
	Vector3 onPlane;
	float norm;

	onPlane = loose - (m_modifiedConstrainAxis * (m_modifiedConstrainAxis % loose));
	norm = onPlane % onPlane;

	if(norm > 0.0)
	{
		if(onPlane.z < 0.0)
		{
			onPlane = Vector3(0.0, 0.0, 0.0) - onPlane;
			onPlane /= sqrtf(norm);
		}
		return onPlane;
	}

	if(m_modifiedConstrainAxis.z == 1)
	{
		onPlane = Vector3(1.0, 0.0, 0.0);
	}
	else
	{
		onPlane = !Vector3(-m_modifiedConstrainAxis.y, m_modifiedConstrainAxis.x, 0.0);	
	}

	return onPlane;
}


//----------------------------------------------------------------------
// ptInNonEdgePoly
//
// Given a point in the screen coordinates check whether this point is
// inside of the face
//
// Input:
// Vector3Vec vertex contains a cube face vertices
// x screen coordinate of the point
// y screen coordinate of the point
//
// Output:
// true if the point is inside of the face, otherwise false
//----------------------------------------------------------------------
bool Model::ptInNonEdgePoly(const Vector3Vec& vertex, long ptX,  long ptY)
{
	// find center of plane in order to create reduced tranlate-region
	float Xmax = 0, Ymax = 0, Xmin = 2000, Ymin = 2000;

	Vector3Vec::const_iterator iVec;

	// determine the largest and smallest (x,y) values
	for(iVec = vertex.begin(); iVec != vertex.end(); iVec++)
	{
		if( iVec->x > Xmax)
		{
			Xmax =  iVec->x;
		}

		if( iVec->x < Xmin)
		{
			Xmin =  iVec->x;
		}

		if( iVec->y > Ymax)
		{
			Ymax =  iVec->y;
		}

		if( iVec->y < Ymin)
		{
			Ymin =  iVec->y;
		}
	}

	long tooSmall =  10;

	// ie force the pt to be in non-edge region (thus translate),
	// if too small to orient
	if( (Xmax - Xmin) < tooSmall  ||  (Ymax - Ymin) < tooSmall )
	{
		return true;
	}

	float iEF = 0.15f;
	float EF =  0.85f;

	Vector3 C(0,0,0);

	for (iVec = vertex.begin(); iVec != vertex.end(); iVec++)
	{
		C += *iVec;
	}

	C *= iEF / vertex.size();

	// the pt C is the center of mass of the polygon in which the pt has been found.
	// use this to scale polygon vertexes toward the center
	if (((EF*vertex.back().x  -ptX + C.x) * (EF*vertex.front().y - ptY + C.y)  - 
		 (EF*vertex.back().y - ptY + C.y) * (EF*vertex.front().x - ptX + C.x )) < 0 )     
	{
		return false;
	}

	for (iVec = vertex.begin(); iVec != vertex.end() - 1; iVec++)
	{
		if (((EF*iVec->x - ptX + C.x) * (EF*(iVec+1)->y - ptY +  C.y) - 
			 (EF*iVec->y - ptY + C.y) * (EF*(iVec+1)->x - ptX  + C.x)) <  0)
		{
			return false;
		}
	}
	
	return true;
}

//***LG added
float Model::GetZoomFactor(void)
{

	return m_AB.scaleFactor;

}

void Model::SetZoomFactor(float factor)
{

    m_AB.scaleFactor = factor;
}

void Model::RotateFaceOnAxis(short axis, long activeFace )
{
	m_constrain = true;
	switch (axis)
	{
		case 1:
			sliceRotateInit(0, 0, activeFace);
			sliceRotate(0, 100);
			break;	

		case 2:
			sliceRotateInit(0, 0, activeFace);
            sliceRotate(100, 0);
			break;

		default:
			break;
	}	
	m_constrain = false;
}



