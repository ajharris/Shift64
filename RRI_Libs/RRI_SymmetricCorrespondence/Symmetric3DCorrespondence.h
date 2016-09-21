// Author: Bernard Chiu
// Date Created: July 06 2009

// vtk Class to correspond two surfaces as described in Papademetris 2002

#ifndef __Symmetric3DCorrespondence_h
#define __Symmetric3DCorrespondence_h

#include "vtkPolyDataAlgorithm.h"

class Symmetric3DCorrespondence : public vtkPolyDataAlgorithm
{
public:

	static Symmetric3DCorrespondence *New();
	// input/output structure
	void SetSurface1Connection(vtkAlgorithmOutput* algOutput)
	{
		this->SetSurfaceConnection(0, 0, algOutput);
	};
	void SetSurface1(vtkPolyData* pd) {this->SetSurface(0, 0, pd);};

	void SetSurface2Connection(vtkAlgorithmOutput* algOutput)
	{
		this->SetSurfaceConnection(1, 0, algOutput);
	};
	void SetSurface2(vtkPolyData* pd) {this->SetSurface(1, 0, pd);};

	void GetSurface1() {this->GetSurface(0,0);};
	void GetSurface2() {this->GetSurface(1,0);};

	vtkSetClampMacro(Weight, double, 0, 1);
	vtkGetMacro(Weight, double);

	vtkSetMacro(Tolerance, double);
	vtkGetMacro(Tolerance, double);

	// Control whether the corresponding lines are generated
	vtkSetMacro(GenerateCorrespondingLines, int);
	vtkGetMacro(GenerateCorrespondingLines, int);
	vtkBooleanMacro(GenerateCorrespondingLines, int);

	vtkSetMacro(SignedThickness, int);
	vtkGetMacro(SignedThickness, int);
	vtkBooleanMacro(SignedThickness, int);

	// Return Corresponding Lines;
	vtkPolyData* GetCorrespondingLines();

	// Return output port of the CorrespondingLines
	vtkAlgorithmOutput* GetCorrespondingLinesPort()
	{
		return this->GetOutputPort(1);
	}

protected:
	Symmetric3DCorrespondence(void);
	~Symmetric3DCorrespondence(void);
	int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
	
	// input/output structure
	int FillInputPortInformation(int port, vtkInformation* info);
	void SetSurface(int SurfaceId, int id, vtkPolyData* pd);
	void SetSurfaceConnection(int SurfaceId, int id, vtkAlgorithmOutput* algOutput);
	vtkPolyData* GetSurface(int SurfaceId, int id);
	void GetFirstLevelNeighbour(vtkPolyData* pd, vtkIdType ptidAtpd, vtkIdList* ptidFirstLevelNeighbour);
	// Weight used for Step 3: Smoothing (default = 0.75)
	double Weight;
	// Termination condition of Step 3
	double Tolerance;
	int GenerateCorrespondingLines;
	// if surface2 outside surface1, +ve; otherwise -ve
	int SignedThickness;
};
#endif