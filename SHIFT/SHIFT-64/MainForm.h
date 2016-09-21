#pragma once

#include "CompileSwitches.h"

#define SLICER_AVAILABLE
#ifdef SLICER_AVAILABLE
#include "SlicerForm.h"
#endif

#define ANDREW_CONTROLS
#ifdef ANDREW_CONTROLS
    #include "AndrewControls.h"
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
    //Andrew Controls
    #ifdef ANDREW_CONTROLS
	public: AndrewControls^ m_mainControls;
    #endif
    //====================================================================================================


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

    private: System::Windows::Forms::ToolStripButton^  zoomIN;
    private: System::Windows::Forms::ToolStripButton^  zoomOUT;

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


#ifdef ANDREW_CONTROLS
	        m_mainControls = gcnew AndrewControls();
			m_mainControls->MdiParent = this;
			m_mainControls->OnAndrewControlsEvent += gcnew EventHandler(this, &SHIFT64::MainForm::AndrewControls_Event); 
			m_mainControls->Hide();
			
#endif



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
            this->zoomIN = (gcnew System::Windows::Forms::ToolStripButton());
            this->zoomOUT = (gcnew System::Windows::Forms::ToolStripButton());
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
            this->m_commonControls->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(13) {
                this->m_selectWorkingFolder,
                    this->toolStripSeparator2, this->Reset1, this->Reset2, this->Reset3, this->toolStripSeparator1, this->AddPoint, this->DeletePoint,
                    this->ClearPoint, this->toolStripSeparator3, this->Ruler, this->zoomIN, this->zoomOUT
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
            // Zoom IN
            // 
            this->zoomIN->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->zoomIN->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"zoomIN.Image")));
            this->zoomIN->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->zoomIN->Name = L"ZoomIN";
            this->zoomIN->Size = System::Drawing::Size(51, 25);
            this->zoomIN->Text = L"Zoom IN";
            this->zoomIN->Click += gcnew System::EventHandler(this, &MainForm::ZoomIN_Click);

            // 
            // Zoom OUT
            // 
            this->zoomOUT->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->zoomOUT->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"zoomOUT.Image")));
            this->zoomOUT->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->zoomOUT->Name = L"ZoomOUT";
            this->zoomOUT->Size = System::Drawing::Size(51, 25);
            this->zoomOUT->Text = L"Zoom OUT";
            this->zoomOUT->Click += gcnew System::EventHandler(this, &MainForm::ZoomOUT_Click);



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

    
    //get dimensions of application
    long appWidth = clientSize.Width;
	long appHeight = clientSize.Height;

    double scaleRatio = 1.0;
    if ( appWidth != defaultClientWidth)
	{
		scaleRatio = (double)appWidth / (double)defaultClientWidth;
	}

    long maxBorder = 4;
    maxBorder *= scaleRatio;

    appWidth -= (maxBorder*3);
    appHeight -= (maxBorder*3);

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
	int slicerHeight = appHeight-imageListHeight-toolBarHeight - (maxBorder*2);


    slicerWidth = (appWidth-controlWidth-(maxBorder*2))/2;
    slicerWidth &= 0xfffffffc;//must be a multiple of 4
    int spaceBetweenSlicers = maxBorder;
    spaceBetweenSlicers *= scaleRatio;


    int slicerYOffset = imageListHeight + toolBarHeight + maxBorder*2;
    //slicerYOffset *= scaleRatio;
    

    //control dimensions
    long controlPosX = maxBorder;
	long controlPosY = maxBorder;//slicerYOffset;
	long controlHeight = appHeight - controlPosY - maxBorder;
    

    //font size
    double maxFontSize = 10.0;
	double newFontSize = (maxFontSize * scaleRatio);

   
   
//MainForm controls=========================================================================================

    //this is the image list control 
	this->m_imageListView->Size = System::Drawing::Size(appWidth-controlWidth-(maxBorder*2), imageListHeight-(maxBorder*2));
	this->m_imageListView->Location = System::Drawing::Point(controlWidth + (maxBorder*2), maxBorder*2);//upper left corner
	this->m_imageListView->Update();

    this->m_commonControls->Size = System::Drawing::Size(appWidth-controlWidth-(maxBorder*2), toolBarHeight);
    this->m_commonControls->Location = System::Drawing::Point(controlWidth+(maxBorder*2), imageListHeight+(maxBorder*2));//upper left corner
    this->m_commonControls->Font = gcnew System::Drawing::Font("Microsoft Sans Seri", newFontSize, FontStyle::Regular);




