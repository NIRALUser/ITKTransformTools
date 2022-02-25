#include <string>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkMatrixOffsetTransformBase.h>
#include <itkTransformFactory.h>
#include <itkAffineTransform.h>
#include <list>
#include <cstring>

int MatrixOffsetToAffine( int argc , char* argv[] );
