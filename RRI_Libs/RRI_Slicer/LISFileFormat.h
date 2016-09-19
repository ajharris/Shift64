// Copyright (c) Robarts Research Institute 2006

//---------------------------------------------------------------------
// File tags associated with LIS image and information files. These 
// should co-exist with DICOM standard files whenever possible. 
// i.e. [16bit group-tag][16bit element-tag][32 bit length-field]
// [optional data-field]
//
// The 'family-tag' should be odd to indicate that its a private tag.
//
/* $History: $
*/
//---------------------------------------------------------------------

#if !defined(LISFILEFORMAT_H_INCLUDED)
#define LISFILEFORMAT_H_INCLUDED

//---------------------------------------------------------------------

struct DTag 
{
	unsigned element:16, group:16; // microsoft puts the first one on low-bit
};

typedef const unsigned short DTAG;

//---------------------------------------------------------------------
// To get the types referred to in the comments below, AcquisitionTypes.h
// must be included (e.g. through AcqParam.h, but only at the cpp level)
//---------------------------------------------------------------------
//
// Image File tags come from AcquisitionTypes.h::eScanGeometry
//
// probeOrientation...LTRBC comes from AcquisitionTypes.h::eProbeOrientation
//
// scanDirection comes from AcquisitionTypes.h::eScanDirection
//
// horzOrientation comes from AcquisitionTypes.h::eHorizontalImageReversal
//
// Clinical procedure ID comes from AcquisitionTypes.h::eClinicalProcedureID
//
// Tracking mode comes from AcquisitionTypes.h::eTrackingMode
//
// Standard DICOM tags that correspond to major LIS structures:
//---------------------------------------------------------------------

DTAG STDG_SIZE		= 0x0028;
DTAG STDE_ROWS		= 0x0010;
DTAG STDE_COLS		= 0x0011;
DTAG STDE_FRAMES	= 0x0008;

DTAG STDG_PIXELDATA	= 0x7fe0;
DTAG STDE_PIXELDATA	= 0x0010;

DTAG STDG_PATIENT	= 0x0010;
DTAG INFO_NAME		= 0x0010;
DTAG INFO_ID		= 0x0020;
DTAG INFO_DOB		= 0x0030;
DTAG INFO_SEX		= 0x0040;

//---------------------------------------------------------------------
// Private tags for LIS specific data
//---------------------------------------------------------------------

DTAG LISG_TYPE			= 0x5653;	 // LIS Volume file type ('LISV' in little endian).
DTAG LISE_TYPE			= 0x494C;

//---------------------------------------------------------------------
// This just messed up the cute little 'LISV' pattern (oh well...)
// This is our 'poison pill' to keep the old programs from attempting 
// to load a compressed file.
//---------------------------------------------------------------------

DTAG LISE_TYPE_C		= 0x494E;    


DTAG LISG_DATA			= 0xff03;

//---------------------------------------------------------------------
// Hmmm. It looks like someone started to forget the 0x in front of the numbers,
// so beware: We cannot change these now since so many volumes are already created
// with the non-hex tags and they would not be compatible.
//
// Also beware that if you start using the hex notation, it's vagely possible that
// your numbers will clash, so I guess a new standard has been created...
// - RM00
//---------------------------------------------------------------------

DTAG ACQ_HORIZ_ORIENT	= 0010;
DTAG ACQ_PROBE_ORIENT	= 0020;
DTAG ACQ_SCAN_DIR		= 0030;
DTAG ACQ_SWEEP_ANGLE	= 0040;
DTAG ACQ_GEOMETRY		= 0050;
DTAG ACQ_TRACKINGTYPE	= 0060;

DTAG CAL_XVOX			= 1000;
DTAG CAL_YVOX			= 1010;
DTAG CAL_ZVOX			= 1020;
DTAG CAL_AOR			= 1030;
DTAG CAL_IPT			= 1040;
DTAG CAL_OPD			= 1050;
DTAG CAL_OPT			= 1060;
DTAG CAL_PD				= 1070;

DTAG COLOR_MAXBW		= 2000;
DTAG COLOR_HASCOLOR     = 2005;
DTAG COLOR_MAX_PVEL		= 2010;		// index
DTAG COLOR_MIN_PVEL		= 2020;		// index
DTAG COLOR_MAX_NVEL		= 2030;		// index
DTAG COLOR_MIN_NVEL		= 2040;		// index
DTAG COLOR_MAXVEL		= 2050;		// stored as cm/s
DTAG COLOR_MINVEL		= 2060;		// stored as cm/s

DTAG COLOR_PALSIZE		= 2070;	// Palette size
DTAG COLOR_PALDATA		= 2080;	// RGB triplets (3*palette_size bytes)

DTAG TIME_PHASES		= 3000;
DTAG TIME_HEARTRATE		= 3010;

DTAG SNAPSHOT_ROWS		= 4000;	// Fullscreen snapshot
DTAG SNAPSHOT_COLS		= 4010;
DTAG SNAPSHOT_TYPE		= 4011;
DTAG SNAPSHOT_DATA		= 4020;

DTAG INFO_NOTES			= 5000;
DTAG INFO_SCANDATE		= 5010;
DTAG INFO_SCANTIME		= 5020;
DTAG INFO_EXAMINER		= 5030;

