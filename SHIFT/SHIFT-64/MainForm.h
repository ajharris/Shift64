#pragma once


#define SLICER_AVAILABLE
#ifdef SLICER_AVAILABLE
#include "SlicerForm.h"
#endif


namespace SHIFT64 {

	using namespace System;
	using namespace System::IO;
	using namespace System::IO::Ports;
	using namespace System::Threading;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;
	using namespace System::Net;
	using namespace System::Text;
	using namespace System::Xml;

	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{


    //====================================================================================================
    //Slicer/Visualizer members
    //====================================================================================================
    #ifdef SLICER_AVAILABLE
	public: SlicerForm^ m_View1;//left view
	public: SlicerForm^ m_View2;//right view
    public: SlicerForm^ m_miniSlicer;
    #endif
    //====================================================================================================

    //====================================================================================================
    //File and folder and imageList members
    //====================================================================================================
    private: System::Windows::Forms::FolderBrowserDialog^  m_folderBrowserDialog1;
    static String^ m_baseFolder = "c:/Scans/";
	static String^ m_defaultFolder = m_baseFolder;
    static String^ m_folderToMonitor = m_defaultFolder;// m_preTreatmentFolder;//default directory
    private: System::Windows::Forms::ImageList^  m_imageList;
    private: System::Windows::Forms::ListView^   m_imageListView;
    static List<String^>^ m_fileList;//container for list of files from folder
    public: System::Windows::Forms::Timer^ m_monitorFolderTimer;// = gcnew System::Windows::Forms::Timer;
    
    //====================================================================================================


    //====================================================================================================
    //toolbar controls
    //====================================================================================================
    private: System::Windows::Forms::ToolStrip^  m_commonControls;

    private: System::Windows::Forms::ToolStripButton^  AddPoint;
    private: System::Windows::Forms::ToolStripButton^  DeletePoint;
    private: System::Windows::Forms::ToolStripButton^  ClearPoint;

    private: System::Windows::Forms::ToolStripButton^  Reset1;
    private: System::Windows::Forms::ToolStripButton^  Reset2;
    private: System::Windows::Forms::ToolStripButton^  Reset3;

    private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
    private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
    private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;

    private: System::Windows::Forms::ToolStripButton^  m_selectWorkingFolder;
    private: System::Windows::Forms::ToolStripButton^  Ruler;
    //====================================================================================================

    //====================================================================================================
    delegate bool AddDICOMImageDelegate(String^ filePath, String^ fileName, int imageCount);
	delegate bool AddRAWImageDelegate(String^ filePath, String^ fileName, int imageCount);
    delegate bool AddMHAImageDelegate(String^ filePath, String^ fileName, int imageCount);
    delegate bool AddBMPImageDelegate(String^ filePath, String^ fileName, int imageCount);
    delegate bool AddJPGImageDelegate(String^ filePath, String^ fileName, int imageCount);
    //====================================================================================================

     //====================================================================================================
     //fun with flags and counters
     //====================================================================================================
     //flags
     static bool m_monitorIsRunning = false;

     //counters
      static int m_imageCount = 0;//count number of images added to image list
     //====================================================================================================

    


	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
            //===================================================================================================================================
#ifdef SLICER_AVAILABLE
			m_View1 = gcnew SlicerForm(100, 100);//will be resized on load of form
			m_View1->GetSlicer()->Initialize(100,100,"");
			m_View1->MdiParent = this;
			m_View1->OnSlicerEvent += gcnew EventHandler(this, &SHIFT64::MainForm::Slicer_Event);
            InitializeView1();
		
			m_View1->Show();
			
			
			m_View2 = gcnew SlicerForm(100, 100);//will be resized on load of form
			m_View2->GetSlicer()->Initialize(100,100,"");
			m_View2->MdiParent = this;
			m_View2->OnSlicerEvent += gcnew EventHandler(this, &SHIFT64::MainForm::Slicer_Event);
            InitializeView2();
			m_View2->Show();//NOTE: show called Load the first time in
			

			//used for loading images into image box
			m_miniSlicer = gcnew SlicerForm(100, 100);
			m_miniSlicer->ClientSize = System::Drawing::Size(100, 100);
			m_miniSlicer->GetSlicer()->Initialize(100,100,"");
#endif
//===================================================================================================================================

            this->m_folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->m_folderBrowserDialog1->Description = L"Select Working Folder";
			this->m_folderBrowserDialog1->SelectedPath = L"c:\\Scans";

            this->m_fileList = gcnew List<String^>();//list of images to load into image list
            //create timer for monitoring folder
			m_monitorFolderTimer = gcnew System::Windows::Forms::Timer;
			m_monitorFolderTimer->Tick += gcnew EventHandler( this, &MainForm::MonitorFolderEventProcessor );
			m_monitorFolderTimer->Interval = 1000;//ms
           

//===================================================================================================================================

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
    private: System::ComponentModel::IContainer^  components;
    protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            this->components = (gcnew System::ComponentModel::Container());
            System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
            this->m_imageList = (gcnew System::Windows::Forms::ImageList(this->components));
            this->m_imageListView = (gcnew System::Windows::Forms::ListView());
            this->m_commonControls = (gcnew System::Windows::Forms::ToolStrip());
            this->m_selectWorkingFolder = (gcnew System::Windows::Forms::ToolStripButton());
            this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->Reset1 = (gcnew System::Windows::Forms::ToolStripButton());
            this->Reset2 = (gcnew System::Windows::Forms::ToolStripButton());
            this->Reset3 = (gcnew System::Windows::Forms::ToolStripButton());
            this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->AddPoint = (gcnew System::Windows::Forms::ToolStripButton());
            this->DeletePoint = (gcnew System::Windows::Forms::ToolStripButton());
            this->ClearPoint = (gcnew System::Windows::Forms::ToolStripButton());
            this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->Ruler = (gcnew System::Windows::Forms::ToolStripButton());
            this->m_commonControls->SuspendLayout();
            this->SuspendLayout();
            // 
            // m_imageList
            // 
            this->m_imageList->ColorDepth = System::Windows::Forms::ColorDepth::Depth8Bit;
            this->m_imageList->ImageSize = System::Drawing::Size(100, 100);
            this->m_imageList->TransparentColor = System::Drawing::Color::Transparent;
            // 
            // m_imageListView
            // 
            this->m_imageListView->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(42)), static_cast<System::Int32>(static_cast<System::Byte>(42)),
                static_cast<System::Int32>(static_cast<System::Byte>(42)));
            this->m_imageListView->BorderStyle = System::Windows::Forms::BorderStyle::None;
            this->m_imageListView->CausesValidation = false;
            this->m_imageListView->Font = (gcnew System::Drawing::Font(L"Lucida Bright", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->m_imageListView->ForeColor = System::Drawing::SystemColors::HotTrack;
            this->m_imageListView->HideSelection = false;
            this->m_imageListView->LargeImageList = this->m_imageList;
            this->m_imageListView->Location = System::Drawing::Point(264, 1);
            this->m_imageListView->MultiSelect = false;
            this->m_imageListView->Name = L"m_imageListView";
            this->m_imageListView->Size = System::Drawing::Size(732, 136);
            this->m_imageListView->TabIndex = 0;
            this->m_imageListView->UseCompatibleStateImageBehavior = false;
            this->m_imageListView->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::m_imageListView_SelectedIndexChanged);
            // 
            // m_commonControls
            // 
            this->m_commonControls->Dock = System::Windows::Forms::DockStyle::None;
            this->m_commonControls->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->m_commonControls->ImageScalingSize = System::Drawing::Size(64, 64);
            this->m_commonControls->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(11) {
                this->m_selectWorkingFolder,
                    this->toolStripSeparator2, this->Reset1, this->Reset2, this->Reset3, this->toolStripSeparator1, this->AddPoint, this->DeletePoint,
                    this->ClearPoint, this->toolStripSeparator3, this->Ruler
            });
            this->m_commonControls->Location = System::Drawing::Point(279, 140);
            this->m_commonControls->Name = L"m_commonControls";
            this->m_commonControls->Size = System::Drawing::Size(472, 28);
            this->m_commonControls->TabIndex = 3;
            this->m_commonControls->Text = L"Tools";
            // 
            // m_selectWorkingFolder
            // 
            this->m_selectWorkingFolder->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->m_selectWorkingFolder->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"m_selectWorkingFolder.Image")));
            this->m_selectWorkingFolder->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->m_selectWorkingFolder->Name = L"m_selectWorkingFolder";
            this->m_selectWorkingFolder->Size = System::Drawing::Size(58, 25);
            this->m_selectWorkingFolder->Text = L"Folder";
            this->m_selectWorkingFolder->Click += gcnew System::EventHandler(this, &MainForm::SelectWorkingFolder_Click);
            // 
            // toolStripSeparator2
            // 
            this->toolStripSeparator2->Name = L"toolStripSeparator2";
            this->toolStripSeparator2->Size = System::Drawing::Size(6, 28);
            // 
            // Reset1
            // 
            this->Reset1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->Reset1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Reset1.Image")));
            this->Reset1->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->Reset1->Name = L"Reset1";
            this->Reset1->Size = System::Drawing::Size(61, 25);
            this->Reset1->Text = L"Reset1";
            this->Reset1->Click += gcnew System::EventHandler(this, &MainForm::Reset1_Click);
            // 
            // Reset2
            // 
            this->Reset2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->Reset2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Reset2.Image")));
            this->Reset2->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->Reset2->Name = L"Reset2";
            this->Reset2->Size = System::Drawing::Size(61, 25);
            this->Reset2->Text = L"Reset2";
            this->Reset2->Click += gcnew System::EventHandler(this, &MainForm::Reset2_Click);
            // 
            // Reset3
            // 
            this->Reset3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->Reset3->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Reset3.Image")));
            this->Reset3->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->Reset3->Name = L"Reset3";
            this->Reset3->Size = System::Drawing::Size(61, 25);
            this->Reset3->Text = L"Reset3";
            this->Reset3->Click += gcnew System::EventHandler(this, &MainForm::Reset3_Click);
            // 
            // toolStripSeparator1
            // 
            this->toolStripSeparator1->Name = L"toolStripSeparator1";
            this->toolStripSeparator1->Size = System::Drawing::Size(6, 28);
            // 
            // AddPoint
            // 
            this->AddPoint->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->AddPoint->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"AddPoint.Image")));
            this->AddPoint->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->AddPoint->Name = L"AddPoint";
            this->AddPoint->Size = System::Drawing::Size(42, 25);
            this->AddPoint->Text = L"Add";
            this->AddPoint->Click += gcnew System::EventHandler(this, &MainForm::AddPoint_Click);
            // 
            // DeletePoint
            // 
            this->DeletePoint->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->DeletePoint->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"DeletePoint.Image")));
            this->DeletePoint->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->DeletePoint->Name = L"DeletePoint";
            this->DeletePoint->Size = System::Drawing::Size(58, 25);
            this->DeletePoint->Text = L"Delete";
            this->DeletePoint->Click += gcnew System::EventHandler(this, &MainForm::DeletePoint_Click);
            // 
            // ClearPoint
            // 
            this->ClearPoint->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->ClearPoint->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"ClearPoint.Image")));
            this->ClearPoint->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->ClearPoint->Name = L"ClearPoint";
            this->ClearPoint->Size = System::Drawing::Size(50, 25);
            this->ClearPoint->Text = L"Clear";
            this->ClearPoint->Click += gcnew System::EventHandler(this, &MainForm::ClearPoint_Click);
            // 
            // toolStripSeparator3
            // 
            this->toolStripSeparator3->Name = L"toolStripSeparator3";
            this->toolStripSeparator3->Size = System::Drawing::Size(6, 28);
            // 
            // Ruler
            // 
            this->Ruler->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->Ruler->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Ruler.Image")));
            this->Ruler->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->Ruler->Name = L"Ruler";
            this->Ruler->Size = System::Drawing::Size(51, 25);
            this->Ruler->Text = L"Ruler";
            this->Ruler->Click += gcnew System::EventHandler(this, &MainForm::Ruler_Click);
            // 
            // MainForm
            // 
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
            this->ClientSize = System::Drawing::Size(1248, 691);
            this->Controls->Add(this->m_commonControls);
            this->Controls->Add(this->m_imageListView);
            this->DoubleBuffered = true;
            this->IsMdiContainer = true;
            this->Name = L"MainForm";
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
            this->Text = L"SHIFT-64";
            this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
            this->SizeChanged += gcnew System::EventHandler(this, &MainForm::MainForm_SizeChanged);
            this->m_commonControls->ResumeLayout(false);
            this->m_commonControls->PerformLayout();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion

