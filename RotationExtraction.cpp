#include "RotationExtraction.h"
#include <itkv3Rigid3DTransform.h>
#include <itkMatrixOffsetTransformBase.h>
#include <itkImageFileReader.h>
#include <itkImage.h>
#include <itkImageMomentsCalculator.h>


itk::Matrix< double , 3 , 3 > ComputeMatrixSquareRoot( itk::Matrix< double , 3 , 3 > matrix )
{
  itk::Matrix< double , 3 , 3 > sqrMatrix;

  vnl_matrix<double> M( 3, 3 );
  M = matrix.GetVnlMatrix();
  vnl_real_eigensystem eig( M );
  vnl_matrix<vcl_complex<double> > D( 3, 3 );
  vnl_matrix<vcl_complex<double> > vnl_sqrMatrix( 3, 3 );
  D.fill( 0.0 );
  for( int i = 0; i < 3; i++ )
    {
    D.put( i, i, vcl_pow( eig.D.get( i, i ), 0.5 ) );
    }
  vnl_sqrMatrix = eig.V * D * vnl_matrix_inverse<vcl_complex<double> >( eig.V );
  vnl_matrix<double> vnl_sqrMatrix_real( 3, 3 );
  vnl_sqrMatrix_real = vnl_real( vnl_sqrMatrix );
  for( int i = 0; i < 3; i++ )
    {
    for( int j = 0; j < 3; j++ )
      {
      sqrMatrix[i][j] = vnl_sqrMatrix_real.get( i, j );
      }
    }
  return sqrMatrix;
}


itk::Matrix< double , 3 , 3 > ComputeRotationMatrixFromTransformationMatrix( itk::Matrix< double , 3 , 3 > inputMatrix )
{
  typedef itk::Matrix< double , 3 , 3 > MatrixType ;
  MatrixType rotationMatrix ;
  vnl_matrix_fixed< double , 3 , 3 > inputMatrixTranspose = inputMatrix.GetTranspose();
  MatrixType matrix;

  try
    {
    matrix = ComputeMatrixSquareRoot( inputMatrix * inputMatrixTranspose ).GetInverse();
    }
  catch( ... )
    {
    std::cerr << "Matrix is not invertible while computing rotation matrix" << std::endl ;
    return rotationMatrix ;
    }
  rotationMatrix = matrix * static_cast< MatrixType >( inputMatrix ) ;
  return rotationMatrix;
}


int RotationExtraction( int argc , char* argv[] )
{
  if( argc != 4 && argc != 6 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " inputAffineTransform outputRigidTransform [--centerOfGravity inputImage]" << std::endl ;
    return 1 ;
  }
  std::string input ;
  input.assign( argv[ 2 ] ) ;
  std::string output ;
  output.assign( argv[ 3 ] ) ;
  //Read transform files
  itk::TransformFileReader::Pointer transformFile ;
  transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( input ) ;
  transformFile->Update() ;
  //Check that transform files contain only one transform
  if( transformFile->GetTransformList()->size() != 1
    )
  {
     std::cout<< "Transform file must contain only 1 transform" << std::endl ;
     return 1 ;
  }
  typedef itk::MatrixOffsetTransformBase< double , 3 , 3 > AffineTransformType ;
  AffineTransformType::Pointer affineTransform ;
  affineTransform
            = dynamic_cast< AffineTransformType* > ( transformFile->GetTransformList()->front().GetPointer() ) ;
  if( !affineTransform )
  {
    std::cerr << "Transformation must be of affine type" << std::endl ;
    return -1 ;
  }
  itk::Matrix< double , 3 , 3 > rotationMatrix ;
  rotationMatrix = ComputeRotationMatrixFromTransformationMatrix( affineTransform->GetMatrix() ) ;
  typedef itkv3::Rigid3DTransform< double > Rigid3DTransformType ;
  Rigid3DTransformType::Pointer rigidTransform = Rigid3DTransformType::New() ;
  rigidTransform->SetMatrix( rotationMatrix ) ;
  rigidTransform->SetTranslation( affineTransform->GetTranslation() ) ;
  //Compute translation to keep center of gravity at same location
  if( argc == 6 )
  {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "WARNING: This functionality has not be testing thoroughly!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    //Load image
    typedef itk::Image< float , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    ReaderType::Pointer reader = ReaderType::New() ;
    reader->SetFileName( argv[ 5 ] ) ;
    reader->Update() ;
    //Compute center of gravity
    typedef itk::ImageMomentsCalculator< ImageType > ImageMomentsCalculatorType ;
    typedef ImageMomentsCalculatorType::VectorType VectorType ;
    ImageMomentsCalculatorType::Pointer imageMomentsCalculatorFilter = ImageMomentsCalculatorType::New() ;
    imageMomentsCalculatorFilter->SetImage( reader->GetOutput() ) ;
    imageMomentsCalculatorFilter->Compute() ;
    VectorType cog = imageMomentsCalculatorFilter->GetCenterOfGravity() ;
    itk::Point< double , 3 > cogPosition ;
    for( int i = 0 ; i < 3 ; i++ )
    {
      cogPosition[ i ] = cog[ i ] ;
    }
    AffineTransformType::Pointer inverseAffine = AffineTransformType::New() ;
    affineTransform->GetInverse( inverseAffine ) ;
    Rigid3DTransformType::Pointer inverseRigid =  Rigid3DTransformType::New() ;
    rigidTransform->GetInverse( inverseRigid ) ;
    itk::Point<double , 3 > cogRotated ;
    cogRotated = inverseRigid->TransformPoint( cogPosition ) ;
    itk::Point<double , 3 > cogAffined ;
    cogAffined = inverseAffine->TransformPoint( cogPosition ) ;
    itk::Vector< double , 3 > additionalTranslation ;
    additionalTranslation = cogAffined - cogRotated ;
    itk::Vector< double , 3 > translation ;
    translation = rigidTransform->GetTranslation() + additionalTranslation ;
    rigidTransform->SetTranslation( translation ) ;
  }
  //Save transform
  itk::TransformFileWriter::Pointer outputTransformFile ;
  outputTransformFile = itk::TransformFileWriter::New() ;
  outputTransformFile->SetFileName( output.c_str() ) ;
  outputTransformFile->SetInput( rigidTransform ) ;
  outputTransformFile->Update() ;
  return 0 ;
}