// TRACKED FREEHAND MODULE ->
DTAG FREEHAND_VOL_X		= 6000;
DTAG FREEHAND_VOL_Y		= 6010;
DTAG FREEHAND_VOL_Z		= 6020;
DTAG FREEHAND_XFORMS	= 6030;
// <- TRACKED FREEHAND MODULE

DTAG PIXELDATA_COMPRESSED = 6100; // same as STDE_PIXELDATA, but compressed

DTAG SCULPTING_DATA		= 7000;

// old format definitions
const long LIS_HEADER_MARKER = 245; // uniquely identify the lis data file....
const long ACQ_PARAM		= 1;
const long WIDTH			= 1;
const long HEIGHT			= 2;
const long NUM_FRAMES		= 3;
const long HORIZ_ORIENT		= 4;
const long PROBE_ORIENT		= 5;
const long SCAN_DIRECTION	= 6;
const long SWEEP_ANGLE		= 7;
const long SCAN_GEOMETRY	= 8;

const long CAL_PARAM = 7;
const long X_VOX_SIZE = 1;
const long Y_VOX_SIZE = 2;
const long Z_VOX_SIZE = 3;
const long AOR = 4;
const long IPT = 6;
const long OPD = 7;
const long OPT = 8;
const long PD = 9;

const long FHND_PARAM = 3;

const long CHDR_PARAM = 4;
const long MAX_BW = 1;
const long ZERO_COLOR = 2;
const long MAX_VEL = 3;		// stored as cm/s
const long MIN_VEL = 4;		// stored as cm/s

const long DI_Palette_PARAM = 5;
const long NT_PALETTE_SIZE = 236;

const long TEMPORAL_PARAM = 6;
const long NUM_PHASES = 1;
const long HEART_RATE = 2;

const long DATA_PARAM =  2;

// Information file tags starts at 0xFF01

// Saved view states
const unsigned short  SVF_TYPE	= 0x4953;	 // Saved view file type ('LISI' in little endian).
const unsigned short  SVE_TYPE	= 0x494C;

const unsigned short SVF_DATA	= 0xFF01;	// saved view family tag

// saved view element tags
const short SVE_NAME			= 0x1;	// saved view name - 32 bytes long
const short SVE_VIEW2MODEL		= 0x2;	// saved view view-to-model matrix Matrix4x4
const short SVE_MODEL2VIEW		= 0x3;	// saved view model-to-view matrix Matrix4x4
const short SVE_SCALE			= 0x4;	// float
const short SVE_SHIFT			= 0x5;	// float[2]

const short SVE_FACEPLANE		= 0x6;	// length is 4*sizeof(float) - Vector3 and a float
const short SVE_FACEVERTEX		= 0x7;	// length is 3*sizeof(float) - Vector3
const short SVE_ENDFACE			= 0x8;	// Marks the end of one face 
const short SVE_ENDVIEW			= 0x9;	// Marks the end of one saved state

const short SVE_RENDER			= 0xA;	// Rendering parameters - 4*sizeof(float)

const short SVE_ANNOTTEXT		= 0xB;	// Annotation - length of string
const short SVE_ANNOTPOSX		= 0xC;  // X Position of text or arrow - sizeof(long)
const short SVE_ANNOTPOSY		= 0xD;  // Y Position of text or arrow - sizeof(long)
const short SVE_ANNOTTYPE		= 0xE;  // Type of annotation  - sizeof(long)
const short SVE_ENDANNOT		= 0xF;  // marks the end of annotation data.

//window params and hue data

const short SVE_WND = 900;
const short SVE_WND_LEVEL = 901;
const short SVE_HUE = 902;
const short SVE_DRAWFLAGS= 903;


// Measurement group tag
const unsigned short MEASURE_DATA		= 0xFF03;

// Measurement element tags (See Measure.h for details of parameters)
const short MEASURE_INSTANCE	= 1000;	// Measurement class instance name (string less than 254 char)
const short MEASURE_ID			= 1010;	// Measurement ID (long)
const short MEASURE_TYPE		= 1020;	// Measurement type (long)
const short MEASURE_NAME		= 1030;	// Measurement name (32 bytes long)
const short MEASURE_SUBTYPE		= 1040;	// Measurement subtype (long)
const short MEASURE_ATTRIB		= 1050;	// Measurement attributes (long)

const short MEASURE_VA_FIRST	= 1052; // Measurement volume axis first point of parallel contours
const short MEASURE_VA_SECOND   = 1053; // Measurement volume axis second of parallel contours

const short MEASURE_CUSTOM		= 1051;	// Measurement custom parameters (long x CUSTOM_ENTRIES)
const short MEASURE_PARAM		= 1060;	// Measurement parameters (float x 6)

const short MEASURE_VEC3		= 1070;	// (float x 3) - for a generic float triplet

const short MEASURE_PLANE_NORMAL= 1080;	// (float x 3)  - plane normal for areas
const short MEASURE_PLANE_DEPTH	= 1090;	// (float) - plane depth for areas
const short MEASURE_END_CONTOUR	= 1100;	// (zero length)
const short MEASURE_END_OBJECT	= 1110;	// (zero length)


#endif // !defined(LISFILEFORMAT_H_INCLUDED)