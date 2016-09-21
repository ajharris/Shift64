// Joseph Awad 

#ifndef DICOMIMAGEREADER_H
#define DICOMIMAGEREADER_H


#include <string>
#include <vector>

// VTK Classes


// VTK Headers.
#include <vtkImageChangeInformation.h>
#include <vtkImageShiftScale.h>
#include <vtkStringArray.h>

//#include <vtkImageData.h>
//#include <vtkStringArray.h>
//#include <vtkImageChangeInformation.h>
//#include <vtkImageShiftScale.h>
//#include <vtkImageCast.h>

#include "vtkGDCMImageReader.h"
#include "gdcmSystem.h"
#include "gdcmDirectory.h"
#include "gdcmIPPSorter.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

// Visualizer Lib
//#include "VisLib.h"

using namespace std;

class DicomImageReader {

public:
	// Constructor
	DicomImageReader();
	// Destructor
	~DicomImageReader();
	
	// Set
	void SetFileName(string Filenm);
	
	//Get
	vtkImageData *GetImage();
	
	//Reading
	long Read();
	// Writing
	//void Write(string filnm);
	
protected:
	// Class members
	string Filename;
	string Path;
	//vector <string> Filenames;
	vtkGDCMImageReader *reader;
	vtkImageData *Img;
	
	double mShift,mScale;
};
#endif