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
 *  REMODEL system to use this class!
 *
 *  Just sub-class ptime etc. for date-manip functionality! KISS!
 *
 *  QuantTime64 is 1 ms resolution
 *  QuantTime32 is 128 ms resolution (17.4Y range)
 *
 *  "Expanded" quant-time, stored in decoded elements, is always relative to
 *  "calculation range start epoch". Only if printable date or out of context use
 *  is needed do we add epoch and return TimeStamp64..
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
 *   - FIN USAGE -
 *      - 8171 YEARS in MINUTES resolution
 *      - 136 YEARS in SECONDS resolution
 *      - 13.7 YEARS in DECISECS (100 millis)
 *   -> - 27.4 YEARS in QUINTISECS (200 millis)
 *      - 50 DAYS in MILLIS resolution
 *
 *   - SCI usage -
 *      - 71 MINUTES in MICROS resolution
 *      - 4.29 SECONDS in NANOS resolution
 *      - 4 MILLIS in PICOS resolution
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
using std::cerr;
#include <cassert>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;

//#define makeTime(A) boost::posix_time::time_from_string(A)

// inline boost::gregorian::date makeDate( int y, int m, int d) {
//    return boost::gregorian::date( y, m, d );
//}

namespace qts {

/*
enum time_scale : uint64_t {
    UNDEF =     0,
    PICOS =     0,
    NANOS =     1,
    MICROS =    1000,
    MILLIS =    1000000,
    DECIS =     100000000,  // Yeah, new fucking invention in time - to make u32-ts possible in some contexts.. 2015-02-20
    QUINTIS =   200000000,  // Yeah, new fucking invention in time - to make u32-ts possible in some contexts.. 2015-02-20
    SECONDS =   1000000000,
    MINUTES =   60000000000,
    HOURS =     3600000000000
};
*/

enum time_scale : uint64_t {
    UNDEF =     0,
    PICOS =     0,
    NANOS =     0,
    MICROS =    0,

    MILLIS =    1,
    DECIS =     100,  // Yeah, new fucking invention in time - to make u32-ts possible in some contexts.. 2015-02-20
    QUINTIS =   200,  // Yeah, new fucking invention in time - to make u32-ts possible in some contexts.. 2015-02-20
    SECONDS =   1000,
    MINUTES =   60000,
    HOURS =     3600000
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

inline auto ms_from_ptime(pxt::ptime ts) ->
uint64_t { // - 2014-11-10/Oscar Campbell
    static pxt::ptime unix_epoch__ =
    pxt::time_from_string("1970-01-01 00:00:00.000");
    return (ts - unix_epoch__).total_milliseconds();
}

//V - 2014-11-20/Oscar Campbell
inline auto millis(pxt::ptime ts) -> uint64_t { return ms_from_ptime(ts); }

inline auto ptime_from_ms(uint64_t ts)
-> pxt::ptime { // - 2014-11-10/Oscar Campbell
    static pxt::ptime unix_epoch__ =
    pxt::time_from_string("1970-01-01 00:00:00.000");
    return { unix_epoch__ + pxt::milliseconds(ts) };
}

inline pxt::ptime
get_prior_aligned_ts(pxt::ptime ts, pxt::time_duration alignment,
                     dt::greg_weekday epochian_weekday = dt::greg_weekday(
                                 dt::greg_weekday::weekday_enum::Sunday))
{
    pxt::ptime epochian_ts;
    cerr << "get_prior_aligned_ts() : ";
    cerr << ", ts:" << ts;
    cerr << ", alignment:" << alignment;
    if (alignment <= pxt::hours(24)) {
        epochian_ts = pxt::ptime(ts.date(), pxt::hours(0));
        cerr << ", intraday epochian_ts = " << epochian_ts;
    }
    else {
        epochian_ts = pxt::ptime(
                          dt::previous_weekday(ts.date(), epochian_weekday), pxt::hours(0));
        cerr << ", weekly epochian_ts = " << epochian_ts;
    }
    pxt::time_duration d = (ts - epochian_ts);
    cerr << ", (ts - epochian_ts) = " << d;
    //  *TODO* CONF of TIME_TIME_SCALE - now lowest is "millis", we might want
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
                                      pxt::time_duration alignment)
{
    return get_prior_aligned_ts(ts, alignment) + alignment;
}

inline pxt::ptime get_next_aligned_ts(pxt::ptime ts,
                                      pxt::time_duration alignment,
                                      dt::greg_weekday epochian_weekday)
{
    return get_prior_aligned_ts(ts, alignment, epochian_weekday) + alignment;
}
//
//
//


template <time_scale TIME_SCALE, typename T>
class QuantTimeStamp
{
  public:
    QuantTimeStamp() { timestamp_ = 0; }

