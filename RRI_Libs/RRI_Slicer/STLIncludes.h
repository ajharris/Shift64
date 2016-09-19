// Copyright (c) Robarts Research Institute 2002
// Developed by Lori Gardi, Senior Software Developer

//---------------------------------------------------------------------
// STLChooser.h - Allows you to choose the STL library you want
//                (for SGI this requires USE_STLPORT is defined in
//                the project, and that your include path points to
//                a directory including the SGI header files...
//
//---------------------------------------------------------------------
#ifndef STLINCLUDES_H_INCLUDED
#define STLINCLUDES_H_INCLUDED



//---------------------------------------------------------------------
// switch iostream and STL implementations
//
// These defines are now handled at the project level (as 'Project 
// Stettings'.) The USE_STLPORT uses the SGI STL library. This library
// was originally used to replace a rather buggy Microsoft 
// implementation of the STL. (RM00)
//
// #define USING_NEW_IOSTREAM	// Use new template based iostreams (class based if undefined (slow))
// #define USE_STLPORT			// Use STLPOrt. Uses Microsoft STL if undefined
//
// Following is used to switch iostream and STL implementations
// WARNING: Do not change this line. If you do, you have to recompile 
// both the DLL and your app with the same settings.
//
// Please use the namespace STL for containers and STD for streams. They are defined as below
//
//
//  Streams		Template lib	STL			STD			Notes
//---------------------------------------------------------------------
//    New		 STL Port		STLPORT		STLPORT		Slow. Make sure include path points to STL Port directory
//    Old		 Stl Port		STLPORT		blank		Fast. Make sure include path points to STL Port directory
//    New		 Microsoft		std			std			Slow & buggy. Make sure include path is blank
//    Old		 Microsoft		-- Doesn't work --		Broken
//
// (note that the above was based on ~VC++5.0 implementation)
//---------------------------------------------------------------------


// Copyright (c) Robarts Research Institute 2002
// Developed by Lori Gardi, Senior Software Developer

//---------------------------------------------------------------------
// STLChooser.h - Allows you to choose the STL library you want
//                (for SGI this requires USE_STLPORT is defined in
//                the project, and that your include path points to
//                a directory including the SGI header files...
//
//---------------------------------------------------------------------
#ifndef STLINCLUDES_H_INCLUDED
#define STLINCLUDES_H_INCLUDED



//---------------------------------------------------------------------
// switch iostream and STL implementations
//
// These defines are now handled at the project level (as 'Project 
// Settings'.) The USE_STLPORT uses the SGI STL library. This library
// was originally used to replace a rather buggy Microsoft 
// implementation of the STL. (RM00)
//
// #define USING_NEW_IOSTREAM	// Use new template based iostreams (class based if undefined (slow))
// #define USE_STLPORT			// Use STLPOrt. Uses Microsoft STL if undefined
//
// Following is used to switch iostream and STL implementations
// WARNING: Do not change this line. If you do, you have to recompile 
// both the DLL and your app with the same settings.
//
// Please use the namespace STL for containers and STD for streams. They are defined as below
//
//
//  Streams		Template lib	STL			STD			Notes
//---------------------------------------------------------------------
//    New		 STL Port		STLPORT		STLPORT		Slow. Make sure include path points to STL Port directory
//    Old		 Stl Port		STLPORT		blank		Fast. Make sure include path points to STL Port directory
//    New		 Microsoft		std			std			Slow & buggy. Make sure include path is blank
//    Old		 Microsoft		-- Doesn't work --		Broken
//
// (note that the above was based on ~VC++5.0 implementation)
//---------------------------------------------------------------------


#if defined(USE_STLPORT)
# define STL STLPORT		// Use STL Port
#else
# define STL std			// Use Microsoft STL
#define USING_NEW_IOSTREAM	// Force the use of new streams library
#endif

#if defined(USING_NEW_IOSTREAM)
 #pragma warning(disable:4786) // avoid getting a bunch of "truncated identifier" warnings - look up error code for more info
 #include <istream>
 #include <ostream>
 #include <iomanip>
 #include <strstream>
 #include <fstream>
 #include <sstream>
#include <string>
 #define STD STL
#else
 #include <istream.h>
 #include <ostream.h>
 #include <iomanip.h>
 #include <strstrea.h>
 #include <fstream.h>
#include <string.h>
 #define STD 
#endif


//---------------------------------------------------------------------
// Used to use stl versions of max and min, but this was SGI only
//---------------------------------------------------------------------

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//---------------------------------------------------------------------
// commonly used vectors
//---------------------------------------------------------------------
#include <vector>

typedef STL::vector<long> longVec;
typedef STL::vector<float> floatVec;
typedef STL::vector<short> shortVec;
typedef STL::vector<unsigned char> ucharVec;

//---------------------------------------------------------------------
// string convenience macros
//---------------------------------------------------------------------



typedef STL::string STLString;
// from http://www.codeproject.com/KB/stl/makestring.aspx
#define MAKE_STRING( msg )  ( ((STL::ostringstream&)(STL::ostringstream() << STL::fixed << STL::setprecision(3) << msg)).str() )
// WARNING: the following macro must be used in-place/immediately since the temporary object will disappear

#define MAKE_CSTRING( msg )  ( MAKE_STRING(msg).c_str() )

#endif //STLINCLUDES_H_INCLUDED


#endif //STLINCLUDES_H_INCLUDED