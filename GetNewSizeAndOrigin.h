#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkTransformFileReader.h>
#include <itkImageFileWriter.h>
#include <math.h>
#include <itkMatrixOffsetTransformBase.h>
#include <sstream>
#include <itkRigid3DTransform.h>

#ifndef EXITSUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif
template< class Type >
int ComputeSizeAndOrigin( itk::Image< unsigned char , 3 >::Pointer image ,
                          typename itk::MatrixOffsetTransformBase< Type , 3 , 3 >::Pointer transform ,
                          char* gridName ,
                          bool giveSpacing ,
                          itk::Image< unsigned char , 3 >::SpacingType spacing
                        );
int GetNewSizeAndOrigin( int argc , char* argv[] );
int Iso( int argc , char** argv );
int Spacing( int argc , char** argv );
int ScalingWithAtlas( int argc , char** argv );
void ComputeIsoSpacing( itk::Image< unsigned char , 3 >::SpacingType &spacing );
int FindArguments( int argc ,
                   char** argv ,
                   char* &gridName ,
                   bool &giveSpacing ,
                   itk::Image< unsigned char , 3 >::SpacingType &spacing
                 );
