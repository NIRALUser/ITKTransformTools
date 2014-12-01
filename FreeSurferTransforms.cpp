#include "FreeSurferTransforms.h"


bool StringtonDoubles( std::string s , int n , std::vector< double > &d )
{
    try
    {
        d.clear() ;
        bool returnval = true ;
        unsigned int count = 0 ;
        unsigned int count2 = 0 ;
        std::string temp ;
        double value ;
        std::istringstream stream ;
        stream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        for(int i = 0 ; i < n ; i++ )
        {
            while( s.at( count ) !='\n' && isspace( s.at( count ) ) )
            {
                count++ ;
            }
            if( s.at( count ) == '\n' )
            {
              throw "error" ; 
            }
            count2 = count ;
            while( count2 < s.size() && !isspace( s.at( count2 ) ) )
            {
                count2++ ;
            }
            if( count2 == s.size() )
            {
                if( i < n-1 ) returnval = false ;
            }
            temp.clear() ;
            temp.assign( s , count , count2-count ) ;
            stream.clear( std::stringstream::goodbit ) ;
            stream.str( temp ) ;
            stream >> value ;
            d.push_back( value ) ;
            if( !returnval )
            {
              throw "error" ; 
            }
            count = count2 + 1 ;
        }
        return returnval;
    }
    catch(...)
    {
        std::cerr << "Error: impossible to extract " << n << " values from the given string" << std::endl ;
        return 0;
    }
}

int FreeSurferTransforms( int argc , char* argv[] )
{
  if( argc != 4 )
  {
    std::cout<< argv[ 0 ] << " " << argv[ 1 ] << " freeSurferTransform itkTransform" << std::endl ;
    return 1 ;
  }
  std::ifstream ifile( argv[ 2 ] ) ;
  if( !ifile )
  {
    std::cerr << "File seemed to end prematurely. Load failed." << std::endl ;
    return -1 ;
  }
  std::string readline ;
  std::vector< double > vec ;
  bool begin = false ;
  while( ifile && !begin )
  {  
    std::getline( ifile , readline ) ;
    if( !readline.compare( 0 , 18 , "Linear_Transform =" , 0 , 18 ) )
    {
      begin = true ;
    }
  }
  if( begin == false )
  {
    std::cerr << "Linear transform not found" << std::endl ;
    return -1 ;
  }
  int count = 0 ;
  itk::Matrix< double , 3 > matrix ;
  itk::Vector< double , 3 > vector ;
  while( ifile && count != 3 )
  {
    std::getline( ifile , readline ) ;
    if( !StringtonDoubles( readline , 4 , vec ) )
    {
      std::cerr << "Problems while reading input file" << std::endl ;
      return -1 ;
    }
    for( int i = 0 ; i < 3 ; i++ )
    {
      matrix[ count ][ i ] = vec[ i ] ;
    }
    vector[ count ] = vec[ 3 ] ;
    count ++ ;
  }
  if( count != 3 )
  {
    std::cerr << "Input File does not have the correct format" << std::endl ;
    return -1 ;
  }
  itk::Matrix< double , 3 >  RAS ;
  RAS.SetIdentity() ;
  RAS[ 0 ][ 0 ] = -1 ;
  RAS[ 1 ][ 1 ] = -1 ;
  matrix = RAS * matrix * RAS ;
  vector = RAS * vector ;
  typedef itk::AffineTransform< double , 3 > AffineTransformType ;
  AffineTransformType::Pointer affineTransform = AffineTransformType::New() ;
  affineTransform->SetMatrix( matrix ) ;
  affineTransform->SetTranslation( vector ) ;
  AffineTransformType::Pointer inverseTransform = AffineTransformType::New() ;
  affineTransform->GetInverse( inverseTransform ) ;
  //Save transform
  itk::TransformFileWriter::Pointer outputTransformFile ;
  outputTransformFile = itk::TransformFileWriter::New() ;
  outputTransformFile->SetFileName( argv[ 3 ] ) ;
  outputTransformFile->SetInput( inverseTransform ) ;
  outputTransformFile->Update() ;
  return 0 ;
}
