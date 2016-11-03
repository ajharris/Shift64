#pragma once

#include "3DImageStitcher.h"



#include "itkCommand.h"

class CommandIterationUpdate : public itk::Command
{
public: 
	typedef  CommandIterationUpdate   Self;
	typedef  itk::Command             Superclass;
	typedef itk::SmartPointer<Self>  Pointer;
	itkNewMacro(Self);
	std::ofstream file;
	int count = 0;

protected:
	CommandIterationUpdate() {
		file.open("observer.txt");
	};
	~CommandIterationUpdate() {
		file.close();
	};


public:

	typedef const OptimizerType *									OptimizerPointer;

	void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE
	{
		Execute((const itk::Object *)caller, event);
	}

	void Execute(const itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE
	{
		OptimizerPointer optimizer = static_cast< OptimizerPointer >(object);

		if (!itk::IterationEvent().CheckEvent(&event))
		{
			return;
		}

		count++;

		//optimizer->Print(file);
		//file << std::endl;
		file << "Iteration: " << optimizer->GetCurrentIteration() << " " << "Calculation: " << count << " " << "Value = " << optimizer->GetCurrentMetricValue() << " " << "Position: " << optimizer->GetCurrentPosition() << std::endl;

	}
};


Correlator3D::Correlator3D(unsigned char *fixedImage, int *fixedImageSize, double *fixedImageSpacing, double fixedOrigin[Dimension],
	unsigned char *movingImage, int *movingImageSize, double *movingImageSpacing, double movingOrigin[Dimension])
{
	errorFile.open("errorFile.txt");
	objectFixedCharImage = CharImageType::New();
	objectMovingCharImage = CharImageType::New();
	fixedCastFilter = CastFilterType::New();
	movingCastFilter = CastFilterType::New();
	movingTransform1 = FloatImageType::New();
	fixedMask = MaskType::New();
	movingMask = MaskType::New();
	bool crop = false;
	int threshold = 40;
	ConvertBufferToImage(fixedImage, fixedImageSize, fixedImageSpacing, fixedOrigin, false, objectFixedCharImage, fixedCastFilter, threshold, fixedMask, fixedSize, fixedSpacing);
	ConvertBufferToImage(movingImage, movingImageSize, movingImageSpacing, movingOrigin, crop, objectMovingCharImage, movingCastFilter, threshold, movingMask, movingSize, movingSpacing);
}

Correlator3D::Correlator3D()
{
}

Correlator3D::~Correlator3D()
{
	errorFile.close();
}

void Correlator3D::Initialize(unsigned char *fixedImage, int *fixedImageSize, double *fixedImageSpacing, double fixedOrigin[Dimension],
	unsigned char *movingImage, int *movingImageSize, double *movingImageSpacing, double movingOrigin[Dimension]){
	//int count = 0;
	//for (int i = 0; i < fixedImageSize[0] * fixedImageSize[1] * fixedImageSize[2]; i++){
	//	if (fixedImage[i] == 255){
	//		std::cout << (long)fixedImage[i];
	//		
	//		if (i % 100 == 0) {
	//			std::cout << std::endl;
	//			count++;
	//		}
	//	}
	//	if (count > 100) break;
	errorFile.open("errorFile.txt");
	objectFixedCharImage = CharImageType::New();
	objectMovingCharImage = CharImageType::New();
	fixedCastFilter = CastFilterType::New();
	movingCastFilter = CastFilterType::New();
	fixedMask = MaskType::New();
	movingMask = MaskType::New();
	fixedROI = ROIFilterType::New();
	movingROI = ROIFilterType::New();
	bool crop = false;
	int threshold = 40;
	ConvertBufferToImage(fixedImage, fixedImageSize, fixedImageSpacing, fixedOrigin, false, objectFixedCharImage, fixedCastFilter, threshold, fixedMask, fixedSize, fixedSpacing);
	ConvertBufferToImage(movingImage, movingImageSize, movingImageSpacing, movingOrigin, crop, objectMovingCharImage, movingCastFilter, threshold, movingMask, movingSize, movingSpacing);
}
/*
void Correlator3D::Initialize(ImageReader fixedImageReader, ImageReader movingImageReader){
	unsigned char *fixedImage = fixedImageReader.GetBuffer();
	int* fixedImageSize = fixedImageReader.GetSize();
	double* fixedImageSpacing = fixedImageReader.GetSpacing();
	double fixedOrigin[] = { 0, 0, 0 };

	unsigned char *	movingImage = movingImageReader.GetBuffer();
	int* movingImageSize = movingImageReader.GetSize();
	double* movingImageSpacing = movingImageReader.GetSpacing();
	double	movingOrigin[] = { 0, 0, 0 };

	objectFixedCharImage = CharImageType::New();
	objectMovingCharImage = CharImageType::New();
	fixedCastFilter = CastFilterType::New();
	movingCastFilter = CastFilterType::New();
	fixedMask = MaskType::New();
	movingMask = MaskType::New();
	bool crop = false;
	int threshold = 100;
	ConvertBufferToImage(fixedImage, fixedImageSize, fixedImageSpacing, fixedOrigin, false, objectFixedCharImage, fixedCastFilter, threshold, fixedMask, fixedSize);
	ConvertBufferToImage(movingImage, movingImageSize, movingImageSpacing, movingOrigin, crop, objectMovingCharImage, movingCastFilter, threshold, movingMask, movingSize);
}
*/
#define useCastFilter
#ifdef useCastFilter
void Correlator3D::ConvertBufferToImage(unsigned char *volumeBuffer, int *imageSize, double *imageSpacing, double origin[3], bool crop, CharImageType::Pointer &image, CastFilterType::Pointer &castFilter, int threshold, MaskType::Pointer &mask, CharImageType::SizeType &objectSize, CharImageType::SpacingType &objectSpacing)
{
	//method will convert array of unsigned char into itk::Image instance indicated using output

	//first the size and spacing of the image represented by the buffer is defined
	int count = 0;

	CharWriterType::Pointer charWriter = CharWriterType::New();

	ImportFilterType::SizeType size;
	size[0] = imageSize[0];
	size[1] = imageSize[1];
	size[2] = imageSize[2];
	objectSize= size;


	ImportFilterType::SpacingType spacing;
	spacing[0] = imageSpacing[0];
	spacing[1] = imageSpacing[1];
	spacing[2] = imageSpacing[2];
	ImportFilterType::IndexType start;
	ImportFilterType::IndexType::IndexValueType value;
	start.Fill(0);
	objectSpacing = spacing;

	ImportFilterType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);


