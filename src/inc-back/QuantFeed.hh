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
#include "QuantBuffers.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

//#define circular boost::circular_buffer

// We will never really need much back-looking on ticks, more than for seconds-worth of chase-operations..  - 2014-08-29/ORC(01:53)
#define TICKS_BUF_LEN 5000

//#define QuantDataFlags unsigned int
#define QuantDataFlags unsigned char






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
                        QuantTick () {};

                        QuantTick (
                            QuantTime, QuantDataFlags,
                            QuantReal, QuantReal,
                            QuantReal, QuantReal
                        );
    //                    ~QuantTick () {};

    inline bool isGhostTick () {
        return flags & 1;
    }

    inline bool setGhostTick () {
        return flags |= 1;
    }

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


class QuantSequentialDataAbstract
{
  public:
    QuantSequentialDataAbstract ();
    ~QuantSequentialDataAbstract ();
    virtual bool init ( QuantTime start_date, QuantTime end_date );
    virtual bool doInit () = 0;

    // *TODO* QuantSequentialData should return a pointer to decoded
    // data-struct - not meddle with application specific data types!
    // - 2014-09-16/ORC(12:33)
    virtual bool readTick ( QuantTick &tick ) = 0;

  //private:
    QuantTime start_date, end_date;

};

class QuantSequentialDataDukascopy : QuantSequentialDataAbstract
{
  public:
    bool doInit() final;
    bool readTick( QuantTick &tick ) final;

    int loadNextSegment();

  private:
    int pos = 0, buf_len = 0;

};


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


enum FeedState {
    UNDEF = 0,
    HISTORICAL,
    LIVE
};

/*
class QuantFeedAbstract {
};
*/


class QuantFeed {
  public:
    QuantFeed (QuantStudyContext &context,
               Str broker_id,
               Str symbol_id,
               QuantTime start_date,
               QuantTime end_date
    );

    ~QuantFeed ();

    bool                readNext ();
    void                emit ();    // When in back-testing mode, this is called 'manually' by the mainloop the keeps track of which feed is next in line
    void                setLiveMode ();

    ////Emitter<void (const vector<QuantTick> &ticks)>
    TurboSignal         onRegulatedTick;
    TurboSignal         onRealTick;

    ReversedCircularStructBuffer<QuantTick> ticks;

    QuantSequentialDataAbstract *sequential_data_interface;

  private:
    void                handleData ();
    QuantStudyContext   &context;
    Str                 broker_id,
                        symbol_id;
    QuantTime           start_date,
                        end_date;
    FeedState           state = UNDEF;
    int                 regulated_interval;
    int                 day_epoch_offset;

};

/*
class QuantFeedAggregator {

    add ( QuantFeedAbstract *feed ) {
        feeds.push_back( feed );
        feed->onRealTick()
    }
};
*/


#endif
