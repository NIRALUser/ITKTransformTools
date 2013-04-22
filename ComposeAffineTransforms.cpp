#include "ComposeAffineTransforms.h"



int ReadTransform( itk::TransformFileReader::Pointer transformFile , itk::AffineTransform< double , 3 >::Pointer &affineTransform )
{
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  typedef itk::Rigid3DTransform< double > Rigid3DTransformType ;
  //Load the transforms
  Rigid3DTransformType::Pointer rigidTransform ;
  affineTransform
            = dynamic_cast< AffineTransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
  if( !affineTransform )
  {
    rigidTransform = dynamic_cast< Rigid3DTransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
    if( !rigidTransform )
    {
       std::cerr << "Transforms must be of type AffineTransform_double_3_3 or Rigid3DTransform_double_3_3" << std::endl ;
       return 1 ;
    }
    affineTransform = AffineTransformType::New() ;
    affineTransform->SetMatrix( rigidTransform->GetMatrix() ) ;
    affineTransform->SetTranslation( rigidTransform->GetTranslation() ) ;
  }
  return 0 ;
}

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
  AffineTransformType::Pointer affineTransform1 ;
  AffineTransformType::Pointer affineTransform2 ;
  if( ReadTransform( transformFile1 , affineTransform1 ) )
  {
    return 1 ;
  }
  if( ReadTransform( transformFile2 , affineTransform2 ) )
  {
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
