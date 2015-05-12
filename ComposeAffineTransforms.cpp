#include "ComposeAffineTransforms.h"
#include <itkTransformFactory.h>


int ReadTransform( itk::TransformFileReader::Pointer transformFile , itk::AffineTransform< double , 3 >::Pointer &affineTransform )
{
  itk::TransformFactory< itk::MatrixOffsetTransformBase<double, 3, 3> >::RegisterTransform();
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  typedef itk::Rigid3DTransform< double > Rigid3DTransformType ;
	typedef itk::MatrixOffsetTransformBase< double , 3 , 3 > MatrixOffsetTransformBaseDoubleType ;
  //Load the transforms
  Rigid3DTransformType::Pointer rigidTransform ;
	MatrixOffsetTransformBaseDoubleType::Pointer matrixOffsetTransformBaseDouble ;
  affineTransform
            = dynamic_cast< AffineTransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
  if( !affineTransform )
  {
    rigidTransform = dynamic_cast< Rigid3DTransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
    if( !rigidTransform )
    {
      matrixOffsetTransformBaseDouble = dynamic_cast< MatrixOffsetTransformBaseDoubleType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
      if( !matrixOffsetTransformBaseDouble )
      {
       std::cerr << "Transforms must be of type AffineTransform_double_3_3, Rigid3DTransform_double_3_3, or MatrixOffsetTransformBaseDoubleType_double_3_3" << std::endl ;
       return 1 ;
      }
      else
      {
        affineTransform = AffineTransformType::New() ;
        affineTransform->SetMatrix( matrixOffsetTransformBaseDouble->GetMatrix() ) ;
        affineTransform->SetOffset( matrixOffsetTransformBaseDouble->GetOffset() ) ;
      }
    }
    else
    {
      affineTransform = AffineTransformType::New() ;
      affineTransform->SetMatrix( rigidTransform->GetMatrix() ) ;
      affineTransform->SetOffset( rigidTransform->GetOffset() ) ;
    }
  }
  transformFile->GetTransformList()->pop_front() ;
  return 0 ;
}

int ComposeAffineWithFile( itk::AffineTransform< double , 3 >::Pointer &affineTransform1 , itk::TransformFileReader::Pointer &transformFile )
{
  //Compose transforms
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  AffineTransformType::Pointer affineTransform2 ;
  while( transformFile->GetTransformList()->size() ) 
  {
    if( ReadTransform( transformFile , affineTransform2 ) )
    {
      return 1 ;
    }
    affineTransform1->Compose( affineTransform2 , true ) ;
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
  //Compose
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  AffineTransformType::Pointer affineTransform ;
  itk::TransformFileReader::Pointer transformFile ;
  itk::TransformFactory< itk::MatrixOffsetTransformBase<double, 3, 3> >::RegisterTransform();
  transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( input1 ) ;
  transformFile->Update() ;
  ReadTransform( transformFile , affineTransform ) ;
  if( ComposeAffineWithFile( affineTransform , transformFile ) )
  {
    return 1 ;
  }
  transformFile->SetFileName( input2 ) ;
  transformFile->Update() ;
  if( ComposeAffineWithFile( affineTransform , transformFile ) )
  {
    return 1 ;
  }
  //Save transform
  itk::TransformFileWriter::Pointer outputTransformFile ;
  outputTransformFile = itk::TransformFileWriter::New() ;
  outputTransformFile->SetFileName( output.c_str() ) ;
  outputTransformFile->SetInput( affineTransform ) ;
  outputTransformFile->Update() ;
  return 0 ;
}
