#include "QuantTick.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-11
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//#include "boost/circular_buffer.hpp"

#include "QuantBase.hh"

//#define QuantTickFlags unsigned int
#define QuantTickFlags unsigned char

/*
struct QuantTickFlags {
    unit2       tick_type;          // 0 = orderbook or swap changes    - NONE TRADE TICK
                                    // 1 = market-trade - unkown side
                                    // 2 = market-buy
                                    // 3 = market-sell

    uint3       data_solidity;      // 0 = straight (real, normal), - BEST
                                    // 1 = interpolated,            - NOT GOOD (MT4)
                                    // 2 = repeated,                - "NECESSARY" EVIL (reason: no tick in 60sec - the last tick value is repeated to keep sample geometry intact)
                                    // 3 = extrapolated             - MIGHT NOT BE IMPLEMENTED (reason: no tick in 60sec - really an interpolation, but called extrapolation because of LACK of original data, not because of data-compositing like MT4-1m tick collections..)
                                    // 4 = extern-extrapolated      - ANOTHER EXCHANGE/BROKER is used as source for the missing data

    uint3       session_level;      // 0 = session state unknown
                                    // 1 = broker down, unplanned (non market-wide)
                                    // 2 = market down, "uncommon reason"/exception,
                                    // 3 = market completely down (regulated [weekends maybe]),
                                    // 4 = market down (convention [weekends]),
                                    // 5 = notably weak off hours,      - FACT OF LIFE
                                    // 6 = primary hours                - BEST
}
*/
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

class QuantTick {
  public:
    QuantTick () {};

    QuantTick (
        QuantTime,
        //QuantTickFlags,
        QuantReal,
        QuantReal,
        QuantReal,
        QuantReal
    );
    //~QuantTick () {};

    inline bool isGhostTick () {
        return last_qty == 0;
    }

    //inline bool setGhostTick () {
    //    return flags |= 1;
    //}

    QuantTime           time;
    //QuantTickFlags      flags;
    QuantReal           ask;
    QuantReal           bid;
    QuantReal           last_price;
    QuantReal           last_qty;
    //QuantReal           swap_long;
    //QuantReal           swap_short;

  //private:
};
#endif

QuantTick::QuantTick (
    QuantTime       time,
    //QuantTickFlags  flags,
    QuantReal       ask,
    QuantReal       bid,
    QuantReal       last_price,
    QuantReal       last_qty
) :
    time( time ),
    ask( ask ),
    bid( bid ),
    last_price( last_price ),
    last_qty( last_qty )
{}

