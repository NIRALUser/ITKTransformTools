#include "dtiprocessFiles/deformationfieldio.h"

int Concatenate( int argc , char* argv[] ) ;
int GetTransformFileType( const char *filename , bool print = false );
int ReadTransformsNames( int argc , char* argv[] , std::vector< std::string > &transformFileNames , std::vector< std::string > &deformationFieldType );
int NotOnlyAffine( std::vector< std::string > transformFileNames , std::vector< std::string > deformationFieldType );
void ResampleDeformationField( DeformationImageType::Pointer & field, itk::Image< float , 3 >::Pointer referenceImage );
int ConcatenateNonRigidTransforms( std::vector< std::string > transformFileNames , std::vector< std::string > deformationFieldType , std::string outputFileName , std::string referenceFileName );
int ConcatenateAffineTransforms( std::vector< std::string > transformFileNames , std::vector< std::string > deformationFieldType , std::string outputFileName );

