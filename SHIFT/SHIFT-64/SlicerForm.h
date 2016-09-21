#pragma once

#include "RRI_SlicerInterface.h"

namespace SHIFT64 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for SlicerForm
	/// </summary>
	public ref class SlicerForm : public System::Windows::Forms::Form	
	{
#define SLICER_EVENT_HANDLER
#ifdef SLICER_EVENT_HANDLER
		public: event EventHandler^ OnSlicerEvent;
#endif

		public: RRI_SlicerInterface* m_slicer;
		public: bool m_updateDisplay;
		public: long tempCount;
		
	
	public:
		SlicerForm(long width, long height)
		{
		
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			m_updateDisplay = false;
			this->m_slicer = new RRI_SlicerInterface();
			
			//m_slicer->Initialize(width, height);
			m_updateDisplay = true;//now we can update the display

			tempCount = 0;
		}
	public:

/*
		unsigned char* GetBuffer(){return m_slicer->GetBuffer();}
		unsigned char* GetBitmapBits(){return m_slicer->GetBitmapBits();}
		unsigned char* GetOverlayBits(){return m_slicer->GetOverlayBits();}
		long GetWidth(){return m_slicer->GetWidth();}
		long GetHeight(){return m_slicer->GetHeight();}
		long GetVolumeWidth(){return m_slicer->GetVolumeWidth();}
		long GetVolumeHeight(){return m_slicer->GetVolumeHeight();}
		long GetVolumeDepth(){return m_slicer->GetVolumeDepth();}
		Visualizer* GetVisualizer(){return m_slicer->GetVisualizer();}
		void SetModelView(long face, float rotX, float rotY, float rotZ, bool resetZoom)
			{m_slicer->SetModelView(face, rotX, rotY, rotZ, resetZoom);}
		void Slice(long face, float distance)
			{m_slicer->Slice(face, distance);}

		
		
		
		
		void ResetCube(){m_slicer->ResetCube();}
		void AddSurfaceObject(std::string text, long type, long red, long green, long blue){m_slicer->AddSurfaceObject(text, type, red, green, blue);}
		
		void DeleteSurfaceObject(long index){m_slicer->DeleteSurfaceObject(index); Refresh();}
		void ShowSurfaceObject(long index, bool show){m_slicer->ShowSurfaceObject(index, show);Refresh();}
		void SetCurrentSurfaceObject(long index){m_slicer->SetCurrentSurfaceObject(index);}
		long CreateVolume(AcqParam acq, CalParam cal, long channels){return m_slicer->CreateVolume(acq, cal, channels);}
		void StartMeasurement(long type){m_slicer->StartMeasurement(type);}

		Vector3 TransformPointDICOM(Vector3 point){return m_slicer->TransformPointDICOM(point);}
		Vector3 TransformPointDICOMInverse(Vector3 point){return m_slicer->TransformPointDICOMInverse(point);}

		Vector3Vec GetUserTargets(){return m_slicer->GetUserTargets();}
		void AddTarget(Vector3 point){m_slicer->AddTarget(point);}
		void AddTargetSurface(Vector3 point, long radius){m_slicer->AddTargetSurface(point, radius);}
		double GetVoxelX(){return m_slicer->GetVoxelX();}
		double GetVoxelY(){return m_slicer->GetVoxelY();}
		double GetVoxelZ(){return m_slicer->GetVoxelZ();}
		Vector3 FindRobotFiducial(Vector3 point){return m_slicer->FindRobotFiducial(point);}
		SurfaceList* GetSurfaceList(){return m_slicer->GetSurfaceList();}
		void ClearTargets(){m_slicer->ClearTargets();}//clears targets and target surfaces
		void ClearTargetSurfaces(){m_slicer->ClearTargetSurfaces();}//clears target surfaces only
		void ClearSurfaces(){m_slicer->ClearSurfaces();}//clears SurfaceList surfaces
		long AddPlanes(){return m_slicer->AddPlanes();}
		void AlignFaceToScreen(Vector3 p1, Vector3 p2, Vector3 p3){m_slicer->GetVisualizer()->AlignFaceToScrn(p1, p2, p3);}
		Vector3 ScreenToModel(long x, long y){return m_slicer->ScreenToModel(x,y);}
		Vector3 GetVolumeToScreen(Vector3 point){return m_slicer->GetVolumeToScreen(point);}
		bool InsideCube(long x, long y){return m_slicer->InsideCube(x, y);}
		Vector3 GetP1(){return m_slicer->GetP1();}
		Vector3 GetP2(){return m_slicer->GetP2();}
		Vector3 GetP3(){return m_slicer->GetP3();}
		void CenterCube(){m_slicer->CenterCube();}
		void DoPan(long x, long y){m_slicer->DoPan(x,y);}
		void SetExtraCursor(long x, long y){m_slicer->SetExtraCursor(x,y);}
		void SetAlphaBlend(long blend){m_slicer->SetAlphaBlend(blend);}
		void ClearAlphaBlend(){m_slicer->ClearAlphaBlend();}
		
		
		*/
		
		void SetSelected(bool selected){m_slicer->SetSelected(selected);}
		void SetMode(long mode){m_slicer->SetMode(mode);}
		bool IsSelected(){return m_slicer->IsSelected();}
		RRI_SlicerInterface* GetSlicer(){return m_slicer;}


		void StartScan(){m_slicer->StartScan();}
		void StopScan(){m_slicer->StopScan();}

		
void UpdateDisplay()
{
	m_slicer->UpdateDisplay();
#ifndef _DEBUG
	this->Refresh();
#endif
}

#ifdef VTK_AVAILABLE
void GenerateSurfaceFromUserPoints()
{
	m_slicer->GenerateSurfaceFromUserPoints();
	UpdateDisplay();

	System::Windows::Forms::MouseEventArgs^  e;//dummy event
	Slicer_Event("SurfaceUpdated", e);
}
#endif


long ImportDicomSingle(String^ folderPath)
{
	//Convert String* to char* 

	String^ newFolderPath = folderPath->Replace(0x5c, '/');

	//filePath = "C:\Scans/Test/";
	char* folderChar = (char*)(void*)Marshal::StringToHGlobalAnsi(newFolderPath);


	long result = m_slicer->ImportDicomSingle(folderChar);

	UpdateDisplay();

	return result;
}


long ImportDicom(String^ folderPath)
{
	//Convert String* to char* 
		   
	String^ newFolderPath = folderPath->Replace(0x5c, '/');

	//filePath = "C:\Scans/Test/";
	char* folderChar = (char*)(void*)Marshal::StringToHGlobalAnsi(newFolderPath);
			
			
	long result = m_slicer->ImportDicom(folderChar);

	UpdateDisplay();

	return result;
}

long ImportThermometry(String^ folderPath, long baseLineIndex, Vector3 thermalRegion, long radius)
{
	//Convert String* to char* 
		   
	String^ newFolderPath = folderPath->Replace(0x5c, '/');

	//filePath = "C:\Scans/Test/";
	char* folderChar = (char*)(void*)Marshal::StringToHGlobalAnsi(newFolderPath);
			
    long result = 0;
	result = m_slicer->ImportThermometry(folderChar, baseLineIndex, thermalRegion, radius);//load in the first image

	UpdateDisplay();

	return result;
}



	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SlicerForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            this->SuspendLayout();
            // 
            // SlicerForm
            // 
            this->AllowDrop = true;
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
            this->ClientSize = System::Drawing::Size(500, 500);
            this->ControlBox = false;
            this->Cursor = System::Windows::Forms::Cursors::Cross;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->Name = L"SlicerForm";
            this->ShowIcon = false;
            this->ShowInTaskbar = false;
            this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
            this->Text = L"SlicerForm";
            this->TopMost = true;
            this->Load += gcnew System::EventHandler(this, &SlicerForm::SlicerForm_Load);
            this->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &SlicerForm::SlicerForm_Scroll);
            this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &SlicerForm::SlicerForm_Paint);
            this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &SlicerForm::SlicerForm_KeyPress);
            this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &SlicerForm::SlicerForm_MouseDown);
            this->MouseEnter += gcnew System::EventHandler(this, &SlicerForm::SlicerForm_MouseEnter);
            this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &SlicerForm::SlicerForm_MouseMove);
            this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &SlicerForm::SlicerForm_MouseUp);
            this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &SlicerForm::SlicerForm_MouseWheel);
            this->ResumeLayout(false);

        }
