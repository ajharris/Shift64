#ifndef VISLIB_H_INCLUDED
#define VISLIB_H_INCLUDED


#include <vector>
#include <string>
#include <assert.h>


//---------------------------------------------------------------------
// switch for iostream and STL implementations
//
// #define USING_NEW_IOSTREAM	// use new template based iostreams 
// #define USE_STLPORT			// use Microsoft STL if undefined
//
// Following is used to switch iostream and STL implementations
// Please use the namespace STL for containers and STD for streams. 
//
#if defined(USE_STLPORT)
 #define STL STLPORT		// use STL Port
#else
 #define STL std			// use Microsoft STL
#define USING_NEW_IOSTREAM	// force the use of new streams library
#endif

#if defined(USING_NEW_IOSTREAM)
 #pragma warning(disable:4786)
 #include <istream>
 #include <ostream>
 #include <strstream>
 #include <fstream>
 #define STD STL
#else
 #include <istream.h>
 #include <ostream.h>
 #include <strstrea.h>
 #include <fstream.h>
 #define STD 
#endif


//#include <afxcmn.h>	


//---------------------------------------------------------------------
// This allows the visualizer to be built both as an export DLL and using 
// internal source files.  define USING_DLL if building a DLL
// 
#if defined( USE_DLL ) // Using the Visualizer DLL

#if defined( MAKE_DLL )
#error Both USE_DLL and MAKE_DLL should not be defined at the same time
#endif

#define SLICER_EXPORT __declspec(dllimport)
#define EXPIMP_TEMPLATE extern

#elif defined( MAKE_DLL )                        // making the Visualizer DLL
#define SLICER_EXPORT __declspec(dllexport)
#define EXPIMP_TEMPLATE

#else                                            // visualizer files are statically linked
#define SLICER_EXPORT
#define EXPIMP_TEMPLATE
#endif



struct DTag 
{
	unsigned element:16, group:16;               // microsoft puts the first one on low-bit
};

typedef const unsigned short DTAG;


//---------------------------------------------------------------------
// l3d file group
//
DTAG STDG_SIZE		      = 0x0028;
DTAG STDE_ROWS		      = 0x0010;
DTAG STDE_COLS		      = 0x0011;
DTAG STDE_FRAMES	      = 0x0008;

DTAG STDG_PIXELDATA	      = 0x7fe0;
DTAG STDE_PIXELDATA	      = 0x0010;

DTAG LISG_TYPE			  = 0x5653;	 
DTAG LISE_TYPE			  = 0x494C;

DTAG LISE_TYPE_C		  = 0x494E;    
DTAG LISG_DATA			  = 0xff03;

DTAG ACQ_HORIZ_ORIENT	  = 0010;
DTAG ACQ_PROBE_ORIENT	  = 0020;
DTAG ACQ_SCAN_DIR		  = 0030;
DTAG ACQ_SWEEP_ANGLE	  = 0040;
DTAG ACQ_GEOMETRY		  = 0050;
DTAG ACQ_TRACKINGTYPE	  = 0060;

DTAG CAL_XVOX			  = 1000;
DTAG CAL_YVOX			  = 1010;
DTAG CAL_ZVOX		      = 1020;
DTAG CAL_AOR			  = 1030;
DTAG CAL_IPT			  = 1040;
DTAG CAL_OPD			  = 1050;
DTAG CAL_OPT			  = 1060;
DTAG CAL_PD				  = 1070;

DTAG PIXELDATA_COMPRESSED = 6100; 


//---------------------------------------------------------------------
// ldd file saved view group
//
const unsigned short  SVF_TYPE	= 0x4953;	// saved view file type ('LISI' in little endian).
const unsigned short  SVE_TYPE	= 0x494C;

const unsigned short SVF_DATA	= 0xFF01;	// saved view family tag

const short SVE_NAME			= 0x1;	    // saved view name - 32 bytes long
const short SVE_VIEW2MODEL		= 0x2;	    // saved view view-to-model matrix Matrix4x4
const short SVE_MODEL2VIEW		= 0x3;	    // saved view model-to-view matrix Matrix4x4
const short SVE_SCALE			= 0x4;	    // float
const short SVE_SHIFT			= 0x5;	    // float[2]