private: void UpdateGUI()
{
    System::Drawing::Size clientSize =  this->ClientSize;

	if (clientSize.Width == 0 || clientSize.Height == 0)
		return;

    //1264 x 730
	int defaultClientWidth = 1264.0;
	int defaultClientHeight = 730.0;
    

    long appWidth = clientSize.Width;
	long appHeight = clientSize.Height;

    double scaleRatio = 1.0;
    if ( appWidth != defaultClientWidth)
	{
		scaleRatio = (double)appWidth / (double)defaultClientWidth;
	}

    long maxBorder = 4;
    maxBorder *= scaleRatio;

    //calculate control dimensions
    int controlWidth = 240;
    controlWidth *= scaleRatio;

    //Calculate image list dimensions
    int imageListHeight = 100;
    imageListHeight *= scaleRatio;
    //calculate toolbar dimensions
    int toolBarHeight = 24;
    toolBarHeight *= scaleRatio;

    //Slicer dimensions
    int slicerWidth = 460;// 440;
    slicerWidth *= scaleRatio;
	int slicerHeight = 440;
    slicerHeight *= scaleRatio;

    slicerWidth = (appWidth-controlWidth-8)/2;
    slicerWidth &= 0xfffffffc;//must be a multiple of 4

    
    int spaceBetweenSlicers = maxBorder;
    spaceBetweenSlicers *= scaleRatio;

    
	
	
    
    int slicerYOffset = imageListHeight + toolBarHeight + maxBorder*2;

    //font size
    double maxFontSize = 10.0;
	double newFontSize = (maxFontSize * scaleRatio);

   
   
//MainForm controls=========================================================================================

    //this is the image list control 
	this->m_imageListView->Size = System::Drawing::Size(appWidth-controlWidth-maxBorder, imageListHeight-maxBorder);
	this->m_imageListView->Location = System::Drawing::Point(controlWidth+2, 2);//upper left corner
	this->m_imageListView->Update();

    this->m_commonControls->Size = System::Drawing::Size(appWidth-controlWidth-maxBorder, toolBarHeight);
    this->m_commonControls->Location = System::Drawing::Point(controlWidth+2, imageListHeight + maxBorder);//upper left corner
    this->m_commonControls->Font = gcnew System::Drawing::Font("Microsoft Sans Seri", newFontSize, FontStyle::Regular);
//==========================================================================================================



//==========================================================================================================
#ifdef SLICER_AVAILABLE
    long window1 = m_View1->GetSlicer()->GetWindow();
    long level1 =  m_View1->GetSlicer()->GetLevel();
    long window2 = m_View2->GetSlicer()->GetWindow();
    long level2 =  m_View2->GetSlicer()->GetLevel();

    mvState* state1 = m_View1->GetSlicer()->GetVisualizer()->GetStateModelView();
    mvState* state2 = m_View2->GetSlicer()->GetVisualizer()->GetStateModelView();

    m_View1->ClientSize = System::Drawing::Size(slicerWidth, slicerHeight);
	m_View1->Location = System::Drawing::Point(controlWidth, slicerYOffset);
	m_View1->GetSlicer()->Initialize(slicerWidth, slicerHeight, "View1", false);
    m_View1->GetSlicer()->SetWindowLevel(window1, level1);
	m_View1->SetSelected(true);

	m_View1->GetSlicer()->GetVisualizer()->SetStateModelView(state1, m_View1->GetSlicer()->GetWidth(), m_View1->GetSlicer()->GetHeight());

	m_View1->Show();

	//Setup VIEW2
	//long slicer2Width = appWidth-controlWidth-slicerWidth-spaceBetweenSlicers;
	//slicer2Width &= 0xfffffffc;
	m_View2->ClientSize = System::Drawing::Size(slicerWidth, slicerHeight);
	m_View2->Location = System::Drawing::Point(controlWidth + slicerWidth + spaceBetweenSlicers, slicerYOffset);
	m_View2->GetSlicer()->Initialize(slicerWidth, slicerHeight, "View2", false);
    m_View2->GetSlicer()->SetWindowLevel(window2, level2);
	m_View2->SetSelected(false);

    m_View2->GetSlicer()->GetVisualizer()->SetStateModelView(state2, m_View1->GetSlicer()->GetWidth(), m_View2->GetSlicer()->GetHeight());

    m_View2->Show();
    m_View2->UpdateDisplay();
    m_View1->UpdateDisplay();

#endif//====================================================================================




}


