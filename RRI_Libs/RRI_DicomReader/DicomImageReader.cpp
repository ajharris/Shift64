//Joseph Awad

#include "stdafx.h"

#include "DicomImageReader.h"

// Constructor
DicomImageReader::DicomImageReader()
{
	Filename="";
	Path="";
	reader=NULL;
	Img=NULL;
	mShift=0.0;
	mScale=1.0;
}
// Destructor
DicomImageReader::~DicomImageReader()
{
	if(reader!=NULL)reader->Delete();
	if(Img!=NULL)Img->Delete();
}
// Set
void DicomImageReader::SetFileName(string Filenm)
{
	Filename=Filenm;
	size_t found;
	found=Filename.find_last_of("/\\");
	Path=Filename.substr(0,found);
	//cout << "folder: " << Path << endl;

}
//Get
vtkImageData *DicomImageReader::GetImage()
{
	return Img;
}
//Reading
long DicomImageReader::Read()
{
	vector <string> Filenames;

	cout << "Loading directory: " << Path << endl;

	bool recursive = false;
	gdcm::Directory directory;
	directory.Load(Path, recursive);
	gdcm::Directory::FilenamesType const &files = directory.GetFilenames();
	for( gdcm::Directory::FilenamesType::const_iterator it = files.begin(); it != files.end(); ++it )
	{
		Filenames.push_back( it->c_str() );
	}

	cout <<"Number of Files="<<Filenames.size()<<endl;
	
	gdcm::IPPSorter sorter;
	sorter.SetComputeZSpacing( true );
	sorter.SetZSpacingTolerance( 1e-3 );
	bool didSorterWork = sorter.Sort( Filenames );

	if( !didSorterWork )
	{
		cerr << "Failed to sort:" << Path << endl;
		return -1;
	}

	cout << "Sorting succeeded:" << endl;

	cout << "Found z-spacing:" << endl;
	cout << sorter.GetZSpacing() << endl;
	double ippzspacing = sorter.GetZSpacing();

	const vector<string> & sorted = sorter.GetFilenames();
	vtkStringArray *FilesArr = vtkStringArray::New();
	vector< string >::const_iterator it = sorted.begin();
	for( ; it != sorted.end(); ++it)
	{
		const string &f = *it;
		FilesArr->InsertNextValue( f.c_str() );
	}

	if(reader==NULL)reader = vtkGDCMImageReader::New();
	reader->SetFileNames( FilesArr );
	reader->UpdateWholeExtent();
	reader->Update();

	FilesArr->Delete();

	const vtkFloatingPointType *spacing = reader->GetOutput()->GetSpacing();
	vtkImageChangeInformation *v16 = vtkImageChangeInformation::New();
	v16->SetInput( reader->GetOutput() );
	v16->SetOutputSpacing( spacing[0], spacing[1], ippzspacing );
	v16->Update();
	
	double Rng[2];
	v16->GetOutput()->GetScalarRange(Rng);
	cout <<"Range="<<Rng[0]<<", "<<Rng[1]<<endl;
	

	mScale=1.0;
	mShift=-Rng[0];

	vtkImageShiftScale *v16a=vtkImageShiftScale::New();
	v16a->SetOutputScalarTypeToUnsignedShort();
	v16a->SetScale(mScale);
	v16a->SetShift(mShift);
	v16a->SetInput(v16->GetOutput());
	v16a->Update();
	
	v16->Delete();


	Img=v16a->GetOutput();
	Img->Register(NULL);
	Img->SetSource(NULL);
	Img->Update();
	
	v16a->Delete();

	return 0;//all OK


}