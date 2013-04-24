#include <string>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>
#include <itkAffineTransform.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_real_eigensystem.h>
#include <vnl/vnl_real.h>

int RotationExtraction( int argc , char* argv[] );
