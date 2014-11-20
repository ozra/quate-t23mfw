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
 * For below, UNCERTAINTY = 1, if not otherwise stated
 *
 * DATUM SAMPLE TIMESTAMP
 *
 * - 32 bit size:
 *      - 8171 YEARS in MINUTES resolution                      (FIN-usage -
 *higher compression basically)
 *      - 136 YEARS in SECONDS resolution                       (FIN-usage)
 *      - 13.7 YEARS in MILLIS resolution w/ UNCERTAINTY = 100  (FIN-usage)
 *      - 6.4 YEARS in MILLIS resolution w/ UNCERTAINTY = 47    (FIN-usage)
 *      - 50 DAYS in MILLIS resolution                          (FIN-usage)
 *      - 71 MINUTES in MICROS resolution                       (SCI-usage)
 *      - 4.29 SECONDS in NANOS resolution                      (SCI-usage)
 *      - 4 MILLIS in PICOS resolution                          (SCI-usage)
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

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
#include "rfx11_types.hh"

#include <iostream>
#include <cassert>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using std::cerr;

namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;

//#define makeTime(A) boost::posix_time::time_from_string(A)

// inline boost::gregorian::date makeDate( int y, int m, int d) {
//    return boost::gregorian::date( y, m, d );
//}

namespace qts {

enum enum_resolution : uint64_t {
    UNDEF = 0,
    NANOS = 1,
    MICROS = 1000,
    MILLIS = 1000000,
    SECONDS = 1000000000,
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
 * TimeStone is used for shorter epochs
 * Epoch is used for more classical Epochs like
 *
 * EpochSec w. Y=1970 should be called:     EpochS70
 * EpochMn w. Y=1900 :                      EpochMn19
 * EpochMn w. Y="0" as epoch:               EpochMn0
 *
 * Hourly epoch in millis:                  TimeStoneMs60
 *
 *
 * EPOCH should be template param - and is only used
 * for conversion to readable date (cout/str etc.) or
 * other EPOCH-bases.
 */

inline auto ms_from_ptime(pxt::ptime ts) -> N64 { // - 2014-11-10/Oscar Campbell
    static pxt::ptime unix_epoch__ =
        pxt::time_from_string("1970-01-01 00:00:00.000");
    return (ts - unix_epoch__).total_milliseconds();
}

//V - 2014-11-20/Oscar Campbell
inline auto millis(pxt::ptime ts) -> N64 { return ms_from_ptime(ts); }

inline auto ptime_from_ms(N64 ts)
    -> pxt::ptime { // - 2014-11-10/Oscar Campbell
    static pxt::ptime unix_epoch__ =
        pxt::time_from_string("1970-01-01 00:00:00.000");
    return { unix_epoch__ + pxt::milliseconds(ts) };
}

inline pxt::ptime
get_prior_aligned_ts(pxt::ptime ts, pxt::time_duration alignment,
                     dt::greg_weekday epochian_weekday = dt::greg_weekday(
                         dt::greg_weekday::weekday_enum::Sunday)) {
    pxt::ptime epochian_ts;

    cerr << "get_prior_aligned_ts() : ";
    cerr << ", ts:" << ts;
    cerr << ", alignment:" << alignment;

    if (alignment <= pxt::hours(24)) {
        epochian_ts = pxt::ptime(ts.date(), pxt::hours(0));
        cerr << ", intraday epochian_ts = " << epochian_ts;
    } else {
        epochian_ts = pxt::ptime(
            dt::previous_weekday(ts.date(), epochian_weekday), pxt::hours(0));
        cerr << ", weekly epochian_ts = " << epochian_ts;
    }

    pxt::time_duration d = (ts - epochian_ts);
    cerr << ", (ts - epochian_ts) = " << d;

    //  *TODO* CONF of TIME_RESOLUTION - now lowest is "millis", we might want
    // to support nanos for scientific data... 2014-10-29/ORC
    pxt::time_duration ofs(pxt::milliseconds(d.total_milliseconds() %
                                             alignment.total_milliseconds()));
    cerr << ", ofs = " << ofs;

    pxt::ptime final_ts = ts - ofs; // + alignment;
    cerr << ", final_ts = " << final_ts;

    cerr << "\n";

    return final_ts;
}

//
//    *TODO* *INVESTIGATE*  - will these hold in fragmental epoch boundary
// crossing???
//
inline pxt::ptime get_next_aligned_ts(pxt::ptime ts,
                                      pxt::time_duration alignment) {
    return get_prior_aligned_ts(ts, alignment) + alignment;
}

inline pxt::ptime get_next_aligned_ts(pxt::ptime ts,
                                      pxt::time_duration alignment,
                                      dt::greg_weekday epochian_weekday) {
    return get_prior_aligned_ts(ts, alignment, epochian_weekday) + alignment;
}
//
//
//

// *TODO* add SUB_RESOLUTION AKA UNCERTAINTY
// template <enum_resolution RESOLUTION, int SUB_RESOLUTION, typename T>
template <enum_resolution RESOLUTION, typename T>
class QuantTimeStamp {
   public:
    QuantTimeStamp() { timestamp = 0; }

