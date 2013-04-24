#include "PrintITKTransform.h"



int PrintITKTransform( int argc , char* argv[] )
{
  if( argc != 3 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " TransformFile" << std::endl ;
    return 1 ;
  }
  std::string input ;
  input.assign( argv[ 2 ] ) ;
  //Read transform files
  itk::TransformFileReader::Pointer transformFile ;
  transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( input ) ;
  transformFile->Update() ;
  //Check that transform files contain only one transform
  if( transformFile->GetTransformList()->size() != 1
    )
  {
     std::cout<< "Transform files must contain only 1 transform" << std::endl ;
     return 1 ;
  }
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  AffineTransformType::Pointer affineTransform ;
  std::cout << transformFile->GetTransformList()->front().GetPointer()->GetTransformTypeAsString() << std::endl ;
  if( ReadTransform( transformFile , affineTransform ) )
  {
    return 1 ;
  }
  std::cout << "Matrix = " << std::endl << affineTransform->GetMatrix() << std::endl ;
  std::cout << "Offset = " << affineTransform->GetOffset() << std::endl ;
  return 0 ;
}
