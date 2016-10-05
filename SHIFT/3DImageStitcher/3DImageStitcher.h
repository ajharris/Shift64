#pragma once
#include <iostream>
#include <fstream>
#include <limits>
#include <time.h>
#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkCenteredTransformInitializer.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkCorrelationImageToImageMetricv4.h"
#include "itkCastImageFilter.h"
#include "itkImageRegistrationMethod.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkPowellOptimizer.h"
#include "itkLBFGSBOptimizerv4.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkImageFileReader.h"
#include "itkThresholdImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkPowellOptimizerv4.h"
//#include "ImageReader.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBinomialBlurImageFilter.h"

// compile switch definitions
#define v4

typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::Image<unsigned char, Dimension>													CharImageType;
typedef itk::Image<float, Dimension>															FloatImageType;
typedef itk::ImportImageFilter< unsigned char, Dimension >										ImportFilterType;
typedef itk::VersorRigid3DTransform<double>														TransformType;
typedef itk::CenteredTransformInitializer<TransformType, FloatImageType, FloatImageType>		TransformInitializerType;
typedef TransformType::VersorType																VersorType;
typedef VersorType::VectorType																	VectorType;
typedef itk::CastImageFilter< CharImageType, FloatImageType >									CastFilterType;
typedef itk::RegionOfInterestImageFilter<CharImageType, CharImageType>							ROIFilterType;
typedef itk::ThresholdImageFilter<CharImageType>												ThresholdFilterType;
typedef itk::ImageMaskSpatialObject<Dimension>													MaskType;
typedef itk::RescaleIntensityImageFilter<CharImageType, CharImageType>							RescaleFilterType;
typedef itk::ResampleImageFilter<CharImageType, CharImageType>									ResampleFilterType;
typedef itk::NearestNeighborInterpolateImageFunction<CharImageType, double>						InterpolatorType;
typedef itk::BinomialBlurImageFilter<CharImageType, CharImageType>								BinomialBlurFilterType;

#ifdef v4
typedef itk::ImageRegistrationMethodv4<FloatImageType, FloatImageType, TransformType>			RegistrationType;
//#define RegularStepGradientDescent
//typedef itk::RegularStepGradientDescentOptimizerv4<double>										OptimizerType;
#define LBFGSB
typedef itk::LBFGSBOptimizerv4																	OptimizerType;
//#define Powell
//typedef itk::PowellOptimizerv4<double>															OptimizerType;
typedef itk::CorrelationImageToImageMetricv4<FloatImageType, FloatImageType>					MetricType;
typedef OptimizerType::ScalesType																OptimizerScalesType;
#endif

#ifndef v4
typedef itk::ImageRegistrationMethod<FloatImageType, FloatImageType>							RegistrationType;
typedef itk::PowellOptimizer																	OptimizerType;
typedef itk::NormalizedCorrelationImageToImageMetric<FloatImageType, FloatImageType>			MetricType;


#endif

typedef OptimizerType::ScalesType																OptimizerScalesType;

class Correlator3D
{
public:

	Correlator3D(unsigned char *fixedImage, int *fixedImageSize, double *fixedImageSpacing, double fixedOrigin[Dimension],
		unsigned char *movingImage, int *movingImageSize, double *movingImageSpacing, double movingOrigin[Dimension]);

	Correlator3D();

	~Correlator3D();
	//void Correlator3D::Initialize(ImageReader fixedImageReader, ImageReader movingImageReader);
	void Correlator3D::ConvertBufferToImage(unsigned char *volumeBuffer, int *imageSize, double *imageSpacing, double origin[3], bool crop, CharImageType::Pointer &image, CastFilterType::Pointer &castFilter, int threshold, MaskType::Pointer &mask, CharImageType::SizeType &objectSize);
	void Correlator3D::GenerateThresholdMask(float threshold, CharImageType::Pointer &charImage, MaskType::Pointer &mask);
	void Correlator3D::ConvertBufferToImage(unsigned char *volumeBuffer, int *imageSize, double *imageSpacing, CharImageType::Pointer &image, FloatImageType::Pointer &floatImage);
	void Correlator3D::ImageCorrelation(double *transformFixed, double *transformMoving);
	void Correlator3D::Initialize(unsigned char *fixedImage, int *fixedImageSize, double *fixedImageSpacing, double fixedOrigin[Dimension],
		unsigned char *movingImage, int *movingImageSize, double *movingImageSpacing, double movingOrigin[Dimension]);

private:
	CharImageType::Pointer objectFixedCharImage;
	CharImageType::Pointer objectMovingCharImage;
	CharImageType::SizeType fixedSize;
	CharImageType::SizeType movingSize;
	CastFilterType::Pointer fixedCastFilter;
	CastFilterType::Pointer movingCastFilter;
	MaskType::Pointer fixedMask;
	MaskType::Pointer movingMask;
	void Correlator3D::CastImageToFloat(CharImageType::Pointer &image, FloatImageType::Pointer &floatImage, CharImageType::SizeType &objectSize);
	void Correlator3D::GenerateTransformMatrix(TransformType::MatrixType &matrix, TransformType::OffsetType &offset, double *transform);
	CharImageType::Pointer Correlator3D::ResampleImage(CharImageType::Pointer &image, CharImageType::SizeType &size);
};