const short SVE_FACEPLANE		= 0x6;	    // length is 4*sizeof(float) - Vector3 and a float
const short SVE_FACEVERTEX		= 0x7;	    // length is 3*sizeof(float) - Vector3
const short SVE_ENDFACE			= 0x8;	    // marks the end of one face 
const short SVE_ENDVIEW			= 0x9;	    // marks the end of one saved state

const short SVE_WND             = 900;
const short SVE_WND_LEVEL       = 901;
const short SVE_HUE             = 902;
const short SVE_DRAWFLAGS       = 903;


//---------------------------------------------------------------------
// ldd file measurement group
//
const unsigned short MEASURE_DATA = 0xFF03;

const short MEASURE_INSTANCE	  = 1000;	// measurement class instance name (string less than 254 char)
const short MEASURE_ID			  = 1010;	// measurement ID (long)
const short MEASURE_TYPE		  = 1020;	// measurement type (long)
const short MEASURE_NAME		  = 1030;	// measurement name (32 bytes long)
const short MEASURE_SUBTYPE		  = 1040;	// measurement subtype (long)
const short MEASURE_ATTRIB		  = 1050;	// measurement attributes (long)

const short MEASURE_VA_FIRST	  = 1052;   // measurement volume axis first point of parallel contours
const short MEASURE_VA_SECOND     = 1053;   // measurement volume axis second of parallel contours

const short MEASURE_CUSTOM		  = 1051;	// measurement custom parameters (long x CUSTOM_ENTRIES)
const short MEASURE_PARAM		  = 1060;	// measurement parameters (float x 6)

const short MEASURE_VEC3		  = 1070;	// (float x 3) - for a generic float triplet

const short MEASURE_PLANE_NORMAL  = 1080;	// (float x 3)  - plane normal for areas
const short MEASURE_PLANE_DEPTH	  = 1090;	// (float) - plane depth for areas
const short MEASURE_END_CONTOUR	  = 1100;	// (zero length)
const short MEASURE_END_OBJECT	  = 1110;	// (zero length)



//***LG Added------------------------------------------------------------
//const enum {LORES, HIGHRES, ZBUFF, NORENDER, ACTIVEFACE, SCALPEL, SHAVE, ZRENREG};
typedef enum {PT_NORMAL, PT_ANCHOR} eVertexType;
//-----------------------------------------------------------------------
// Image and modes related constants
//
const long OVERLAY_LOGO = 0x1; 

const enum {CUBE_PAL, VIDEO_PAL};

const enum {DRAW_ORIENTATION, DRAW_MEASUREMENTS, DRAW_CONTOURS, DRAW_WIREFRAME, DRAW_PRINTMEASUREMENTS, DRAW_ROTATION,
			DRAW_ONE_THREE_PLANES, DRAW_PIVOT_ROTATION, DRAW_ANCHOR_POINTS};

const enum {NO_CONTOUR_EDITING, GO_TO_CONTOUR_SLICE, UNDO_LAST_CONTOUR_EDIT,
			MOVE_CONTOUR, SCALEIN_CONTOUR, SCALEOUT_CONTOUR, DELETE_CONTOUR, APPEND_CONTOUR, ADD_CONTOUR,
			APPEND_RADIAL_CONTOUR, DELETE_VOLUME, MOVE_VOLUME, DELETE_ALL_VOLUMES,
            ADD_CONTOUR_POINT, MOVE_CONTOUR_POINT, DELETE_CONTOUR_POINT, ANCHOR_CONTOUR_POINT, REFINE_CONTOUR};

const enum {BEGIN, CONTINUE, END}; 
const enum {ALREADY_SET, ROTATE, SLICE, SLICE_ROTATE, DOING_NOTHING, PAN}; 
const enum {SM_SLICE, SM_MEASURE};
const enum {LORES, HIGHRES, ZBUFF, NORENDER, ACTIVEFACE};//ZBUFF used for for MIP
const enum {TEX, MIP};
const enum {ALIGN_TO_SCRN, NONALIGN_TO_SCRN};
const enum {MOVE_TO = 1, LINE_TO};

const int MAX_VIEW_NAME = 30;



typedef unsigned char tVoxel;

