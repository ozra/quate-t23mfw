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

#include <stdint.h>
//#include <types.h>

// Integers
typedef int             /*int,*/Z;
typedef int8_t          int8,   Z8,  Z127;      // +-127
typedef int16_t         int16,  Z16, Z8M;       // +-8 million
typedef int32_t         int32,  Z32, Z2B;       // +-2 billion
typedef int64_t         int64,  Z64, Z9Q,  ZZ;  // +-9 quadrillion

// Naturals
typedef unsigned int    nat,    N;
typedef uint8_t         nat8,   N8,  N255;      // <= 255
typedef uint16_t        nat16,  N16, N16M;      // <= 16 million (actually closer to 17, but..)
typedef uint32_t        nat32,  N32, N4B;       // <= 4 billion
typedef uint64_t        nat64,  N64, N18Q, NN;  // <= 18 quadrillion

// Reals
typedef double          real,   R;
typedef float           real32, R32, R6D;       // 6 guaranteed solid digits
typedef double          real64, R64, R15D;      // 15 guaranteed solid digits

// Character and byte
typedef char            A;                      // Think "Alphanumeric" or "A char"
typedef unsigned char   byte, B;                // byte, buffer

// Bools
typedef bool            L;                      // Logic

// Nothing
typedef void            nothing, V, O;

// Basic pointers
typedef void *          Vp;
typedef char *          Ap;
typedef const char *    cAp; //, cAS;
typedef byte *          Bp;

// Complex fundamental datatypes - string, time, etc.
#include <string>
typedef std::string     string, S;


//
// Domain Specifics
//

typedef     int         arbitrary_numeric_code;
typedef     NN          ncount;
typedef     size_t      dsize, nsize;
typedef     off_t       doffs, dpos, npos, nix;

#endif
