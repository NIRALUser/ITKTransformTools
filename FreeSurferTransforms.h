#include <itkTransformFileWriter.h>
#include <itkAffineTransform.h>
#include <vector>
#include <sstream>
#include <fstream>

bool StringtonDoubles( std::string s , int n , std::vector< double > &d ) ;
int FreeSurferTransforms( int argc , char* argv[] ) ;
