#include "GetNewSizeAndOrigin.h"

template< class Type >
int ComputeSizeAndOrigin( itk::Image< unsigned char , 3 >::Pointer image ,
                          typename itk::MatrixOffsetTransformBase< Type , 3 , 3 >::Pointer transform ,
                          char* gridName ,
                          bool giveSpacing ,
                          itk::Image< unsigned char , 3 >::SpacingType spacing
                        )
{
   //Inversion of the transform
   typename itk::MatrixOffsetTransformBase< Type , 3 , 3 >::InverseTransformBaseType::Pointer inverseBase ;
   typedef itk::MatrixOffsetTransformBase< Type , 3 , 3 > TransformType ;
   typename TransformType::Pointer inverse ;
   inverseBase = transform->GetInverseTransform() ;
   if( !inverseBase )
   {
      std::cerr << "Transform is not invertible" << std::endl ;
      return EXIT_FAILURE ;
   }
   inverse = dynamic_cast< TransformType* >( inverseBase.GetPointer() ) ;
   if( !inverse )
   {
      std::cerr << "Problem inverting the transform" << std::endl ;
      return EXIT_FAILURE ;
   }
   typedef itk::Image< unsigned char , 3 > ImageType ;
   /*itk::ImageFileReader< ImageType >::Pointer imageReader 
         = itk::ImageFileReader< ImageType >::New() ;
   imageReader->SetFileName( imageFileName ) ;
   imageReader->UpdateOutputInformation() ;*/
   ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize() ;
   ImageType::PointType min ;
   min.Fill( std::numeric_limits< double >::max() ) ;
   ImageType::PointType max ;
   max.Fill( -std::numeric_limits< double >::max() ) ;
   for( int i = 0 ; i < 8 ; i++ )
   {
      ImageType::PointType corner ;
      ImageType::IndexType index ;
      for( int j = 0 ; j < 3 ; j++ )
      {
         index[ j ] = ( i >> j ) % 2 ? size[ j ] - 1 : 0 ;
      }
      ImageType::PointType point ;
      image->TransformIndexToPhysicalPoint( index , point ) ;
      corner = inverse->TransformPoint( point ) ;
      for( int j = 0 ; j < 3 ; j++ )
      {
         min[ j ] = ( corner[ j ] < min[ j ] ? corner[ j ] : min[ j ] ) ;
         max[ j ] = ( corner[ j ] > max[ j ] ? corner[ j ] : max[ j ] ) ;
      }
   }
//   ImageType::SpacingType spacing ;
   if( !giveSpacing )
   {
     spacing = image->GetSpacing() ;
   }
//   spacing = imageReader->GetOutput()->GetSpacing() ;
   ImageType::SizeType newSize ;
   for( int i = 0 ; i < 3 ; i++ )
   {
     newSize[ i ] = (long)( vcl_floor( ( max[ i ] - min[ i ] ) / spacing[ i ] + 1.5 ) ) ;//+1 for size and +0.5 to transform floor into round
   }
   if( !gridName )
   {
     std::cout << "New Size: " << newSize[ 0 ] << "," << newSize[ 1 ] << "," << newSize[ 2 ] << std::endl ;
     std::cout << "New Origin: " << min[ 0 ] << "," << min[ 1 ] << "," << min[ 2 ] << std::endl ;
   }
   else
   {
     ImageType::Pointer new_image = ImageType::New() ;
     new_image->SetRegions( newSize ) ;
     new_image->SetSpacing( spacing ) ;
     new_image->SetOrigin( min ) ;
     new_image->Allocate() ;
     new_image->FillBuffer( 0 ) ;
     typedef itk::ImageFileWriter< ImageType > WriterType ;
     WriterType::Pointer writer = WriterType::New() ;
     writer->SetFileName( gridName ) ;
     writer->UseCompressionOn() ;
     writer->SetInput( new_image ) ;
     writer->Update() ;
   }
   return EXIT_SUCCESS ;
}


int FindArguments( int argc ,
                   char** argv ,
                   char* &gridName ,
                   bool &giveSpacing ,
                   itk::Image< unsigned char , 3 >::SpacingType &spacing
                 )
{
  for( int i = 4 ; i < argc ; i++ )
  {
    if( !strcmp( argv[ i ] , "--grid" ) && i + 1 < argc )
    {
      gridName = argv[ i + 1 ] ;
      i++ ;
    }
    else if( !strcmp( argv[ i ] , "--spacing" ) && i + 3 < argc )
    {
      giveSpacing = true ;
      for( int j = 1 ; j < 4 ; j++ )
      {
        std::istringstream temp( argv[ j + i ] ) ;
        temp >> spacing[ j - 1 ] ;
      }
      i += 3 ;
      std::cout<<spacing[ 0 ] <<" "<<spacing[ 1 ] <<" "<<spacing[ 2 ] <<std::endl;
    }
    else
    {
       return EXIT_FAILURE ;
    }
  }
  return EXIT_SUCCESS ;
}