public: void InitializeView1()
{

	long channels = 1;
	//setup default parameters
	AcqParam acq;										//parameters for acquisition
	CalParam cal;										//parameters for calibration
	acq.width = 300;
	acq.height = 300;
	acq.numFrames = 300;							    //number of images to capture
	acq.horizontalReflection = HIR_PowerupDefault;		//no image reflection
	acq.scanDirection = SD_Clockwise;					//counter clockwise scan
	acq.probeOrientation = PO_Top;						//bottom orientation
	acq.scanGeometry = SG_LinearRaw;        
	acq.sweepAngle = 0.0;								//no shearing

	cal.axisOfRotation = 0;//acq.width/2;					//center of ROI is center of rotation
	cal.inPlaneTilt = 0.0f;								//not currently used
	cal.outPlaneDisplacement = 0.0f;					//not currently used
	cal.outPlaneTilt = 0.0f;							//not currently used
	cal.probeDistance = 0.0;								//not currently used
	cal.xVoxelSize = 0.15;
	cal.yVoxelSize = 0.15;
	cal.zVoxelSize = 0.15f;						//size of voxel in z direction (mm)

	m_View1->GetSlicer()->CreateVolume(acq,cal,channels);
	m_View1->GetSlicer()->ResetCube(2);
	m_View1->GetSlicer()->GetVisualizer()->Zoom(0.7f);


}

