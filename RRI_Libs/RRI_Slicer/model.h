#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include "geometry.h"
#include "VisLib.h"


const long ORIGINAL_FACES_NUM = 6;
const float FUDGE =  0.991f;

/*
public ref class mvState
{
public:
	mvState();
	virtual ~mvState();
};
*/

const long CVID[6][4] = 
{	
	{tlf, trf, brf, blf},
	{trb, tlb, blb, brb},
	{tlb, tlf, blf, blb},
	{trf, trb, brb, brf},
	{tlb, trb, trf, tlf},
	{brf, brb, blb, blf}
};

// Adjacent vertex id's
struct AdjVert
{
	unsigned short x, y, z;
};

// for each vertex {tlf, trf, blf, brf, tlb, trb, blb, brb}, this table defines the 
// adjacent vertices along X, Y and Z axes.
const AdjVert orientID[8] = 
{
//	  X	   Y    Z
	{trf, blf, tlb}, // for tlf
	{tlf, brf, trb}, // for trf
	{brf, tlf, blb}, // for blf
	{blf, trf, brb}, // for brf
	{trb, blb, tlf}, // for tlb
	{tlb, brb, trf}, // for trb
	{brb, tlb, blf}, // for blb
	{blb, trb, brf}  // for brb
};




// arcball 
struct tBallData
{
    Vector3 viewCenter;
    float radius;
	float scaleFactor;

    Quaternion qDrag;
	Quaternion qRotSess;

	Vector3 vFrom, vTo;
	Vector3 vDown, vNow;
};



class Model  
{

public:

		Model(TmodelInit modelInit);
		virtual ~Model();

		void ActionModelView(long x, long y, long temporalMode, long actionMode);
		long Slice(long faceID, float distMM);
		void Zoom(float s);
        float GetZoomFactor();
        void SetZoomFactor(float factor);;

		void SetCenterAB(long x, long y);
		void SetScale(long x, long y);

		const viewORTHO& GetView();
		const viewORTHO& GetView(Vector3 normal, float distFactor, long alignMode, float rotation);
	
		void SetView(Vector3 normal, float distFactor, long alignMode, eCubeRotation cubeRot);	
		void SetDefaultModelView(long clientX, long clientY, eCubeRotation cubeRot = rotCurrent);
        void SetModelView(long clientX, long clientY, long face, float rotX, float rotY, float rotZ, bool resetZoom = true);//***NEW

		long GetEdgeFace(long x, long y);
		long GetActiveEdgeFace();
		
		long GetFaceID(long x, long y);
		void DeleteFace(long faceID);

		void AlignFaceToScrn(long faceID);
		void AlignFaceToScrn(Vector3 norm);
		void AlignFaceToScrn(Vector3 firstpoint, Vector3 secondpoint);
        void AlignFaceToScrn(Vector3 firstpoint, Vector3 secondpoint, Vector3 thirdPoint);
        void AlignPlaneToNormal(Vector3 newNormal);//***new
		
		bool ScrnPtToViewPt(long x, long y, Vector3* vec);
		bool ScrnPtToModelPt(long x, long y, Vector3* vec);
		bool ModelPtToViewPt(Vector3 modelpt, Vector3* viewpt);

        //***NEW LG===================================================
        bool ScrnPtToViewPt(long face, long x, long y, Vector3* vec);
        bool ScreenToModel(long face, long x, long y, Vector3* vec);
        //============================================================
	
		void GetStateModelView(mvState *state);
		void SetStateModelView(mvState *state, long clientX, long clientY, bool scaleAbs = true);
		
		Vector3 GetNormal(long faceID);
		float GetDmm(long faceID);

		void SetPivotPoint(Vector3 point);
        void PivotRotationToggle(bool pivot){m_pointRotationMode = pivot;}
		void PivotRotationToggle(){m_pointRotationMode = !m_pointRotationMode;}
		bool GetRotationToggleState(){return m_pointRotationMode;}

		void SetConstrainAxis(Vector3 axis);
		void SetRotationAngle(float angle){m_rotationAngle = angle/2.0f;}


		void RotateCubeOnAxis(short axis);
		void RotateFaceOnAxis(short axis);
        void RotateFaceOnAxis(short axis, long activeFace );//***LG
	
		long GetModelSize();

		OrientMarker GetOrientMarker(long origin, long label); 
		OrientMarker m_orentationMarker;

        //***TEMP
        void PureRotate(Vector3 axis, float angle);

        float GetScaleAbs(){return m_scaleAbs;}
        void SetScaleAbs(float scale){m_scaleAbs = scale;}

        float GetScaleRel(){return m_scaleRel;}
        void SetScaleRel(float scale){m_scaleRel = scale;}
	

protected:

		Vector3 m_modelVerts[8];	

		FaceVec m_model;	
		FaceVec m_originalModel;

		viewORTHO m_view;

		float m_diag;

		long m_activeFace;
		long m_actionMode;

		bool m_creatingFace;
		bool m_actionDone;

		tBallData m_AB;

		float m_scaleAbs;
		float m_scaleRel;

		Vector3 m_shiftAbs;
		Vector3 m_shiftRel;

		Matrix4x4 m_model2View, m_view2Model;
		Matrix4x4 m_model2ViewRot, m_view2ModelRot;

		Matrix4x4 m_S, m_Sinv;
		Matrix4x4 m_T, m_Tinv;

		Matrix4x4 m_Tcenter, m_TcenterInv;

		Vector3 m_pivotPoint;
		bool m_pointRotationMode;

		bool m_constrain;
		Vector3 m_constrainAxis;
		Vector3 m_modifiedConstrainAxis;
		float m_rotationAngle;


protected:

		long newFace(Plane plane, FaceVec &model);
		long newFace(long coincidentFace, FaceVec &model);

		float maxEdgeLength(long face);

		void centroidCorrect();

		void mouseOnSphere(long x, long y, Vector3 *res);
		void abRot();

		Vector3 constrainToAxis(Vector3 loose);
	
		void sliceRotate(long x, long y);
		void sliceRotateInit(long x, long y, long activeFace);
		void slice(long x, long y);
		void sliceInit(long x, long y, long activeFace);
		void rotate(long x, long y);
		void rotateInit(long x, long y);
		void pan(long x, long y);
		void panInit(long x, long y);
		
		void clipFace(Face* face, Face* clip, Vector3Vec& intersections);				
		void computeVertices(FaceVec &model);
		
		void transformView(viewORTHO &view, FaceVec &model);
		void transformView(viewORTHO &view, FaceVec &model, Matrix4x4 M2V);

		void updateModelView();
		void updateModelViewTranslate(float x, float y);

		static bool ptInNonEdgePoly(const Vector3Vec& vertex, long ptX,  long ptY);
};

#endif // #ifndef MODEL_H_INCLUDED
