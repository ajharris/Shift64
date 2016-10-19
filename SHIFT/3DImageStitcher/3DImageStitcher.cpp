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
	objectFixedCharImage = CharImageType::New();
	objectMovingCharImage = CharImageType::New();
	fixedCastFilter = CastFilterType::New();
	movingCastFilter = CastFilterType::New();
	fixedMask = MaskType::New();
	movingMask = MaskType::New();
	bool crop = false;
	int threshold = 40;
	ConvertBufferToImage(fixedImage, fixedImageSize, fixedImageSpacing, fixedOrigin, false, objectFixedCharImage, fixedCastFilter, threshold, fixedMask, fixedSize);
	ConvertBufferToImage(movingImage, movingImageSize, movingImageSpacing, movingOrigin, crop, objectMovingCharImage, movingCastFilter, threshold, movingMask, movingSize);
}

Correlator3D::Correlator3D()
{
}

Correlator3D::~Correlator3D()
{
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

	//	//else if (count > 100) break;
	//}

	//std::cin >> count;
	objectFixedCharImage = CharImageType::New();
	objectMovingCharImage = CharImageType::New();
	fixedCastFilter = CastFilterType::New();
	movingCastFilter = CastFilterType::New();
	fixedMask = MaskType::New();
	movingMask = MaskType::New();
	bool crop = false;
	int threshold = 40;
	ConvertBufferToImage(fixedImage, fixedImageSize, fixedImageSpacing, fixedOrigin, false, objectFixedCharImage, fixedCastFilter, threshold, fixedMask, fixedSize);
	ConvertBufferToImage(movingImage, movingImageSize, movingImageSpacing, movingOrigin, crop, objectMovingCharImage, movingCastFilter, threshold, movingMask, movingSize);
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
void Correlator3D::ConvertBufferToImage(unsigned char *volumeBuffer, int *imageSize, double *imageSpacing, double origin[3], bool crop, CharImageType::Pointer &image, CastFilterType::Pointer &castFilter, int threshold, MaskType::Pointer &mask, CharImageType::SizeType &objectSize)
{
	//method will convert array of unsigned char into itk::Image instance indicated using output

	//first the size and spacing of the image represented by the buffer is defined
	int count = 0;

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
	


	CharImageType::Pointer resampledImage = ResampleImage(image, objectSize);

	try{
		resampledImage->Update();
	}
	catch (itk::ExceptionObject &err){
		std::cerr << err << std::endl;
	}
	int repititions = 5;
	//BinomialBlurFilterType::Pointer blurFilter = BinomialBlurFilterType::New();
	//blurFilter->SetInput(resampledImage);
	//blurFilter->SetRepetitions(repititions);

	if (crop){
		// Code below will crop image to 1/4 of the original image size, centered around the origin

		CharImageType::IndexType roiStart;
		roiStart[0] = size[0] / 4;
		roiStart[1] = size[1] / 4;
		roiStart[2] = size[2] / 4;

		CharImageType::SizeType roiSize;
		roiSize[0] = size[0] / 2;
		roiSize[1] = size[1] / 2;
		roiSize[2] = size[2] / 2;

		CharImageType::RegionType desiredRegion;
		desiredRegion.SetSize(roiSize);
		desiredRegion.SetIndex(roiStart);

		ROIFilterType::Pointer roi = ROIFilterType::New();
		//roi->SetInput(blurFilter->GetOutput());
		roi->SetInput(resampledImage);
		roi->SetRegionOfInterest(desiredRegion);

		castFilter->SetInput(roi->GetOutput());

		CharImageType::Pointer roiImage = roi->GetOutput();


		try{
			castFilter->Update();
		}
		catch (itk::ExceptionObject &err){
			std::cerr << err << std::endl;
		}

		GenerateThresholdMask(threshold, roiImage, mask);

	}
	else{


		RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
		rescaleFilter->SetInput(resampledImage);
		rescaleFilter->SetOutputMinimum(0);
		rescaleFilter->SetOutputMaximum(255);

		//castFilter->SetInput(blurFilter->GetOutput());

		GenerateThresholdMask(threshold, resampledImage, mask);

		//castFilter->SetInput(mask->GetImage());
		castFilter->SetInput(mask->GetImage());
		try
		{
			castFilter->Update();
		}
		catch (itk::ExceptionObject &err)
		{
			std::cerr << err << std::endl;
		}

		
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
		std::cerr << err << std::endl;
	}
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName("C:/Scans/canny.nrrd");
	writer->SetInput(cannyFilter->GetOutput());
	try{
		writer->Update();
	}
	catch (itk::ExceptionObject &err){
		std::cerr << err << std::endl;
	}

	CastToCharImageType::Pointer castToCharFilter = CastToCharImageType::New();
	castToCharFilter->SetInput(cannyFilter->GetOutput());

	try{
		castToCharFilter->Update();
	}
	catch (itk::ExceptionObject &err){
		std::cerr << err << std::endl;
	}

	castFilter->SetInput(castToCharFilter->GetOutput());

	try
	{
		castFilter->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		std::cerr << err << std::endl;
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
		std::cerr << err;
	}
	thresholdFilter->SetInput(rescaleFilter->GetOutput());
	thresholdFilter->ThresholdBelow(upperThreshold);
	try{
		thresholdFilter->Update();
	}
	catch (itk::ExceptionObject &err){
		std::cerr << err;
	}

	mask->SetImage(thresholdFilter->GetOutput());
	try{
		mask->Update();
	}
	catch (itk::ExceptionObject &err){
		std::cerr << err << std::endl;
	}

	
	MaskToImageType::Pointer maskWriter = MaskToImageType::New();
	maskWriter->SetInput(mask);
	try{
		maskWriter->Update();
	}
	catch (itk::ExceptionObject &err){
		std::cerr << err << std::endl;
	}
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName("C:/Scans/mask.nrrd");
	writer->SetInput(maskWriter->GetOutput());
	try{
		writer->Update();
	}
	catch (itk::ExceptionObject &err){
		std::cerr << err << std::endl;
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

	registration->SetFixedImage(fixedCastFilter->GetOutput());
	registration->SetMovingImage(movingCastFilter->GetOutput());

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
	boundSelect.Fill(OptimizerType::UNBOUNDED);
	upperBound.Fill(100.0);
	lowerBound.Fill(0.0);
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
	newSpacing[0] = spacing[0] / reductionFactor;
	newSpacing[1] = spacing[1] / reductionFactor;
	newSpacing[2] = spacing[2] / reductionFactor;
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

	return filter->GetOutput();
}