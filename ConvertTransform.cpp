#include <string>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkAffineTransform.h>
#include <itkRigid3DTransform.h>
#include <list>
#include <string.h>
#include "ConvertTransform.h"

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
  itk::TransformFileWriter::Pointer transformWriter = itk::TransformFileWriter::New() ;
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
  typename InputAffineType::Pointer affine = InputAffineType::New() ;
  affine = dynamic_cast< InputAffineType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
  if( affine )
  {
     typedef itk::AffineTransform< V , 3 > OutputAffineType ;
     return ConvertType< InputAffineType , OutputAffineType , V >( affine , output ) ;
  }
  else
  {
    typename InputRigidType::Pointer rigid = InputRigidType::New() ;
    rigid = dynamic_cast< InputRigidType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
    if( rigid )
    {
      typedef itk::Rigid3DTransform< V > OutputRigidType ;
      return ConvertType< InputRigidType , OutputRigidType , V >( rigid , output ) ;
    }
    else
    {
       std::cerr << "Transform type not supported or wrong input transform type selected" << std::endl ;
       return -1 ;
    }
  }
}

int Convert( int argc , char* argv[] )
{
  if( argc != 5 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputTransform outputTransform f2d/f2d" << std::endl ;
    return 1 ;
  }
  std::string input = argv[ 2 ] ;
  std::string output = argv[ 3 ] ;
  std::string type = argv[ 4 ] ;
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
}
