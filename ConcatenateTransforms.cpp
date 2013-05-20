#include "ConcatenateTransforms.h"
#include "ComposeAffineTransforms.h"
#include "string.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include <list>
#include <itkVectorLinearInterpolateImageFunction.h>
#include <itkTransformDeformationFieldFilter.h>
#include <itkMatrix.h>
#include <itkPoint.h>
#include "dtiprocessFiles/deformationfieldio.h"
#include <itkVectorResampleImageFilter.h>
#include "itkWarpTransform3D.h"
#include <itkTransformFileReader.h>
#include <itkBSplineDeformableTransform.h>
#include <sstream> 
#include <itkImageFileWriter.h>
#include <itkTransformFactory.h>

void Print( std::ostream &stream , std::string text , bool print )
{
  if( print )
  {
    stream << text << std::endl ;
  }
}

int GetTransformFileType( const char *filename , bool print = false )
{
  typedef itk::ImageFileReader< DeformationImageType > ReaderType ;
  typedef itk::TransformFileReader TransformReaderType ;
  itk::TransformFactory< itk::MatrixOffsetTransformBase<double, 3, 3> >::RegisterTransform();
  ReaderType::Pointer reader = ReaderType::New() ;
  int val = 1 ;
  try
  {
    //tries to open image
    reader->SetFileName( filename ) ;
    reader->UpdateOutputInformation() ;
    Print( std::cout , (std::string)filename + " -  deformation field" , print ) ;
    return 0 ;
  }
  catch(...)
  {
    TransformReaderType::Pointer transformFile = TransformReaderType::New() ;
    try
    {
      //tries to open file as an ITK transform file
      transformFile->SetFileName( filename ) ;
      transformFile->Update() ; 
      Print( std::cout , (std::string)filename + " -  ITK transformation file" , print ) ;
    }
    catch(...)
    {
      //neither a deformation field or an ITK transform file
      Print( std::cerr , (std::string)filename + " -  Error - Not a transformation file" , print ) ;
      return -1 ;
    }
    int i = 0 ;
    while( transformFile->GetTransformList()->size() )
    {
      std::string spacing = "   " ;
      std::stringstream pos ;
      pos << i++ ;
      //check if a bspline is contained in file
      if( transformFile->GetTransformList()->front().GetPointer()->GetTransformTypeAsString() ==
          "BSplineDeformableTransform_double_3_3" )
      {
        //yes
        Print( std::cout , spacing + pos.str() + " - BSpline transform" , print ) ;
        val = 2 ;
      }
      else
      {
        Print( std::cout , spacing + pos.str() + " - rigid/affine transform" , print ) ;
      }
      transformFile->GetTransformList()->pop_front();
    }
    //no spline
    return val ;
  } 
}


int ReadTransformsNames( int argc , char* argv[] , std::vector< std::string > &transformFileNames , std::vector< std::string > &deformationFieldType )
{
  for( int i = 0 ; i < argc ; i++ )
  {
    int transformType = GetTransformFileType( argv[ i ] , true ) ;
    if( transformType < 0 )
    {
      return 1 ;
    }
    else if( transformType == 0 )
    {
      //checks deformation field type. Checks if argument is given
      if( i >= argc - 1 )
      {
        //argument giving deformation field type is missing
        std::cerr << "Error: Deformation field type not specified" << std::endl ;
        return 1 ;
      }
      else
      {
        if( strcmp( argv[ i + 1 ] , "h-Field" ) && strcmp( argv[ i + 1 ] , "displacement" ) )//argument found does not correspond to argument expected
        {
          std::cerr << "Error: Deformation field type can be either displacement or h-Field" << std::endl ;
          return 1 ;
        }
        deformationFieldType.push_back( argv[ i + 1 ] ) ;
      }
      //saves transform file name
      transformFileNames.push_back( argv[ i ] ) ;
      i++ ;
    }
    else //ITK transform file
    {
      deformationFieldType.push_back( "" ) ;
      //saves transform file name
      transformFileNames.push_back( argv[ i ] ) ;
    }
  }
  return 0 ;
}