#pragma endregion
	


private: System::Void SlicerForm_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
{
	
	this->DoubleBuffered = true;//NOTE: this prevents the flickering problem experienced when updating buffer
	//update slicer object
	if (m_updateDisplay)
	{
		m_updateDisplay = false;


		m_slicer->UpdateDisplay();

					
		//get data from slicer object
		unsigned char* bits = m_slicer->GetBitmapBits();
		long width = m_slicer->GetWidth();
		long height = m_slicer->GetHeight();
		long stride = 4 * ((width * 3 + 3) / 4); // must be multiple of 4

					

		//memset(bits, 128, width*height*3);
		Bitmap^ localBitmap = gcnew Bitmap(width, height, stride,  PixelFormat::Format24bppRgb, System::IntPtr(bits));
					

		//copy offscreen bitmap to "Image" and then draw to the graphics window
		IntPtr handle = localBitmap->GetHbitmap();
		Image^ bitmapImage = Image::FromHbitmap(handle);

		e->Graphics->DrawImage(bitmapImage,0,0);

		DeleteObject( (HGDIOBJ)handle );

		m_updateDisplay = true;

		GC::Collect();

	}
}

private: System::Void SlicerForm_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	UINT nFlags = 0;

	String^ slicerEvent = "";


#ifdef SLICER_EVENT_HANDLER
	if (!m_slicer->IsSelected())
	{
		std::string slicerLabel = m_slicer->GetSlicerLabel();
		String^ convertedLabel = gcnew String(slicerLabel.c_str());
		convertedLabel += "SlicerSelected";
		Slicer_Event(convertedLabel, e);
	}

	if (!m_slicer->InsideCube(e->Location.X, e->Location.Y))
	{
		std::string slicerLabel = m_slicer->GetSlicerLabel();
		String^ convertedLabel = gcnew String(slicerLabel.c_str());
		convertedLabel += "ResetStateEvent";
		Slicer_Event(convertedLabel, e);
	}

