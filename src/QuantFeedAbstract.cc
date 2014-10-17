#include "QuantFeedAbstract.hh"

#ifdef INTERFACE
/**
* Created:  2014-08-11
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//#include "boost/circular_buffer.hpp"

#include "QuantBasic_DesignChoices.hh"

#include "SwiftSignal.hh"
#include "HardSignal.hh"

#include "QuantBase.hh"
#include "QuantStudyContextAbstract.hh"
#include "QuantBuffers.hh"
#include "QuantTick.hh"

#include "QuantSequentialData.hh"
#include "QuantSequentialData_DukascopyTicks.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


// We will never really need much back-looking on ticks, more than for seconds-worth of chase-operations..  - 2014-08-29/ORC(01:53)
//#define TICKS_BUF_LEN 5000


enum FeedState {
    UNDEF = 0,
    HISTORICAL,
    LIVE
};

/*
class QuantFeedAbstract {
};
*/

class QuantFeedAbstract {
  public:
    /*
    QuantFeedAbstract (
        QuantStudyContextAbstract *context,
        Str broker_id,
        Str symbol_id
    );
     */

    QuantFeedAbstract (
        Str broker_id,
        Str symbol_id,
        int lookback = 0,
        QuantKeeperJar *the_jar = global_actives.active_jar
    );

    ~QuantFeedAbstract ();

    void setRegulatedInterval ( double p_regulated_interval );
    void setDateRange (
        QuantTime start_date,
        QuantTime end_date
    );
    void setLiveMode ();

    virtual bool readNext ();
    virtual void emit () = 0;

    #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_FEEDS
        HardSignal< QuantPeriodizationAbstract, QuantFeedAbstract & >    onRegulatedTick_P;
    #else
        SwiftSignal     onRegulatedTick;
        SwiftSignal     onRealTick;

        //void            onRegulatedTick ( boost::function<void()> );
        //void            onRegulatedTick ( float, boost::function<void()> );
    #endif

    ReversedCircularStructBuffer<QuantTick> ticks;
    QuantTick       buffered_tick;
    bool            has_buffered = false;

    QuantSequentialData_DukascopyTicks *sequential_data_interface;

    #ifdef IS_DEBUG
    inline void    reset_debug_counts () {
        real_tick_count = 0;
        ghost_tick_count = 0;
    }

    int     real_tick_count = 0;
    int     ghost_tick_count = 0;

    #endif

  private:
    void                        handleData ();
    //QuantStudyContextAbstract   *context;
    QuantKeeperJar              *the_jar;
    Str                         broker_id;
    Str                         symbol_id;

    int                 lookback;
    FeedState           state = UNDEF;
    QuantTime           start_date;
    QuantTime           end_date;
    QuantTime           next_regulated_tick_time;
    QuantDuration       regulated_interval = pxt::hours( 999999 );   // pxt::max_date_time; // *TODO* --> numbers<QuantTime>.max_value;
    int                 day_epoch_offset = 0;

};


#endif

/*
QuantFeedAbstract::QuantFeedAbstract (
    QuantStudyContextAbstract *context,
    Str broker_id,
    Str symbol_id
)
: ticks( 5000 )
, context( context )
, broker_id( broker_id )
, symbol_id( symbol_id )
{
    global_actives.active_feed = this;
    sequential_data_interface = new QuantSequentialData_DukascopyTicks();
    context->add( this );
}
*/

QuantFeedAbstract::QuantFeedAbstract (
    Str broker_id,
    Str symbol_id,
    int lookback,
    QuantKeeperJar *the_jar
) :
    the_jar { the_jar },
    ticks ( lookback + 2 ),     // +1 for current tick, +1 for prev. tick
    broker_id { broker_id },
    symbol_id { symbol_id },
    lookback { lookback }
{
    global_actives.active_feed = this;  // *TODO* pass the "global_actives" as param (could then ofcourse be set as default param) 141013/ORC

    // *TODO* this has NOTHING to do in QuantFeedAbstract!!!
    sequential_data_interface = new QuantSequentialData_DukascopyTicks();


    the_jar->add( this );
}

QuantFeedAbstract::~QuantFeedAbstract ()
{
    cerr << "QuantFeed::~QuantFeed - - DESTRUCTOR - -" << "\n";
    //delete []ticks;
}

void QuantFeedAbstract::setDateRange (
   QuantTime p_start_date,
   QuantTime p_end_date
) {
    start_date = p_start_date;
    end_date = p_end_date;

    next_regulated_tick_time = start_date;

    // *TODO* PROPER PLACEMENT!
    sequential_data_interface->init(symbol_id, 0.00001, start_date, end_date);
}

void QuantFeedAbstract::setRegulatedInterval ( double p_regulated_interval )
{
    // *TODO* *DUPE* *DRY* ( QuantFeed : commonInit() )
    if ( p_regulated_interval >= 1) {
        p_regulated_interval = ( p_regulated_interval * 60 );  // Passed in minutes, store as seconds
    } else {
        p_regulated_interval = ( p_regulated_interval * 100 );  // From the notation-value of decimal to integer seconds
    }

    QuantDuration tmp_interval = pxt::seconds( p_regulated_interval );
    regulated_interval = MIN( regulated_interval, tmp_interval );
}