//==========================================================================================================
#ifdef SLICER_AVAILABLE
//==========================================================================================================
    long window1 = m_View1->GetSlicer()->GetWindow();
    long level1 =  m_View1->GetSlicer()->GetLevel();
    long window2 = m_View2->GetSlicer()->GetWindow();
    long level2 =  m_View2->GetSlicer()->GetLevel();

    mvState* state1 = m_View1->GetSlicer()->GetVisualizer()->GetStateModelView();
    mvState* state2 = m_View2->GetSlicer()->GetVisualizer()->GetStateModelView();

    m_View1->ClientSize = System::Drawing::Size(slicerWidth, slicerHeight);
	m_View1->Location = System::Drawing::Point(controlWidth + (maxBorder*2), slicerYOffset);
	m_View1->GetSlicer()->Initialize(slicerWidth, slicerHeight, "View1", false);
    m_View1->GetSlicer()->SetWindowLevel(window1, level1);
	m_View1->SetSelected(true);

	m_View1->GetSlicer()->GetVisualizer()->SetStateModelView(state1, m_View1->GetSlicer()->GetWidth(), m_View1->GetSlicer()->GetHeight());

	m_View1->Show();

	//Setup VIEW2
	//long slicer2Width = appWidth-controlWidth-slicerWidth-spaceBetweenSlicers;
	//slicer2Width &= 0xfffffffc;
	m_View2->ClientSize = System::Drawing::Size(slicerWidth, slicerHeight);
	m_View2->Location = System::Drawing::Point(controlWidth + slicerWidth + spaceBetweenSlicers + (maxBorder*2), slicerYOffset);
	m_View2->GetSlicer()->Initialize(slicerWidth, slicerHeight, "View2", false);
    m_View2->GetSlicer()->SetWindowLevel(window2, level2);
	m_View2->SetSelected(false);

    m_View2->GetSlicer()->GetVisualizer()->SetStateModelView(state2, m_View1->GetSlicer()->GetWidth(), m_View2->GetSlicer()->GetHeight());

    m_View2->Show();
    m_View2->UpdateDisplay();
    m_View1->UpdateDisplay();

#endif


//====================================================================================
//Main controls
//====================================================================================

    {
        m_mainControls->SetFontSize(newFontSize);
        m_mainControls->SetNewSize(controlWidth - 4, controlHeight);
        m_mainControls->SetNewPos(controlPosX, controlPosY);
        m_mainControls->UpdateGUI();
        m_mainControls->Show();
    }

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

                //MessageBox::Show(folderPath);
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

private: System::Void ZoomIN_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if (m_View1->IsSelected())
	{
		m_View1->GetSlicer()->Zoom(1.1);
		m_View1->UpdateDisplay();

	}
	else
	if (m_View2->IsSelected())
	{
		m_View2->GetSlicer()->Zoom(1.1);
		m_View2->UpdateDisplay();
	
	}

}

private: System::Void ZoomOUT_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if (m_View1->IsSelected())
	{
		m_View1->GetSlicer()->Zoom(0.9);
		m_View1->UpdateDisplay();

	}
	else
	if (m_View2->IsSelected())
	{
		m_View2->GetSlicer()->Zoom(0.9);
		m_View2->UpdateDisplay();
	
	}

}



//==============================================================================================

//Control Panel Events
#ifdef ANDREW_CONTROLS
public: void AndrewControls_Event(Object^ sender, EventArgs^ e)
{
    String^ code = (String^)sender;

	if (code == "AlphaBlendChanged")
	{
        OnAlphaBlendChanged();
	}
    else
    if (code == "RegistrationEvent")
    {
        OnRegistrationEvent();
    }
    else
    if (code == "LinkCubesEvent")
    {
        OnLinkCubesEvent();
    }
    else
    if (code == "TransformPoints")
    {
        OnTransformPoints();
    }
    else
    if (code == "LoadTransform")
    {
       // LoadTransformApplyToSelectedView();
        DoLandmarkRegistration();
    }

}
#endif



void OnTransformPoints()
{
    RRI_SlicerInterface* source = 0;
    RRI_SlicerInterface* target = 0;

    if (m_View1->IsSelected())
    {
        source = m_View1->GetSlicer();
        target = m_View2->GetSlicer();
    }
    else
    {
        target = m_View1->GetSlicer();
        source = m_View2->GetSlicer();
    }

    Vector3Vec targetPoints = source->GetUserTargets();
    long size = targetPoints.size();

    for (int i=0; i<size; i++)
    {
        Vector3 point = targetPoints.at(i);
        Vector3 convertedPoint = ConvertPoint(point);
        target->AddTarget(convertedPoint);
    }   

    m_View1->UpdateDisplay();
    m_View2->UpdateDisplay();


}

