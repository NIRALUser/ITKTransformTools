#include "CombineITKTransforms.h"


int CombineITKTransforms( int argc , char* argv[] )
{
  //check if next to last is "-o"
  bool badArguments = false ;
  if( argc >= 5 )
  {
    if( strcmp( argv[ argc - 2 ] , "-o" ) )
    {
      badArguments = true ;
    }
  }
  if( argc < 5 || badArguments )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputTransform1 [inputTransform2 inputTransform3 ...] -o outputTransform" << std::endl ;
    std::cout<< "inputTransforms should be in the order in which they are applied to the input image" << std::endl ;
    return 1 ;
  }
  //Set parameters
  std::list< std::string > listTransformFiles ;
  for( int i = 2 ; i < argc - 2 ; i++ )//argc - 1: outputFileName ; argc - 2: "-o"
  {
    std::string input ;
    input.assign( argv[ i ] ) ;
    listTransformFiles.push_back( input ) ;
  }
  //Creation of the itkTransformFileWriter
  itk::TransformFileWriter::Pointer transformWriter = itk::TransformFileWriter::New() ;
  transformWriter->SetFileName( argv[ argc - 1 ] ) ;
  do
  {
      itk::TransformFileReader::Pointer transformFile = itk::TransformFileReader::New() ;
      transformFile->SetFileName( listTransformFiles.front().c_str() ) ;
      transformFile->Update() ;
      do
      {
        transformWriter->AddTransform( transformFile->GetTransformList()->front() ) ;
        transformFile->GetTransformList()->pop_front() ;
      }while( transformFile->GetTransformList()->size() ) ;
      listTransformFiles.pop_front() ;
  }while( listTransformFiles.size() ) ;
  transformWriter->Update() ;
  return 0 ;
}
