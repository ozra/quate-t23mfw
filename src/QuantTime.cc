#include "QuantTime.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-10
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/


/*
 *
 *   *TODO* *EXPERIMENTAL*
 *
 *
 */


/*
 * Thoughts.
 *
 * We need to represent EPOCH of arbitrary poisitons in time. The epochs them
 * selves are relative to "encoding_epoch" ( ie 1970-01-01 ).
 *
 * We need to represent "datum sample point TIMESTAMP" which is always related
 * to epoch of the sampled data buffers initial timestamp.
 *
 * A buffer / chunk / segment (in file on disk, per json, etc.) therefor always
 * have an epoch given. And that epoch is given in relation to systematic epoch.
 *
 * DATUM SAMPLE TIMESTAMP
 *
 * - 32 bit size:
 *      - 8171 YEARS in MINUTES resolution  (FIN-usage - higher compression basically)
 *      - 136 YEARS in SECONDS resolution   (FIN-usage)
 *      - 50 DAYS in MILLIS resolution
 *      - 71 MINUTES in MICROS resolution
 *      - 4.29 SECONDS in NANOS resolution  (SCI-usage)
 *      - 4 MILLIS in PICOS resolution      (SCI-usage)
 *
 * - 64 bit size:
 *      - 884 YEARS in NANOS resolution   (SCI-usage)
 *      - 213 DAYS in PICOS resolution
 *
 *
 * SEGMENT EPOCH
 *
 *  - 32 bit size:
 *      - 8171 YEARS in MINUTES resolution
 *
 *  - 64 bit size:
 *      - 584942417 YEARS in MILLIS resolution
 */

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#include <cassert>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;

//#define makeTime(A) boost::posix_time::time_from_string(A)

//inline boost::gregorian::date makeDate( int y, int m, int d) {
//    return boost::gregorian::date( y, m, d );
//}


namespace qts {

enum enum_resolution : uint64_t {
    UNDEF =             0,
    NANOS =             1,
    MICROS =         1000,
    MILLIS =      1000000,
    SECONDS =  1000000000,
    MINUTES = 60000000000,
    HOURS = 3600000000000
};

/*
enum enum_formulation {
    UNDEF = 0,
    ULONGLONG = 1,
    DOUBLE = 2
};
*/


/*
 *
 *   *TODO*
 *
 * EpochMn w. Y=1970 should be called:  TimeStoneMn70
 * EpochMn w. Y=1900 :                  TimeStoneMn19
 * EpochMn w. Y="0" as epoch:           TimeStoneMn0
 *
 * EPOCH should be template param - and is only used
 * for conversion to readable date (cout/str etc.) or
 * other EPOCH-bases.
 */

template <enum_resolution RESOLUTION, typename T>
class QuantTime {
  public:

    QuantTime () {
        time_base = 0;
    }

    QuantTime ( T duration )
    :
        time_base { duration }
    {}

    ~QuantTime () {}

    operator T() const {
        return time_base;
    }

    void operator+= ( T duration ) {
        time_base += duration;
    }

    template <enum_resolution IN_RES, typename IN_T>
    void operator+= ( QuantTime<IN_RES, IN_T> &quant_time ) {
        time_base += ( quant_time.time_base * IN_RES / RESOLUTION );
    }

    static const T nanos ( T duration ) {
        return ( enum_resolution::NANOS / RESOLUTION ) * duration;
    }
    static const T micros ( T duration ) {
        return ( enum_resolution::MICROS / RESOLUTION ) * duration;
    }
    static const T millis ( T duration ) {
        return ( enum_resolution::MILLIS / RESOLUTION ) * duration;
    }
    static const T seconds ( T duration ) {
        return ( enum_resolution::SECONDS / RESOLUTION ) * duration;
    }
    static const T minutes ( T duration ) {
        return ( enum_resolution::MINUTES / RESOLUTION ) * duration;
    }
    static const T hours ( T duration ) {
        return ( enum_resolution::HOURS / RESOLUTION ) * duration;
    }

    const T to_nanos ( ) {
        return ( (time_base * RESOLUTION) / enum_resolution::NANOS );
    }
    const T to_micros ( ) {
        return ( (time_base * RESOLUTION) / enum_resolution::MICROS );
    }
    const T to_millis ( ) {
        return ( (time_base * RESOLUTION) / enum_resolution::MILLIS );
    }
    const T to_seconds ( ) {
        return ( (time_base * RESOLUTION) / enum_resolution::SECONDS );
    }
    const T to_minutes ( ) {
        return ( (time_base * RESOLUTION) / enum_resolution::MINUTES );
    }
    const T to_hours ( ) {
        return ( (time_base * RESOLUTION) / enum_resolution::HOURS );
    }

    // // // // // //
    T time_base;

};

template <enum_resolution RES_A, typename T_A, enum_resolution RES_B, typename T_B>
QuantTime<RES_A, T_A> operator+ ( QuantTime<RES_A, T_A> &qts_a, QuantTime<RES_B, T_B> &qts_b ) {
    return QuantTime<RES_A, T_A>( qts_a.time_base + ( (qts_b.time_base * RES_B) / RES_A ) );
}

typedef QuantTime<MINUTES, uint32_t>    EpochMn;
typedef QuantTime<SECONDS, uint64_t>    EpochS;
typedef QuantTime<MILLIS, uint64_t>     EpochMs;

typedef QuantTime<MINUTES, uint32_t>    TsMin;
typedef QuantTime<SECONDS, uint32_t>    TsS;
typedef QuantTime<MILLIS, uint64_t>     TsMs;
typedef QuantTime<MICROS, uint64_t>     TsUs;
typedef QuantTime<NANOS, uint64_t>      TsNs;

}



#endif