/*
void QuantFeed::onRegulatedTick ( float p_regulated_interval, boost::function<void()> callback )
{
    setRegulatedInterval( p_regulated_interval );
    regulatedTickSignal.connect( callback );
}
*/

/*
void QuantFeedAbstract::onRegulatedTick ( boost::function<void()> callback )
{
    onRegulatedTick.connect( callback );
}
*/

bool QuantFeedAbstract::readNext () {
    //cerr << "readNext() - Advance ticks" << "\n";


    // *TODO* use reference directly instead of making a pointer!?

    #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
        QuantTick &tick = ticks.advance();
    #else
        QuantTick *tick = &ticks.advance();
    #endif

    //cerr << "readTick from seq-interface" << "\n";

    if ( LIKELY( has_buffered == false ) ) {
        //cerr << "r";
        #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
            bool ret = sequential_data_interface->readTick( tick );
        #else
            bool ret = sequential_data_interface->readTick( *tick );
        #endif

        if ( ret == false) {
            cerr << "seq-data-iface returned false" << "\n";
            return false;
        }

        #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
        if ( tick.time <= next_regulated_tick_time || ticks.size <= 1 ) {
        #else
        if ( tick->time <= next_regulated_tick_time || ticks.size <= 1 ) {
        #endif
            #ifdef IS_DEBUG
                ++real_tick_count;
            #endif
            return true;

        } else {
            //cerr << "Just GOT IN TO GhostTickin" << "\n";
            #ifdef IS_DEBUG
            ++ghost_tick_count;
            #endif

            // *TODO* test if making specific field copies (Since not all are
            // used) is faster - and done in correct sequence of course


            #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
                #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_2
                    const QuantTick &prev = ticks[1];
                    buffered_tick = tick;
                    tick.time = next_regulated_tick_time;
                    tick.ask = prev.ask;
                    tick.bid = prev.bid;
                    tick.last_price = prev.last_price;
                    tick.volume = 0; // Ghost tick..
                    has_buffered = true;
                    next_regulated_tick_time += regulated_interval;
                #else
                    buffered_tick = tick;
                    tick = ticks[1];
                    tick.time = next_regulated_tick_time;
                    tick.volume = 0; // Ghost tick..
                    has_buffered = true;
                    next_regulated_tick_time += regulated_interval;
                #endif
            #else
                #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_2
                    const QuantTick &prev = ticks[1];
                    buffered_tick = tick;
                    tick->time = next_regulated_tick_time;
                    tick->ask = prev.ask;
                    tick->bid = prev.bid;
                    tick->last_price = prev.last_price;
                    tick->volume = 0; // Ghost tick..
                    has_buffered = true;
                    next_regulated_tick_time += regulated_interval;
                #else
                    buffered_tick = *tick;
                    *tick = ticks[1];
                    tick->time = next_regulated_tick_time;
                    tick->volume = 0; // Ghost tick..
                    has_buffered = true;
                    next_regulated_tick_time += regulated_interval;
                #endif

            #endif
            return true;
        }

    } else {
        //cerr << "b";
        if ( buffered_tick.time <= next_regulated_tick_time ) {
            #ifdef IS_DEBUG
            ++real_tick_count;
            #endif

            //cerr << "Just GOT OUT OF GhostTickin" << "\n";
            #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
                tick = buffered_tick;
            #else
                *tick = buffered_tick;
            #endif
            has_buffered = false;
            return true;

        } else {
            #ifdef IS_DEBUG
            ++ghost_tick_count;
            #endif

            // *TODO* test if making specific field copies (Since not all are
            // used) is faster - and done in correct sequence of course
            #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
                #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_2
                    const QuantTick &prev = ticks[1];
                    tick.time = next_regulated_tick_time;
                    tick.ask = prev.ask;
                    tick.bid = prev.bid;
                    tick.last_price = prev.last_price;
                    tick.volume = 0; // Ghost tick..
                #else
                    tick = ticks[1];
                    tick.time = next_regulated_tick_time;
                    tick.volume = 0; // Ghost tick..
                #endif
            #else
                #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_2
                    tick->time = next_regulated_tick_time;
                    tick->ask = ticks[1].ask;
                    tick->bid = ticks[1].bid;
                    tick->last_price = ticks[1].last_price;
                    tick->volume = 0; // Ghost tick..
                #else
                    *tick = ticks[1];
                    tick->time = next_regulated_tick_time;
                    tick->volume = 0; // Ghost tick..
                #endif
            #endif


            next_regulated_tick_time += regulated_interval;
            return true;
        }

    }

    //cerr << "readNext is done." << "\n";
    return false;
}


// In event of asynchronous live data
void QuantFeedAbstract::handleData ()
{
    // *TODO* *IMPL*

}

void QuantFeedAbstract::setLiveMode ()
{
    state = LIVE;
    // Do some other shit like setting up listeners etc. (should've been done earlier probably though) *TODO*
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#ifdef INTERFACE

/*
 *  Aggregation can veeery simply be setup by the strategy implementing user
 * simply by listening to more feeds and updating periodicals from that
 *
 * class QuantFeedAggregator {

    add ( QuantFeedAbstract *feed ) {
        feeds.push_back( feed );
        feed->onRealTick()
    }
};
*/


#endif

