/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkCurvilinearArraySpecialCoordinatesImage.h"
#include "itkSliceSeriesSpecialCoordinatesImage.h"

#include "itkUltrasoundImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkEuler3DTransform.h"
#include "itkMath.h"
#include "itkTestingMacros.h"


int itkSliceSeriesSpecialCoordinatesImageTest( int argc, char * argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage outputImage";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }
  const char * inputImageFileName = argv[1];
  const char * outputImageFileName = argv[2];

  const unsigned int Dimension = 3;
  const unsigned int SliceDimension = Dimension - 1;

  typedef unsigned char PixelType;
  typedef double        ParametersValueType;

  typedef itk::Image< PixelType, Dimension >                                        ImageType;
  typedef itk::CurvilinearArraySpecialCoordinatesImage< PixelType, SliceDimension > SliceImageType;
  typedef itk::Euler3DTransform< ParametersValueType >                              TransformType;
  typedef itk::SliceSeriesSpecialCoordinatesImage< SliceImageType, TransformType >  SliceSeriesImageType;


  typedef itk::UltrasoundImageFileReader < SliceImageType > SliceReaderType;
  SliceReaderType::Pointer sliceReader = SliceReaderType::New();
  sliceReader->SetFileName( inputImageFileName );
  try
    {
    sliceReader->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }
  SliceImageType::Pointer sliceImage = sliceReader->GetOutput();
  sliceImage->Print( std::cout );

  typedef itk::ImageFileReader< SliceSeriesImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImageFileName );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }
  SliceSeriesImageType::Pointer image = reader->GetOutput();
  image->SetSliceImage( sliceImage );

  double elevationAngle = 0.0;
  const itk::SizeValueType numberOfSlices = image->GetLargestPossibleRegion().GetSize()[Dimension - 1];
  for( itk::SizeValueType sliceIndex = 0; sliceIndex < numberOfSlices; ++sliceIndex )
    {
    TransformType::Pointer transform = TransformType::New();
    transform->SetRotation( sliceIndex * 5.0 * itk::Math::pi / 180.0, 0.0, 0.0 );
    image->SetSliceTransform( sliceIndex, transform );
    std::cout << "sliceIndex: " << sliceIndex << std::endl;
    std::cout << "transform: " << std::endl;
    transform->Print( std::cout );
    }
  TransformType::ConstPointer obtainedTransform = image->GetSliceTransform( 0 );

  image->Print( std::cout );

  SliceSeriesImageType::IndexType index;
  index[0] = 1000;
  index[1] = 150;
  index[2] = 1;
  std::cout << "Input index: " << index << std::endl;
  SliceSeriesImageType::PointType point;
  image->TransformIndexToPhysicalPoint( index, point );
  std::cout << "Transformed point: " << point << std::endl;
  SliceSeriesImageType::IndexType transformedIndex;
  image->TransformPhysicalPointToIndex( point, transformedIndex );
  std::cout << "Transformed index: " << transformedIndex << std::endl;
  std::cout << std::endl;

  typedef itk::ContinuousIndex< double, Dimension > ContinuousIndexType;
  ContinuousIndexType continuousIndex;
  continuousIndex[0] = 1000.5;
  continuousIndex[1] = 150.5;
  continuousIndex[2] = 1.5;
  std::cout << "Continuous index: " << continuousIndex << std::endl;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, point );
  std::cout << "Transformed point: " << point << std::endl;
  ContinuousIndexType continuousTransformedIndex;
  image->TransformPhysicalPointToContinuousIndex( point, continuousTransformedIndex );
  std::cout << "Transformed continuous index: " << continuousTransformedIndex << std::endl;
  std::cout << std::endl;

  continuousIndex[0] = 500.0;
  continuousIndex[1] = 50.0;
  continuousIndex[2] = 0.0;
  std::cout << "Continuous index: " << continuousIndex << std::endl;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, point );
  std::cout << "Transformed point: " << point << std::endl;
  image->TransformPhysicalPointToContinuousIndex( point, continuousTransformedIndex );
  std::cout << "Transformed continuous index: " << continuousTransformedIndex << std::endl;
  std::cout << std::endl;

  continuousIndex[0] = 0.0;
  continuousIndex[1] = 0.0;
  continuousIndex[2] = 0.0;
  std::cout << "Continuous index: " << continuousIndex << std::endl;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, point );
  std::cout << "Transformed point: " << point << std::endl;
  image->TransformPhysicalPointToContinuousIndex( point, continuousTransformedIndex );
  std::cout << "Transformed continuous index: " << continuousTransformedIndex << std::endl;
  std::cout << std::endl;

  continuousIndex[0] = 2047.0;
  continuousIndex[1] = 240.0;
  continuousIndex[2] = 2.0;
  std::cout << "Continuous index: " << continuousIndex << std::endl;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, point );
  std::cout << "Transformed point: " << point << std::endl;
  image->TransformPhysicalPointToContinuousIndex( point, continuousTransformedIndex );
  std::cout << "Transformed continuous index: " << continuousTransformedIndex << std::endl;
  std::cout << std::endl;

  ImageType::SizeType inputSize = image->GetLargestPossibleRegion().GetSize();

  typedef itk::ResampleImageFilter< SliceSeriesImageType, ImageType > ResamplerType;
  ResamplerType::Pointer resampler = ResamplerType::New();
  resampler->SetInput( image );
  resampler->SetDefaultPixelValue( 33 );
  ImageType::SizeType outputSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  outputSize[0] = 100;
  outputSize[1] = 100;
  outputSize[2] = 20;
  resampler->SetSize( outputSize );

  continuousIndex[0] = inputSize[0] - 1;
  continuousIndex[1] = 0;
  continuousIndex[2] = 0;
  SliceSeriesImageType::PointType xMinPoint;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, xMinPoint );
  std::cout << "xMinPoint: " << xMinPoint << std::endl;

  continuousIndex[0] = inputSize[0] - 1;
  continuousIndex[1] = inputSize[1] - 1;
  continuousIndex[2] = 0;
  SliceSeriesImageType::PointType xMaxPoint;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, xMaxPoint );
  std::cout << "xMaxPoint: " << xMaxPoint << std::endl;

  continuousIndex[0] = 0;
  continuousIndex[1] = 0;
  continuousIndex[2] = inputSize[2] - 1;
  SliceSeriesImageType::PointType yMinPoint;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, yMinPoint );
  std::cout << "yMinPoint: " << yMinPoint << std::endl;

  continuousIndex[0] = inputSize[0] - 1;
  continuousIndex[1] = inputSize[1] / 2;
  continuousIndex[2] = 0;
  SliceSeriesImageType::PointType yMaxPoint;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, yMaxPoint );
  std::cout << "yMaxPoint: " << yMaxPoint << std::endl;

  continuousIndex[0] = 0;
  continuousIndex[1] = 0;
  continuousIndex[2] = 0;
  SliceSeriesImageType::PointType zMinPoint;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, zMinPoint );
  std::cout << "zMinPoint: " << zMinPoint << std::endl;

  continuousIndex[0] = inputSize[0] - 1;
  continuousIndex[1] = inputSize[1] / 2;
  continuousIndex[2] = inputSize[2] - 1;
  SliceSeriesImageType::PointType zMaxPoint;
  image->TransformContinuousIndexToPhysicalPoint( continuousIndex, zMaxPoint );
  std::cout << "zMaxPoint: " << zMaxPoint << std::endl;

  std::cout << std::endl;
  ImageType::PointType outputOrigin;
  outputOrigin[0] = xMinPoint[0];
  outputOrigin[1] = yMinPoint[1];
  outputOrigin[2] = zMinPoint[2];
  resampler->SetOutputOrigin( outputOrigin );
  std::cout << "Output origin: " << outputOrigin << std::endl;

  ImageType::SpacingType outputSpacing;
  outputSpacing[0] = (xMaxPoint[0] - xMinPoint[0]) / (outputSize[0] - 1);
  outputSpacing[1] = (yMaxPoint[1] - yMinPoint[1]) / (outputSize[1] - 1);
  outputSpacing[2] = (zMaxPoint[2] - zMinPoint[2]) / (outputSize[2] - 1);
  resampler->SetOutputSpacing( outputSpacing );
  std::cout << "Output spacing: " << outputSpacing << std::endl;

  typedef itk::WindowedSincInterpolateImageFunction< SliceSeriesImageType, 3 > WindowedSincInterpolatorType;
  WindowedSincInterpolatorType::Pointer sincInterpolator = WindowedSincInterpolatorType::New();
  sincInterpolator->SetInputImage( image );

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( resampler->GetOutput() );
  writer->SetUseCompression( true );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }

  // Check CopyInformation
  SliceSeriesImageType::Pointer imageCopiedInformation = SliceSeriesImageType::New();
  imageCopiedInformation->CopyInformation( image );

  continuousIndex[0] = 1000.5;
  continuousIndex[1] = 150.5;
  continuousIndex[2] = 1.5;
  std::cout << "\nContinuous index: " << continuousIndex << std::endl;
  imageCopiedInformation->TransformContinuousIndexToPhysicalPoint( continuousIndex, point );
  std::cout << "Transformed point: " << point << std::endl;
  std::cout << imageCopiedInformation << std::endl;
  TEST_EXPECT_TRUE( itk::Math::FloatAlmostEqual( point[0], 20.2306, 10, 1e-3 ) );
  TEST_EXPECT_TRUE( itk::Math::FloatAlmostEqual( point[1], 74.3784, 10, 1e-3 ) );
  TEST_EXPECT_TRUE( itk::Math::FloatAlmostEqual( point[2], 9.7921, 10, 1e-3 ) );

  return EXIT_SUCCESS;
}