int GetNewSizeAndOrigin( int argc , char** argv )
{
  char* gridName = NULL ;
  bool giveSpacing = false ;
  typedef itk::Image< unsigned char , 3 > ImageType ;
  ImageType::SpacingType userSpacing ;
  int argsprob = FindArguments( argc , argv , gridName , giveSpacing , userSpacing ) ;
  if( argsprob || argc < 4 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " InputImage Transform[-]  [--grid gridName][--spacing x y z]" << std::endl ;
    return EXIT_FAILURE ;
  }
  typedef itk::MatrixOffsetTransformBase< double , 3 , 3 > TransformTypeDouble ;
  typedef itk::MatrixOffsetTransformBase< float , 3 , 3 > TransformTypeFloat ;
  itk::ImageFileReader< ImageType >::Pointer imageReader 
        = itk::ImageFileReader< ImageType >::New() ;
  imageReader->SetFileName( argv[ 2 ] ) ;
  imageReader->UpdateOutputInformation() ;
  if( !strcmp( argv[ 3 ] , "-" ) )
  {
     TransformTypeFloat::Pointer transformFloat = TransformTypeFloat::New() ;
     transformFloat->SetIdentity() ;
     return ComputeSizeAndOrigin< float >( imageReader->GetOutput() , transformFloat , gridName , giveSpacing , userSpacing ) ;
  }
  itk::TransformFileReader::Pointer transformFile = itk::TransformFileReader::New() ;
  transformFile->SetFileName( argv[ 3 ] ) ;
  transformFile->Update() ;
  itk::TransformFileReader::TransformType::Pointer transform ;
  if( transformFile->GetTransformList()->size() != 1 )
  {
     std::cerr << "Transform file must contain only one transform" << std::endl ;
     return EXIT_FAILURE ;
  }
  transform = transformFile->GetTransformList()->front() ;
  TransformTypeDouble::Pointer transformDouble = dynamic_cast< TransformTypeDouble* > ( transform.GetPointer() ) ;
  if( transformDouble )
  {
     return ComputeSizeAndOrigin< double >( imageReader->GetOutput() , transformDouble , gridName , giveSpacing , userSpacing ) ;
  }
  TransformTypeFloat::Pointer transformFloat = dynamic_cast< TransformTypeFloat* > ( transform.GetPointer() ) ;
  if( transformFloat )
  {
     return ComputeSizeAndOrigin< float >( imageReader->GetOutput() , transformFloat , gridName , giveSpacing , userSpacing ) ;
  }
  std::cerr << "TransformType not handled. Sorry!" << std::endl ;
  return EXIT_FAILURE ;
}


void ComputeIsoSpacing( itk::Image< unsigned char , 3 >::SpacingType &spacing )
{
   double min = std::numeric_limits< double >::max() ;
   for( int i = 0 ; i < 3 ; i++ )
   {
      if( spacing[ i ] < min )
      {
         min = spacing[ i ] ;
      }
   }
   for( int i = 0 ; i < 3 ; i++ )
   {
      spacing[ i ] = min ;
   }
}