int NotOnlyAffine( std::vector< std::string > transformFileNames , std::vector< std::string > deformationFieldType )
{
  int foundNotAffine = 0 ;
  if( transformFileNames.size() != deformationFieldType.size() )
  {
    std::cerr << "Transformation file name container and deformation field type container do not have the same size" << std::endl ;
    return -1 ;
  }
  for( unsigned int i = 0 ; i < transformFileNames.size() ; i++ )//check if contains bspline transforms
  {
    if( deformationFieldType[ i ] == "" )
    {
      if( GetTransformFileType( transformFileNames[ i ].c_str() ) == 2 )
      {
        foundNotAffine += 1 ;
      }
    }
    else
    {
      foundNotAffine += 1 ;
    }
  }
  return foundNotAffine ;
}


// resamples field to output image size; local filter so that the memory is freed once it has run
void ResampleDeformationField( DeformationImageType::Pointer & field, itk::Image< float , 3 >::Pointer referenceImage )
{
  // Check if the field does not already have the same properties as the output image:
  // It would save some time if we did not have to resample the field
  itk::Point<double, 3>     origin ;
  itk::Vector<double, 3>    spacing ;
  itk::Size<3>              size ;
  itk::Matrix<double, 3, 3> direction ;

  origin = referenceImage->GetOrigin();
  spacing = referenceImage->GetSpacing();
  size = referenceImage->GetLargestPossibleRegion().GetSize();
  direction = referenceImage->GetDirection();
  if(  field->GetLargestPossibleRegion().GetSize() == size
       && field->GetSpacing() == spacing
       && field->GetDirection() == direction
       && field->GetOrigin() == origin
       )
    {
    return;
    }
  typedef itk::VectorLinearInterpolateImageFunction<DeformationImageType> VectorInterpolatorType;
  VectorInterpolatorType::Pointer linearVectorInterpolator = VectorInterpolatorType::New();
  typedef itk::VectorResampleImageFilter<DeformationImageType,
                                         DeformationImageType,
                                         double
                                         > ResampleImageFilter;
  ResampleImageFilter::Pointer resampleFieldFilter = ResampleImageFilter::New() ;
  DeformationPixelType defaultPixel ;
  defaultPixel.Fill( 0.0 );
  resampleFieldFilter->SetDefaultPixelValue( defaultPixel ) ;
  resampleFieldFilter->SetInput( field ) ;
  resampleFieldFilter->SetInterpolator( linearVectorInterpolator ) ;
  resampleFieldFilter->SetOutputDirection( direction ) ;
  resampleFieldFilter->SetSize( size ) ;
  resampleFieldFilter->SetOutputSpacing( spacing ) ;
  resampleFieldFilter->SetOutputOrigin( origin ) ;
  resampleFieldFilter->Update() ;
  field = resampleFieldFilter->GetOutput() ;
}

int AddTransforms( DeformationImageType::Pointer & field , itk::Transform<double, 3, 3 >::Pointer  transform )
{
  typedef itk::TransformDeformationFieldFilter<double, double, 3> itkTransformDeformationFieldFilterType;
  itkTransformDeformationFieldFilterType::Pointer transformDeformationFieldFilter =
       itkTransformDeformationFieldFilterType::New();
  transformDeformationFieldFilter->SetInput( field );
  transformDeformationFieldFilter->SetTransform( transform ) ;
  transformDeformationFieldFilter->SetNumberOfThreads( 1) ;
  transformDeformationFieldFilter->Update() ;
  field = transformDeformationFieldFilter->GetOutput() ;
  field->DisconnectPipeline() ;
}