//---------------------------------------------------------------------
// commonly used vectors
//
//video calibration
struct VideoCalibration
{
	std::string depthLabel;
	double XPixel;
	double YPixel;
	double probeTipX;
	double probeTipY;
	double ROILeft;
	double ROITop;
	double ROIWidth;
	double ROIHeight;
	long   leftMarkerX;
	long   leftMarkerY;
    long   needleGuide1X;
    long   needleGuide1Y;
    long   needleGuide2X;
    long   needleGuide2Y;
};

typedef std::vector<VideoCalibration> VideoCalibrationList;
typedef std::vector<std::string> StringList;

typedef STL::vector<long> longVec;
typedef STL::vector<float> floatVec;
typedef STL::vector<short> shortVec;

struct Vector2
{
	float x,y;
	bool a;

	Vector2(float X, float Y)
	{
		x = X; 
		y = Y;
	}
	Vector2(){}
};


// vector3: row vector (x,y,z) of floats (non-homogeneous 3-space coordinates)
struct Vector3
{
	float x,y,z;
	bool a;

	Vector3(float X, float Y, float Z)
	{
		x = X; 
		y = Y;
		z = Z;
	}
	Vector3(){}
};

typedef STL::vector<Vector3> Vector3Vec;
typedef STL::vector<Vector3Vec>Vector3VecVec;
typedef STL::vector<bool> BoolVector;

// bitfields makes sure that the structure is packed
struct Plot
{
    unsigned id:16;				// each type has its own id number space.
	unsigned plotCode:8;		// 256 possible plotting codes 
	unsigned plotSpecial:8;		// special instructions
	signed   x:16, y:16;		// value of each  coordinate is -32767 to 32767
    unsigned red:8;
    unsigned green:8;
    unsigned blue:8;
	
	Plot()
	{
		id = plotCode = plotSpecial = x = y = 0;
        //default colour is green
        red = 0;
        green = 255;
        blue = 0;
	}
};

typedef STL::vector<Plot> PlotVec;


//---------------------------------------------------------------------
// parameters for orientation marker 
// vertex identifiers: {top, bottom}, {left, right}, {front, back}
//
const enum {tlf = 0, trf, blf, brf, tlb, trb, blb, brb};

struct OrientMarker 
{ 
	Vector3 start;
	Vector3 endX, endY, endZ;
	Vector3 label;
};


//---------------------------------------------------------------------
// default cube rotation
//
typedef const enum {
	rotCurrent, 
	rotNone, 
	rotUpDown, 
	rotFrontBack, 
	rotSagittal, 
	rotTransverse, 
	rotCoronal, 
	rotTransverseNoSlice, 
	rotSagittalNoSlice, 
	rotCoronalNoSlice, 
	rotCardiac,
    rotAxialNoOffsets,
    rotSagittalNoOffsets,
    rotSideFired
    ,rotSagittal2
    ,rotSagittal3
}	
eCubeRotation;


struct Matrix4x4
{
	float	a,b,c,d,
			e,f,g,h,
			i,j,k,l,
			m,n,o,p;

	Matrix4x4(float A, float B, float C, float D,
		      float E, float F, float G, float H,
			  float I, float J, float K, float L,
			  float M, float N, float O, float P)
	{
		a = A; b = B; c = C; d = D;
		e = E; f = F; g = G; h = H;
		i = I; j = J; k = K; l = L;
		m = M; n = N; o = O; p = P;
	}


	// scale matrix
	Matrix4x4(float s)
	{
		a = s;    b = 0.0f; c = 0.0f; d = 0.0f;
		e = 0.0f; f = s;    g = 0.0f; h = 0.0f;
		i = 0.0f; j = 0.0f; k = s;    l = 0.0f;
		m = 0.0f; n = 0.0f; o = 0.0f; p = 1.0f;
	}

	//translation matrix
	Matrix4x4(float x, float y, float z)
	{
		a = 1.0f; b = 0.0f; c = 0.0f; d = 0.0f;
		e = 0.0f; f = 1.0f; g = 0.0f; h = 0.0f;
		i = 0.0f; j = 0.0f; k = 1.0f; l = 0.0f;
		m = x;    n = y;    o = z;    p = 1.0f;
	}

