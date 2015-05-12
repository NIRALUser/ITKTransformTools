#include "AffineTransformCentered2NotCentered.h"
#include "CombineITKTransforms.h"
#include "ComposeAffineTransforms.h"
#include "GetNewSizeAndOrigin.h"
#include "PrintITKTransform.h"
#include "DirectionToTransformMatrix.h"
#include "FreeSurferTransforms.h"
#include "MatrixOffsetToAffine.h"
#include "RotationExtraction.h"
#include "ConvertTransform.h"
#include "ConcatenateTransforms.h"
#include "Invert.h"
#include <string.h>

#define ITKTransformVersion "1.2.3"

int main( int argc , char* argv[] )
{
  char* pos = strrchr( argv[ 0 ] , '/' ) ;
  if( pos )
  {
    argv[ 0 ] = pos + 1 ;
  }
  pos = strrchr( argv[ 0 ] , '\\' ) ;
  if( pos )
  {
    argv[ 0 ] = pos + 1 ;
  }
  if( argc < 2
      || ( strcmp( argv[1] , "combine" )
           && strcmp( argv[1] , "compose" )
           && strcmp( argv[1] , "centered" )
           && strcmp( argv[1] , "size" )
           && strcmp( argv[1] , "iso" )
           && strcmp( argv[1] , "scale" )
           && strcmp( argv[1] , "spacing" )
           && strcmp( argv[1] , "direction" )
           && strcmp( argv[1] , "MO2Aff" )
           && strcmp( argv[1] , "print" )
           && strcmp( argv[1] , "rotation" )
           && strcmp( argv[1] , "convert" )
           && strcmp( argv[1] , "freesurfer" )
           && strcmp( argv[1] , "concatenate" )
           && strcmp( argv[1] , "invert" )
           && strcmp( argv[1] , "version" )
         )
    )
  {
    std::cout << "       " << argv[ 0 ] << " combine ... Combines ITK transforms from several files into one file" << std::endl ;
    std::cout << "       " << argv[ 0 ] << " compose ... Composes 2 affine ITK transforms into one transform" << std::endl ;
    std::cout<< "      " << argv [ 0 ] << " centered ... Adds an offset equivalent to centering the source and the target image to a transform " << std::endl ;
    std::cout << "          " << argv[ 0 ] << " size ... Computes the size and the origin of the transformed image" << std::endl ;
    std::cout << "           " << argv[ 0 ] << " iso ... Computes the size and the origin of the isotropic image (based on the smallest spacing)" << std::endl ;
    std::cout << "         " << argv[ 0 ] << " scale ... Computes the grids to resample an image and an atlas with the same isotropic spacing" << std::endl ;
    std::cout << "       " << argv[ 0 ] << " spacing ... Computes the grid to resample an image to a reference image using the spacing of another image" << std::endl ;
    std::cout << "     " << argv[ 0 ] << " direction ... Computes the transform to resample an image with an identity direction matrix" << std::endl ;
    std::cout << "    " << argv[ 0 ] << " freesurfer ... Converts FreeSurfer affine transforms to itkTransforms" << std::endl ;
    std::cout << "        " << argv[ 0 ] << " MO2Aff ... Modifies the transform file created by ANTs from a MatrixOffsetTransformBase_double_3_3 type to an AffineTransform_double_3_3" << std::endl ;
    std::cout << "         " << argv[ 0 ] << " print ... Prints the transformation matrix and the offset of the transform" << std::endl ;
    std::cout << "      " << argv[ 0 ] << " rotation ... Extracts the rotation from the affine transform (Finite Strain)" << std::endl ;
    std::cout << "       " << argv[ 0 ] << " convert ... Converts transforms from double->float or float->double" << std::endl ;
    std::cout << "   " << argv[ 0 ] << " concatenate ... Concatenates transforms" << std::endl ;
    std::cout << "        " << argv[ 0 ] << " invert ... inverts a transform" << std::endl ;
    std::cout << "       " << argv[ 0 ] << " version ... prints version number" << std::endl ;
    return 1 ;
  }
  else
  {
    if( !strcmp( argv[ 1 ] , "version") )
    {
      std::cout<<argv[ 0 ]<< " version: " << ITKTransformVersion << std::endl ;
      return 0 ;
    }
    if( !strcmp( argv[ 1 ] , "combine") )
    {
      return CombineITKTransforms( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "compose") )
    {
      return ComposeAffineTransforms( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "centered") )
    {
      return AffineTransformCentered2NotCentered( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "size") )
    {
      return GetNewSizeAndOrigin( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "iso") )
    {
       return Iso( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "scale" ) )
    {
       return ScalingWithAtlas( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "spacing" ) )
    {
       return Spacing( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "direction" ) )
    {
       return DirectionToTransformMatrix( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "freesurfer" ) )
    {
       return FreeSurferTransforms( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "MO2Aff" ) )
    {
       return MatrixOffsetToAffine( argc , argv ) ;
    }
    else if( !strcmp( argv[ 1 ] , "print" ) )
    {
       return PrintITKTransform( argc , argv ) ;
    }
   else if( !strcmp( argv[ 1 ] , "rotation" ) )
    {
       return RotationExtraction( argc , argv ) ;
    }
   else if( !strcmp( argv[ 1 ] , "convert" ) )
    {
       return Convert( argc , argv ) ;
    }
   else if( !strcmp( argv[ 1 ] , "concatenate" ) )
    {
       return Concatenate( argc , argv ) ;
    }
   else if( !strcmp( argv[ 1 ] , "invert" ) )
    {
       return Invert( argc , argv ) ;
    }
  }
  return 1 ;
}