int ConcatenateNonRigidTransforms( std::vector< std::string > transformFileNames , std::vector< std::string > deformationFieldType , std::string outputFileName , std::string referenceFileName )
{
  if( transformFileNames.size() != deformationFieldType.size() )
  {
    return 1 ;
  }
  //Non-rigid transforms are given, let's create an empty displacement field
  typedef itk::Image< float , 3 > ReferenceImageType ;
  typedef itk::ImageFileReader< ReferenceImageType > ReferenceReaderType ;
  ReferenceReaderType::Pointer referenceImageReader = ReferenceReaderType::New() ;
  referenceImageReader->SetFileName( referenceFileName ) ;
  referenceImageReader->UpdateOutputInformation() ;
  DeformationImageType::Pointer field = DeformationImageType::New() ;
  field->SetSpacing( referenceImageReader->GetOutput()->GetSpacing() );
  field->SetOrigin( referenceImageReader->GetOutput()->GetOrigin() );
  field->SetRegions( referenceImageReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  field->SetDirection( referenceImageReader->GetOutput()->GetDirection() );
  field->Allocate();
  DeformationPixelType vectorNull;
  vectorNull.Fill( 0.0 );
  field->FillBuffer( vectorNull );
  typedef itk::Transform<double, 3, 3 > TransformType ;
  for( unsigned int i = 0 ; i < transformFileNames.size() ; i++ )
  {
    if( deformationFieldType[ i ] != "" )
    {
      //load deformation field and add
      DeformationImageType::Pointer loadedField;
      // set if the field is a displacement or a H- field
      DeformationFieldType dftype = HField;
      if( deformationFieldType[ i ] == "displacement" )
      {
        dftype = Displacement ;
      }
      // reads deformation field and if it is a h-field, it transforms it to a displacement field
      loadedField = readDeformationField( transformFileNames[ i ] , dftype ) ;
//      ResampleDeformationField( loadedField , referenceImageReader->GetOutput() ) ;//Not sure if it should be done here. If Hfield and reference space are not aligned, may lose a lot of information
      // Create warp transform
      typedef itk::WarpTransform3D<double> WarpTransformType;
      WarpTransformType::Pointer warpTransform = WarpTransformType::New();
      warpTransform->SetDeformationField( loadedField ) ;
      TransformType::Pointer transform ;
      transform = warpTransform ;
      AddTransforms( field , transform ) ;
    }
    else
    {
      //load file and check if bspline or apply all transforms
      typedef itk::TransformFileReader TransformReaderType ;
      itk::TransformFactory< itk::MatrixOffsetTransformBase<double, 3, 3> >::RegisterTransform();
      TransformReaderType::Pointer transformFile = TransformReaderType::New() ;
      transformFile->SetFileName( transformFileNames[ i ] ) ;
      transformFile->Update() ; 
      while( transformFile->GetTransformList()->size() )
      {
        TransformType::Pointer transform ;
        transform = dynamic_cast< TransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
        if( !transform )
        {
          std::cerr << "Transform cannot be loaded" << std::endl ;
          return 1 ;
        }
        transformFile->GetTransformList()->pop_front() ;
        // order=3 for the BSpline seems to be standard among tools in Slicer3 and BRAINTools
        typedef itk::BSplineDeformableTransform<double, 3, 3> BSplineDeformableTransformType ;
        BSplineDeformableTransformType::Pointer BSplineTransform;
        BSplineTransform = dynamic_cast<BSplineDeformableTransformType *>( transform.GetPointer() );
        if( BSplineTransform && transformFile->GetTransformList()->size() ) // Checks if transform file contains a BSpline and a bulk transform
        {
          TransformType::Pointer bulkTransform;
          bulkTransform = dynamic_cast< TransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
          if( !bulkTransform )
          {
            std::cerr << "Bulk transform cannot be loaded" << std::endl ;
            return 1 ;
          }
          BSplineTransform->SetBulkTransform( bulkTransform ) ;
          transformFile->GetTransformList()->pop_front() ;
        }
        AddTransforms( field , transform ) ;
      }
    }
  }
  typedef itk::ImageFileWriter<DeformationImageType> WriterType ;
  WriterType::Pointer writer = WriterType::New() ;
  writer->SetFileName( outputFileName.c_str() ) ;
  writer->SetInput( field ) ;
  writer->UseCompressionOn();
  writer->Update() ;
  return 0 ;
}

int ConcatenateAffineTransforms( std::vector< std::string > transformFileNames , std::vector< std::string > deformationFieldType , std::string outputFileName )
{
  //Only affine transforms
  std::cout << "Only affine transforms" << std::endl ;
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  AffineTransformType::Pointer affineTransform ;
  itk::TransformFileReader::Pointer transformFile ;
  itk::TransformFactory< itk::MatrixOffsetTransformBase<double, 3, 3> >::RegisterTransform();
  transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( transformFileNames[ 0 ] ) ;
  transformFile->Update() ;
  ReadTransform( transformFile , affineTransform ) ;
  for( unsigned int i = 1 ; i < transformFileNames.size() ; i++ )
  {
    transformFile->SetFileName( transformFileNames[ i ] ) ;
    transformFile->Update() ;
    if( ComposeAffineWithFile( affineTransform , transformFile ) )
    {
      return 1 ;
    }
  }
  //Save transform
  itk::TransformFileWriter::Pointer outputTransformFile ;
  outputTransformFile = itk::TransformFileWriter::New() ;
  outputTransformFile->SetFileName( outputFileName.c_str() ) ;
  outputTransformFile->SetInput( affineTransform ) ;
  outputTransformFile->Update() ;
  return 0 ;
}

int Concatenate( int argc , char* argv[] )
{
  //check if contains the output file name and at least 2 transforms
  bool badArguments = false ;
  bool reference = false ;
  std::string referenceFileName ;
  unsigned int const minumNumberOfArguments = 4 ;
  int firstTransform = minumNumberOfArguments - 1 ;
  unsigned int numberOfOptionalArguments = 0 ;
  std::string outputFileName ;
  if( argc >= minumNumberOfArguments && !strcmp( argv[ firstTransform ] , "-r") )
  {
    reference = true ;
    numberOfOptionalArguments += 2 ;
  }
  if( !badArguments && argc < minumNumberOfArguments + numberOfOptionalArguments )
  {
    badArguments = true ;
  }
  firstTransform += numberOfOptionalArguments ;
  std::vector< std::string > deformationFieldType ;
  std::vector< std::string > transformFileNames ;
  if( !badArguments && ReadTransformsNames( argc - firstTransform , argv + firstTransform , transformFileNames , deformationFieldType ) )
  {
    badArguments = true ;
  }
  if( badArguments )
  {
    std::cout << argv[ 0 ] << " " << argv[ 1 ] << " outputTransform [-r ReferenceImage] [inputTransform1 [inputTransform2] [type:displacement/h-Field] ... ]" << std::endl ;
    std::cout << "inputTransforms should be in the order in which they are applied to the input image" << std::endl ;
    std::cout << "outputTransform is either an ITKTransform (if only affine transforms are given) or a displacement field saved as an ITK image. In the later case, a reference image is necessary" << std::endl ;
    return 1 ;
  }
  outputFileName = argv[ 2 ] ;
  if( NotOnlyAffine( transformFileNames , deformationFieldType ) )
  {
    if( !reference )
    {
      std::cerr << "Error: At least one transform is non-rigid. Please give a reference image to set the properties of the output deformation field" << std::endl ;
      return 1 ;
    }
    referenceFileName = argv[ firstTransform - 1 ] ;
    if( ConcatenateNonRigidTransforms( transformFileNames , deformationFieldType , outputFileName , referenceFileName ) )
    {
      return 1 ;
    }
  }
  else
  {
    if( ConcatenateAffineTransforms( transformFileNames , deformationFieldType , outputFileName ) )
    {
      return 1 ;
    }
  }
  return 0 ;
}