void OnRegistrationEvent()
{
    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    m_View1->GetSlicer()->SetDicomMatrix(matrix);
    m_View2->GetSlicer()->SetDicomMatrix(matrix);

    matrix->Delete();

}

void OnLinkCubesEvent()
{
    
    LinkCubes();
    

}

bool LinkCubes()
{

	Vector3 p1,p2,p3;
    std::string view1RegistrationLabel = m_View1->GetSlicer()->GetRegistrationLabel();
    std::string view2RegistrationLabel = m_View2->GetSlicer()->GetRegistrationLabel();
    std::string view1VolumeLabel = m_View1->GetSlicer()->GetVolumeLabel();
    std::string view2VolumeLabel = m_View2->GetSlicer()->GetVolumeLabel();

    RRI_SlicerInterface* source = 0;
    RRI_SlicerInterface* target = 0;

    if (m_View1->IsSelected())
    {
        source = m_View1->GetSlicer();
        target = m_View2->GetSlicer();
    }
    else//assume View2 is selected
    {
        target = m_View1->GetSlicer();
        source = m_View2->GetSlicer();
    }

    bool isRegistered = false;


    //get three arbitrary points on the View1 active face
    long screenWidth = source->GetWidth();
    long screenHeight = source->GetHeight();
    long midWidth = screenWidth / 2;
    long midHeight = screenHeight / 2;
    p1 = source->ScreenToModel(midWidth, midHeight);//m_View2->GetSlicer()->GetP1();
    p2 = source->ScreenToModel(midWidth+10, midHeight);//m_View2->GetSlicer()->GetP2();
    p3 = source->ScreenToModel(midWidth+10, midHeight+10);//m_View2->GetSlicer()->GetP3();
    

    Vector3 convertedP1 = ConvertPoint(p1);
    Vector3 convertedP2 = ConvertPoint(p2);
    Vector3 convertedP3 = ConvertPoint(p3);


    if (source->IsManuallyRegistered() && target->IsManuallyRegistered())
    {
        isRegistered = true;

    }
    else
    if (source->IsDicomMatrixSet() || target->IsDicomMatrixSet())
    {

        isRegistered = true;
    }
    else
    {
        MessageBox::Show("These two volumes are not registered");
        isRegistered = false;
    }                   



    if (isRegistered)
    {
        //reset cube, slice in a bit to get an active face (6)
        target->ResetCube(1);
        target->Slice(0, -0.5);
        //align face to the normal of the three converted points

        String^ parameterFilePath = m_folderToMonitor + "RegistrationSettings.txt";
        long reverseNormal = (long)GetDoubleValue(m_folderToMonitor + "RegistrationSettings.txt", "ReverseNormal");

        if (reverseNormal == 0)
            target->GetVisualizer()->AlignFaceToScrn(convertedP1, convertedP2, convertedP3);
        else
            target->GetVisualizer()->AlignFaceToScrn(convertedP3, convertedP2, convertedP1);
   
               
        Vector3 normal = Vector3(0, 0, -1);//normal to screen

        //NEW way
        Vector3 centerPoint = Vector3(0, 0, 0);
        Vector3 verticalPoint = Vector3(0, 0, -30);

        Vector3 centerPointToScreen = target->GetVolumeToScreen(centerPoint);
        Vector3 otherPointToScreen = target->GetVolumeToScreen(verticalPoint);

        //given two points, calculate angle of virtual needle relative to needle guide
        //NOTE: this is projected to the screen using GetVolumeToScreen above
        float lengthX = (float)(otherPointToScreen.x-centerPointToScreen.x);
        float lengthY = (float)(otherPointToScreen.y-centerPointToScreen.y);
        float lengthOfLine = sqrt(lengthX*lengthX + lengthY*lengthY);
        float angle = (asin((float)lengthX/(float)lengthOfLine)*180.0/PI);

        

#ifdef DO_PURE_ROTATE

    target->GetVisualizer()->PureRotate(normal, angle);

        //correct for 180 degree flip 
        centerPointToScreen = target->GetVolumeToScreen(centerPoint);
        otherPointToScreen = target->GetVolumeToScreen(verticalPoint);

        //given two points, calculate angle of virtual needle relative to needle guide
        //NOTE: this is projected to the screen using GetVolumeToScreen above
        lengthX = (float)(otherPointToScreen.x-centerPointToScreen.x);
        lengthY = (float)(otherPointToScreen.y-centerPointToScreen.y);
        lengthOfLine = sqrt(lengthX*lengthX + lengthY*lengthY);
        angle = (asin((float)lengthX/(float)lengthOfLine)*180.0/PI);
        target->GetVisualizer()->PureRotate(normal, angle);
#endif
        //zoom target cube to fit on the screen
        {
                    
            target->GetVisualizer()->Zoom(0.7);
        }

        //move the cube to the center of the screen
        {
           
            Vector3 cubeCenter = target->ModelToScreen(Vector3(0,0,0));
            float screenCenterX = target->GetWidth()/2;//width of screen
            float screenCenterY = target->GetHeight()/2;//height of screen
			long diffX = cubeCenter.x - screenCenterX;
			long diffY = cubeCenter.y - screenCenterY;
			target->DoPan(-diffX, -diffY);
        }
            
		m_View1->UpdateDisplay();
		m_View2->UpdateDisplay();

    }

    return true;
}