#endif

	long keyState1 = GetKeyState(MK_SHIFT);
	long keyState2 = GetKeyState(MK_CONTROL);
	
	if (e->Button.ToString() == "Left")
	{
		nFlags += MK_LBUTTON;
		slicerEvent = "SlicerLeftMouseDown";
	}
	else
	if (e->Button.ToString() == "Right")
	{
		nFlags += MK_RBUTTON;
		slicerEvent = "SlicerRightMouseDown";
		
	}

	if (ModifierKeys == Keys::Shift)
	{
		nFlags += MK_SHIFT;
	}

	if (ModifierKeys == Keys::Control)
	{
		nFlags += MK_CONTROL;
	}
	
	m_slicer->MouseDownEvent(nFlags, e->Location.X, e->Location.Y);

	Slicer_Event(slicerEvent, e);

	UpdateDisplay();
}

private: System::Void SlicerForm_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
#define  SLICE_ON_SCROLL_WHEEL
#ifdef   SLICE_ON_SCROLL_WHEEL

    double scrollDirection = -1.0 * (double)(e->Delta * SystemInformation::MouseWheelScrollLines) / 1000.0;
    Vector3 cubeSize = m_slicer->GetVisualizer()->GetCubeSize();
    Vector3 cubeDim = m_slicer->GetVisualizer()->GetCubeDim();
    float voxelX = cubeSize.x / cubeDim.x;
    float voxelY = cubeSize.y / cubeDim.y;
    float voxelZ = cubeSize.z / cubeDim.z;

    long currentFace = m_slicer->GetVisualizer()->GetFaceID(m_slicer->m_currentCursorX , m_slicer->m_currentCursorY);
	if (currentFace != -1)
	{

		double distance = voxelX;
		m_slicer->Slice(currentFace, distance*scrollDirection);

		if (m_slicer->InsideCube(e->Location.X, e->Location.Y))
		{

			Slicer_Event("SlicerScrollWheel", e);
		}
	}