	//translation matrix
	Matrix4x4(Vector3 v)
	{
		a = 1.0f; b = 0.0f; c = 0.0f; d = 0.0f;
		e = 0.0f; f = 1.0f; g = 0.0f; h = 0.0f;
		i = 0.0f; j = 0.0f; k = 1.0f; l = 0.0f;
		m = v.x;  n = v.y;  o = v.z;  p = 1.0f;
	}

	//matrix from quaterion
	Matrix4x4(float s, Vector3 V)
	{
		float sx,sy,sz,xx,xy,xz,yx,yy,yz,zx,zy,zz;
	
		sx = 2.0f * s * V.x;
		sy = 2.0f * s * V.y;
		sz = 2.0f * s * V.z;
		xx = 2.0f * V.x * V.x;
		xy = 2.0f * V.x * V.y;
		xz = 2.0f * V.x * V.z;
		yx = 2.0f * V.y * V.x;
		yy = 2.0f * V.y * V.y;
		yz = 2.0f * V.y * V.z;
		zx = 2.0f * V.z * V.x;
		zy = 2.0f * V.z * V.y;
		zz = 2.0f * V.z * V.z;


		a = 1.0f - yy - zz;
		b = xy + sz;
		c = xz - sy;
		d = 0.0f;
		e = xy - sz;
		f = 1.0f - xx - zz;
		g = yz + sx;
		h = 0.0f;
		i = xz + sy;
		j = yz - sx;
		k = 1.0f - xx - yy;
		l = 0.0f;
		m = 0.0f;
		n = 0.0f;
		o = 0.0f;
		p = 1.0f;
	}

	Matrix4x4(){}
};


struct Plane
{
	Vector3 normal;	
	float D;		

	Plane(Vector3 n, float d)
	{
		normal = n;
		D = d;
	}

	Plane(){}
};


struct Rect
{ 
	long top, left, bottom, right;

	Rect(long T, long L, long B, long R)
	{
		top = T;
		left = L;
		bottom = B;
		right = R;
	}

	Rect(){}
};


struct Face
{
	Plane plane;
	Vector3Vec vert;
	
	Face(Vector3 normal, float D)
	{
		plane = Plane(normal, D);
		vert.erase(vert.begin(), vert.end() );
	}

	Face(Plane pln)
	{
		plane = pln;
		vert.erase(vert.begin(), vert.end());
	}

	Face(){}
};


struct FilledBuff
{
	tVoxel *buff;
	long dim[4];

	FilledBuff(tVoxel *bf, long x, long y)
	{
		buff = bf;
		dim[0] = x;
		dim[1] = y;
	}

	FilledBuff(){}
};

typedef STL::vector<Face> FaceVec;

//triangles for generating surfaces
struct Triangle 
{
	Vector3 V1, V2, V3, N1, N2, N3;
	short contour;
};


//list of triangles in surface object
typedef STL::vector<Triangle> TriangleVec;


struct viewORTHO
{
	Matrix4x4 View2Model;
	Matrix4x4 Model2View;
	FaceVec face;
};


struct mvState
{
	char name[MAX_VIEW_NAME+1];

	FaceVec model;

	Matrix4x4 View2ModelRot;
	Matrix4x4 Model2ViewRot;

	float scaleAbs;   
	float modelShift[2];

	int window, level, hue;
	unsigned long drawflags;

	mvState& operator= (const mvState &right)
	{
		memcpy(name, right.name, MAX_VIEW_NAME+1);
		model = right.model;
		View2ModelRot = right.View2ModelRot;
		Model2ViewRot = right.Model2ViewRot;
		scaleAbs = right.scaleAbs;

		modelShift[0] = right.modelShift[0];
		modelShift[1] = right.modelShift[1];

		window = right.window;
		level = right.level;
		hue = right.hue;
		drawflags = right.drawflags;

		return *this;
	}
};

typedef STL::vector<mvState> mvStateVec;

struct Trgb 
{
	unsigned char r, g, b ;
	
	Trgb(unsigned char red, unsigned char green, unsigned char blue)
	{
		r = red;
		g = green;
		b = blue;
	}
	
	Trgb(){}
};



