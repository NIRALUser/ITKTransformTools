#include <string>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkAffineTransform.h>
#include <itkImageFileReader.h>

//Compute the translation vector that moves the center of the image into 0,0,0
itk::Vector< double , 3 > ComputeTranslationToCenter( itk::Image< unsigned short , 3 >::Pointer image );
int AffineTransformCentered2NotCentered( int argc , char* argv[] );