public: double GetDoubleValue(String^ filePath, String^ parameter)
{

	double parameterValue;
	bool parametersFound = false;

	String^ currentElement = "";

	try
	{
		XmlTextReader^ rdr = gcnew XmlTextReader(filePath);

		while (rdr->Read())
		{

			// do something with the data
			switch (rdr->NodeType)
			{
			case XmlNodeType::Element:

				if (rdr->Name->Contains("BINGSettings"))
				{
					parametersFound = true;
				}

				else
				{
					if (parametersFound)
					{
						currentElement = rdr->Name;
					}
				}



				break;

			case XmlNodeType::EndElement:
			{
				if (rdr->Name->Contains("BINGSettings"))
				{
					//store current calibration

					parametersFound = false;//invalidate for next one
				}
			}
			//Console::WriteLine(L"-> End element node, name={0}", rdr->Name);
			break;


			case XmlNodeType::Text:
			{
				if (currentElement->Contains(parameter))
				{
							
					String^ value = rdr->Value;
					bool success = Double::TryParse(value, parameterValue);

					if (success)
					{

					}
					else
					{
                        parameterValue = -1;//invalidate
						//AddMessage("Error parsing Encoder counts per unit move: " + encoderCounts);
					}
				}
			}
			break;

			default:
				Console::WriteLine(L"** Unknown node type");
				break;
			}

		}
	}
	catch (Exception^ pe)
	{
		bool done = true;
	}

	return parameterValue;
}


void OnAlphaBlendChanged()
{
    long alphaBlend = m_mainControls->GetAlphaBlend();
    

   

#ifdef OLD_ALPHA_BLEND

    if (m_View2->IsSelected())
    {
        m_View2->GetSlicer()->SetAlphaBlend(alphaBlend);
        m_View1->GetSlicer()->SetAlphaBlend(0);
        AlphaBlendView1WithView2();
            
    }
    else
    {
        m_View1->GetSlicer()->SetAlphaBlend(alphaBlend);
        m_View2->GetSlicer()->SetAlphaBlend(0);
        AlphaBlendView2WithView1();
           
    }
#endif

#ifdef NEW_ALPHA_BLEND

    RRI_SlicerInterface* source = 0;
    RRI_SlicerInterface* target = 0;

    if (m_View1->IsSelected())
    {
        //blending from View2 to View1
        source = m_View2->GetSlicer();//get pixels from here
        target = m_View1->GetSlicer();//blend pixels to here
        
    }
    else
    if (m_View2->IsSelected())
    {
        //blending from View1 to View2
        source = m_View1->GetSlicer();//get pixels from here
        target = m_View2->GetSlicer();//blend pixels to here
    }

    target->SetAlphaBlend(alphaBlend);
    source->SetAlphaBlend(0);
    AlphaBlendImages(source, target);
    
#endif

    m_View2->UpdateDisplay();
    m_View1->UpdateDisplay();

}


#ifdef OLD_ALPHA_BLEND
void AlphaBlendView1WithView2()
{
//    if (m_View2->GetSlicer()->GetAlphaBlend() == 0) 
//		return;

    long width = m_View2->GetSlicer()->GetWidth();
    long height =  m_View2->GetSlicer()->GetHeight();

    unsigned char* overlayBuffer = m_View2->GetSlicer()->GetOverlayBits();//destination
	unsigned char* screenBuffer = m_View1->GetSlicer()->GetBitmapBits();//source

    AlphaBlendImages(overlayBuffer, screenBuffer, width, height);
}

