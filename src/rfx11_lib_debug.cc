#include "rfx11_lib_debug.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-19
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <cassert>
#include <iostream>

// using namespace std;
using std::cerr;
using std::cout;

#ifdef IS_DEBUG
    #define _D(A) cerr << A
    #define _Dn(A) cerr << A << "\n";

    #define _DBG(A) A

    #ifdef IS_DEEPBUG
        #define _DP(A) cerr << A
        #define _DPn(A) cerr << A << "\n";
    #else
        #define _DP(A) ;  // if(false){cerr<<A;}
        #define _DPn(A) ; // if(false){cerr<<A;}
    #endif

#else
    #define _D(A) ;   // if(false){cerr<<A;}
    #define _Dn(A) ;  // if(false){cerr<<A;}
    #define _DP(A) ;  // if(false){cerr<<A;}
    #define _DPn(A) ; // if(false){cerr<<A;}

    #define _DBG(A) ; // if(false){A;}

#endif

#endif