	//the size and region settings are applied to a new import filter
	ImportFilterType::Pointer importFilter = ImportFilterType::New();
	importFilter->SetRegion(region);

	importFilter->SetOrigin(origin);
	importFilter->SetSpacing(spacing);

	unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	const bool importImageFilterWillOwnTheBuffer = false; // setting this to true results in the original pointer being destroyed and the program crashing

	//the import filter is assigned the input volume buffer
	importFilter->SetImportPointer(volumeBuffer, numberOfPixels, importImageFilterWillOwnTheBuffer);

	image->SetOrigin(origin);

	image = importFilter->GetOutput();
	
	charWriter->SetInput(image);
	charWriter->SetFileName("C:/Scans/charImage.nrrd");

	try{
		charWriter->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}

	//CharImageType::Pointer resampledImage = ResampleImage(image, objectSize);

	/*try{
		resampledImage->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}*/


	castFilter->SetInput(image);
	try
	{
		castFilter->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		errorFile << err << std::endl;
	}
	
	CannyEdgeDetectionFilterType::Pointer cannyFilter = CannyEdgeDetectionFilterType::New();
	cannyFilter->SetInput(castFilter->GetOutput());

	float variance = 0.5;
	float upperThreshold = 0.0;
	float lowerThreshold = 0.0;

	cannyFilter->SetVariance(variance);
	cannyFilter->SetUpperThreshold(upperThreshold);
	cannyFilter->SetLowerThreshold(lowerThreshold);

