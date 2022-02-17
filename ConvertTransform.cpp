#include <string>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkAffineTransform.h>
#include <itkv3Rigid3DTransform.h>
#include <list>
#include <cstring>
#include "ConvertTransform.h"
#include "itkVersion.h"


#if (ITK_VERSION_MAJOR == 4 && ITK_VERSION_MINOR >= 5) || ITK_VERSION_MAJOR> 4
template< class U >
int ConvertTransforms( std::string input , std::string output )
{

  typename itk::TransformFileReaderTemplate< U >::Pointer transformFile =
  itk::TransformFileReaderTemplate< U >::New() ;
  transformFile->SetFileName( input.c_str() ) ;
  transformFile->Update() ;

  typename itk::TransformFileWriterTemplate< U >::Pointer transformWriter =
   itk::TransformFileWriterTemplate< U >::New() ;
  transformWriter->SetFileName( output.c_str() ) ;
  while( !transformFile->GetTransformList()->empty() )
  {
    transformWriter->AddTransform( transformFile->GetTransformList()->front() ) ;
    transformFile->GetTransformList()->pop_front() ;
  }
  transformWriter->Update() ;
  return 0 ;
}
#else
template< class S , class T , class V >
int ConvertType( typename S::Pointer inputTransform , std::string output )
{
  typename T::Pointer outputTransform = T::New() ;
  typename S::ParametersType inputParam ;
  typename T::ParametersType outputParam ;
  ///Parameters
  inputParam = inputTransform->GetParameters() ;
  outputParam.SetSize( inputParam.GetSize() ) ;
  for( unsigned int i = 0 ; i < outputParam.GetSize() ; i++ )
  {
     outputParam.SetElement( i , static_cast< V >( inputParam.GetElement( i ) ) ) ;
  }
  outputTransform->SetParameters( outputParam ) ;
  ///Fixed Parameters
  inputParam = inputTransform->GetFixedParameters() ;
  outputParam.SetSize( inputParam.GetSize() ) ;
  for( unsigned int i = 0 ; i < outputParam.GetSize() ; i++ )
  {
     outputParam.SetElement( i , static_cast< V >( inputParam.GetElement( i ) ) ) ;
  }
  outputTransform->SetFixedParameters( outputParam ) ;
  itk::TransformFileWriter::Pointer transformWriter = itk::TransformFileWriter::New();
  transformWriter->SetFileName( output.c_str() ) ;
  transformWriter->AddTransform( outputTransform ) ;
  transformWriter->Update() ;
  return 0 ;
}

template< class U , class V >
int ReadTransforms( std::string input , std::string output )
{
  typedef itk::AffineTransform< U , 3 > InputAffineType ;
  typedef itk::Rigid3DTransform< U > InputRigidType ;
  itk::TransformFileReader::Pointer transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( input.c_str() ) ;
  transformFile->Update() ;
  typename InputAffineType::Pointer affine ;
  affine = dynamic_cast< InputAffineType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
  if( affine )
  {
     typedef itk::AffineTransform< V , 3 > OutputAffineType ;
     return ConvertType< InputAffineType , OutputAffineType , V >( affine , output ) ;
  }
  else
  {
    typename InputRigidType::Pointer rigid ;
    rigid = dynamic_cast< InputRigidType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
    if( rigid )
    {
      typedef itkv3::Rigid3DTransform< V > OutputRigidType ;
      return ConvertType< InputRigidType , OutputRigidType , V >( rigid , output ) ;
    }
    else
    {
       std::cerr << "Transform type not supported or wrong input transform type selected" << std::endl ;
       return -1 ;
    }
  }
}

#endif

int Convert( int argc , char* argv[] )
{
  if( argc != 5 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputTransform outputTransform f2d|2d|d2f|2f" << std::endl ;
    return 1 ;
  }
  std::string input = argv[ 2 ] ;
  std::string output = argv[ 3 ] ;
  std::string type = argv[ 4 ] ;
  #if (ITK_VERSION_MAJOR == 4 && ITK_VERSION_MINOR >= 5) || ITK_VERSION_MAJOR> 4
  //We keep backward compatibility but really we just save the input transform in
  //double or float, without looking at the input format, so we added the new flags "2d" and "2f"
  //to reflect this modification.
  if( type == "f2d" || type == "2d" )
  {
    return ConvertTransforms< double >( input , output ) ;
  }
  else if( type == "d2f" || type == "2f" )
  {
    return ConvertTransforms< float >( input , output ) ;
  }
  else
  {
    std::cerr << "Argument 5 is : " << type << std::endl ;
    std::cerr << "Must be either 'f2d', '2d', 'd2f' or '2f'" << std::endl ;
    return -1 ;
  }
  #else
  if( !type.compare( "f2d" ) )
  {
    return ReadTransforms< float , double >( input , output ) ;
  }
  else if( !type.compare( "d2f" ) )
  {
    return ReadTransforms< double , float >( input , output ) ;
  }
  else
  {
    std::cerr << "Argument 5 is : " << type << std::endl ;
    std::cerr << "Must be either 'f2d' or 'd2f'" << std::endl ;
    return -1 ;
  }
  #endif
}