void AlphaBlendView2WithView1()
{
    
//	if (m_View1->GetSlicer()->GetAlphaBlend() == 0) 
//		return;

			
	long width = m_View1->GetSlicer()->GetWidth();
	long height =  m_View1->GetSlicer()->GetHeight();
	long imageSize = width*height;

    unsigned char* overlayBuffer = m_View1->GetSlicer()->GetOverlayBits();//destination
	unsigned char* screenBuffer = m_View2->GetSlicer()->GetBitmapBits();//source

    AlphaBlendImages(overlayBuffer, screenBuffer, width, height);

}
#endif

#ifdef NEW_ALPHA_BLEND

//View2 has the overlay buffer
void AlphaBlendImages(RRI_SlicerInterface* source, RRI_SlicerInterface* target)//unsigned char* overlayBuffer, unsigned char* screenBuffer, long width, long height)
{

    //here, we want to take each screen coordinate
    //check to see if inside the cube.
    //in inside, find the corresponding voxel in the source volume
    //paint corresponding voxel from the source volume into the overlay buffer in the target volume			
    long width = target->GetWidth();
    long height = target->GetHeight();
    

    unsigned char* overlayBuffer = target->GetOverlayBits();//blend to target view

	long imageSize = width*height;

    //for each point in the source, find the pixel value in the target
	bool done2 = false;
	long index = 0;
	long imageIndex = 0;
	while (!done2)
	{
		long y = index/width;
		long x = index%width;
		
        bool insideCube = target->InsideCube(x, y);

        float voxelX = target->GetVoxelX();
        float voxelY = target->GetVoxelY();
        float voxelZ = target->GetVoxelZ();

            
		if (insideCube)
		{
            Vector3 screenPoint = Vector3(x, y, 0);

            //convert screen point to volume point in View 1
            Vector3 viewPoint = target->ScreenToModel(x, y);
            
            Vector3 convertedViewPoint = ConvertPoint(viewPoint);//convert from source to target

            //convert to array coordinates
            Vector3 arrayCoordinate;
            Vector3 cubeSize = source->GetVisualizer()->GetCubeSize();
            arrayCoordinate.x = (convertedViewPoint.x + cubeSize.x/2.0)/voxelX;
            arrayCoordinate.y = (convertedViewPoint.y + cubeSize.y/2.0)/voxelY;
            arrayCoordinate.z = (convertedViewPoint.z + cubeSize.z/2.0)/voxelZ;


            unsigned char voxelValue = source->GetVisualizer()->GetVoxValue(arrayCoordinate.x, arrayCoordinate.y, arrayCoordinate.z);//get voxel value from source

            if (voxelValue != 0)
            {
                long test = 0;
            }
                
			//paint into target overlay buffer
			long bufferIndex = (long)screenPoint.y * width * 3 + (long)screenPoint.x * 3;
			if (bufferIndex >=0 && bufferIndex<width*height*3)
			{
				unsigned char value1 = voxelValue;//(unsigned char)screenBuffer[bufferIndex+0];
				unsigned char value2 = voxelValue;//(unsigned char)screenBuffer[bufferIndex+1];
				unsigned char value3 = voxelValue;//(unsigned char)screenBuffer[bufferIndex+2];
				//paint onto control view screen
				//and put put into view 2 overlay buffer
				//if (value1 == value2 == value3)

                //filter out non greyscale pixels, NOTE: this may not work in the new paradigm
                if (abs(value1 - value2) > 50)
                {

                    overlayBuffer[imageIndex+0] = 0;
					overlayBuffer[imageIndex+1] = 0;
					overlayBuffer[imageIndex+2] = 0;
                }
                else
				{	
							
					overlayBuffer[imageIndex+0] = value1;
					overlayBuffer[imageIndex+1] = value2;
					overlayBuffer[imageIndex+2] = value3;
						
				}
			}
						
		}
		else
		{
			overlayBuffer[imageIndex+0] = 0;
			overlayBuffer[imageIndex+1] = 0;
			overlayBuffer[imageIndex+2] = 0;
		}
					

		

		index++;
		imageIndex += 3;
		if (index >= imageSize)
		{					
			done2 = true;
		}

	}

    //MessageBox::Show("done");
}

#endif


