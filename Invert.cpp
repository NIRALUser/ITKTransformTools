#include "Invert.h"
#include <itkTransformFactory.h>
#include <itkMatrixOffsetTransformBase.h>

int Invert( int argc , char* argv[] )
{
  //check arguments
  bool badArguments = false ;
  if( argc != 4 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputTransform outputTransform" << std::endl ;
    return 1 ;
  }
  typedef itk::MatrixOffsetTransformBase< double , 3 , 3 > TransformType ;
  itk::TransformFactory< TransformType >::RegisterTransform();
  itk::TransformFileReader::Pointer transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( argv[ 2 ] ) ;
  transformFile->Update() ;
  if( transformFile->GetTransformList()->size() != 1 )
  {
    std::cerr << "Please give a transform file containing only one transformation" << std::endl ;
    return 1 ;
  }
  TransformType::Pointer transform ;
  transform = dynamic_cast< TransformType* >
        ( transformFile->GetTransformList()->front().GetPointer() ) ;
  if( !transform )
  {
    std::cerr << "Transform type is not handled. Please convert your transform first" << std::endl ;
    return 1 ;
  }
  TransformType::Pointer inverse = TransformType::New() ;
  transform->GetInverse( inverse ) ;
  itk::TransformFileWriter::Pointer transformWriter = itk::TransformFileWriter::New() ;
  transformWriter->SetFileName( argv[ 3 ] ) ;
  transformWriter->AddTransform( inverse ) ;
  transformWriter->Update() ;
}
