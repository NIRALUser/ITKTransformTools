#include <string>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkAffineTransform.h>
#include <itkRigid3DTransform.h>

int ReadTransform( itk::TransformFileReader::Pointer transformFile , itk::AffineTransform< double , 3 >::Pointer &affineTransform );
int ComposeAffineTransforms( int argc , char* argv[] );