	try{
		cannyFilter->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName("C:/Scans/canny.nrrd");
	writer->SetInput(cannyFilter->GetOutput());
	try{
		writer->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}

	CastToCharImageType::Pointer castToCharFilter = CastToCharImageType::New();
	castToCharFilter->SetInput(cannyFilter->GetOutput());

	try{
		castToCharFilter->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}
	CastFilterType::Pointer castFilterBack = CastFilterType::New();
	castFilterBack->SetInput(castToCharFilter->GetOutput());

	try
	{
		castFilterBack->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		errorFile << err << std::endl;
	}
}

void Correlator3D::GenerateThresholdMask(float upperThreshold, CharImageType::Pointer &charImage, MaskType::Pointer &mask){
	// will mask out any values below the threshold amount so they won't be considered in correlation
	ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
	RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
	
	rescaleFilter->SetInput(charImage);
	rescaleFilter->SetOutputMinimum(0);
	rescaleFilter->SetOutputMaximum(255);
	try{
		rescaleFilter->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err;
	}
	thresholdFilter->SetInput(rescaleFilter->GetOutput());
	thresholdFilter->SetOutsideValue(0);
	thresholdFilter->ThresholdBelow(upperThreshold);
	
	try{
		thresholdFilter->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err;
	}

	mask->SetImage(thresholdFilter->GetOutput());
	try{
		mask->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}

	
	MaskToImageType::Pointer maskWriter = MaskToImageType::New();
	maskWriter->SetInput(mask);
	try{
		maskWriter->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName("C:/Scans/mask.nrrd");
	writer->SetInput(maskWriter->GetOutput());
	try{
		writer->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}
	
}

void Correlator3D::ImageCorrelation(double *transformFixed, double *transformMoving){
	MetricType::Pointer metric = MetricType::New();
	OptimizerType::Pointer optimizer = OptimizerType::New();
	RegistrationType::Pointer registration = RegistrationType::New();
	CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();

	optimizer->AddObserver(itk::IterationEvent(), observer);

	registration->SetMetric(metric);
	registration->SetOptimizer(optimizer);

	TransformType::Pointer initialTransform = TransformType::New();
	TransformInitializerType::Pointer initializer = TransformInitializerType::New();
	initializer->SetTransform(initialTransform);
	initializer->SetFixedImage(fixedCastFilter->GetOutput());
	initializer->SetMovingImage(movingCastFilter->GetOutput());
	initializer->MomentsOn();
	initializer->InitializeTransform();
	VersorType rotation;
	VectorType axis;
	axis[0] = 0.0;
	axis[1] = 0.0;
	axis[2] = 1.0;
	const double angle = 0;
	rotation.Set(axis, angle);
	initialTransform->SetRotation(rotation);
	//initialTransform->SetIdentity();  // not sure if this is going to do anything, copying from 2D Deformable registration example

	registration->SetFixedImage(fixedROI->GetOutput());
	registration->SetMovingImage(movingROI->GetOutput());

#ifdef v4
	registration->SetInitialTransform(initialTransform);
	const unsigned int numberOfLevels = 1;
	RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	shrinkFactorsPerLevel.SetSize(1);
	shrinkFactorsPerLevel[0] = 1;
	RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	smoothingSigmasPerLevel.SetSize(1);
	smoothingSigmasPerLevel[0] = 0;
	registration->SetNumberOfLevels(numberOfLevels);
	registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
#endif

#ifdef LBFGSB
	const unsigned int numParameters = initialTransform->GetNumberOfParameters();
	OptimizerType::BoundSelectionType boundSelect(numParameters);
	OptimizerType::BoundValueType upperBound(numParameters);
	OptimizerType::BoundValueType lowerBound(numParameters);
	boundSelect.Fill(OptimizerType::BOTHBOUNDED);
	upperBound[0] = 0.03;
	lowerBound[0] = 0.0;
	upperBound[1] = 0.03;
	lowerBound[1] = 0.0;
	upperBound[2] = 0.03;
	lowerBound[2] = 0.0;
	upperBound[3] =10.0;
	lowerBound[3] =0.0;
	upperBound[4] =10.0;
	lowerBound[4] =0.0;
	upperBound[5] =10.0;
	lowerBound[5] =0.0;
	optimizer->SetBoundSelection(boundSelect);
	optimizer->SetUpperBound(upperBound);
	optimizer->SetLowerBound(lowerBound);
	optimizer->SetCostFunctionConvergenceFactor(1e+12);
	optimizer->SetGradientConvergenceTolerance(1.0e-35);
	optimizer->SetNumberOfIterations(500);
	optimizer->SetMaximumNumberOfFunctionEvaluations(500);
	optimizer->SetMaximumNumberOfCorrections(5);

#endif


	//metric->SetFixedImageMask(fixedMask);
	//metric->SetMovingImageMask(movingMask);


	std::string info;

	clock_t time = clock();

	try
	{
		registration->Update();
		info = "Registration Successful";
	}
	catch (itk::ExceptionObject &err)
	{
		info = err.GetDescription();
	}

	time = clock() - time;

	float seconds = (float)time / CLOCKS_PER_SEC;

	// Include finalParameters for testing accuracy on shifted/rotated data
	TransformType::ParametersType finalParameters = registration->GetOutput()->Get()->GetParameters();
	double versorX = finalParameters[0];
	double versorY = finalParameters[1];
	double versorZ = finalParameters[2];
	double finalTranslationX = finalParameters[3];
	double finalTranslationY = finalParameters[4];
	double finalTranslationZ = finalParameters[5];
	unsigned int numberOfIterations = optimizer->GetCurrentIteration();
	double bestValue = optimizer->GetValue();
	double correlation = metric->GetCurrentValue();

	TransformType::ConstPointer finalTransform = registration->GetTransform();
	TransformType::MatrixType matrix = finalTransform->GetMatrix();
	TransformType::TranslationType translation = finalTransform->GetTranslation();
	GenerateTransformMatrix(matrix, translation, transformMoving);
	FloatResampleFilterType::Pointer resampler = FloatResampleFilterType::New();

	resampler->SetInput(movingCastFilter->GetOutput());
	resampler->SetTransform(registration->GetOutput()->Get());


	resampler->SetSize(fixedCastFilter->GetOutput()->GetLargestPossibleRegion().GetSize());
	resampler->SetOutputOrigin(fixedCastFilter->GetOutput()->GetOrigin());
	resampler->SetOutputSpacing(fixedCastFilter->GetOutput()->GetSpacing());
	resampler->SetOutputDirection(fixedCastFilter->GetOutput()->GetDirection());
	resampler->SetDefaultPixelValue(0);

	movingTransform1 = resampler->GetOutput();
	
	try{
		movingTransform1->Update();
	}
	catch (itk::ExceptionObject &err){
		errorFile << err << std::endl;
	}

}
//reassigning matrix to Q to fit notation in documentation
void Correlator3D::GenerateTransformMatrix(TransformType::MatrixType &Q, TransformType::TranslationType &translation, double *transform){

	//distribute elements according to http://fabiensanglard.net/doom3_documentation/37726-293748.pdf
	for (int row= 0; row< 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			if (row< 3 && column < 3)
				transform[4 * row+ column] = Q(column, row); // Rotation submatrix
			else if (row< 3 && column == 3)
				transform[4 * row+ column] = 0; // zeros on right
			else if (row == 3 && column< 3)
				transform[4 * row + column] = translation.GetElement(column); // translation vector
			else
				transform[4 * row+ column] = 1; // trailing 1 in lower right hand corner
		}
	}

}
#endif

CharImageType::Pointer Correlator3D::ResampleImage(CharImageType::Pointer &image, CharImageType::SizeType &size){
	ResampleFilterType::Pointer filter = ResampleFilterType::New();
	TransformType::Pointer transform = TransformType::New();
	InterpolatorType::Pointer interpolator = InterpolatorType::New();
	int max = 250;
	double reductionFactor = 1;
	while (size[0] * reductionFactor > max || size[1] * reductionFactor > max || size[2] * reductionFactor > max){
		reductionFactor -= 0.001;
	}

	filter->SetInterpolator(interpolator);
	filter->SetDefaultPixelValue(0);

	CharImageType::SpacingType newSpacing;
	CharImageType::SpacingType spacing;
	spacing = image->GetSpacing();
	newSpacing[0] = spacing[0] * reductionFactor;
	newSpacing[1] = spacing[1] * reductionFactor;
	newSpacing[2] = spacing[2] * reductionFactor;
	filter->SetOutputSpacing(newSpacing);
	spacing = newSpacing;

	filter->SetOutputOrigin(image->GetOrigin());
	CharImageType::DirectionType direction;
	direction.SetIdentity();
	filter->SetOutputDirection(direction);

	CharImageType::SizeType newSize;
	newSize[0] = size[0] * reductionFactor;
	newSize[1] = size[1] * reductionFactor;
	newSize[2] = size[2] * reductionFactor;
	filter->SetSize(newSize);
	size = newSize;

	filter->SetInput(image);
	filter->Update();

	CharWriterType::Pointer charWriter = CharWriterType::New();
	charWriter->SetInput(filter->GetOutput());
	charWriter->SetFileName("C:/Scans/resampledImage.nrrd");

	try{
		charWriter->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		errorFile << err << std::endl;
	}

	return filter->GetOutput();
}


bool TestValidPoints(Vector3Vec &fixedPoints, Vector3Vec &movingPoints);
void FindExtrema(Vector3 &minimum, Vector3 &maximum, Vector3Vec &list, CharImageType::SizeType &imageSize, CharImageType::SpacingType &imageSpacing, std::ofstream &errorFile);
void SetBounds(Vector3 &minimum, Vector3 &maximum, ROIFilterType::Pointer &ROIFilter, std::ofstream &errorFile);

void Correlator3D::SetROI(Vector3Vec &fixedPoints, Vector3Vec &movingPoints){

	
	if (TestValidPoints(fixedPoints, movingPoints)){
		Vector3 fixedMinimum;
		Vector3 fixedMaximum;
		Vector3 movingMinimum;
		Vector3 movingMaximum;

		FindExtrema(fixedMinimum, fixedMaximum, fixedPoints, fixedSize, fixedSpacing, errorFile);
		FindExtrema(movingMinimum, movingMaximum, movingPoints, movingSize, movingSpacing, errorFile);

		fixedROI->SetInput(fixedCastFilter->GetOutput());
		SetBounds(fixedMinimum, fixedMaximum, fixedROI, errorFile);

		movingROI->SetInput(movingCastFilter->GetOutput());
		SetBounds(movingMinimum, movingMaximum, movingROI, errorFile);

		WriterType::Pointer writer = WriterType::New();
		
		writer->SetInput(fixedCastFilter->GetOutput());


		writer->SetFileName("C:/Scans/fixedCastFilter.nrrd");
		try{
			writer->Update();
		}
		catch (itk::ExceptionObject &err)
		{
			errorFile << err << std::endl;
		}

		writer->SetInput(movingROI->GetOutput());
		writer->SetFileName("C:/Scans/movingROI.nrrd");
		try{
			writer->Update();
		}
		catch (itk::ExceptionObject &err)
		{
			errorFile << err << std::endl;
		}
		transformMax = movingMinimum;
		transformMin = movingMaximum;
		
	}

}
void Correlator3D::SetROI(Vector3 &max, Vector3 &min){
	movingROI->SetInput(movingTransform1);
	SetBounds(min, max, movingROI, errorFile);
}
bool TestValidPoints(Vector3Vec &fixedPoints, Vector3Vec &movingPoints){
	
	if (movingPoints.size() < 4)
	{
		//MessageBox::Show("Left volume does not have any landmark points to use for registration");
		return false;
	}

	if (fixedPoints.size() < 4)
	{
		//MessageBox::Show("Right volume does not have any landmark points to use for registration");
		return false;
	}
	return true;
}
void FindExtrema(Vector3 &minimum, Vector3 &maximum, Vector3Vec &list, CharImageType::SizeType &imageSize, CharImageType::SpacingType &imageSpacing, std::ofstream &errorFile){
	minimum.x = list.at(0).x;
	minimum.y = list.at(0).y;
	minimum.z = list.at(0).z;
	maximum.x = list.at(0).x;
	maximum.y = list.at(0).y;
	maximum.z = list.at(0).z;

	for (Vector3 vector : list){
		if (minimum.x > vector.x)
			minimum.x = vector.x;
		if (minimum.y > vector.y)
			minimum.y = vector.y;
		if (minimum.z > vector.z)
			minimum.z = vector.z;
		if (maximum.x < vector.x)
			maximum.x = vector.x;
		if (maximum.y < vector.y)
			maximum.y = vector.y;
		if (maximum.z < vector.z)
			maximum.z = vector.z;
	}



	//errorFile << minimum.x << "\t" << minimum.y << "\t" << minimum.z << std::endl;
	//errorFile << maximum.x << "\t" << maximum.y << "\t" << maximum.z << std::endl;

	// convert from origin at cube center to origin at bottom left corner and from mm to voxels
	minimum.x = (minimum.x / imageSpacing[0] + imageSize[0] / 2);
	minimum.y = (minimum.y / imageSpacing[1] + imageSize[1] / 2);
	minimum.z = (minimum.z / imageSpacing[2] + imageSize[2] / 2);

	maximum.x = (maximum.x / imageSpacing[0] + imageSize[0] / 2);
	maximum.y = (maximum.y / imageSpacing[1] + imageSize[1] / 2);
	maximum.z = (maximum.z / imageSpacing[2] + imageSize[2] / 2);

	errorFile << minimum.x << "\t" << minimum.y << "\t" << minimum.z << std::endl;
	errorFile << maximum.x << "\t" << maximum.y << "\t" << maximum.z << std::endl;
}
void SetBounds(Vector3 &minimum, Vector3 &maximum, ROIFilterType::Pointer &ROIFilter, std::ofstream &errorFile){
	
	FloatImageType::IndexType start;
	start[0] = minimum.x;
	start[1] = minimum.y;
	start[2] = minimum.z;

	FloatImageType::SizeType size;
	size[0] = maximum.x - minimum.x;
	size[1] = maximum.y - minimum.y;
	size[2] = maximum.z - minimum.z;
	
	FloatImageType::RegionType regionOfInterest;
	regionOfInterest.SetSize(size);
	regionOfInterest.SetIndex(start);

	ROIFilter->SetRegionOfInterest(regionOfInterest);
	
	try{
		ROIFilter->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		errorFile << err << std::endl;
	}

	
}

bool Correlator3D::SetROISource(int source){
	switch (source){
	case MAIN:
		fixedROI->SetInput(fixedCastFilter->GetOutput());
		movingROI->SetInput(movingCastFilter->GetOutput());
		return true;
	case CANNY:
		
		return false;
	}

	return false;
}

void Correlator3D::DeformableRegistration(double *transformMoving){
	SetROI(transformMin, transformMax);
	
	MetricType::Pointer         metric = MetricType::New();
	OptimizerType::Pointer      optimizer = OptimizerType::New();
	RegistrationType::Pointer   registration = RegistrationType::New();
	registration->SetMetric(metric);
	registration->SetOptimizer(optimizer);

	BSplineTransformType::Pointer    transform = BSplineTransformType::New();

	InitializerType::Pointer transformInitializer = InitializerType::New();

	unsigned int numberOfGridNodesInOneDimension = 8;
	BSplineTransformType::MeshSizeType meshSize;
	meshSize.Fill(numberOfGridNodesInOneDimension - SplineOrder);

	transformInitializer->SetTransform(transform);
	transformInitializer->SetImage(fixedCastFilter->GetOutput());
	transformInitializer->SetTransformDomainMeshSize(meshSize);
	transformInitializer->InitializeTransform();

	transform->SetIdentity();

	registration->SetInitialTransform(transform);
	registration->InPlaceOn();

	registration->SetFixedImage(fixedROI->GetOutput());
	registration->SetMovingImage(movingROI->GetOutput());

	ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
	scalesEstimator->SetMetric(metric);
	scalesEstimator->SetTransformForward(true);
	scalesEstimator->SetSmallParameterVariation(1.0);

	const unsigned int numParameters = transform->GetNumberOfParameters();
	optimizer->SetGradientConvergenceTolerance(5e-2);
	//optimizer->SetLineSearchAccuracy(1.2);
	//optimizer->SetDefaultStepLength(1.5);
	optimizer->TraceOn();
	optimizer->SetMaximumNumberOfFunctionEvaluations(1000);
	optimizer->SetScalesEstimator(scalesEstimator);
	OptimizerType::BoundSelectionType boundSelect(numParameters);
	OptimizerType::BoundValueType upperBound(numParameters);
	OptimizerType::BoundValueType lowerBound(numParameters);
	boundSelect.Fill(OptimizerType::UNBOUNDED);
	upperBound.Fill(0.0);
	lowerBound.Fill(0.0);
	optimizer->SetBoundSelection(boundSelect);
	optimizer->SetUpperBound(upperBound);
	optimizer->SetLowerBound(lowerBound);

	const unsigned int numberOfLevels = 1;

	RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	shrinkFactorsPerLevel.SetSize(1);
	shrinkFactorsPerLevel[0] = 1;
	RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	smoothingSigmasPerLevel.SetSize(1);
	smoothingSigmasPerLevel[0] = 0;
	registration->SetNumberOfLevels(numberOfLevels);
	registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

	try
	{
		
		registration->Update();
		
		errorFile << "Optimizer stop condition = "
			<< registration->GetOptimizer()->GetStopConditionDescription()
			<< std::endl;
	}
	catch (itk::ExceptionObject & err)
	{
		errorFile << "ExceptionObject caught !" << std::endl;
		errorFile << err << std::endl;
	}
	// Report the time and memory taken by the registration

}