public: void InitializeView2()
{

	long channels = 1;
	//setup default parameters
	AcqParam acq;										//parameters for acquisition
	CalParam cal;										//parameters for calibration
	acq.width = 300;
	acq.height = 300;
#ifdef FRAMEGRABBER_AVAILABLE
	//acq.width = m_frameGrabber->GetROIWidth();								//width of ROI (video sub region)
	//acq.height = m_frameGrabber->GetROIHeight();								//height of ROI (video sub region)
#endif
	acq.numFrames = 300;							    //number of images to capture
	acq.horizontalReflection = HIR_PowerupDefault;		//no image reflection
	acq.scanDirection = SD_Clockwise;					//counter clockwise scan
	acq.probeOrientation = PO_Top;						//bottom orientation
	acq.scanGeometry = SG_LinearRaw;        
	acq.sweepAngle = 0.0;								//no shearing

	cal.axisOfRotation = 0;//acq.width/2;					//center of ROI is center of rotation
	cal.inPlaneTilt = 0.0f;								//not currently used
	cal.outPlaneDisplacement = 0.0f;					//not currently used
	cal.outPlaneTilt = 0.0f;							//not currently used
	cal.probeDistance = 0.0;								//not currently used
	cal.xVoxelSize = 1.0f;
	cal.yVoxelSize = 1.0f;
	cal.zVoxelSize = 1.0f;						//size of voxel in z direction (mm)

	m_View2->GetSlicer()->CreateVolume(acq,cal,channels);
	m_View2->GetSlicer()->ResetCube(2);
	m_View2->GetSlicer()->GetVisualizer()->Zoom(0.7f);
	//m_View1->UpdateDisplay();

}


private: System::Void MainForm_SizeChanged(System::Object^  sender, System::EventArgs^  e) 
{

    UpdateGUI();
}

//==============================================================================================
public: void Slicer_Event(Object^ sender, EventArgs^ e)
{
	String^ code = (String^)sender;
    if (code == "View1SlicerSelected")
	{
        

        //m_linkCubes = false;
        //m_extraControls->LinkImagesButton->Text = "Link";
        
        if (m_View1->IsSelected())
        {
            m_View1->Focus();
        }
        else
        {
            m_View1->SetSelected(true);
		    m_View2->SetSelected(false);

		    m_View1->GetSlicer()->StopAddingPoints();
		    m_View2->GetSlicer()->StopAddingPoints();
		
		    //UpdateSurfaceList();
		    //UpdateSurfaceView(true);

		    //ResetStates();

            //m_segControls->Focus();
            m_View1->Focus();
            m_View1->GetSlicer()->EnableSlicing();
            

        }
		
		
	}
	else
	if (code == "View2SlicerSelected")
	{
        
        //m_linkCubes = false;
        //m_extraControls->LinkImagesButton->Text = "Link";

        if (m_View2->IsSelected())
        {
            m_View2->Focus();
        }
        else
        {
            m_View1->SetSelected(false);
		    m_View2->SetSelected(true);

		    m_View1->GetSlicer()->StopAddingPoints();
		    m_View2->GetSlicer()->StopAddingPoints();

		    //UpdateSurfaceList();
		    //UpdateSurfaceView(true);

		    //ResetStates();
            m_View2->Focus();
            m_View2->GetSlicer()->EnableSlicing();
            

        }
        
	}

    m_View1->UpdateDisplay();
    m_View2->UpdateDisplay();
	
}

bool SelectWorkingFolder()
{

//select working folder and download the files

	//// Show the FolderBrowserDialog.
      System::Windows::Forms::DialogResult result = m_folderBrowserDialog1->ShowDialog();

	  
      if ( result == System::Windows::Forms::DialogResult::OK )
      {
			m_defaultFolder = m_folderBrowserDialog1->SelectedPath;
			m_defaultFolder += "/";
			m_folderToMonitor = m_defaultFolder;//
			
      }
	  else
	  {
		  //do nothing
		  return false;
	  }

      
      return true;

}

void MonitorFolderEventProcessor( Object^ /*myObject*/, EventArgs^ /*myEventArgs*/ )
{
	//MonitorFolderForDicom();//monitor folder for new dicom images
	MonitorForNewScans();//monitor folder for new 3D ultrasound scans
	//m_monitorFolderTimer->Stop();
}

void MonitorForNewScans()
{

	int c = 0;

    //look for .raw images
	{
		String^ path = m_folderToMonitor;

		String^ searchPattern = "*.raw";//look for these kind of files
		DirectoryInfo^ di = gcnew DirectoryInfo(path);
		array<DirectoryInfo^>^ directories = di->GetDirectories(searchPattern, SearchOption::TopDirectoryOnly);
		array<FileInfo^>^ files = di->GetFiles(searchPattern, SearchOption::TopDirectoryOnly);

		//block for loading in all the files
		{
			for each (FileInfo^ file in files)//for each directory
			{
				//call thread safe function
				bool found = false;

				//check to see if we already loaded this dicom file
				for each (String^ text in this->m_fileList)//NOTE: m_fileList is created during AddDICOMImage so only images that are "added" are put on the list
				{
					if (text == file->FullName)
					{
						found = true;
					}
				}

				if (!found)
				{
					this->AddRAWImage(file->FullName, file->Name, m_imageCount);
	
				}

			}

		}
		
	}

    //look for .mha
	{
		String^ path = m_folderToMonitor;

		String^ searchPattern = "*.mha";//look for these kind of files
		DirectoryInfo^ di = gcnew DirectoryInfo(path);
		array<DirectoryInfo^>^ directories = di->GetDirectories(searchPattern, SearchOption::TopDirectoryOnly);
		array<FileInfo^>^ files = di->GetFiles(searchPattern, SearchOption::TopDirectoryOnly);

		//block for loading in all the files
		{
			for each (FileInfo^ file in files)//for each directory
			{
				//call thread safe function
				bool found = false;

				//check to see if we already loaded this dicom file
				for each (String^ text in this->m_fileList)//NOTE: m_fileList is created during AddDICOMImage so only images that are "added" are put on the list
				{
					if (text == file->FullName)
					{
						found = true;
					}
				}

				if (!found)
				{
					this->AddMHAImage(file->FullName, file->Name, m_imageCount);
	
				}

			}

		}
		
	}

 

     
}


void MonitorFolderForDicom()
{
	if (m_monitorIsRunning) return;//already running

	m_monitorIsRunning = true;

	int c = 0;

	{
		String^ path = m_folderToMonitor;

		String^ searchPattern = "*.*";//look for these kind of files
		DirectoryInfo^ di = gcnew DirectoryInfo(path);
		array<DirectoryInfo^>^ directories = di->GetDirectories(searchPattern, SearchOption::TopDirectoryOnly);
		array<FileInfo^>^ files = di->GetFiles(searchPattern, SearchOption::TopDirectoryOnly);

		//block for loading in all the files
		{
			for each (DirectoryInfo^ directory in directories)//for each directory
			{
				//call thread safe function
				bool found = false;

				//check to see if we already loaded this dicom file
				for each (String^ text in this->m_fileList)//NOTE: m_fileList is created during AddDICOMImage so only images that are "added" are put on the list
				{
					if (text == directory->FullName)
					{
						found = true;
					}
				}

				if (!found)
				{
					this->AddDICOMImage(directory->FullName, directory->Name, m_imageCount);
				}

			}

		}
		
	}

	m_monitorIsRunning = false;
}