    QuantTimeStamp(T duration) : timestamp{ duration } {}

    ~QuantTimeStamp() {}

    operator T() const { return timestamp; }

    void operator+=(T duration) { timestamp += duration; }

    template <enum_resolution IN_RES, typename IN_T>
    void operator+=(QuantTimeStamp<IN_RES, IN_T>& quant_time) {
        timestamp += (quant_time.timestamp * IN_RES / RESOLUTION);
    }

    static const T nanos(T duration) {
        return (enum_resolution::NANOS / RESOLUTION) * duration;
    }
    static const T micros(T duration) {
        return (enum_resolution::MICROS / RESOLUTION) * duration;
    }
    static const T millis(T duration) {
        return (enum_resolution::MILLIS / RESOLUTION) * duration;
    }
    static const T seconds(T duration) {
        return (enum_resolution::SECONDS / RESOLUTION) * duration;
    }
    static const T minutes(T duration) {
        return (enum_resolution::MINUTES / RESOLUTION) * duration;
    }
    static const T hours(T duration) {
        return (enum_resolution::HOURS / RESOLUTION) * duration;
    }

    const T to_nanos() {
        return ((timestamp * RESOLUTION) / enum_resolution::NANOS);
    }
    const T to_micros() {
        return ((timestamp * RESOLUTION) / enum_resolution::MICROS);
    }
    const T to_millis() {
        return ((timestamp * RESOLUTION) / enum_resolution::MILLIS);
    }
    const T to_seconds() {
        return ((timestamp * RESOLUTION) / enum_resolution::SECONDS);
    }
    const T to_minutes() {
        return ((timestamp * RESOLUTION) / enum_resolution::MINUTES);
    }
    const T to_hours() {
        return ((timestamp * RESOLUTION) / enum_resolution::HOURS);
    }

    // // // // // //
    T timestamp;
};

template <enum_resolution RES_A, typename T_A, enum_resolution RES_B,
          typename T_B>
QuantTimeStamp<RES_A, T_A> operator+(QuantTimeStamp<RES_A, T_A>& qts_a,
                                     QuantTimeStamp<RES_B, T_B>& qts_b) {
    return QuantTimeStamp<RES_A, T_A>(qts_a.timestamp +
                                      ((qts_b.timestamp * RES_B) / RES_A));
}

typedef QuantTimeStamp<MINUTES, uint32_t> EpochMn;
typedef QuantTimeStamp<SECONDS, uint64_t> EpochS;
typedef QuantTimeStamp<MILLIS, uint64_t> EpochMs;
typedef QuantTimeStamp<MILLIS, uint32_t>
TimeStoneMs; // Can handle about 24 days

typedef QuantTimeStamp<MINUTES, uint32_t> TsMin;
typedef QuantTimeStamp<SECONDS, uint32_t> TsS;
typedef QuantTimeStamp<MILLIS, uint64_t> TsMs;
typedef QuantTimeStamp<MICROS, uint64_t> TsUs;
typedef QuantTimeStamp<NANOS, uint64_t> TsNs;
}

#endif

/* *TODO* */

int day_of_week(int d, int m, int y) {
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= m < 3;
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}
