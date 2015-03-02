#include "QuantElementModel_Tick.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-11
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//#include "boost/circular_buffer.hpp"

#include "QuantBase.hh"
#include <string>
#include <sstream>
#include <boost/date_time.hpp>
//#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include "QuantTime.hh"


//#define QuantTickFlags unsigned int
#define QuantTickFlags unsigned char


/*
struct QuantElement_TickFlags {
    unit2   tick_type;      // 0 = orderbook or swap changes    - NONE TRADE TICK
                            // 1 = market-trade - unkown side
                            // 2 = market-buy
                            // 3 = market-sell

    uint3   data_solidity;  // 0 = straight (real, normal), - BEST
                            // 1 = interpolated,            - NOT GOOD (MT4)
                            // 2 = repeated,                - "NECESSARY" EVIL (reason: no tick in 60sec - the last tick value is repeated to keep sample geometry intact)
                            // 3 = extrapolated             - MIGHT NOT BE IMPLEMENTED (reason: no tick in 60sec - really an interpolation, but called extrapolation because of LACK of original data, not because of data-compositing like MT4-1m tick collections..)
                            // 4 = extern-extrapolated      - ANOTHER EXCHANGE/BROKER is used as source for the missing data

    uint3   session_level;  // 0 = session state unknown
                            // 1 = broker down, unplanned (non market-wide)
                            // 2 = market down, "uncommon reason"/exception,
                            // 3 = market completely down (regulated [weekends maybe]),
                            // 4 = market down (convention [weekends]),
                            // 5 = notably weak off hours,      - FACT OF LIFE
                            // 6 = primary hours                - BEST
}
*/
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


/*
inline std::stringbuf & operator<< ( std::stringbuf &s, Time_T &t ) {
    string << boost::posix_time::to_iso_extended_string( t ).c_str();
    return s;
}
*/

template <typename Time_T, typename Real_T>
struct QuantElement_Tick {
    QuantElement_Tick() {};

    QuantElement_Tick(
        Time_T       time,
        Real_T       ask,
        Real_T       bid,
        Real_T       last_price,
        Real_T       ask_volume,
        Real_T       bid_volume
    ) :
        time { time },
        ask { ask },
        bid { bid },
        last_price { last_price },
        ask_volume { ask_volume },
        bid_volume { bid_volume }
    {}

    inline bool isGhostTick() const
    {
        return ask_volume == 0 && bid_volume == 0;  //
    }

    inline string to_str() const
    {
        std::stringstream buf;
        buf << ": " << ask << "(" << ask_volume << "), " << bid << "(" << bid_volume <<
            ")" << " ";
        return buf.str();
    }

    Time_T           time;
    Real_T           ask;
    Real_T           bid;
    Real_T
    last_price;     // In many cases (most except XBT brokers, which are "true" markets) this is always the same as bid for sell, and ask for buy - an hence - in analysis - always "one of them".
    Real_T           ask_volume;
    Real_T           bid_volume;

};


template <typename Time_T, typename Real_T, typename Type_T>
struct QuantElement_Trade {
    QuantElement_Trade() {};

    QuantElement_Trade(
        Time_T       time,
        Real_T       price,
        Real_T       qty,
        Type_T       type
    ) :
        time { time },
        price { price },
        qty { qty },
        type { type }
    {}

    inline bool isGhostTick() const
    {
        constexpr Type_T GHOST_TICK =
            64;    // *TODO* Check T23M/V8 for value reference...
        return ((type & GHOST_TICK) != 0);
        //return qty == 0;
    }

    inline string to_str() const
    {
        std::stringstream buf;
        buf << ": " << price << "(" << qty << "), " << type << " ";
        return buf.str();
    }

    Time_T       time;
    Real_T       price;
    Real_T       qty;
    Type_T       type;

};

template <typename Time_T, typename Real_T, int SIZE>
struct QuantElement_Obshot {
    Time_T       time;
    Real_T       asks[SIZE];
    Real_T       ask_qtys[SIZE];
    Real_T       bids[SIZE];
    Real_T       bid_qtys[SIZE];

};


/*
 * *TODO* the fucking templating...
 */
/*
template <typename T_A, typename T_B>
inline auto QuantElement_Tick<T_A, T_B>::to_str () -> std::string {
    std::stringstream buf;
    if ( typeid(T_A) == typeid(QuantTime) ) {
        buf << boost::posix_time::to_iso_extended_string( time ) << ": " << ask << "(" << ask_volume << "), " << bid << "(" << bid_volume << ")" << " ";
    } else {
        buf << time << ": " << ask << "(" << ask_volume << "), " << bid << "(" << bid_volume << ")" << " ";
    }
    return buf.str();
}
*/
/*
template <typename T_A, typename T_B>
inline auto QuantElement_Tick<QuantTime, T_B>::to_str () -> std::string {
    std::stringstream buf;
    buf << boost::posix_time::to_iso_extended_string( time ) << ": " << ask << "(" << ask_volume << "), " << bid << "(" << bid_volume << ")" << " ";
    return buf.str();
}

*/

typedef QuantElement_Tick<QuantTime, QuantReal> QuantTick;
typedef QuantElement_Tick<uint64_t, int64_t>    QuantTickFixed;


#endif
