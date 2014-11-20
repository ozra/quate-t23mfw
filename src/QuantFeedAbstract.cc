#include "QuantFeedAbstract.hh"

#ifdef INTERFACE
/**
* Created:  2014-08-11
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//#include "boost/circular_buffer.hpp"

#include "QuantBasic_DESIGN_CHOICES.hh"

#include "SwiftSignal.hh"
#include "HardSignal.hh"

#include "QuantBase.hh"
#include "QuantStudyContextAbstract.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedHeap.hh"
#include "QuantBuffersSynchronizedBufferAbstract.hh"
#include "QuantBuffersReverseIndexedCircular.hh"
#include "QuantTick.hh"

//#include "QuantSequentialData.hh"
#include "QuantSequentialData_TradeTicksAbstract.hh"
#include "QuantSequentialData_DukascopyTicks.hh"
#include "QuantSequentialData_T23MFWTicks.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #

// We will never really need much back-looking on ticks, more than for
// seconds-worth of chase-operations..  - 2014-08-29/ORC(01:53)
//#define TICKS_BUF_LEN 5000

enum FeedState { UNDEF = 0, HISTORICAL, LIVE };

/*
class QuantFeedAbstract {
};
*/

class QuantFeedAbstract {
   public:
    /*
    QuantFeedAbstract (
        QuantStudyContextAbstract *context,
        S broker_id,
        S symbol_id
    );
     */

    QuantFeedAbstract(S broker_id, S symbol_id, Z lookback = 0,
                      QuantMultiKeeperJar* the_jar = global_actives.active_jar);

    ~QuantFeedAbstract();

    void setRegulatedInterval(R p_regulated_interval);
    void setDateRange(QuantTime start_date, QuantTime end_date);
    void setLiveMode();

    virtual L readNext();
    virtual void emit() = 0;

#ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_FEEDS
    HardSignal<QuantPeriodizationAbstract, QuantFeedAbstract&>
    onRegulatedTick_P;
#else
    SwiftSignal onRegulatedTick;
    SwiftSignal onRealTick;

// void            onRegulatedTick ( boost::function<void()> );
// void            onRegulatedTick ( float, boost::function<void()> );
#endif

    ReversedCircularStructBuffer<QuantTick> ticks;
    QuantTick buffered_tick;
    L has_buffered = false;

    /*
    struct {
        inline operator [] ( Z ix ) {
            return ticks[ix].ask;
        }
    } ask;
    */

    // QuantSequentialData_DukascopyTicks *sequential_data_interface;
    QuantSequentialData_TradeTicksAbstract* sequential_data_interface;

#ifdef IS_DEBUG
    inline void reset_debug_counts() {
        real_tick_count = 0;
        ghost_tick_count = 0;
    }

    Z real_tick_count = 0;
    Z ghost_tick_count = 0;

#endif

   private:
    void handleData();
    // QuantStudyContextAbstract   *context;
    QuantMultiKeeperJar* the_jar;
    S broker_id;
    S symbol_id;

    Z lookback;
    FeedState state = UNDEF;
    QuantTime start_date;
    QuantTime end_date;
    QuantTime next_regulated_tick_time;
    QuantDuration regulated_interval =
        pxt::hours(999999); // pxt::max_date_time; // *TODO* -->
                            // numbers<QuantTime>.max_value;
    // Z                 day_epoch_offset = 0;

    L do_debug_the_time = false;
};

inline void debug_a_tick(QuantTick& tick) {
    cerr << tick.time << " " << tick.ask << " " << tick.bid << " "
         << tick.ask_volume << " " << tick.bid_volume << " "
         << "\n";
}

#endif