#ifdef OLD_ALPHA_BLEND
//View2 has the overlay buffer
void AlphaBlendImages(unsigned char* overlayBuffer, unsigned char* screenBuffer, long width, long height)
{

	//here we want to take each point from the linked image and map it back to the control image
	//for each screen coordinate in the control view, find the corresponding voxel value in the linked view and paint into overlay
			
	
	long imageSize = width*height;

   
	bool done2 = false;
	long index = 0;
	long imageIndex = 0;
	while (!done2)
	{
		long y = index/width;
		long x = index%width;
		{
            bool insideCube = false;
		    //for each pixel in the view 2 screen
            if (m_View1->IsSelected())
            {
                insideCube = m_View1->GetSlicer()->InsideCube(x, y);

            }
            else
            {
                insideCube = m_View2->GetSlicer()->InsideCube(x, y);
            }


			if (insideCube)
			{
                Vector3 screenPoint = Vector3(x, y, 0);

                //if cubes are linked, transform, then alphablend, otherwise, alphablend view 1 and view 2 directly without linking
                if (m_View1->IsSelected())
                {
                    //convert screen point to volume point in View 1
                    Vector3 viewPoint = m_View1->GetSlicer()->ScreenToModel(x, y);

                    Vector3 convertedViewPoint = ConvertPoint(viewPoint);//convert to View 1 coordinate system
                    //convert this point back to screen coordinates from view 2
                    screenPoint = m_View2->GetSlicer()->GetVolumeToScreen(convertedViewPoint);//get screen coordinate from view 1

                }
                else
                if (m_View2->IsSelected())
                {
                    //convert screen point to volume point in View 1
                    Vector3 viewPoint = m_View2->GetSlicer()->ScreenToModel(x, y);

                    Vector3 convertedViewPoint = ConvertPoint(viewPoint);//convert to View 1 coordinate system
                    //convert this point back to screen coordinates from view 2
                    screenPoint = m_View1->GetSlicer()->GetVolumeToScreen(convertedViewPoint);//get screen coordinate from view 1

                }
                
                
				//get screen pixel value from view 1 
				long bufferIndex = (long)screenPoint.y * width * 3 + (long)screenPoint.x * 3;
				if (bufferIndex >=0 && bufferIndex<width*height*3)
				{
					unsigned char value1 = (unsigned char)screenBuffer[bufferIndex+0];
					unsigned char value2 = (unsigned char)screenBuffer[bufferIndex+1];
					unsigned char value3 = (unsigned char)screenBuffer[bufferIndex+2];
					//paint onto control view screen
					//and put put into view 2 overlay buffer
					//if (value1 == value2 == value3)

                    if (abs(value1 - value2) > 50)
                    {

                        overlayBuffer[imageIndex+0] = 0;
						overlayBuffer[imageIndex+1] = 0;
						overlayBuffer[imageIndex+2] = 0;
                    }
                    else
					{	
							
						overlayBuffer[imageIndex+0] = value1;
						overlayBuffer[imageIndex+1] = value2;
						overlayBuffer[imageIndex+2] = value3;
						
					}
				}
						
			}
			else
			{
				overlayBuffer[imageIndex+0] = 0;
				overlayBuffer[imageIndex+1] = 0;
				overlayBuffer[imageIndex+2] = 0;
			}
					

		}

		index++;
		imageIndex += 3;
		if (index >= imageSize)
		{					
			done2 = true;
		}

	}
}
#endif


//convert point using either manual registration or the dicom transforms
Vector3 ConvertPoint(Vector3 point)
{
    Vector3 convertedPoint = point;
    Vector3 view1Point, view2Point;

    std::string view1RegistrationLabel = m_View1->GetSlicer()->GetRegistrationLabel();
    std::string view2RegistrationLabel = m_View2->GetSlicer()->GetRegistrationLabel();
    std::string view1VolumeLabel = m_View1->GetSlicer()->GetVolumeLabel();
    std::string view2VolumeLabel = m_View2->GetSlicer()->GetVolumeLabel();

    //convert from view 2 to view 1
    if (m_View2->IsSelected())
    {
		if (m_View2->GetSlicer()->IsManuallyRegistered())
		{
            
            //check to see if volume 2 is registered to volume 1
            //if (-1 != view2RegistrationLabel.find(view1VolumeLabel))
            { 
                vtkMatrix4x4* matrix = vtkMatrix4x4::New();
                m_View2->GetSlicer()->GetRegMatrix(matrix);
                convertedPoint = m_View1->GetSlicer()->HelperTransformPointLinear(matrix, point);
                matrix->Delete();
            }
            //else
            {
                //MessageBox::Show("These volumes are not registered with each other.");
                
            }
		}
		else
		{
				view1Point = m_View2->GetSlicer()->TransformPointDICOM(point);
				convertedPoint = m_View1->GetSlicer()->TransformPointDICOMInverse(view1Point);
		}
    }
    else//m_View1 is selected, convert from view 1 to view 2
    if (m_View1->IsSelected())
    {
            
		if (m_View1->GetSlicer()->IsManuallyRegistered())
		{
            
            //check to see if volume 2 is registered to volume 1
            //if (-1 != view1RegistrationLabel.find(view2VolumeLabel))
            { 
                vtkMatrix4x4* matrix = vtkMatrix4x4::New();
                m_View1->GetSlicer()->GetRegMatrix(matrix);
                convertedPoint = m_View2->GetSlicer()->HelperTransformPointLinear(matrix, point);
                matrix->Delete();
            }

		}
		else
        if (m_View1->GetSlicer()->IsDicomMatrixSet() && m_View2->GetSlicer()->IsDicomMatrixSet())
		{
				view1Point = m_View1->GetSlicer()->TransformPointDICOM(point);
				convertedPoint = m_View2->GetSlicer()->TransformPointDICOMInverse(view1Point);
		}
    }

    return convertedPoint;
}