//thread safe function to add images to the image list box
bool AddRAWImage(String^ filePath, String^ fileName, int imageIndex)
{
	
	if (this->m_imageListView->InvokeRequired)
	{
		AddRAWImageDelegate^ d = gcnew AddRAWImageDelegate(this, &MainForm::AddRAWImage);
		this->Invoke(d, gcnew array<Object^> { filePath, fileName, imageIndex });

	}
	else
	{
		IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
		std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());


		if (0 == m_miniSlicer->GetSlicer()->ImportRaw(convertedFilePath))
		{
			//------------------------------------------------------------------------------
			//Slice into cube half way for display in image list
			//m_miniSlicer->GetSlicer()->SetModelView(-1, 0.001f, 0.001f, 0.001f, false);//don't reset zoom
			int width = m_miniSlicer->GetSlicer()->GetWidth();
			int height = m_miniSlicer->GetSlicer()->GetHeight();
			m_miniSlicer->GetSlicer()->ClearSurfaces();
			

			Vector3 cubeSize = m_miniSlicer->GetSlicer()->GetVisualizer()->GetCubeSize();
			//double distance = -(cubeSize.x/2.0) - centroid.x; 
			m_miniSlicer->GetSlicer()->ResetCube(2);
			//m_miniSlicer->GetSlicer()->Slice(3, distance);//slice in x-direction using face 3

			{
				std::string importFolder = m_miniSlicer->GetSlicer()->GetVolumeFolderPath();
				String^ convertedString = gcnew String(importFolder.c_str());
				String^ filePath = convertedString;// + "SE3.xml";

				IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
				IntPtr ptrToNativeString2 = Marshal::StringToHGlobalAnsi(filePath);

				std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());
				std::string convertedFolderPath = static_cast<char*>(ptrToNativeString2.ToPointer());

				convertedFilePath += "/" + m_miniSlicer->GetSlicer()->GetVolumeLabel() + ".xml";

				m_miniSlicer->GetSlicer()->ImportSurfaces(convertedFolderPath, convertedFilePath);
	
					
			}

			//rename folder with name of series description
			String^ indexString = imageIndex.ToString();
			IntPtr ptrToNativeIndexString = Marshal::StringToHGlobalAnsi(indexString);
			std::string convertedIndexString = static_cast<char*>(ptrToNativeIndexString.ToPointer());
			std::string newFolderName = m_miniSlicer->GetSlicer()->GetVolumeFolderPath() + convertedIndexString  + "_" + m_miniSlicer->GetSlicer()->GetDisplayText();
			//rename(filePath, newFolderName);
			m_miniSlicer->GetSlicer()->GetVisualizer()->Zoom(0.7);
			m_miniSlicer->GetSlicer()->UpdateDisplay();
			m_miniSlicer->UpdateDisplay();
			//------------------------------------------------------------------------------

			//add to image list-------------------------------------------------------------
			unsigned char* buffer = m_miniSlicer->GetSlicer()->GetBitmapBits();
			int stride = 4 * ((width * 3 + 3) / 4); // must be multiple of 4
			Bitmap^ localBitmap = gcnew Bitmap(width, height, stride,  PixelFormat::Format24bppRgb, System::IntPtr(buffer));
			this->m_imageList->Images->Add(localBitmap);
			this->m_imageListView->Items->Add(fileName, imageIndex);//add a text label and the index of the image from the imagelist
			this->m_fileList->Add(filePath);
			//------------------------------------------------------------------------------

			m_imageCount++;//successfully added image to list
		}
		else
		{
			
		}

	}

	return true;

}


//thread safe function to add images to the image list box
bool AddMHAImage(String^ filePath, String^ fileName, int imageIndex)
{
	
	if (this->m_imageListView->InvokeRequired)
	{
		AddMHAImageDelegate^ d = gcnew AddMHAImageDelegate(this, &MainForm::AddMHAImage);
		this->Invoke(d, gcnew array<Object^> { filePath, fileName, imageIndex });

	}
	else
	{
		IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
		std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());


		if (0 == m_miniSlicer->GetSlicer()->ImportMHA(convertedFilePath))
		{
			//------------------------------------------------------------------------------
			//Slice into cube half way for display in image list
			//m_miniSlicer->GetSlicer()->SetModelView(-1, 0.001f, 0.001f, 0.001f, false);//don't reset zoom
			int width = m_miniSlicer->GetSlicer()->GetWidth();
			int height = m_miniSlicer->GetSlicer()->GetHeight();
			m_miniSlicer->GetSlicer()->ClearSurfaces();
			
			m_miniSlicer->GetSlicer()->ResetCube(2);

			//m_miniSlicer->GetSlicer()->Slice(3, distance);//slice in x-direction using face 3

			


			{
				std::string importFolder = m_miniSlicer->GetSlicer()->GetVolumeFolderPath();
				String^ convertedString = gcnew String(importFolder.c_str());
				String^ filePath = convertedString;// + "SE3.xml";

				IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
				IntPtr ptrToNativeString2 = Marshal::StringToHGlobalAnsi(filePath);

				std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());
				std::string convertedFolderPath = static_cast<char*>(ptrToNativeString2.ToPointer());

				convertedFilePath += "/" + m_miniSlicer->GetSlicer()->GetVolumeLabel() + ".xml";

				m_miniSlicer->GetSlicer()->ImportSurfaces(convertedFolderPath, convertedFilePath);
	
					
			}

			//rename folder with name of series description
			String^ indexString = imageIndex.ToString();
			IntPtr ptrToNativeIndexString = Marshal::StringToHGlobalAnsi(indexString);
			std::string convertedIndexString = static_cast<char*>(ptrToNativeIndexString.ToPointer());
			std::string newFolderName = m_miniSlicer->GetSlicer()->GetVolumeFolderPath() + convertedIndexString  + "_" + m_miniSlicer->GetSlicer()->GetDisplayText();
			//rename(filePath, newFolderName);
			m_miniSlicer->GetSlicer()->GetVisualizer()->Zoom(0.7);
			m_miniSlicer->GetSlicer()->UpdateDisplay();
			m_miniSlicer->UpdateDisplay();
			//------------------------------------------------------------------------------

			//add to image list-------------------------------------------------------------
			unsigned char* buffer = m_miniSlicer->GetSlicer()->GetBitmapBits();
			int stride = 4 * ((width * 3 + 3) / 4); // must be multiple of 4
			Bitmap^ localBitmap = gcnew Bitmap(width, height, stride,  PixelFormat::Format24bppRgb, System::IntPtr(buffer));
			this->m_imageList->Images->Add(localBitmap);
			this->m_imageListView->Items->Add(fileName, imageIndex);//add a text label and the index of the image from the imagelist
			this->m_fileList->Add(filePath);
			//------------------------------------------------------------------------------

			m_imageCount++;//successfully added image to list
		}
		else
		{
			
		}

	}

	return true;

}

