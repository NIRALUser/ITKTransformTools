#include "AffineTransformCentered2NotCentered.h"


//Compute the translation vector that moves the center of the image into 0,0,0
itk::Vector< double , 3 > ComputeTranslationToCenter( itk::Image< unsigned short , 3 >::Pointer image )
{
  typedef itk::Image< unsigned short , 3 > ImageType ;
  ImageType::IndexType index ;
  ImageType::SizeType size ;
  size = image->GetLargestPossibleRegion().GetSize() ;
  for( int i = 0 ; i < 3 ; i++ )
  {
    index[ i ] = size[ i ] - 1 ;
  }
  ImageType::PointType corner ;
  image->TransformIndexToPhysicalPoint( index , corner ) ;
  ImageType::PointType origin ;
  origin = image->GetOrigin() ;
  itk::Vector< double , 3 > translation ;
  for( int i = 0 ; i < 3 ; i++ )
  {
    translation[ i ] = ( corner[ i ] + origin[ i ] ) / 2.0 ;
  }
  return translation ;
}


int AffineTransformCentered2NotCentered( int argc , char* argv[] )
{
  if( argc != 6 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputTransform Source Target outputTransform" << std::endl ;
    return 1 ;
  }
  std::string inputTransform ;
  inputTransform.assign( argv[ 2 ] ) ;
  std::string source ;
  source.assign( argv[ 3 ] ) ;
  std::string target ;
  target.assign( argv[ 4 ] ) ;
  std::string outputTransform ;
  outputTransform.assign( argv[ 5 ] ) ;
  //Read transform files
  itk::TransformFileReader::Pointer transformFile ;
  transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( inputTransform ) ;
  transformFile->Update() ;
  //Check that transform file contains only one transform
  if( transformFile->GetTransformList()->size() != 1
    )
  {
     std::cout<< "Transform file must contain only 1 transform" << std::endl ;
     return 1 ;
  }
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  AffineTransformType::Pointer affineTransform
            = dynamic_cast< AffineTransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
          if( !affineTransform )
            {
            std::cout << "Transform must be of type AffineTransform_double_3_3" << std::endl ;
            return 1 ;
            }
  typedef itk::Image< unsigned short , 3 > ImageType ;
  typedef itk::ImageFileReader< ImageType > ReaderType ;
  ReaderType::Pointer reader = ReaderType::New() ;
  reader->SetFileName( source.c_str() ) ;
  reader->UpdateOutputInformation() ;
  itk::Vector< double , 3 > tSource ;
  tSource = ComputeTranslationToCenter( reader->GetOutput() ) ;
  reader->SetFileName( target.c_str() ) ;
  reader->UpdateOutputInformation() ;
  itk::Vector< double , 3 > tTarget ;
  tTarget = - ComputeTranslationToCenter( reader->GetOutput() ) ;
//  std::cout<<"Target: "<< tTarget<<std::endl;
//  std::cout<<"Source: "<< tSource<<std::endl;
//  itk::Vector< double , 3 > tSourceTransform ;
//  tSourceTransform = affineTransform->TransformVector( tSource ) ;
  itk::Vector< double , 3 > tTargetTransform ;
  tTargetTransform = affineTransform->TransformVector( tTarget ) ;
  itk::Vector< double , 3 > translation ;
  translation = affineTransform->GetTranslation() ;
//std::cout<<translation<<std::endl;
//  std::cout<<translation<<std::endl;
//  translation += tSourceTransform ;
  translation += tTargetTransform ;
//  std::cout<<"Transformed Ttarget: "<<tTargetTransform<<std::endl;
//  std::cout<<translation<<std::endl;
//  translation += tTarget ;
  translation += tSource ;
//  std::cout<<translation<<std::endl;
  affineTransform->SetTranslation( translation ) ;
  
  //Compose transforms
  //Save transform
  itk::TransformFileWriter::Pointer outputTransformFile ;
  outputTransformFile = itk::TransformFileWriter::New() ;
  outputTransformFile->SetFileName( outputTransform.c_str() ) ;
  outputTransformFile->SetInput( affineTransform ) ;
  outputTransformFile->Update() ;
  return 0 ;
}