//given the raw data file, replace .raw with .tfm b
void ImportTransformFile(std::string filePath)
{
    String^ convertedString = gcnew String(filePath.c_str());
    if (!File::Exists(convertedString))
    {
        return;
    }

    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    vtkMatrix4x4* matrixIdentity = vtkMatrix4x4::New();

    //String^ convertedTransformFileName = gcnew String(filePath.c_str());
    HelperParseTransformFileSingle(filePath, matrix);//load original transforms from BING system

    if (m_View1->IsSelected())
	{
        m_View1->GetSlicer()->SetDicomMatrix(matrix);//this does a deep copy
        m_View1->GetSlicer()->SetRegistrationLabel(m_View2->GetSlicer()->GetVolumeLabel());
    
    }
    else
    {
        m_View2->GetSlicer()->SetDicomMatrix(matrix);//this does a deep copy
        m_View2->GetSlicer()->SetRegistrationLabel(m_View1->GetSlicer()->GetVolumeLabel());
    }

    matrix->Delete();
    matrixIdentity->Delete();

}


String^ HelperSelectFile(String^ directory)
{

    String^ filePath = "";


    OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

    openFileDialog1->InitialDirectory = directory;
    //openFileDialog1->Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
    openFileDialog1->FilterIndex = 2;
    openFileDialog1->RestoreDirectory = true;

    if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
    {
        filePath = openFileDialog1->FileName;
        
       
    }
    else
    {
        
    }


    return filePath;
}

void LoadTransformApplyToSelectedView()
{
    String^ selectedFile = HelperSelectFile(m_folderToMonitor);
    std::string selectedFile_STD = ConvertString(selectedFile);

    MessageBox::Show(selectedFile);

    if (selectedFile == "")
    {
        return;
    }

    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    vtkMatrix4x4* matrixIdentity = vtkMatrix4x4::New();
    HelperParseTransformFileSingle(selectedFile_STD, matrix);


   //vtkTransform* transform = vtkTransform::New();
   //transform->PostMultiply();

    if (m_View1->IsSelected())
    {
        m_View2->GetSlicer()->SetRegMatrix(matrixIdentity);//set view 2 as identity
        m_View1->GetSlicer()->SetRegMatrix(matrix);

    }
    else
    {
        m_View1->GetSlicer()->SetRegMatrix(matrixIdentity);//set view 1 as identity
        m_View2->GetSlicer()->SetRegMatrix(matrix);
    }

    m_View1->GetSlicer()->SetRegistrationLabel(m_View2->GetSlicer()->GetVolumeLabel());
    m_View2->GetSlicer()->SetRegistrationLabel(m_View1->GetSlicer()->GetVolumeLabel());

    
    //transform->SetMatrix(matrix);


    
    matrix->Delete();
    matrixIdentity->Delete();
    //transform->Delete();

}