//thread safe function to add images to the image list box
bool AddDICOMImage(String^ filePath, String^ fileName, int imageIndex)
{
	
	if (this->m_imageListView->InvokeRequired)
	{
		AddDICOMImageDelegate^ d = gcnew AddDICOMImageDelegate(this, &MainForm::AddDICOMImage);
		this->Invoke(d, gcnew array<Object^> { filePath, fileName, imageIndex });

	}
	else
	{

		if (filePath->Contains("Surfaces"))
		{
			//do nothing
		}
		else
		{

			String^ folderPath = filePath->Replace('\\', '/') + "/";
			DirectoryInfo^ di = gcnew DirectoryInfo(folderPath);
			int fileCount = di->GetFiles("*.*", SearchOption::TopDirectoryOnly)->Length;

            /*
			if (folderPath->Contains("TEMPMAP"))
			{
				//do nothing for now
				//m_miniSlicer->ImportThermometry(folderPath, 10, 20);//test
                m_miniSlicer->ImportDicomSingle(folderPath);

			}
			else
            */

			if (0 == m_miniSlicer->ImportDicomSingle(folderPath) && fileCount >2)
			{

                MessageBox::Show(folderPath);
				//------------------------------------------------------------------------------
				//Slice into cube half way for display in image list
				m_miniSlicer->GetSlicer()->SetModelView(-1, 0.001f, 0.001f, 0.001f, false);//don't reset zoom
				int width = m_miniSlicer->GetSlicer()->GetWidth();
				int height = m_miniSlicer->GetSlicer()->GetHeight();
				Vector3 depth = m_miniSlicer->GetSlicer()->GetVisualizer()->GetCubeSize();
				int px = width/2;
				int py = height/2;
				int activeFace = m_miniSlicer->GetSlicer()->GetVisualizer()->GetFaceID(px,py);
				m_miniSlicer->GetSlicer()->Slice(activeFace, -depth.z/2);//slice half way into cube

				{
					std::string importFolder = m_miniSlicer->GetSlicer()->GetVolumeFolderPath();
					String^ convertedString = gcnew String(importFolder.c_str());
					String^ filePath = convertedString;// + "SE3.xml";

					IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
					IntPtr ptrToNativeString2 = Marshal::StringToHGlobalAnsi(filePath);

					std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());
					std::string convertedFolderPath = static_cast<char*>(ptrToNativeString2.ToPointer());

					convertedFilePath += "/" + m_miniSlicer->GetSlicer()->GetVolumeLabel() + ".xml";

					m_miniSlicer->GetSlicer()->ImportSurfaces(convertedFolderPath, convertedFilePath);
					m_miniSlicer->GetSlicer()->ImportMetaData(convertedFolderPath, convertedFilePath);

					
					
				}

				//rename folder with name of series description
				String^ indexString = imageIndex.ToString();
				IntPtr ptrToNativeIndexString = Marshal::StringToHGlobalAnsi(indexString);
				std::string convertedIndexString = static_cast<char*>(ptrToNativeIndexString.ToPointer());
				std::string newFolderName = m_miniSlicer->GetSlicer()->GetVolumeFolderPath() + convertedIndexString  + "_" + m_miniSlicer->GetSlicer()->GetDisplayText();
				//rename(filePath, newFolderName);

				m_miniSlicer->GetSlicer()->UpdateDisplay();
				m_miniSlicer->UpdateDisplay();
				//------------------------------------------------------------------------------

				//add to image list-------------------------------------------------------------
				unsigned char* buffer = m_miniSlicer->GetSlicer()->GetBitmapBits();
				int stride = 4 * ((width * 3 + 3) / 4); // must be multiple of 4
				Bitmap^ localBitmap = gcnew Bitmap(width, height, stride,  PixelFormat::Format24bppRgb, System::IntPtr(buffer));
				this->m_imageList->Images->Add(localBitmap);
				this->m_imageListView->Items->Add(fileName, imageIndex);//add a text label and the index of the image from the imagelist
				this->m_fileList->Add(filePath);
				//------------------------------------------------------------------------------

				m_imageCount++;//successfully added image to list

			}
			else
			{
				
			}

			
		}
		
	}

	return true;

}

