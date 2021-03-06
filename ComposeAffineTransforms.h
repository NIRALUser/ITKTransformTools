#include <string>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkAffineTransform.h>
#include <itkv3Rigid3DTransform.h>
#include <itkMatrixOffsetTransformBase.h>

int ReadTransform( itk::TransformFileReader::Pointer transformFile , itk::AffineTransform< double , 3 >::Pointer &affineTransform );
int ComposeAffineTransforms( int argc , char* argv[] );
int ComposeAffineWithFile( itk::AffineTransform< double , 3 >::Pointer &affineTransform1 , itk::TransformFileReader::Pointer &transformFile ) ;

