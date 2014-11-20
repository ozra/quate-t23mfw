#ifdef INTERFACE
/**
* Created:  2014-11-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "rfx11_types.hh" // 2014-11-19/Oscar Campbell

//#include <chrono>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

typedef R32 QuantFloat;
typedef R64 QuantDouble;

#ifdef DESIGN_CHOICE__USE_32B_SPACE_FOR_QUANT_CALCULATIONS
*TODO*;
typedef N32 QuantTypeSized;
typedef QuantTimeStamp<MILLIS, 100, N32> QuantDuration;
typedef QuantTimeStamp<MILLIS, 100, N32> QuantTime;
typedef R32 QuantReal;
typedef Z32 QuantInt;
typedef N32 QuantUInt; // which ever is fastest of unsigned and regular
typedef S QuantString;

#else
typedef N64 QuantTypeSized;
typedef pxt::time_duration QuantDuration;
typedef pxt::ptime QuantTime;
typedef R64 QuantReal;
typedef Z64 QuantInt;
typedef N64 QuantUInt; // which ever is fastest of unsigned and regular
typedef S QuantString;
#endif

#define MILLISECONDS 1000000 // In nano seconds
#define QUANT_TIMESTAMP_RESOLUTION                                             \
    MILLISECONDS // The smallest resolution of timestamps

#define QUANT_TIMESTAMP_UNCERTAINTY                                            \
    100 // In timestamp resolution ticks - nanoseconds, micros, _millis_ or
        // seconds.

#endif
