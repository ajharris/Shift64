// Measure.h: interface for the CMeasure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(MEASURE_H_INCLUDED)
#define MEASURE_H_INCLUDED



const enum {
	MPOINT		= 1, 
	MLINE		= 2,
	MANGLE		= 3,
	MAREA		= 4,
	MVOLUME		= 5,
	MRULER      = 6,
	MRADIAL     = 7,
	MORIENT		= 8,
	MTEXT		= 9,
	ML1         = 11,
	ML2         = 12,
	ML3         = 13,
	ML4         = 14
};

// Measurement subtypes
const enum {
	MSUB_UNDEFINED		= 0xFFFF, 
	MSUB_ORIENTLINE		= 1,
	MSUB_ORIENTTEXT		= 2,
	MSUB_ORIENT_X_LABEL	= 3,
	MSUB_ORIENT_Y_LABEL	= 4,
	MSUB_ORIENT_Z_LABEL	= 5,
	MSUB_AUTOSEGMENT	= 6,
	MSUB_SEED			= 7,
	MSUB_NEEDLE			= 8,
	MSUB_TEMPLATE		= 9,
	MSUB_PATIENTPT      = 10,
	MSUB_MARKER			= 11,
	MSUB_TEMPAXIS       = 12,
	MSUB_ANCHOR         = 13
};

// Measurement attributes
const enum {
	MATTR_NONE			= 0,
	MATTR_SELECT		= 0x1,
	MATTR_INCOMPLETE	= 0x2,
	MATTR_DENSEPOINTS	= 0x4,
	MATTR_ACTIVATE      = 0x8,
	MATTR_NEW           = 0x10,
	MATTR_ACTUAL        = 0x100,
	MATTR_ALL			= 0xFFFF
};

const enum eMEditMode
{
	M_NON_MEASURE = -1,	//not in measure mode
	M_LINE = 100,		//draw line mode
	M_ANGLE,			//draw angle mode
	M_POLYGON,			//draw polygon mode
	M_ELLIPSE,			//draw ellipse mode
	M_ARROW,			//draw arrow mode
	M_TEXT,				//draw text mode
	M_COPY,				//copy measure object mode
	M_MOVE,				//copy measure object mode
	M_SCALEIN,			//scale in measure object mode
	M_SCALEOUT,			//sclae out measure object mode
	M_DELPOINT,			//delete a point mode
	M_ADDPOINT,			//add a point mode
	M_MOVEPOINT,		//move a point mode
	M_ANCHORPOINT,		//anchor point 
	M_DELETE,			//delete a measure object mode
	M_DELETEALL,		//delete all measure objects mode
	M_UNDO,				//undo previous measure activity mode.
	M_REFINE,
	M_UPDATEAUTOPLOT,
	M_GOTO_MEASUREMENT,
	M_DELETESINGLEPOINT,
	M_MOVESINGLEPOINT,
};


const long CUSTOM_ENTRIES = 4;
const long PARAM_ENTRIES = 6;

struct viewORTHO;
struct Vector3;
struct Triangle;



// base class for all measurement objects (64 bytes)
// The type and subtype should be converted to enumerated types
// MVOLUME1 is currently used to indicate incomplete volumes. This also be changed to 
// use one of the attrib flags
struct MObj  
{
	unsigned short type;	// Type specifies if its a point, line, circle, polygon, volume etc
	unsigned short id;		// each type has its own id number space.
	unsigned short subtype;	// Measurement subtype - allows 2^16 subtypes
	unsigned short attrib;	// attribute flags - allows 16 bit flags
	unsigned short resample;// resampling indicator
	Vector3 axis_first;		// axis of contours sampling
	Vector3 axis_second;
	char name[MAX_VIEW_NAME+1];
	union { float f; long i;} custom[CUSTOM_ENTRIES];
	float param[PARAM_ENTRIES];		// parameters about the measurement object

	// assignment operator
	MObj& operator= (const MObj &right)
	{
		type = right.type;
		id = right.id;
		subtype = right.subtype;
		attrib = right.attrib;
		resample = right.resample;
		axis_first.x = right.axis_first.x;
		axis_first.y = right.axis_first.y;
		axis_first.z = right.axis_first.z;
		axis_second.x = right.axis_second.x;
		axis_second.y = right.axis_second.y;
		axis_second.z = right.axis_second.z;
		memcpy(name, right.name, sizeof(name));
		memcpy(custom, right.custom, sizeof(custom));
		memcpy(param, right.param, sizeof(param));
		return *this;
	}

	// default constructor
	MObj()
	{
		type = id = subtype = attrib = resample = 0;
		axis_first.x = axis_first.y = axis_first.z = 0.0;
		axis_second.x = axis_second.y = axis_second.z = 0.0;
		memset(name, 0, sizeof(name));
		memset(custom, 0, sizeof(custom));
		memset(param, 0, sizeof(param));
	}

};

typedef STL::vector<MObj *> MObjpVec;