/*
QuantFeedAbstract::QuantFeedAbstract (
    QuantStudyContextAbstract *context,
    S broker_id,
    S symbol_id
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

QuantFeedAbstract::QuantFeedAbstract(S broker_id, S symbol_id, Z lookback,
                                     QuantMultiKeeperJar* the_jar)
    : ticks(lookback + 2)
    , // +1 for current tick, +1 for prev. tick
    the_jar{ the_jar }
    , broker_id{ broker_id }
    , symbol_id{ symbol_id }
    , lookback{ lookback } {
    global_actives.active_feed = this; // *TODO* pass the "global_actives" as
                                       // param (could then ofcourse be set as
                                       // default param) 141013/ORC

    // *TODO* this has NOTHING to do in QuantFeedAbstract!!!
    if (broker_id == "DUKASCOPY") {
        sequential_data_interface = new QuantSequentialData_T23MFWTicks();

    } else if (broker_id == "DUKASCOPY_RAW") {
        sequential_data_interface = new QuantSequentialData_DukascopyTicks();

    } else {
        sequential_data_interface = new QuantSequentialData_T23MFWTicks();
    }

    the_jar->add(this);
}

QuantFeedAbstract::~QuantFeedAbstract() {
    cerr << "QuantFeed::~QuantFeed - - DESTRUCTOR - -"
         << "\n";
}

/*
QuantFeedAbstract::setDateRange  ::=    QuantTime QuantTime -> void
QuantFeedAbstract::setDateRange         p_start_date p_end_date =
    start_date = p_start_date
    end_date = p_end_date

    next_regulated_tick_time = start_date

*/
auto QuantFeedAbstract::setDateRange(QuantTime p_start_date,
                                     QuantTime p_end_date) -> void {
    start_date = p_start_date;
    end_date = p_end_date;

    next_regulated_tick_time = start_date;

    // *TODO* PROPER PLACEMENT!
    sequential_data_interface->setDateRange(start_date, end_date);
    sequential_data_interface->init(broker_id, symbol_id);

    /*
        if ( broker_id == "DUKASCOPY" ) {
            ( (QuantSequentialData_DukascopyTicks *)
       sequential_data_interface)->init( symbol_id );

        } else if ( broker_id == "DUKASCOPY_RAW" ) {
            ( (QuantSequentialData_DukascopyTicks *)
       sequential_data_interface)->init( symbol_id );

        } else {
            ( (QuantSequentialData_DukascopyTicks *)
       sequential_data_interface)->init( symbol_id );
        }
    */
}

void QuantFeedAbstract::setRegulatedInterval(R p_regulated_interval) {
    // *TODO* *DUPE* *DRY* ( QuantFeed : commonInit() )
    if (p_regulated_interval >= 1) {
        p_regulated_interval =
            (p_regulated_interval * 60); // Passed in minutes, store as seconds
    } else {
        p_regulated_interval =
            (p_regulated_interval *
             100); // From the notation-value of decimal to integer seconds
    }

    QuantDuration tmp_interval = pxt::seconds(p_regulated_interval);
    if (tmp_interval <
        regulated_interval) { // 141104/ORC - was gt but should be lt (fixed)
        regulated_interval = tmp_interval;
    }
}

