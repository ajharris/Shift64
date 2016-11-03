#pragma once




namespace SHIFT64 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for AndrewControls
	/// </summary>
	public ref class AndrewControls : public System::Windows::Forms::Form
	{

    public: static long m_alphaBlendValue = 0;
    public: event EventHandler^ OnAndrewControlsEvent;
    static long m_controlWidth = 0;
	static long m_controlHeight = 0;
	static long m_controlPosX = 0;
	static long m_controlPosY = 0;

    static bool m_sizeChanging = false;
    private: System::Windows::Forms::Button^  TransformPoints;
    private: System::Windows::Forms::Button^  LoadTransform;
	private: System::Windows::Forms::Button^  loadTransformButton;
	public:
        static bool m_positionChanging = false;




	public:
		AndrewControls(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~AndrewControls()
		{
			if (components)
			{
				delete components;
			}
		}
    private: System::Windows::Forms::Button^  ApplyRegistration;
    protected:
    private: System::Windows::Forms::Button^  LinkButton;
    private: System::Windows::Forms::Label^  AlphaBlendLabel;
    private: System::Windows::Forms::HScrollBar^  AlphaBlendScrollBar;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(AndrewControls::typeid));
			this->ApplyRegistration = (gcnew System::Windows::Forms::Button());
			this->LinkButton = (gcnew System::Windows::Forms::Button());
			this->AlphaBlendLabel = (gcnew System::Windows::Forms::Label());
			this->AlphaBlendScrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->TransformPoints = (gcnew System::Windows::Forms::Button());
			this->LoadTransform = (gcnew System::Windows::Forms::Button());
			this->loadTransformButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// ApplyRegistration
			// 
			this->ApplyRegistration->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(42)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)), static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->ApplyRegistration->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"ApplyRegistration.BackgroundImage")));
			this->ApplyRegistration->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ApplyRegistration->Cursor = System::Windows::Forms::Cursors::Arrow;
			this->ApplyRegistration->FlatAppearance->BorderColor = System::Drawing::Color::Black;
			this->ApplyRegistration->FlatAppearance->BorderSize = 2;
			this->ApplyRegistration->FlatAppearance->MouseDownBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->ApplyRegistration->FlatAppearance->MouseOverBackColor = System::Drawing::Color::Transparent;
			this->ApplyRegistration->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->ApplyRegistration->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ApplyRegistration->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->ApplyRegistration->Location = System::Drawing::Point(29, 31);
			this->ApplyRegistration->Margin = System::Windows::Forms::Padding(6, 4, 6, 4);
			this->ApplyRegistration->Name = L"ApplyRegistration";
			this->ApplyRegistration->Size = System::Drawing::Size(213, 65);
			this->ApplyRegistration->TabIndex = 61;
			this->ApplyRegistration->Text = L"Register Volumes";
			this->ApplyRegistration->UseVisualStyleBackColor = false;
			this->ApplyRegistration->Click += gcnew System::EventHandler(this, &AndrewControls::ApplyRegistration_Click);
			// 
			// LinkButton
			// 
			this->LinkButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"LinkButton.BackgroundImage")));
			this->LinkButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->LinkButton->FlatAppearance->BorderColor = System::Drawing::Color::Black;
			this->LinkButton->FlatAppearance->BorderSize = 2;
			this->LinkButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->LinkButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->LinkButton->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->LinkButton->Location = System::Drawing::Point(29, 281);
			this->LinkButton->Name = L"LinkButton";
			this->LinkButton->Size = System::Drawing::Size(213, 65);
			this->LinkButton->TabIndex = 64;
			this->LinkButton->Text = L"Link";
			this->LinkButton->UseVisualStyleBackColor = true;
			this->LinkButton->Click += gcnew System::EventHandler(this, &AndrewControls::LinkButton_Click);
			// 
			// AlphaBlendLabel
			// 
			this->AlphaBlendLabel->AutoSize = true;
			this->AlphaBlendLabel->ForeColor = System::Drawing::Color::MintCream;
			this->AlphaBlendLabel->Location = System::Drawing::Point(82, 403);
			this->AlphaBlendLabel->Name = L"AlphaBlendLabel";
			this->AlphaBlendLabel->Size = System::Drawing::Size(107, 20);
			this->AlphaBlendLabel->TabIndex = 66;
			this->AlphaBlendLabel->Text = L"Blend Images";
			// 
			// AlphaBlendScrollBar
			// 
			this->AlphaBlendScrollBar->Location = System::Drawing::Point(29, 363);
			this->AlphaBlendScrollBar->Maximum = 255;
			this->AlphaBlendScrollBar->Name = L"AlphaBlendScrollBar";
			this->AlphaBlendScrollBar->Size = System::Drawing::Size(213, 35);
			this->AlphaBlendScrollBar->TabIndex = 65;
			this->AlphaBlendScrollBar->ValueChanged += gcnew System::EventHandler(this, &AndrewControls::AlphaBlendScrollBar_ValueChanged);
			// 
			// TransformPoints
			// 
			this->TransformPoints->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"TransformPoints.BackgroundImage")));
			this->TransformPoints->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->TransformPoints->FlatAppearance->BorderColor = System::Drawing::Color::Black;
			this->TransformPoints->FlatAppearance->BorderSize = 2;
			this->TransformPoints->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->TransformPoints->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->TransformPoints->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->TransformPoints->Location = System::Drawing::Point(29, 194);
			this->TransformPoints->Name = L"TransformPoints";
			this->TransformPoints->Size = System::Drawing::Size(213, 65);
			this->TransformPoints->TabIndex = 67;
			this->TransformPoints->Text = L"Transform Points";
			this->TransformPoints->UseVisualStyleBackColor = true;
			this->TransformPoints->Click += gcnew System::EventHandler(this, &AndrewControls::TransformPoints_Click);
			// 
			// LoadTransform
			// 
			this->LoadTransform->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"LoadTransform.BackgroundImage")));
			this->LoadTransform->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->LoadTransform->FlatAppearance->BorderColor = System::Drawing::Color::Black;
			this->LoadTransform->FlatAppearance->BorderSize = 2;
			this->LoadTransform->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->LoadTransform->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->LoadTransform->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->LoadTransform->Location = System::Drawing::Point(29, 113);
			this->LoadTransform->Name = L"LoadTransform";
			this->LoadTransform->Size = System::Drawing::Size(213, 65);
			this->LoadTransform->TabIndex = 68;
			this->LoadTransform->Text = L"Landmark Transform";
			this->LoadTransform->UseVisualStyleBackColor = true;
			this->LoadTransform->Click += gcnew System::EventHandler(this, &AndrewControls::LandmarkTransform_Click);
			// 
			// loadTransformButton
			// 
			this->loadTransformButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"loadTransformButton.BackgroundImage")));
			this->loadTransformButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->loadTransformButton->FlatAppearance->BorderColor = System::Drawing::Color::Black;
			this->loadTransformButton->FlatAppearance->BorderSize = 2;
			this->loadTransformButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->loadTransformButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->loadTransformButton->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->loadTransformButton->Location = System::Drawing::Point(29, 437);
			this->loadTransformButton->Name = L"loadTransformButton";
			this->loadTransformButton->Size = System::Drawing::Size(213, 65);
			this->loadTransformButton->TabIndex = 69;
			this->loadTransformButton->Text = L"Load Transform";
			this->loadTransformButton->UseVisualStyleBackColor = true;
			this->loadTransformButton->Click += gcnew System::EventHandler(this, &AndrewControls::loadTransformButton_Click);
			// 
			// AndrewControls
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(42)), static_cast<System::Int32>(static_cast<System::Byte>(42)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->ClientSize = System::Drawing::Size(288, 660);
			this->ControlBox = false;
			this->Controls->Add(this->loadTransformButton);
			this->Controls->Add(this->LoadTransform);
			this->Controls->Add(this->TransformPoints);
			this->Controls->Add(this->AlphaBlendLabel);
			this->Controls->Add(this->AlphaBlendScrollBar);
			this->Controls->Add(this->LinkButton);
			this->Controls->Add(this->ApplyRegistration);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->Name = L"AndrewControls";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"AndrewControls";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

public: long GetAlphaBlend()
{
	return m_alphaBlendValue;

}



public: void SetFontSize(float fontSize)
{
	this->Font = gcnew System::Drawing::Font("Microsoft Sans Seri", fontSize, FontStyle::Regular);
}

public: void SetNewSize(int width, int height)
{
    if (m_controlWidth != width || m_controlHeight != height)
    {
        m_sizeChanging = true;
    }

	m_controlWidth = width;
	m_controlHeight = height;
	
}

public: void SetNewPos(int xPos, int yPos)
{
    if (m_controlPosX != xPos || m_controlPosY != yPos)
    {
        m_sizeChanging = true;
    }

	m_controlPosX = xPos;
	m_controlPosY = yPos;

}


public: void UpdateGUI()
{
    System::Drawing::Size clientSize =  this->ClientSize;

	if (m_controlWidth == 0 || m_controlHeight == 0)
	{
		m_controlWidth = clientSize.Width;
		m_controlHeight = clientSize.Height;
	}
	
    if (m_sizeChanging)
    {
        this->ClientSize = System::Drawing::Size(m_controlWidth, m_controlHeight);
        m_sizeChanging = false;
    }

    //if (m_positionChanging)
    {
        this->Location = System::Drawing::Point(m_controlPosX, m_controlPosY); 
        m_positionChanging = false;
    }
}

//===========================================================================================================
//Control Panel events
//===========================================================================================================

public: void AndrewControls_Event(System::Object^  sender, System::EventArgs^  e)
{
       OnAndrewControlsEvent(sender, e);//send event to client registered for this event
}

private: System::Void AlphaBlendScrollBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	m_alphaBlendValue = AlphaBlendScrollBar->Value;


	AndrewControls_Event("AlphaBlendChanged", e);


}

private: System::Void ApplyRegistration_Click(System::Object^  sender, System::EventArgs^  e) 
{
    AndrewControls_Event("RegistrationEvent", e);
}

private: System::Void LinkButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
    AndrewControls_Event("LinkCubesEvent", e);
}


private: System::Void TransformPoints_Click(System::Object^  sender, System::EventArgs^  e) 
{
    AndrewControls_Event("TransformPoints", e);
}
private: System::Void LandmarkTransform_Click(System::Object^  sender, System::EventArgs^  e)
{
    AndrewControls_Event("LandmarkTransform", e);
}

private: System::Void loadTransformButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	AndrewControls_Event("LoadTransform", e);
}
};
}