struct MPoint : public MObj
{
	mvState state;
};

struct MLine : public MObj
{
	Vector3 start, end;
	mvState state;
};

struct MAngle : public MObj
{
	Vector3 start, middle, end;
	mvState state;
};

struct MPoly2D : public MObj
{
	Vector3 cgravity;
	Vector3 planeNormal;
	float planeD;
	float area;
	Vector3 min_coord;
	Vector3 max_coord;
	Vector3Vec poly2d;
	mvState state;
};

typedef STL::vector<MPoly2D> MPoly2DVec;

struct MPoly3D : public MObj
{
	Vector3 min_coord;
	Vector3 max_coord;
	MPoly2DVec poly3d;
};


class SLICER_EXPORT CMeasure  
{

public:

	CMeasure();
	virtual ~CMeasure();

	long SetPoints(const viewORTHO& view, unsigned short measuremode, const Vector3Vec& poly, long face, unsigned short subtype = 0, unsigned short attrib = 0);
	long SetPoints(const viewORTHO& view, const mvState* state, unsigned short measuremode, const Vector3Vec& poly, long face, unsigned short subtype = 0, unsigned short attrib = 0);

	const PlotVec& GetPoints(const viewORTHO& view, long index = -1);
	const PlotVec& GetPoints(const viewORTHO& view, const TriangleVec *surface);

	const PlotVec& GetContourPoints(const viewORTHO& view);

	const MObjpVec& GetMeasureObjects();

    MObjpVec* GetMeasureList(){return &m_measureList;}

	bool PointInsideTarget(const viewORTHO& view1, long face1, Vector3 Point, short TargetID);

	void Undo();
	void Redo();

	// Clear operations cannot be undone
	void Clear();
	void Clear(unsigned short type, unsigned short subtype);

	void SetOrientation(OrientMarker m); 

/*
    //***LG-------------------------------------------------
    NeedleMarker m_needleMark;//***LG
    void ClearNeedleMark();
	void SetNeedle(Vector3 a, Vector3 b)
	{
		m_needleMark.start = a; m_needleMark.end = b;
	}
*/
	long GetSize(){return m_measureList.size();}
    //------------------------------------------------------


	// object selection
	bool Select(long index);
	bool Select(){return Select(m_measureList.size()-1);} // select the last one
	bool Select(unsigned short type, unsigned short id) {return Select(GetObject(type, id));}
	bool Select(Vector3 point, unsigned short type, unsigned short subtype, float delta= -1){return Select(GetObject(point, type, subtype, delta));}

	long GetSelect(){return m_curObj;}

	bool SetAttrib(long index, unsigned short attrib); // set attribute (uses OR operator)
	bool ClearAttrib(long index, unsigned short attrib=0xffff); // Clear the attribute ( uses AND with negated attribute)

	bool SetName(long index, const char *name);
	bool GetName(long index, char *name);

	// All these returns the index of the affected object. Returns -1 if not successfull
	// Get the index of the matching object
	long GetObject(unsigned short type, unsigned short id);
	long GetObject(Vector3 point, unsigned short type, unsigned short subtype, float delta = -1);

	// Delete the matching object. Can be undone/redone by calling mUndo and mRedo
	long Delete(long index);
	long Delete() {return Delete(m_measureList.size()-1);}; // Delete last one
	long Delete(unsigned short type, unsigned short id){return Delete(GetObject(type, id)); }
	long Delete(Vector3 point, unsigned short type, unsigned short subtype, float delta = -1){return Delete(GetObject(point, type, subtype, delta));}

	// object editing functions. They use the currently selected object (via mSelect) unless specified
	bool DeletePt(Vector3 Pt, Vector3 *Pt1, float delta = 5.0);
	bool AddPt(Vector3 pt, long insert, long section = 0);
	bool MovePt(Vector3 offset, long vertex, long section = 0);
	bool GetVertex(const Vector3 point, long *vertex, long *section, float delta = 5.0);
	bool GetEdge(const Vector3 point, long *vertex, long *section, float delta = 5.0);

	long CopyObject(const Vector3 offset, long index);
	long CopyObject(const Vector3 offset) {return CopyObject(offset, m_curObj);}

	bool MoveObject(const Vector3 offset, long index);
	bool MoveObject(const Vector3 offset) {return MoveObject(offset, m_curObj);}
	bool MoveObject(const Vector3 offset, unsigned short type, unsigned short subtype);

	// Scale wrt a given origin
	bool ScaleObject(Vector3 scale, Vector3 Origin, long index);
	bool ScaleObject(Vector3 scale, Vector3 Origin) {return ScaleObject(scale, Origin, m_curObj);}
	bool ScaleObject(Vector3 scale, Vector3 Origin, unsigned short type, unsigned short subtype);

	// scale wrt center of gravity
	bool ScaleObject(Vector3 scale, long index);
	bool ScaleObject(Vector3 scale) {return ScaleObject(scale, m_curObj);}
	bool ScaleObject(Vector3 scale, unsigned short type, unsigned short subtype);