std::string ConvertString(String^ data)
{
    IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(data);
	std::string convertedData = static_cast<char*>(ptrToNativeString1.ToPointer());

    return convertedData;

}
void LoadSelectedImage()
{
    int selectedIndex = -1;
	int count = m_imageListView->SelectedItems->Count;
	if (count == 1)
	{
		selectedIndex = m_imageListView->FocusedItem->Index;
	}
    RRI_SlicerInterface* slicer = 0;

    if (m_View1->IsSelected())
    {
        slicer = m_View1->GetSlicer();

    }
    else
    {
        slicer = m_View2->GetSlicer();
    }


	{
	
		if (selectedIndex != -1)
		{
			String^ filePath = m_fileList[selectedIndex];

            //check to see if this is not one of the other supported formats
			if (!filePath->Contains(".raw") && !filePath->Contains(".mha") && !filePath->Contains(".bmp") && !filePath->Contains(".jpg"))
			{//here we assume this is a DICOM file
                filePath = filePath->Replace('\\', '/');
				filePath += "/";

                {   
                    
                    std::string convertedFilePath = ConvertString(filePath);
                    slicer->ImportDicom(convertedFilePath.c_str());
                    //vtkImageData* vtkVolume = m_surfaceView1->GetVolume();
				    //ConvertView1(vtkVolume);//update vtkImageData::Volume in surface view
                    //ImportDefaultSurfaces();//from this view
                }
				
			}
			else
            if (filePath->Contains(".raw"))
			{

				IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
				std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());
				slicer->ImportRaw(convertedFilePath);
                slicer->SetWindowLevel(255, 128);
                //NOTE: there was a bug where and ultrasound (8-bit) iamge would not load after loading an MRI image.
                //Setting window and level here fixed that problem.
                slicer->SetWindowLevel(255, 128);
				//vtkImageData* vtkVolume = m_surfaceView1->GetVolume();
				//ConvertView1(vtkVolume);//update vtkImageData::Volume in surface view
                //ImportDefaultSurfaces();//from this view



				
			}
            else
            if (filePath->Contains(".mha"))
            {
                IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
				std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());
				slicer->ImportMHA(convertedFilePath);
                slicer->SetWindowLevel(255, 128);
				//vtkImageData* vtkVolume = m_surfaceView1->GetVolume();
				//ConvertView1(vtkVolume);//update vtkImageData::Volume in surface view
                //ImportDefaultSurfaces();//from this view
            }
            else
            if (filePath->Contains(".jpg") || filePath->Contains(".bmp"))
            {

                filePath = filePath->Replace('/', '\\');
                long length = filePath->Length;
                String^ filePathModified = filePath->Remove(length-1);
                //filePath = filePathModified;

                IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
                std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());

                //Load Bitmap
                Bitmap^ localBitmap = gcnew Bitmap(filePath);
                int width = localBitmap->Width;
                int height = localBitmap->Height;
                int imageCount = 3;//m_View1->GetSlicer()->GetVolumeDepth();
                int size = width*height;
                int depth = localBitmap->GetPixelFormatSize(localBitmap->PixelFormat);


                String^ filePath = m_folderToMonitor + "PixelDimensions.txt";
                //float pixelValue = HelperReadPixelDimensions(filePath);
                float voxelX = 1.0;//pixelValue;// m_segControls->GetVoxelSize();//m_View1->GetSlicer()->GetVoxelX();
                float voxelY = 1.0;//pixelValue;// m_segControls->GetVoxelSize();//m_View1->GetSlicer()->GetVoxelY();
                float voxelZ = 1.0;//m_View1->GetSlicer()->GetVoxelZ();

                long channels = 1;
                PixelFormat pixelFormat = localBitmap->PixelFormat;
                if (pixelFormat == System::Drawing::Imaging::PixelFormat::Format24bppRgb)
                {
                    channels = 3;
                }

                //Lock Bits
                // Create rectangle to lock
                BitmapData^ bmpData = localBitmap->LockBits(System::Drawing::Rectangle(0,0,width,height), ImageLockMode::ReadWrite, pixelFormat);
                Byte* pixelArray = (Byte*)(void*)bmpData->Scan0;

                slicer->CreateVolumeFromBitmap((unsigned char*)pixelArray, width, height, imageCount,
                                                            voxelX, 
                                                            voxelY, 
                                                            voxelZ, 128, 256, channels);



               slicer->UpdateVolumeInfo(convertedFilePath);//sets the folder path, file path and label of the volume

                //vtkImageData* vtkVolume = m_surfaceView1->GetVolume();
                //ConvertView1(vtkVolume);//update vtkImageData::Volume in surface view
                //m_View1->GetSlicer()->SetTargetSurfaceRadius(m_View1->GetSlicer()->GetVoxelX()*12);
                //ImportDefaultSurfaces();//from this view

            }

#ifdef SHARED_CONTROLS_EVENT_HANDLER
    System::EventArgs^  e;
	SharedControls_Event("AxialResetEvent", e);
#endif 

		}

        m_imageListView->SelectedItems->Clear();
	}
	
}

void ResetDefault()
{
    RRI_SlicerInterface* slicer = 0;
    if (m_View1->IsSelected())
    {

        slicer = m_View1->GetSlicer();

    }
    else
    {
        slicer = m_View2->GetSlicer();
    }


	{
		Vector3 cubeSize = slicer->GetVisualizer()->GetCubeSize();
			
		Vector3 centroid = Vector3(0,0,0);
            
        //m_View1->GetSlicer()->CalculateCentroid(currentSelection);

		if (   slicer->GetScanGeometry() == SG_Linear16 
            || slicer->GetScanGeometry() == SG_LinearRaw 
            || slicer->GetScanGeometry() == SG_Linear24
            || slicer->GetScanGeometry() == SG_AxialNoOffsets )
		{
			slicer->ResetCube(1);
			double distance = -(cubeSize.z/2.0) - centroid.z + slicer->GetVoxelZ(); 
			slicer->Slice(0, (float)distance);

		}
		else
		if (slicer->GetScanGeometry() == SG_Hybrid || slicer->GetScanGeometry() == SG_Hybrid24)
		{
				
			{
				slicer->ResetCube(2);
				double distance = -(cubeSize.x/2.0) - centroid.x; 
				slicer->Slice(3, distance);//slice in x-direction using face 3

			}

		}

		slicer->GetVisualizer()->Zoom(0.7);
        //***slicer->UpdateLinkingPoints();
			

        if (m_View1->IsSelected())
        {
            m_View1->SetMode(MODE_SLICER);
            m_View1->Focus();//put focus to this window
            m_View1->UpdateDisplay();
        }
        else
        {
            m_View2->SetMode(MODE_SLICER);
            m_View2->Focus();//put focus to this window
            m_View2->UpdateDisplay();

        }

		
		
		
    }

}


void StopFileMonitor()
{
	m_monitorFolderTimer->Stop();
}

void StartFileMonitor()
{
	m_monitorFolderTimer->Start();
}


//==============================================================================================
//Reset Events
//==============================================================================================
void ViewReset1()
{
    RRI_SlicerInterface* slicer = 0;
    if (m_View1->IsSelected())
    {
        slicer = m_View1->GetSlicer();
    }
    else
    {
        slicer = m_View2->GetSlicer();
    }

	Vector3 cubeSize = slicer->GetVisualizer()->GetCubeSize();
			
	Vector3 centroid = Vector3(0,0,0);
            
    //slicer->CalculateCentroid(currentSelection);

	if (   slicer->GetScanGeometry() == SG_Linear16 
        || slicer->GetScanGeometry() == SG_LinearRaw 
        || slicer->GetScanGeometry() == SG_Linear24
        || slicer->GetScanGeometry() == SG_AxialNoOffsets )
	{
		slicer->ResetCube(1);
		double distance = -(cubeSize.z/2.0) - centroid.z + slicer->GetVoxelZ(); 
		slicer->Slice(0, (float)distance);

	}
	else
	if (slicer->GetScanGeometry() == SG_Hybrid || slicer->GetScanGeometry() == SG_Hybrid24)
	{
				
		{
			slicer->ResetCube(2);
			double distance = -(cubeSize.x/2.0) - centroid.x; 
			slicer->Slice(3, distance);//slice in x-direction using face 3

		}

	}

	slicer->GetVisualizer()->Zoom(0.7);
    slicer->UpdateLinkingPoints();
			

    if (m_View1->IsSelected())
    {
        m_View1->SetMode(MODE_SLICER);
        m_View1->UpdateDisplay();
    }
    else
    {
        m_View2->SetMode(MODE_SLICER);
        m_View2->UpdateDisplay();
    }
}