bool HelperParseTransformFileSingle(std::string filePath, vtkMatrix4x4* matrix1)
{
   // IntPtr ptrToNativeString1 = Marshal::StringToHGlobalAnsi(filePath);
	//std::string convertedFilePath = static_cast<char*>(ptrToNativeString1.ToPointer());

    StringList stringVec;
    //=====================================================
    //load  transform from file
    //=====================================================
    std::ifstream t;
    t.open(filePath);
    std::string buffer;
    std::string line;
    while(t)
    {
        std::getline(t, line);

        stringVec.push_back(line);
    }
    t.close();

//=====================================================
//extract  transform 
//=====================================================

    std::string temp;
    float a1,b1,c1,d1;
    float a2,b2,c2,d2;
    float a3,b3,c3,d3;
    float a4,b4,c4,d4;

//video to world

    long size = stringVec.size();
    
    temp = stringVec.at(1);//tracker transform line a
    sscanf(temp.c_str(), "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &a1,&b1,&c1,&d1, &a2,&b2,&c2,&d2,  &a3,&b3,&c3,&d3,  &a4,&b4,&c4,&d4);
    matrix1->SetElement(0,0,a1);
    matrix1->SetElement(0,1,b1);
    matrix1->SetElement(0,2,c1);
    matrix1->SetElement(0,3,d1);

    matrix1->SetElement(1,0,a2);
    matrix1->SetElement(1,1,b2);
    matrix1->SetElement(1,2,c2);
    matrix1->SetElement(1,3,d2);

    matrix1->SetElement(2,0,a3);
    matrix1->SetElement(2,1,b3);
    matrix1->SetElement(2,2,c3);
    matrix1->SetElement(2,3,d3);

    matrix1->SetElement(3,0,a4);
    matrix1->SetElement(3,1,b4);
    matrix1->SetElement(3,2,c4);
    matrix1->SetElement(3,3,d4);


    return true;

}


//note: source volume is the one that will be transformed
//      target volume is the volume the registration is going to match
bool DoLandmarkRegistration()
{
	Vector3Vec targetUserPoints = m_View2->GetSlicer()->GetUserTargets();
    Vector3Vec sourceUserPoints = m_View1->GetSlicer()->GetUserTargets();

	//check to see if the source and target user point sizes are the same
    if (targetUserPoints.size() != sourceUserPoints.size())
    {
        MessageBox::Show("Number of Target Points do not match number of Source Points");
        return false;
    }

	 if (targetUserPoints.size() == 0)
    {
        MessageBox::Show("Left volume does not have any landmark points to use for registration");
        return false;
    }

    if (sourceUserPoints.size() == 0)
    {
        MessageBox::Show("Right volume does not have any landmark points to use for registration");
        return false;
    }

	vtkPoints*  sourcePoints = vtkPoints::New();
	vtkPoints*  targetPoints = vtkPoints::New();


	//get source user points
    {
        for (int i=0; i<sourceUserPoints.size(); i++)
        {
            Vector3 point = sourceUserPoints.at(i);
            sourcePoints->InsertNextPoint(point.x, point.y, point.z);//MRI
        }
    }

    //get target user points
    {
        for (int i=0; i<targetUserPoints.size(); i++)
        {
            Vector3 point = targetUserPoints.at(i);
            targetPoints->InsertNextPoint(point.x, point.y, point.z);//US
        }
    }


	//generate landmark transform
    vtkLandmarkTransform* imageTransform = vtkLandmarkTransform::New();
	imageTransform->SetSourceLandmarks(sourcePoints);	
	imageTransform->SetTargetLandmarks(targetPoints);
	imageTransform->SetModeToRigidBody();
	imageTransform->Update();

  
    vtkMatrix4x4* registrationMatrix = vtkMatrix4x4::New();

	//get image matrix from image transform so we can transform our triangles
    imageTransform->GetMatrix(registrationMatrix);

	std::string view1Label = m_View1->GetSlicer()->GetVolumeLabel();
	std::string view2Label = m_View2->GetSlicer()->GetVolumeLabel();

	IntPtr ptrToNativeString = Marshal::StringToHGlobalAnsi(m_defaultFolder);
	std::string folderToMonitor = static_cast<char*>(ptrToNativeString.ToPointer());
	std::string matrixFilePath = folderToMonitor + view1Label + "_2_" + view2Label + ".txt";
	//m_View2->GetSlicer()->ExportMatrixToFile(matrixFilePath, registrationMatrix);


	m_View1->GetSlicer()->SetRegMatrix(registrationMatrix);//store new matrix in m_View2 for future use
    registrationMatrix->Invert();
    m_View2->GetSlicer()->SetRegMatrix(registrationMatrix);//store inverted matrix in m_View1 for future use

    //set labels for registered volumes
    m_View1->GetSlicer()->SetRegistrationLabel(m_View2->GetSlicer()->GetVolumeLabel());
    m_View2->GetSlicer()->SetRegistrationLabel(m_View1->GetSlicer()->GetVolumeLabel());


	sourcePoints->Delete();
	targetPoints->Delete();
	imageTransform->Delete();
    registrationMatrix->Delete(); 

	m_View1->GetSlicer()->ClearTargets();
    m_View1->GetSlicer()->ClearTargetSurfaces();
	m_View2->GetSlicer()->ClearTargets();
    m_View2->GetSlicer()->ClearTargetSurfaces();

//	UpdateSurfaceList();

	return true;

}


};
}
