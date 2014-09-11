/**
* Created:  2014-08-11
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef QUANTFEED_HH
#define QUANTFEED_HH

#include "boost/circular_buffer.hpp"

#include "TurboSignals.hh"

#include "QuantBase.hh"
#include "QuantStudyContext.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

//template <typename T>
//typedef boost::circular_buffer<T> circular<T>;
#define circular boost::circular_buffer

// We will never really need much back-looking on ticks, more than for seconds-worth of chase-operations..  - 2014-08-29/ORC(01:53)
#define TICKS_BUF_LEN 5000

#define QuantDataFlags int
/*

struct QuantDataFlags {
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
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

class QuantTick {
  public:
    //                    QuantTick () {};
    //                    ~QuantTick () {};

    QuantTime           time;
    QuantDataFlags      flags;
    QuantReal           ask;
    QuantReal           bid;
    QuantReal           last_price;
    QuantReal           last_qty;
    //QuantReal           swap_long;
    //QuantReal           swap_short;

  //private:

};


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

enum FeedState {
    UNDEF = 0,
    HISTORICAL,
    LIVE
};

class QuantFeed {
  public:
    QuantFeed (QuantStudyContext &context, Str broker_id, Str symbol_id,
               QuantTime start_date, QuantTime end_date);

    QuantFeed (QuantStudyContext &context, Str broker_id, Str symbol_id,
               QuantTime start_date);

    ~QuantFeed ();

    bool                readNext ();
    void                emit ();    // When in back-testing mode, this is called 'manually' by the mainloop the keeps track of which feed is next in line
    void                setLiveMode();

    TurboSignal         onPreTick;
    //Emitter<void (const vector<QuantTick> &ticks)>
    Emitter<void ()>    onTick;

    #define circ_buf boost::circular_buffer
    circ_buf<QuantTick> ticks;
    //circular<QuantTick>   // *TODO* pure circular buffer will be perfect here!
    //    ticks(TICKS_BUF_LEN);

  private:
    void                handleData ();
    int                 loadNextSegment ();

    //Mantissation &
    //    mantisator;
    QuantStudyContext   &context;
    Str                 broker_id, symbol_id;
    QuantTime           start_date, end_date;
    FeedState           state = UNDEF;
};

#endif
