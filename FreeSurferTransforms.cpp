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

int LinearTransformReader( std::ifstream &ifile , itk::Matrix< double , 3 > &matrix , itk::Vector< double , 3 > &vector )
{
  int count = 0 ;
  std::string readline ;
  std::vector< double > vec ;
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
    return 1 ;
  }
  return 0 ;
}

int ltaReader( std::ifstream &ifile , itk::Matrix< double , 3 > &matrix , itk::Vector< double , 3 > &vector )
{
  std::string readline ;
  std::vector< double > vec ;
  bool begin = false ;
  while( ifile && !begin )
  {
    std::getline( ifile , readline ) ;
    if( !readline.compare( 0 , 5 , "sigma" , 0 , 5 ) )
    {
      begin = 1 ;
    }
  }
  if( !begin )
  {
    std::cerr << "Could not find beginning of transform" << std::endl ;
    return 1 ;
  }
  //Read line containing transform matrix size and make sure that it is a 4x4 matrix
  std::getline( ifile , readline ) ;
  if( !StringtonDoubles( readline , 3 , vec ) )
  {
    std::cerr << "Problems while reading input file: input transform matrix size could not be read" << std::endl ;
    return 2 ;
  }
  if( vec[ 1 ] != 4 || vec[ 2 ] != 4 )
  {
    std::cerr << "Problems while reading input file: input transform matrix is not a 4x4 matrix" << std::endl ;
    return 3 ;
  } 
  //
  int count = 0 ;
  while( ifile && count != 3 )
  {
    std::getline( ifile , readline ) ;
    if( !StringtonDoubles( readline , 4 , vec ) )
    {
      std::cerr << "Problems while reading input file" << std::endl ;
      return 4 ;
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
    std::cerr << "Input File do not have the correct format" << std::endl ;
    return 5 ;
  }
  return 0 ;
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
  int begin = 0 ;
  while( ifile && !begin )
  {  
    std::getline( ifile , readline ) ;
    if( !readline.compare( 0 , 18 , "Linear_Transform =" , 0 , 18 ) )
    {
      begin = 1 ;
    }
    else if( !readline.compare( 0 , 14 , "# transform file" , 0 , 14 ) )
    {
      begin = 2 ;//lta files
    }
  }
  itk::Matrix< double , 3 > matrix ;
  itk::Vector< double , 3 > vector ;
  if( begin == 1 )
  {
    std::cout << "Linear transform found" << std::endl ;
    if( LinearTransformReader( ifile , matrix , vector ) )
    {
      return -1 ;
    }
  }
  else if( begin == 2 )
  {
    std::cout << "lta transform found" << std::endl ;
    if( ltaReader( ifile , matrix , vector ) )
    {
      return -1 ;
    }
  }
  else
  {
    std::cerr << "Transform type not found" << std::endl ;
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
  if( begin == 1 )
  {
    outputTransformFile->SetInput( inverseTransform ) ;
  }
  else
  {
    outputTransformFile->SetInput( affineTransform ) ;
  }
  outputTransformFile->Update() ;
  return 0 ;
}
