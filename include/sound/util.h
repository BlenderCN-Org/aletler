//
//  util.h
//  soundmath
//
//  Created by Katherine Breeden on 12/6/12.
//

#ifndef soundmath_util_h
#define soundmath_util_h

#include <dirent.h>
#include <iomanip>
#include <sstream>


inline
double random_double(const double min, const double max)
{
	double range = max - min;
	return min + (double(rand()) / double(RAND_MAX)) * range;
}

inline
int random_int(const int min, const int max)
{
    int range = max - min;
    return min + int((double(rand())/double(RAND_MAX)) * range);
}


// (kb) got this from SO : http://tinyurl.com/a5vfb5g
static bool DirectoryExists( const char* test_path )
{
    if ( test_path == NULL) {
        std::cerr<<"WARNING: checking for existence of empty directory, returning false.\n";
        return false;
    }
    
    DIR *dir;
    bool exists = false;
    
    dir = opendir (test_path);
    
    if (dir != NULL)
    {
        exists = true;
        (void) closedir (dir);
    }
    
    return exists;
}


// Helper function from StackOverflow -- this should be moved
// out of this class
static std::string ZeroPadNumber(size_t num, int width)
{
  std::ostringstream ss;
  ss << std::setw( width ) << std::setfill( '0' ) << num;
  return ss.str();
}


#endif
