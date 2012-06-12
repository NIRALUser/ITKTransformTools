#include "MatrixOffsetToAffine.h"

//returns what? in the .h too!
int MatrixOffsetToAffine( int argc , char* argv[] )
{

	/// if the input is incorrect
	if( argc != 4 ) {
		std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputTransform outputTransform" << std::endl ;
		return 1 ;
	}
	
	///settings
	//defining the types : input and output
	typedef itk::MatrixOffsetTransformBase< double , 3 , 3 > MatrixOffsetTransformBaseDoubleType ;
	typedef itk::AffineTransform< double , 3 > AffineDoubleType ;
	//instancing the transforms
	itk::TransformFactory<MatrixOffsetTransformBaseDoubleType>::RegisterTransform();
	itk::TransformFactory<AffineDoubleType>::RegisterTransform();
	
	///pointers
	MatrixOffsetTransformBaseDoubleType::Pointer matrixOffsetTransformBaseDouble ;
	AffineDoubleType::Pointer affineDouble = AffineDoubleType::New() ;
	
	///reader
	itk::TransformFileReader::Pointer transformFile = itk::TransformFileReader::New() ;
	//param of the reader
	transformFile->SetFileName( argv[ 2 ] ) ;
	transformFile->Update() ;
	
	matrixOffsetTransformBaseDouble = dynamic_cast< MatrixOffsetTransformBaseDoubleType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;

	///converting
	if( !matrixOffsetTransformBaseDouble ) //making sure the transform we have to process is MatrixOffset type
	{
		std::cerr << "Input Transformation is not a matrix offset transform base with doubles or an affine transform with doubles" << std::endl ;
		return 1 ;
	}

	MatrixOffsetTransformBaseDoubleType::ParametersType moParam ;
	AffineDoubleType::ParametersType affParam ;
	
	moParam = matrixOffsetTransformBaseDouble->GetParameters() ;
	affParam.SetSize( moParam.GetSize() ) ;
	for( unsigned int i = 0 ; i < affParam.GetSize() ; i++ )
	{
     //std::cout<<doubleParam.GetElement( i )<<std::endl;
		affParam.SetElement( i , (double)moParam.GetElement( i ) ) ;
		//std::cout<< affParam.GetElement(i) << std::endl;
	}
	affineDouble->SetParameters( affParam ) ;
	
	  //Fixed Parameters
	moParam = matrixOffsetTransformBaseDouble->GetFixedParameters() ;
	affParam.SetSize( moParam.GetSize() ) ;
  //std::cout<<"fixed"<<std::endl;
	for( unsigned int i = 0 ; i < affParam.GetSize() ; i++ )
	{
     //std::cout<<doubleParam.GetElement( i )<<std::endl;
		affParam.SetElement( i , (double)moParam.GetElement( i ) ) ;
	}
	affineDouble->SetFixedParameters( affParam ) ;

	
	///writer
	itk::TransformFileWriter::Pointer transformWriter = itk::TransformFileWriter::New() ;
	transformWriter->SetFileName( argv[ 3 ] ) ;
	transformWriter->AddTransform( affineDouble ) ;
	transformWriter->Update() ;
	
	return 0 ;
}