int Iso( int argc , char** argv )
{
   char* gridName = NULL ;
   if( !(argc == 3 || ( argc == 5 && !strcmp( argv[ 3 ] , "--grid" ) ) ) )
   {
      std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " InputImage [--grid gridName]" << std::endl ;
      return EXIT_FAILURE ;
   }
   if( argc == 5 )
   {
      gridName = argv[ 4 ] ;
   }
   itkv3::Rigid3DTransform< float >::Pointer rigid = itkv3::Rigid3DTransform< float >::New() ;
   rigid->SetIdentity() ;
   typedef itk::MatrixOffsetTransformBase< float , 3 , 3 > TransformTypeFloat ;
   TransformTypeFloat::Pointer transformFloat = dynamic_cast< TransformTypeFloat* > ( rigid.GetPointer() ) ;
   typedef itk::Image< unsigned char , 3 > ImageType ;
   ImageType::SpacingType spacing ;
   itk::ImageFileReader< ImageType >::Pointer imageReader 
         = itk::ImageFileReader< ImageType >::New() ;
   imageReader->SetFileName( argv[ 2 ] ) ;
   imageReader->UpdateOutputInformation() ;
   spacing = imageReader->GetOutput()->GetSpacing() ;
   ComputeIsoSpacing( spacing ) ;
/*   double min = std::numeric_limits< double >::max() ;
   for( int i = 0 ; i < 3 ; i++ )
   {
      if( spacing[ i ] < min )
      {
         min = spacing[ i ] ;
      }
   }
   for( int i = 0 ; i < 3 ; i++ )
   {
      spacing[ i ] = min ;
}*/
   if( !gridName )
   {
      std::cout << "New Spacing: " << spacing[ 0 ] << "," << spacing[ 1 ] << "," << spacing[ 2 ] << std::endl ;
   }
   return ComputeSizeAndOrigin< float >( imageReader->GetOutput() , transformFloat , gridName , true , spacing ) ;
}


int Spacing( int argc , char** argv )
{
   if( argc != 5 )
   {
      std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " ReferenceImage SpacingImage outputGridImage" << std::endl ;
      return EXIT_FAILURE ;
   }
   itkv3::Rigid3DTransform< float >::Pointer rigid = itkv3::Rigid3DTransform< float >::New() ;
   typedef itk::MatrixOffsetTransformBase< float , 3 , 3 > TransformTypeFloat ;
   TransformTypeFloat::Pointer transformFloat
          = dynamic_cast< TransformTypeFloat* > ( rigid.GetPointer() ) ;
   typedef itk::Image< unsigned char , 3 > ImageType ;
   ImageType::SpacingType spacing ;
   itk::ImageFileReader< ImageType >::Pointer imageReader 
         = itk::ImageFileReader< ImageType >::New() ;
   imageReader->SetFileName( argv[ 3 ] ) ;
   imageReader->UpdateOutputInformation() ;
   spacing = imageReader->GetOutput()->GetSpacing() ;
   imageReader->SetFileName( argv[ 2 ] ) ;
   imageReader->UpdateOutputInformation() ;
   if( ComputeSizeAndOrigin< float >( imageReader->GetOutput() , transformFloat , argv[ 4 ] , true , spacing ) )
   {
      return EXIT_FAILURE ;
   }
   return EXIT_SUCCESS ;
}

int ScalingWithAtlas( int argc , char** argv )
{
   if( argc != 6 )
   {
      std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " InputImage AtlasImage outputGridImage outputAtlasGridImage" << std::endl ;
      return EXIT_FAILURE ;
   }
   itkv3::Rigid3DTransform< float >::Pointer rigid = itkv3::Rigid3DTransform< float >::New() ;
   typedef itk::MatrixOffsetTransformBase< float , 3 , 3 > TransformTypeFloat ;
   TransformTypeFloat::Pointer transformFloat
          = dynamic_cast< TransformTypeFloat* > ( rigid.GetPointer() ) ;
   typedef itk::Image< unsigned char , 3 > ImageType ;
   ImageType::SpacingType spacing ;
   itk::ImageFileReader< ImageType >::Pointer imageReader 
         = itk::ImageFileReader< ImageType >::New() ;
   imageReader->SetFileName( argv[ 2 ] ) ;
   imageReader->UpdateOutputInformation() ;
   spacing = imageReader->GetOutput()->GetSpacing() ;
   ComputeIsoSpacing( spacing ) ;
   
   ImageType::SpacingType spacingAtlas ;
   itk::ImageFileReader< ImageType >::Pointer atlasImageReader 
         = itk::ImageFileReader< ImageType >::New() ;
   atlasImageReader->SetFileName( argv[ 3 ] ) ;
   atlasImageReader->UpdateOutputInformation() ;
   spacingAtlas = atlasImageReader->GetOutput()->GetSpacing() ;
   ComputeIsoSpacing( spacingAtlas ) ;
   if( spacingAtlas[ 0 ] < spacing[ 0 ] )
   {
      spacing = spacingAtlas ;
   }
   if( ComputeSizeAndOrigin< float >( imageReader->GetOutput() , transformFloat , argv[ 4 ] , true , spacing ) )
   {
      return EXIT_FAILURE ;
   }
   return ComputeSizeAndOrigin< float >( atlasImageReader->GetOutput() , transformFloat , argv[ 5 ] , true , spacing ) ;
}
