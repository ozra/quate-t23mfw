#ifdef INTERFACE
/**
* Created:  2014-11-19
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/*
//#include <chrono>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;
*/

//#include <stdint.h>
#include <cstdint>
#include <cstddef>
//#include <sys/types.h>
//#include <types.h>

// Integers
//typedef int             /*int,*/int;
/*
typedef int8_t          int8;       // +-127
typedef int16_t         int16;      // +-8 million
typedef int32_t         int32;      // +-2 billion
typedef int64_t         int64;      // +-9 quadrillion
*/

typedef unsigned int    uint, natural, nat;

// Reals
typedef double          real;
typedef float           real32;         // 6 guaranteed solid digits
typedef double          real64;         // 15 guaranteed solid digits

typedef unsigned char   byte;           // byte, buffer

typedef int_fast64_t
f_size_t;       // Good up til shitload enourmous amount of XXX-bytes (+-9 quadrillion)
typedef int_fast32_t    ff_size_t;      // Good up til 2GB
typedef int_fast32_t    f_off_t;        // Good up til 2GB
typedef int_fast32_t    f_ptrdiff_t;    // Good up til 2GB


// Complex fundamental datatypes - string, time, etc.
#include <string>
//typedef std::string     string;
using std::string;


//
// Domain Specifics
//

typedef     int         arbitrary_return_code;



/*
 *
// Integers
typedef int             / *int,* /int;
typedef int8_t          int8,   Z8,  Z127;      // +-127
typedef int16_t         int16,  Z16, Z8M;       // +-8 million
typedef int32_t         int32,  Z32, Z2B;       // +-2 billion
typedef int64_t         int64,  int64_t, Z9Q,  ZZ;  // +-9 quadrillion

// Naturals
typedef unsigned int    nat,    int;
typedef uint8_t         nat8,   N8,  N255;      // <= 255
typedef uint16_t        nat16,  N16, N16M;      // <= 16 million (actually closer to 17, but..)
typedef uint32_t        nat32,  uint32_t, N4B;       // <= 4 billion
typedef uint64_t        nat64,  uint64_t, N18Q, int;  // <= 18 quadrillion

// Reals
typedef double          real, real;
typedef float           real32, real32, real6D;       // 6 guaranteed solid digits
typedef double          real64, real64, real15D;      // 15 guaranteed solid digits

 * // Character and byte
typedef char            A;                      // Think "Alphanumeric" or "A char"
typedef unsigned char   byte, byte;                // byte, buffer

// Bools
typedef bool            bool;                      // Logic

// Nothing
typedef void            nothing, V, void;

// Basic pointers
typedef void *          Vp;
typedef char *          char*;
typedef const char *    const char*; //, cAS;
typedef const char      const char; //, cAS;
typedef byte *          byte*;


// Complex fundamental datatypes - string, time, etc.
#include <string>
typedef std::string     string, string;


//
// Domain Specifics
//

typedef     int         arbitrary_return_code;
typedef     int          f_size_t, Ncount;
typedef     size_t      dsize, nsize, Nsize;
typedef     off_t       doffs, dpos, npos, nix, Noffs, Npos, Nix;



*/



#endif
