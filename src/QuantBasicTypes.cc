#ifdef INTERFACE
/**
* Created:  2014-11-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
* Code Cls: Blackbox Fundamentals
**/

#include "rfx11_types.hh" // 2014-11-19/Oscar Campbell

//#include <chrono>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;

#include "QuantTime.hh"

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#ifdef DESIGN_CHOICE__USE_32B_SPACE_FOR_QUANT_CALCULATIONS
    * TODO*;
    typedef uint32_t QuantTypeSized;
    typedef QuantTimeStamp<MILLIS, uint64_t> QuantEpoch;
    typedef QuantTimeStamp<QUINTIS, uint32_t> QuantDuration;
    typedef QuantTimeStamp<QUINTIS, uint32_t> QuantTime;
    typedef real32 QuantReal;
    typedef int32_t QuantInt;
    typedef uint32_t QuantNat;
    typedef string QuantString;

#else
    typedef uint64_t QuantTypeSized;
    // typedef QuantTimeStamp<MILLIS, uint64_t> QuantDuration;
    // typedef QuantTimeStamp<MILLIS, uint64_t> QuantTime;
    typedef pxt::ptime QuantEpoch;
    typedef pxt::time_duration QuantDuration;
    typedef pxt::ptime QuantTime;
    typedef real64 QuantReal;
    typedef int64_t QuantInt;
    typedef uint64_t QuantNat;
    typedef string QuantString;
#endif

typedef real32 QuantFloat;
typedef real64 QuantDouble;


//#define MILLISECONDS 1000000 // In nano seconds
// ↓ The smallest resolution of timestamps
//#define QUANT_TIMESTAMP_RESOLUTION MILLISECONDS
// ↓ In timestamp resolution ticks - nanoseconds, micros, _millis_ or seconds.
//#define QUANT_TIMESTAMP_UNCERTAINTY 100

#endif