/*
void QuantFeed::onRegulatedTick ( R32 p_regulated_interval,
boost::function<void()> callback )
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

L QuantFeedAbstract::readNext() {
    // cerr << "readNext() - Advance ticks" << "\n";

    if (LIKELY(has_buffered == false)) {
#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
        QuantTick& tick = ticks.advance();
#else
        QuantTick* tick = &ticks.advance();
#endif

// cerr << "r";
#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
        L ret = sequential_data_interface->readTick(tick);
#else
        L ret = sequential_data_interface->readTick(*tick);
#endif

#ifdef IS_DEEPBUG
        if (!do_debug_the_time) {
            if (ticks.size > 1 &&
                ticks[0].time < ticks[1].time) { // pxt::ptime( dt::date( 2014,
                                                 // 3, 20 ), pxt::hours( 0 ) ) )
                                                 // {
                // if ( tick.time > pxt::ptime( dt::date( 2014, 3, 20 ),
                // pxt::hours( 0 ) ) ) {
                do_debug_the_time = true;
                cerr << "\n\n\n\nWORM SNOT FUCK!\n\n\n";

                cerr << "Last in buffer for " << symbol_id << " (" << ticks.size
                     << "): "
                     << "\n";

                for (Z ix = ticks.size - 1; ix >= 0; --ix) {
                    debug_a_tick(ticks[ix]);
                }

                throw 474747;
            }

        } else {
            cerr << "readTick from seq-interface: " << symbol_id << ": ";
#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
            debug_a_tick(tick);
#else
            debug_a_tick(*tick);
#endif
        }

        if (ret == false) {
            cerr << "\n\nseq-data-iface returned false"
                 << "\n\n";
            return false;
        }
#endif

#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
        if (tick.time <= next_regulated_tick_time || ticks.size <= 1) {
#else
        if (tick->time <= next_regulated_tick_time || ticks.size <= 1) {
#endif
#ifdef IS_DEBUG
            ++real_tick_count;
#endif
            return true;

        } else {
// cerr << "Just GOT IN TO GhostTickin" << "\n";
#ifdef IS_DEBUG
            ++ghost_tick_count;
#endif

// *TODO* test if making specific field copies (Since not all are
// used) is faster - and done in correct sequence of course

#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
            const QuantTick& prev = ticks[1];
            buffered_tick = tick;
            tick.time = next_regulated_tick_time;
            tick.ask = prev.ask;
            tick.bid = prev.bid;
            tick.last_price = prev.last_price;
            tick.ask_volume = 0; // Ghost tick..
            tick.bid_volume = 0; // Ghost tick..
            has_buffered = true;
            next_regulated_tick_time += regulated_interval;
#else
            const QuantTick& prev = ticks[1];
            buffered_tick = tick;
            tick->time = next_regulated_tick_time;
            tick->ask = prev.ask;
            tick->bid = prev.bid;
            tick->last_price = prev.last_price;
            tick->ask_volume = 0; // Ghost tick..
            tick->bid_volume = 0; // Ghost tick..
            has_buffered = true;
            next_regulated_tick_time += regulated_interval;

#endif
            return true;
        }

    } else {
        // Ghost ticks will be overwritten again and again until a real tick
        // is written in to its position - then we advance the next time, and
        // put a new temporary ghost tick if needed, or the next real tick.
        // ( Rationale: when looking back in ticks buffer - we're not
        // interested in ghost ticks generated only for the sake of trigging
        // periodicals in time in slow markets.
        // etc. 141104/ORC

        // cerr << "b";
        if (buffered_tick.time <= next_regulated_tick_time) {
#ifdef IS_DEBUG
            ++real_tick_count;
#endif

            // cerr << "Just GOT OUT OF GhostTickin" << "\n";
            //#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
            ticks.set(buffered_tick);
            //#else
            //    ticks[0] = buffered_tick;
            //#endif
            has_buffered = false;
            return true;

        } else {
#ifdef IS_DEBUG
            ++ghost_tick_count;
#endif

#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
            auto& tick = ticks();
#else
            auto* tick = &ticks();
#endif

#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3
            const QuantTick& prev = ticks[1];
            tick.time = next_regulated_tick_time;
            tick.ask = prev.ask;
            tick.bid = prev.bid;
            tick.last_price = prev.last_price;
            tick.ask_volume = 0; // Ghost tick..
            tick.bid_volume = 0; // Ghost tick..
#else
            tick->time = next_regulated_tick_time;
            tick->ask = ticks[1].ask;
            tick->bid = ticks[1].bid;
            tick->last_price = ticks[1].last_price;
            tick->ask_volume = 0; // Ghost tick..
            tick->bid_volume = 0; // Ghost tick..
#endif

            next_regulated_tick_time += regulated_interval;
            return true;
        }
    }

    // cerr << "readNext is done." << "\n";
    return false;
}

// In event of asynchronous live data
void QuantFeedAbstract::handleData() {
    // *TODO* *IMPL*
}

void QuantFeedAbstract::setLiveMode() {
    state = LIVE;
    // Do some other shit like setting up listeners etc. (should've been done
    // earlier probably though) *TODO*
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #

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