    QuantTimeStamp(T duration) : timestamp_{ duration } {}

    ~QuantTimeStamp() {}

    operator T() const { return timestamp_; }

    void operator+=(T duration) { timestamp_ += duration; }

    template <time_scale IN_SCALE, typename IN_T>
    void operator+=(QuantTimeStamp<IN_SCALE, IN_T> & quant_time)
    {
        timestamp_ += (quant_time.timestamp_ * IN_SCALE / TIME_SCALE);
    }

    /*
    static inline const T nanos(T duration) {
        return (time_scale::NANOS / TIME_SCALE) * duration;
    }
    static inline const T micros(T duration)
    {
        return (time_scale::MICROS / TIME_SCALE) * duration;
    }
    */
    static inline const T from_millis(T duration)
    {
        return (duration * time_scale::MILLIS) / TIME_SCALE;
    }
    static inline const T from_decis(T duration)
    {
        return (duration * time_scale::DECIS) / TIME_SCALE;
    }
    static inline const T from_quintis(T duration)
    {
        return duration * (time_scale::QUINTIS / TIME_SCALE);
    }
    static inline const T from_seconds(T duration)
    {
        return duration * (time_scale::SECONDS / TIME_SCALE);
    }
    static inline const T from_minutes(T duration)
    {
        return duration * (time_scale::MINUTES / TIME_SCALE);
    }
    static inline const T from_hours(T duration)
    {
        return duration * (time_scale::HOURS / TIME_SCALE);
    }

    /*
    inline const T to_nanos()
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::NANOS);
    }
    inline const T to_micros()
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::MICROS);
    }
    */
    inline const T to_millis() const
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::MILLIS);
    }
    inline const T to_decis() const
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::DECIS);
    }
    inline const T to_quintis() const
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::QUINTIS);
    }
    inline const T to_seconds() const
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::SECONDS);
    }
    inline const T to_minutes() const
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::MINUTES);
    }
    inline const T to_hours() const
    {
        return ((timestamp_ * TIME_SCALE) / time_scale::HOURS);
    }

    inline const pxt::ptime to_ptime_as_unix_epoched() const
    {
        static pxt::ptime unix_epoch__ =
            pxt::time_from_string("1970-01-01 00:00:00.000");
        return { unix_epoch__ + pxt::milliseconds(timestamp_ * TIME_SCALE) };
    }
    /*
    inline const pxt::ptime to_ptime_with_ms_epoch(uint64_t epoch)
    {
        return { pxt::milliseconds(epoch + timestamp_ * TIME_SCALE) };
    }
    inline const pxt::ptime to_ptime_with_epoch(pxt::time_duration epoch)
    {
        return { epoch + pxt::milliseconds(timestamp_ * TIME_SCALE) };
    }
    */
    inline const pxt::ptime to_ptime_with_epoch(pxt::ptime epoch) const
    {
        return { epoch + pxt::milliseconds(timestamp_ * TIME_SCALE) };
    }

    // // // // // //
    T timestamp_;
};

template <time_scale RES_A, typename T_A, time_scale RES_B,
          typename T_B>
QuantTimeStamp<RES_A, T_A> operator+(QuantTimeStamp<RES_A, T_A> & qts_a,
                                     QuantTimeStamp<RES_B, T_B> & qts_b)
{
    return QuantTimeStamp<RES_A, T_A>(qts_a.timestamp_ +
                                      ((qts_b.timestamp_ * RES_B) / RES_A));
}

typedef QuantTimeStamp<MINUTES, uint32_t> EpochMn;
typedef QuantTimeStamp<SECONDS, uint64_t> EpochS;
typedef QuantTimeStamp<MILLIS, uint64_t> EpochMs;
typedef QuantTimeStamp<MILLIS, uint32_t>
TimeStoneMs; // Can handle about 49 days

typedef QuantTimeStamp<MINUTES, uint32_t> TsMin;
typedef QuantTimeStamp<SECONDS, uint32_t> TsS;
typedef QuantTimeStamp<MILLIS, uint64_t> TsMs;
typedef QuantTimeStamp<MICROS, uint64_t> TsUs;
typedef QuantTimeStamp<NANOS, uint64_t> TsNs;
}

#endif

/* *TODO* */

natural day_of_week(natural d, natural m, natural y)
{
    static natural t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= m < 3;
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}
