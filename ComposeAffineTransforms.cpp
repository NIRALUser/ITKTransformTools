#include "ComposeAffineTransforms.h"



int ComposeAffineTransforms( int argc , char* argv[] )
{
  if( argc != 5 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " Transform1 Transform2 outputTransform" << std::endl ;
    return 1 ;
  }
  std::string input1 ;
  input1.assign( argv[ 2 ] ) ;
  std::string input2 ;
  input2.assign( argv[ 3 ] ) ;
  std::string output ;
  output.assign( argv[ 4 ] ) ;
  //Read transform files
  itk::TransformFileReader::Pointer transformFile1 ;
  transformFile1 = itk::TransformFileReader::New() ;
  transformFile1->SetFileName( input1 ) ;
  transformFile1->Update() ;
  itk::TransformFileReader::Pointer transformFile2 ;
  transformFile2 = itk::TransformFileReader::New() ;
  transformFile2->SetFileName( input2 ) ;
  transformFile2->Update() ;
  //Check that transform files contain only one transform
  if( transformFile1->GetTransformList()->size() != 1
   || transformFile2->GetTransformList()->size() != 1
    )
  {
     std::cout<< "Transform files must contain only 1 transform" << std::endl ;
     return 1 ;
  }
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  //AffineTransformType::Pointer composedTransform = AffineTransform::New() ;
  //composedTransform->SetIdentity() ;
  //Load the transforms
  AffineTransformType::Pointer affineTransform1
            = dynamic_cast< AffineTransformType* > ( transformFile1->GetTransformList()->front().GetPointer() ) ;
  AffineTransformType::Pointer affineTransform2
            = dynamic_cast< AffineTransformType* > ( transformFile2->GetTransformList()->front().GetPointer() ) ;
          if( !affineTransform1 || !affineTransform2 )
            {
            std::cout << "Transforms must be of type AffineTransform_double_3_3" << std::endl ;
            return 1 ;
            }
  affineTransform1->Compose( affineTransform2 , true ) ;
  //Compose transforms
  //Save transform
  itk::TransformFileWriter::Pointer outputTransformFile ;
  outputTransformFile = itk::TransformFileWriter::New() ;
  outputTransformFile->SetFileName( output.c_str() ) ;
  outputTransformFile->SetInput( affineTransform1 ) ;
  outputTransformFile->Update() ;
  return 0 ;
}