#endif


#ifdef ZOOM_ON_SCROLL_WHEEL
	double zoomDirection = (double)(e->Delta * SystemInformation::MouseWheelScrollLines) / 1000.0;

	long posX = e->Location.X;
	long posY = e->Location.Y;

	Vector3 modelPoint = m_slicer->ScreenToModel(posX, posY);

	if ((long)modelPoint.x == -1000 && (long)modelPoint.y == -1000 && (long)modelPoint.z == -1000)
		return;
	
	if (zoomDirection >= 0)
	{
		m_slicer->Zoom(1.1);
	}
	else
	{
		m_slicer->Zoom(0.9);
	}

	Vector3 newScreenPoint = m_slicer->GetVolumeToScreen(modelPoint);
	//shift this point back to cursor position
	long diffX = (long)newScreenPoint.x - posX;
	long diffY = (long)newScreenPoint.y - posY;
	m_slicer->DoPan(-diffX, -diffY);
#endif

    if (m_updateDisplay)
        UpdateDisplay();
}

private: System::Void SlicerForm_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	UINT nFlags = 0;

	String^ slicerEvent = "";

	if (e->Button.ToString() == "Left")
	{
		nFlags += MK_LBUTTON;
		slicerEvent = "SlicerLeftMouseUp";
	}
	else
	if (e->Button.ToString() == "Right")
	{
		nFlags += MK_RBUTTON;
		slicerEvent = "SlicerRightMouseUp";
	}
	
	if (ModifierKeys == Keys::Shift)
	{
		nFlags += MK_SHIFT;
	}

	if (ModifierKeys == Keys::Control)
	{
		nFlags += MK_CONTROL;
	}

	m_slicer->MouseUpEvent(nFlags, e->Location.X, e->Location.Y);


#ifdef SLICER_EVENT_HANDLER
		Slicer_Event(slicerEvent, e);
#endif
	
	UpdateDisplay();
			
}

private: System::Void SlicerForm_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
    if (!m_slicer->IsSlicingEnabled())
    {

//        return;
    }

	UINT nFlags = 0;

	String^ slicerEvent = "MouseMovedEvent";
	
	if (e->Button.ToString() == "Left")
	{
		nFlags += MK_LBUTTON;

	}
	else
	if (e->Button.ToString() == "Right")
	{
		nFlags += MK_RBUTTON;
		
	}

	if (ModifierKeys == Keys::Shift)
	{
		nFlags += MK_SHIFT;
	}

	if (ModifierKeys == Keys::Control)
	{
		nFlags += MK_CONTROL;
	}

	m_slicer->MouseMoveEvent(nFlags, e->Location.X, e->Location.Y);

	

#ifdef SLICER_EVENT_HANDLER
		if (m_slicer->InsideCube(e->Location.X, e->Location.Y) && m_slicer->GetCurrentState() == ST_SLICE)
		{
			Slicer_Event(slicerEvent, e);
		}
#endif

		UpdateDisplay();

	

}



private: System::Void SlicerForm_Load(System::Object^  sender, System::EventArgs^  e) 
{
}


#ifdef SLICER_EVENT_HANDLER
public: void Slicer_Event(System::Object^  sender, System::EventArgs^  e)
{
       OnSlicerEvent(sender, e);
}
#endif


public: void SetDisplayText(String^ text)
{
	long length = text->Length;
	//char* characters = text->Chars;
}



private: System::Void SlicerForm_Scroll(System::Object^  sender, System::Windows::Forms::ScrollEventArgs^  e) 
{
	long temp = 0;
}
private: System::Void SlicerForm_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
{
	long temp = 0;
}
private: System::Void SlicerForm_MouseEnter(System::Object^  sender, System::EventArgs^  e) 
{
    this->Focus();
    this->Activate();
    this->TopMost = true;
}
};

}