void ViewReset2()
{
    RRI_SlicerInterface* slicer = 0;
    if (m_View1->IsSelected())
    {
        slicer = m_View1->GetSlicer();
    }
    else
    {
        slicer = m_View2->GetSlicer();
    }

	Vector3 cubeSize = slicer->GetVisualizer()->GetCubeSize();
			
	Vector3 centroid = Vector3(0,0,0);
            
    //slicer->CalculateCentroid(currentSelection);

	if (   slicer->GetScanGeometry() == SG_Linear16 
        || slicer->GetScanGeometry() == SG_LinearRaw 
        || slicer->GetScanGeometry() == SG_Linear24
        || slicer->GetScanGeometry() == SG_AxialNoOffsets )
	{
		slicer->ResetCube(2);
		double distance = -(cubeSize.x/2.0) - centroid.x; 
		slicer->Slice(3, distance);

	}
	else
	if (slicer->GetScanGeometry() == SG_Hybrid || slicer->GetScanGeometry() == SG_Hybrid24)
	{
				
		{
			slicer->ResetCube(1);
			double distance = -(cubeSize.z/2.0) - centroid.z; 
			slicer->Slice(0, distance);//slice in x-direction using face 3

		}

	}

	slicer->GetVisualizer()->Zoom(0.7);
    slicer->UpdateLinkingPoints();
			

    if (m_View1->IsSelected())
    {
        m_View1->SetMode(MODE_SLICER);
        m_View1->UpdateDisplay();
    }
    else
    {
        m_View2->SetMode(MODE_SLICER);
        m_View2->UpdateDisplay();
    }

}

void ViewReset3()
{
    RRI_SlicerInterface* slicer = 0;
    if (m_View1->IsSelected())
    {
        slicer = m_View1->GetSlicer();
    }
    else
    {
        slicer = m_View2->GetSlicer();
    }

	Vector3 cubeSize = slicer->GetVisualizer()->GetCubeSize();
			
	Vector3 centroid = Vector3(0,0,0);
            
    //slicer->CalculateCentroid(currentSelection);

	if (   slicer->GetScanGeometry() == SG_Linear16 
        || slicer->GetScanGeometry() == SG_LinearRaw 
        || slicer->GetScanGeometry() == SG_Linear24
        || slicer->GetScanGeometry() == SG_AxialNoOffsets )
	{
		slicer->ResetCube(3);
		double distance = -(cubeSize.y/2.0) - centroid.y; 
		slicer->Slice(4, distance);

	}
	else
	if (slicer->GetScanGeometry() == SG_Hybrid || slicer->GetScanGeometry() == SG_Hybrid24)
	{
				
		{
			slicer->ResetCube(3);
			double distance = -(cubeSize.y/2.0) - centroid.y; 
			slicer->Slice(4, distance);//slice in x-direction using face 3

		}

	}

	slicer->GetVisualizer()->Zoom(0.7);
    slicer->UpdateLinkingPoints();
			

    if (m_View1->IsSelected())
    {
        m_View1->SetMode(MODE_SLICER);
        m_View1->UpdateDisplay();
    }
    else
    {
        m_View2->SetMode(MODE_SLICER);
        m_View2->UpdateDisplay();
    }

}

//==============================================================================================
//Toolbar Events
//==============================================================================================

private: System::Void m_imageListView_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
{
    LoadSelectedImage();
    ResetDefault();

}

private: System::Void SelectWorkingFolder_Click(System::Object^  sender, System::EventArgs^  e) 
{
    if (true == SelectWorkingFolder())
    {
        MonitorFolderForDicom();//monitor folder for new dicom images
	    MonitorForNewScans();//monitor folder for new 3D ultrasound scans
    }
}
private: System::Void Reset1_Click(System::Object^  sender, System::EventArgs^  e) 
{
    ViewReset1();
}
private: System::Void Reset2_Click(System::Object^  sender, System::EventArgs^  e) 
{
    ViewReset2();
}
private: System::Void Reset3_Click(System::Object^  sender, System::EventArgs^  e) 
{
    ViewReset3();
}
private: System::Void AddPoint_Click(System::Object^  sender, System::EventArgs^  e) 
{
    if (m_View1->IsSelected())
	{
		m_View1->SetMode(MODE_ADD_POINT);
	}
	else
	if (m_View2->IsSelected())
	{
		m_View2->SetMode(MODE_ADD_POINT);
	}
}
private: System::Void DeletePoint_Click(System::Object^  sender, System::EventArgs^  e) 
{
    if (m_View1->IsSelected())
    {
        m_View1->SetMode(MODE_DELETE_POINT);

    }
    else
    if (m_View2->IsSelected())
    {
        m_View2->SetMode(MODE_DELETE_POINT);

    }


}
private: System::Void ClearPoint_Click(System::Object^  sender, System::EventArgs^  e) 
{
    if (m_View1->IsSelected())
    {
        m_View1->GetSlicer()->ClearTargets();//clears targets and surfaces
        m_View1->GetSlicer()->StopAddingPoints();
        m_View1->UpdateDisplay();

    }
    else
    if (m_View2->IsSelected())
    {
        m_View2->GetSlicer()->ClearTargets();//clears targets and surfaces
        m_View2->GetSlicer()->StopAddingPoints();
        m_View2->UpdateDisplay();

    }

}
private: System::Void Ruler_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if (m_View1->IsSelected())
	{
		m_View1->GetSlicer()->StartMeasurement(MLINE);
		m_View1->UpdateDisplay();

	}
	else
	if (m_View2->IsSelected())
	{
		m_View2->GetSlicer()->StartMeasurement(MLINE);
		m_View2->UpdateDisplay();
	
	}

}

//==============================================================================================

};
}