typedef enum
{
	SG_Invalid			= -1,
	SG_ViewRaw			= 0,
	SG_LinearRaw		= 1,
	SG_Fan				= 2,
	SG_AxialNoOffsets	= 3,
	SG_AxialGeneral		= 4,
	SG_Pullback			= 6,
    SG_Linear16         = 7,
    SG_Linear24         = 8,
    SG_Fan24            = 9,
    SG_Axial24          = 10,
	SG_Hybrid           = 11,
	SG_Hybrid24         = 12
}
eScanGeometry;

//---------------------------------------------------------------------
// e.g PO_Top means that on the ultrasound screen,
// the probe's location (source of the beam) is
// from the top of the screen
typedef enum
{
	PO_Invalid	= -1,
	PO_Left		= 1,
	PO_Top		= 2,
	PO_Right	= 3,
	PO_Bottom	= 4,
	PO_Centre	= 5
}
eProbeOrientation;

typedef enum
{
	SD_Invalid				= 0,
	SD_Clockwise			= 1,
	SD_AntiClockwise		= -1,
	SD_InferiorToSuperior	= 1,
	SD_SuperiorToInferior	= -1,
	SD_AwayFromOperator		= 1,
	SD_TowardsOperator		= -1
}
eScanDirection;

typedef enum
{
	HIR_Invalid			= 0,
	HIR_PowerupDefault	= 1,
	HIR_Reflected		= -1
}	
eHorizontalImageReflection;

typedef enum
{
	TM_Invalid		= -1,
	TM_Untracked	= 0,
	TM_MCM			= 1,
	TM_TF			= 2
}
eTrackingMode;


//---------------------------------------------------------------------
// Conversion macros
//---------------------------------------------------------------------
#define DEG_TO_RAD(deg) ((deg) * (PI/180.0))
#define RAD_TO_DEG(rad) ((rad) * (180.0/PI))

//---------------------------------------------------------------------
// AcqParam 
//---------------------------------------------------------------------
struct AcqParam 
{
	eScanGeometry				scanGeometry;			// one of linear, fan, axial freehand
	long						width, height;			// width/heigth of acquired frames in pixels  
	long						numFrames;				// number of acquired frames in raw file  
	eHorizontalImageReflection	horizontalReflection;	// flag indicating that left and right are reversed in grabbed image
	eProbeOrientation			probeOrientation;		// probe location in image (1=left, 2=top, 3=right, 4=bottom)
	eScanDirection				scanDirection;			// direction of sweep
	eTrackingMode				trackingMode;			// see AcquisitionTypes.cpp
	float						sweepAngle;				// number of degrees swept by probe (or shear if linear)
};

//---------------------------------------------------------------------
// Calibration Parameters
//---------------------------------------------------------------------
struct CalParam
{
	float			
		xVoxelSize,				// width  of voxels (mm/voxel) 
		yVoxelSize,				// height of voxels (mm/voxel) 
		zVoxelSize;				// depth  of voxels (mm/voxel) (calculated at time of reconstruction)
	float
		axisOfRotation,			// axis of rotation (pixels from left edge) 
		outPlaneDisplacement,	// out-of-plane displacement (pixels) 
		probeDistance;			// probe tip location (pixels from ?TOP edge) 
	float
		inPlaneTilt,			// in-plane tilt (degrees)
		outPlaneTilt;			// out-of-plane tilt (degrees) 
			// ig
			// temporary solution for 4D 
			// inPlaneTilt used to store number of 3D volumes inside 4D volume
};



//---------------------------------------------------------------------
// initialization struct for Texture class
//---------------------------------------------------------------------
struct TtexInit
{
	long clientX;
	long clientY;

	long texMode;
	long reconType;
};


//---------------------------------------------------------------------
// initialization struct for Voxel class
//---------------------------------------------------------------------
struct TvoxelInit
{
	tVoxel *data;

	AcqParam acq; 
	CalParam cal;
};

//---------------------------------------------------------------------
// initialization struct for Model class
//---------------------------------------------------------------------
struct TmodelInit
{
	float sizeX,
		  sizeY,
		  sizeZ;

	long  clientX, 
		  clientY;
};


#endif//VISLIB_H_INCLUDED