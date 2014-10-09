#include "QuantFeed.hh"

#ifdef INTERFACE
/**
* Created:  2014-08-11
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//#include "boost/circular_buffer.hpp"

#include "SwiftSignal.hh"

#include "QuantBase.hh"
#include "QuantStudyContextAbstract.hh"
#include "QuantBuffers.hh"
#include "QuantTick.hh"

#include "QuantSequentialData.hh"
#include "QuantSequentialData_DukascopyTicks.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


// We will never really need much back-looking on ticks, more than for seconds-worth of chase-operations..  - 2014-08-29/ORC(01:53)
#define TICKS_BUF_LEN 5000


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
    QuantFeed (
        QuantStudyContextAbstract *context,
        Str broker_id,
        Str symbol_id
    );

    QuantFeed (
        Str broker_id,
        Str symbol_id,
        QuantKeeperJar *the_jar = global_actives.active_jar
    );

    ~QuantFeed ();

    void setRegulatedInterval ( float p_regulated_interval );
    void setDateRange (
        QuantTime start_date,
        QuantTime end_date
    );
    void setLiveMode ();

    bool readNext ();
    void emit ();    // When in back-testing mode, this is called 'manually' by the mainloop the keeps track of which feed is next in line

    SwiftSignal     regulatedTickSignal;
    SwiftSignal     onRealTick;

    void            onRegulatedTick ( boost::function<void()> );
    void            onRegulatedTick ( float, boost::function<void()> );

    ReversedCircularStructBuffer<QuantTick> ticks;
    QuantTick       buffered_tick;
    bool            has_buffered = false;

    QuantSequentialData_DukascopyTicks *sequential_data_interface;

  private:
    void                        handleData ();
    QuantStudyContextAbstract   *context;
    Str                         broker_id;
    Str                         symbol_id;

    FeedState           state = UNDEF;
    QuantTime           start_date;
    QuantTime           end_date;
    QuantTime           next_tick_time;
    QuantDuration       regulated_interval = pxt::hours( 999999 ); // *TODO* --> numbers<QuantTime>.max_value;
    int                 day_epoch_offset = 0;

};

#endif

QuantFeed::QuantFeed (
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

QuantFeed::QuantFeed (
    Str broker_id,
    Str symbol_id,
    QuantKeeperJar *the_jar
)
: ticks( 5000 )
, broker_id( broker_id )
, symbol_id( symbol_id )
{
    global_actives.active_feed = this;
    sequential_data_interface = new QuantSequentialData_DukascopyTicks();
    the_jar->add( this );
}




QuantFeed::~QuantFeed ()
{
    cerr << "QuantFeed::~QuantFeed - - DESTRUCTOR - -" << "\n";
    //delete []ticks;
}

void QuantFeed::setDateRange (
   QuantTime p_start_date,
   QuantTime p_end_date
) {
    start_date = p_start_date;
    end_date = p_end_date;

    next_tick_time = start_date;

    // *TODO* PROPER PLACEMENT!
    sequential_data_interface->init(symbol_id, 0.00001, start_date, end_date);
}

void QuantFeed::setRegulatedInterval ( float p_regulated_interval )
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

void QuantFeed::onRegulatedTick ( float p_regulated_interval, boost::function<void()> callback )
{
    setRegulatedInterval( p_regulated_interval );
    regulatedTickSignal.connect( callback );
}

void QuantFeed::onRegulatedTick ( boost::function<void()> callback )
{
    regulatedTickSignal.connect( callback );
}

// In event of asynchronous live data
void QuantFeed::handleData ()
{
    // *TODO* *IMPL*

}

bool QuantFeed::readNext ()
{
    //cerr << "readNext() - Advance ticks" << "\n";
    QuantTick *tick = &ticks.advance();
    //cerr << "readTick from seq-interface" << "\n";

    if ( has_buffered ) {
        //cerr << "b";
        if ( buffered_tick.time > next_tick_time ) {
            *tick = ticks[1];
            cerr << "Stacks up a ghost tick " << "\n";
            tick->last_qty = 0; // Ghost tick..
            tick->time = next_tick_time;
            next_tick_time += regulated_interval;

        } else {
            cerr << "Just GOT OUT OF GhostTickin" << "\n";
            *tick = buffered_tick;
            has_buffered = false;
        }
        return true;

    } else {
        //cerr << "r";
        bool ret = sequential_data_interface->readTick( *tick );

        if ( ret == false) {
            cerr << "seq-data-iface returned false" << "\n";
            return false;
        }

        if ( tick->time > next_tick_time && ticks.size > 1 ) {
            cerr << "Just GOT IN TO GhostTickin" << "\n";
            buffered_tick = *tick;
            *tick = ticks[1];
            tick->last_qty = 0; // Ghost tick..
            tick->time = next_tick_time;
            has_buffered = true;
            next_tick_time += regulated_interval;
        }
        return true;
    }

    //cerr << "readNext is done." << "\n";
    return false;
}

void QuantFeed::emit ()
{
    //cerr << "QuantFeed::emit() - regulatedTickSignal" << "\n";
    regulatedTickSignal.emit();

    if ( ticks[0].isGhostTick() == false ) {
        //cerr << "QuantFeed::emit() realtick" << "\n";
        onRealTick.emit();
    }
}

void QuantFeed::setLiveMode ()
{
    state = LIVE;
    // Do some other shit like setting up listeners etc. (should've been done earlier probably though) *TODO*
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#ifdef INTERFACE

/*
class QuantFeedAggregator {

    add ( QuantFeedAbstract *feed ) {
        feeds.push_back( feed );
        feed->onRealTick()
    }
};
*/


#endif



/*
 *
long long fakeData[COUNT] = {
    { 1410306056861, 135000, 135004 },
    { 1410306056872, 135003, 135007 },
    { 1410306059003, 135108, 135111 }
};

*/