	bool RotateObject(double sintheta, double costheta, Vector3 Origin, long index);
	bool RotateObject(double sintheta, double costheta, Vector3 Origin) {return RotateObject(sintheta, costheta, Origin, m_curObj);}
	bool RotateObject(double sintheta, double costheta, Vector3 Origin, unsigned short type, unsigned short subtype);

	// Volume contour editing functions
	bool DeletePolygon(long obj_no, long contour_no);
	bool UndoLastDeletePolygon();
	bool MovePolygon(const Vector3 offset, long obj_no, long contour_no);
	bool InsertPolygon(long index, const viewORTHO& view, const mvState *state, const Vector3Vec& poly, long face, unsigned short subtype = 0, unsigned short attrib = 0);
	bool ScalePolygon(Vector3 scale, long obj_no, long contour_no);
	bool RotatePolygon( double sintheta, double costheta, Vector3 Origin, long obj_no, long contour_no);
	bool ReplacePolygon(long obj_no, long contour_no, const Vector3Vec& poly);

	// Area contour editing functions
	bool ReplaceArea(long obj_no, const Vector3Vec& poly);

	void AnchorPt(long vertex, long section, bool fix);
	void AnchorPts(long section, bool fix);

	// Save measurements
	long SaveMeasurements(STD::ofstream& out, const char *instanceName = 0);
	long LoadMeasurements(const char *fileName, const char *instanceName = 0);

    //***LG: new
    void CalculateParameters(MPoly2D *obj){polyParam(obj);}

protected:

	MObjpVec m_measureList, m_undoList;

	MPoly2DVec	m_undoPoly;
	OrientMarker m_orientMark;

	Vector3Vec m_cutPoints;
	PlotVec m_outPoints;
	
	long m_pointID, m_lineID, m_areaID, m_volumeID, m_angleID, m_segID;
	long m_curObj, m_prevSide, m_firstSide, m_prevX, m_prevY, m_prevZ;

	MPoly2D m_curPoly, m_curVol3, m_curVol4;
	long m_curVol;
	Vector3Vec m_cutPoly;
	Plot m_out;
	bool m_tubeVol;


protected:

	long detSide(Vector3* point, const Plane* plane);
	bool inPolygon(MPoly2D *ipa, Vector3 pt);
	void determinVolpts(const viewORTHO& view, Vector3 offset, Vector3 offset2, long face_no, long listno);
	void polyParam(MPoly2D *obj);
	
	long ptInFace(const viewORTHO& view, float x, float y, float z, long measuremode);
	bool ptInPoly(const Vector3Vec& vertex, float ptX,  float ptY);
	bool ptInFaceGiven(const viewORTHO& view, float x, float y, float z, long face_no);
	
	void findIntersections(Vector3 point1, Vector3 point2, Vector3 point);
	bool lineIntersect(const Vector3 *vec1, const Vector3 *vec2, Vector3 *vec3, Vector3 *vec4, Vector3 *res);

	void determinPoints(const viewORTHO& view, Vector3 *point1, Vector3 *point2, long face_no, long type, long listno);
	void determinPts_diff_pl(const viewORTHO& view, Vector3 *point1, Vector3 *point2, long face_no);
	void determinPts_same_pl(const viewORTHO& view, Vector3 *point1, Vector3 *point2, long face_no, long listno);
	void calculateEnclosedCube(MPoly3D *ipv);
		
	float calcVolume(MPoly3D *ipv);

	float getMax(float inval1, float inval2){if(inval1 > inval2) return inval1; else return inval2;}
	float getMin(float inval1, float inval2){if(inval1 < inval2) return inval1; else return inval2;}

	long getClosest(const Vector3 point, const Vector3Vec& poly2d, float delta);
	long getClosestEdge(const Vector3 point, const Vector3Vec& poly2d, float delta);
	long getClosestSection(Vector3 point, MPoly3D *vol, float delta);

	void movePoly2D(MPoly2D* obj, const Vector3 point);
	void scalePoly2D(MPoly2D *obj, Vector3 scale, Vector3 Origin);
	void scalePoly2D(MPoly2D *obj, float scale); // scale on center of gravity
	void rotatePoly2D(MPoly2D *obj, double sintheta, double costheta, Vector3 Origin);

	void determinTrianglePts(Vector3 corner1, Vector3 corner2, Vector3 corner3, const viewORTHO& view, long face_no, long contid);

	// Copy base type param's in measureList[index] to the second object
	void copyParam(long index, MObj &mobj);

	// Save measurements
	void writeField(STD::ostream& out, const DTag tag, const long value, const char *data=0);
	void writeField(STD::ostream& out, const DTag tag, const long value, const long data);
	void writeField(STD::ostream &out, long tag1, long tag2, long value, const char *data = 0);
	
	void saveViewsToFile(STD::ostream& input, const mvState& state);

    float minDist;
};

#endif // !defined(MEASURE_H_INCLUDED)