#include "DirectionToTransformMatrix.h"


int DirectionToTransformMatrix( int argc , char* argv[] )
{
  if( argc != 4 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputImage outputTransform" << std::endl ;
    return 1 ;
  }
  typedef itk::Image< unsigned char , 3 > ImageType ;
  typedef itk::ImageFileReader< ImageType > ImageReaderType ;
  ImageReaderType::Pointer imageFile = ImageReaderType::New() ;
  imageFile->SetFileName( argv[ 2 ] ) ;
  imageFile->Update() ;
  typedef ImageType::DirectionType MatrixType ;
  MatrixType matrix = imageFile->GetOutput()->GetDirection() ;
  //Set parameters
  typedef itk::Rigid3DTransform< double > RigidTransformType ;
  RigidTransformType::Pointer rigid = RigidTransformType::New() ;
  rigid->SetMatrix( matrix ) ;
  //Creation of the itkTransformFileWriter
  itk::TransformFileWriter::Pointer transformWriter = itk::TransformFileWriter::New() ;
  transformWriter->SetFileName( argv[ 3 ] ) ;
  transformWriter->AddTransform( rigid ) ;
  transformWriter->Update() ;
  return 0 ;
}
