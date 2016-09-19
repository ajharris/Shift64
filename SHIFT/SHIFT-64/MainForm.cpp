#include "MainForm.h"
//http://www.bogotobogo.com/cplusplus/application_visual_studio_2013.php

using namespace System;
using namespace System::Windows::Forms;


[STAThread]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	SHIFT64::MainForm form;
	Application::Run(%form);
}
  