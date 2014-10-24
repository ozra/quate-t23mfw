#include "QuantPeriodizationAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedHeap.hh"
#include "QuantBuffersSynchronizedBuffer.hh"
#include "QuantFeed.hh"

#include "HardSignal.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


/*
 *
 *
 *
 *

* LOOKUP DOXYGEN / SIMILAR BEST SUITED INLINE DOCUMENTATION
* REMOVE -1 ABILITY BUFFERS - ALL BUFFERS ARE "TO" 0
* ADD LOCAL VARS FOR futureBar / inProgressBar
* ADD METHODS:

    futureCompletion()
    futureTime()
    futureOpen()
    futureHigh()
    futureLow()
    futureClose()
    futureTickVolume()
    futureRealVolume()
    futureSpread()

OR

    progressingCompletion()
    progressingTime()
    progressingOpen()
    progressingHigh()
    progressingLow()
    progressingClose()
    progressingTickVolume()
    progressingRealVolume()
    progressingSpread()

 *
 *
 *
 *
*/





class QuantPeriodizationAbstract {
  public:

    /*
     * The minutes vs seconds issue.
     * If a duration in seconds is wanted - divide by ten - pass it as decimal
     * fraction.
     *
     * ie.:
     *
     * 0.5F = 5 seconds
     * 0.15F = 15 seconds
     * 0.05F = 0.5 seconds
     * 1.0F = 1 minute
     * 240.0F = 240 minutes
     *
     */
    QuantPeriodizationAbstract (
        double period,
        QuantPeriodizationAbstract &quant_period,
        int lookback,
        QuantKeeperJar *the_jar = global_actives.active_jar
    );
    QuantPeriodizationAbstract (
        double period,
        QuantFeedAbstract &quant_feed,
        int lookback,
        QuantKeeperJar *the_jar = global_actives.active_jar
    );

    ~QuantPeriodizationAbstract ();

    void commonInit ( float p_period );

    void setDateRange (
       QuantTime start_date,
       QuantTime end_date
    );

    void add ( QuantBufferAbstract &buffer );
    void add ( QuantBufferAbstract &buffer, int default_buf_size );

    #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
        #ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
            HardSignal<QuantPeriodizationAbstract,int> onBarClose_P;
        #endif
    #else
        SwiftSignal onBarClose;
    #endif

    Str to_str ();

    // // // - -  // // //
    // // // VARS // // //
    // // // - -  // // //
    QuantBufferSynchronizedHeap     buffer_heap;

    QuantBuffer<QuantTime>       time;
    // *TODO*  time could perhaps be implemented as a calculated value instead!
    // We simply store session-breaks times - or a representation for that
    // too ( holidays are special exceptions )
    QuantBuffer<QuantReal>       open;
    QuantBuffer<QuantReal>       high;
    QuantBuffer<QuantReal>       low;
    QuantBuffer<QuantReal>       close;
    //QuantBuffer<QuantUInt,QuantReal>       tvolume;
    QuantBuffer<QuantReal>       tvolume;    // For same-datatype concept!
    QuantBuffer<QuantReal>       ask_volume;
    QuantBuffer<QuantReal>       bid_volume;
    QuantBuffer<QuantReal>       spread;

/*
    //QuantReal   *curr_open;   // Not needed - is set only once - on bar open
    QuantReal   *curr_high;
    QuantReal   *curr_low;
    QuantReal   *curr_close;
    QuantReal   *curr_tvolume;  //QuantUInt   *curr_tvolume;
    QuantReal   *curr_rvolume;
    QuantReal   *curr_spread;
*/

    QuantTime   progressing_time;
    QuantReal   progressing_open;
    QuantReal   progressing_high;
    QuantReal   progressing_low;
    QuantReal   progressing_close;
    QuantReal   progressing_tvolume;  //QuantUInt   *progressing_tvolume;
    QuantReal   progressing_ask_volume;
    QuantReal   progressing_bid_volume;
    QuantReal   progressing_spread;


    virtual void emit_signal () = 0;

    /*
    inline double progressingCompletion () {
        // *TODO* calculate progressed time between progressing-time-start and next-per-time - based on last received tick
        // Return as 0.0 - 1.0
        return 0.4747474747;
    }
    */
    inline QuantTime progressingTime () {
        return progressing_time;
    }
    inline QuantReal progressingOpen () {
        return progressing_open;
    }
    inline QuantReal progressingHigh () {
        return progressing_high;
    }
    inline QuantReal progressingLow () {
        return progressing_low;
    }
    inline QuantReal progressingClose () {
        return progressing_close;
    }
    inline QuantReal progressingTickVolume () {
        return progressing_tvolume;
    }
    inline QuantReal progressingAskVolume () {
        return progressing_ask_volume;
    }
    inline QuantReal progressingBidVolume () {
        return progressing_bid_volume;
    }
    inline QuantReal progressingSpread () {
        return progressing_spread;
    }


    void handleFeedTick_HARD ( QuantFeedAbstract & feed ) {
        //cerr << "onRegulatedTick HAAARD lambda callback in Periodization\n";

        #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST
            #define THE_T feed.ticks()
        #else
            QuantTick &t = feed.ticks; // [0];
            #define THE_T t
        #endif

        //cerr << "Call handleTick with QuantTick-reference\n";
        handleTick(
            //qt.flags,
            THE_T.time,
            THE_T.ask,
            THE_T.ask,
            THE_T.ask,
            THE_T.ask,
            (THE_T.ask_volume == 0 && THE_T.bid_volume == 0 ? 0 : 1),                  // one tick is... 1 tick - unless ghost-tick.
            THE_T.ask_volume,
            THE_T.bid_volume,
            THE_T.ask - THE_T.bid
            //qt.swap_long,
            //qt.swap_short
        );
    };

    void handlePeriodBar_HARD ( int ) { // QuantPeriodizationAbstract & per ) {
        handleTick(
            period_feed->time,
            period_feed->open,
            period_feed->high,
            period_feed->low,
            period_feed->close,
            period_feed->tvolume,
            period_feed->ask_volume,
            period_feed->bid_volume,
            period_feed->spread
        );
    };

    inline int barCount () {
        return cached_bars;
    }

  private:
    // Methods
    inline void handleTick (
        QuantTime       ttime,
        QuantReal       topen,
        QuantReal       thigh,
        QuantReal       tlow,
        QuantReal       tclose,
        QuantReal       ttvolume,   //QuantUInt       ttvolume,
        QuantReal       task_volume,
        QuantReal       tbid_volume,
        QuantReal       tspread
    ) {
        //cerr << "QuantPeriodization<" << period << ">::handleTick():  " << ttime;
        //cerr << " " << topen << " " << thigh << " " << tlow << " " << tclose << " " << ttvolume << " " << trvolume << " " << tspread << "\n";

        if ( LIKELY( ttime < next_bar_time ) )   {
            // It's still an update to the open bar

            if ( thigh > progressing_high ) {
                progressing_high = thigh;
            }
            //progressing_high = MAX( progressing_high, thigh );
            if ( tlow < progressing_low) {
                progressing_low = tlow;
            }
            //progressing_low = MIN( progressing_low, tlow );
            progressing_close = tclose;
            progressing_tvolume += tvolume;
            progressing_ask_volume += task_volume;
            progressing_bid_volume += tbid_volume;

            if ( tspread > progressing_spread ) {
                progressing_spread = tspread;
            }

        } else {
            //cerr << "Periodization::addNewBar() - gonna advance heap - next_epoch is: " << next_epoch << " next_bar_time is: " << next_bar_time << "\n";

            if ( ttime - next_bar_time > period ) {
                cerr << "Missing bars since last - HOW LONG TIME? SESSION BREAK?" << "\n";

                //if ( next_bar_time == dt::min_date_time ) {
                //
                //}

            }

            // // Create a new closed bar to insert the currently open values
            buffer_heap.advance();

            time |= progressing_time;
            open |= progressing_open;
            high |= progressing_high;
            low |= progressing_low;
            close |= progressing_close;
            tvolume |= progressing_tvolume;
            ask_volume |= progressing_ask_volume;
            bid_volume |= progressing_bid_volume;
            spread |= progressing_spread;

            if ( cached_bars < capacity ) {
                ++cached_bars;
            }
            ++total_bars;

            // // Establish a new open bar
            progressing_time = next_bar_time;
            progressing_open = topen;
            progressing_high = thigh;
            progressing_low = tlow;
            progressing_close = tclose;
            progressing_tvolume = ttvolume;
            progressing_ask_volume = task_volume;
            progressing_bid_volume = tbid_volume;
            progressing_spread = tspread;

            // // // - Calculate time movement - // // //
            next_bar_time += period;



            // // //
            // *TODO* *INVESTIGATE* Look this logic over!
            // // //
            if ( next_bar_time > next_epoch ) {
                cerr << "\n\n - - - > Past an PERIODIZATION EPOCH! " << next_epoch << " -  " << next_bar_time << "\n\n";
                if ( next_bar_time < next_epoch + period ) {  // In case of uneven periodization units, the day-breaking one will be shortened so that the next begins on day break
                    cerr << "Had to adjust next_bar_time at periodization epoch crossing! " << next_bar_time << " -> " << next_epoch + period << "\n";
                    next_bar_time = next_epoch + period;
                }

                next_epoch += epoch_duration;
            }
            // // //
            // // //

            // // // - If we have at least one _closed_ bar - emit! // // //
            // *TODO* *INVESTIGATE* - one off error????
            if ( cached_bars >= 2 ) { // +1 for the open bar, +1 for closed bar
                emit_signal();
            }

        }

    }

    QuantKeeperJar              *the_jar;

    QuantFeedAbstract           *quant_feed;
    QuantPeriodizationAbstract  *period_feed;

    //QuantTime           likely_start_date;
    //QuantTime           likely_end_date;
    QuantDuration       period;
    QuantDuration       epoch_offset = dt::min_date_time;       // Close the bars earlier (or later) than regular timing (midnight offset)
    QuantTime           next_epoch;
    QuantDuration       epoch_duration;
    QuantTime           next_bar_time = dt::min_date_time;

    int default_buf_size = 0;
    int capacity = 0;
    int total_bars = 0;      // total bars processed in running time, may be higher than actual cached bars because of buffer roll-out
    int cached_bars = 0;
    //int open_bar_is_pure_ghost;     // while only ghost ticks has formed the
    // bar - we are open to re-set it to first _real_ tick for open...
    // WE IGNORE this for now...

};

#endif


QuantPeriodizationAbstract::QuantPeriodizationAbstract (
    double period,
    QuantPeriodizationAbstract &period_feed,
    int lookback,
    QuantKeeperJar *the_jar
)
:
    buffer_heap { lookback },
    the_jar { the_jar },
    period_feed { &period_feed },
    default_buf_size { lookback },
    capacity { lookback }
{
    commonInit( period );

    #ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
        #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
            period_feed.onBarClose_P( this, &QuantPeriodizationAbstract::handlePeriodBar_HARD );

        #else
            period_feed.onBarClose( [this] {
                //cerr << "Periodization::LAMBDA: src_period->onBarClose()\n";
                #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST
                    #define sp this->period_feed
                #else
                    auto s_per_feed = this->period_feed;
                    #define sp s_per_feed
                #endif
                //cerr << "this->period: " << this->period << "\n";
                //cerr << "source_per->period: " << sp->period << "\n";
                //cerr << "sp->time->head_ptr: " << (unsigned int) sp->time.head_ptr << "\n";
                //cerr << "sp->close->head_ptr: " << (unsigned int) sp->close.head_ptr << "\n";
                //cerr << "sp close[-1], close[0]: " << sp->close[-1] << " " << sp->close[0] << "\n";


                this->handleTick(
                    sp->time,
                    sp->open,
                    sp->high,
                    sp->low,
                    sp->close,
                    sp->tvolume,
                    sp->rvolume,
                    sp->spread
                );
            });
        #endif
    #endif
}

QuantPeriodizationAbstract::QuantPeriodizationAbstract (
    double period,
    QuantFeedAbstract &quant_feed,
    int lookback,
    QuantKeeperJar *the_jar
)
:
    buffer_heap { lookback },
    the_jar { the_jar },
    quant_feed { &quant_feed },
    default_buf_size { lookback },
    capacity { lookback }
{
    global_actives.active_periodization = this;

    commonInit( period );
    quant_feed.setRegulatedInterval( period );



    // Simply switching out the generalized BOOST_FUNCTION ptrs using lambdas
    // and replacing them with HARDSIGNALS - solely for ticks-listening (not
    // periodical-hard-emitting turned on) increases performance by 15% (!!!)

    #ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS

        #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_FEEDS
            quant_feed.onRegulatedTick_P( this, &QuantPeriodizationAbstract::handleFeedTick_HARD );

        #else
            quant_feed.onRegulatedTick( [this] {
                //cerr << "onRegulatedTick lambda callback in Periodization\n";

                #ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST
                    #define THE_T this->quant_feed->ticks()
                #else
                    QuantTick & qt = this->quant_feed->ticks;
                    #define THE_T qt
                #endif
                //cerr << "Call handleTick with QuantTick-reference\n";
                this->handleTick(
                    //qt.flags,
                    THE_T.time,
                    THE_T.ask,
                    THE_T.ask,
                    THE_T.ask,
                    THE_T.ask,
                    (THE_T.ask_volume == 0 && THE_T.bid_volume == 0) ? 0 : 1),                  // one tick is... 1 tick - unless ghost-tick.
                    THE_T.ask_volume,
                    THE_T.bid_volume,
                    THE_T.ask - THE_T.bid
                    //qt.swap_long,
                    //qt.swap_short
                );
            });
        #endif
    #endif
}

QuantPeriodizationAbstract::~QuantPeriodizationAbstract ()
{
    cerr << "QuantPeriodization::~QuantPeriodization - - DESTRUCTOR - -\n";
}

void QuantPeriodizationAbstract::commonInit ( float p_period ) {
    the_jar->add( this );

    //int default_buf_size = PERIODIZATION_DEFAULT_SIZE;

    if ( p_period >= 1) {
        period = pxt::minutes( p_period );  // Passed in minutes, store as seconds
    } else {
        period = pxt::seconds( p_period * 100 );  // From the notation-value of decimal to integer seconds
    }

    next_bar_time = global_actives.run_context->start_date; //dt::min_date_time;
    next_epoch = global_actives.run_context->start_date;

    if ( period <= pxt::hours( 24 ) ) {
        epoch_duration = pxt::hours( 24 );
    } else {
        epoch_duration = pxt::hours( 7 * 24 );
        // *TODO* set next_epoch to a sunday (beginning of trade-week)!
    }

    cerr << "p_period = " << p_period << " period = " << period << " seconds of that: " << period.seconds() << " epoch_duration = " << epoch_duration.hours() << "\n";

    /*
    buffer_heap.add( time, default_buf_size );  // Would be nice to be able to make this one computional - but - there are special circumstances like holiday etc...
    buffer_heap.add( open, default_buf_size );
    buffer_heap.add( high, default_buf_size );
    buffer_heap.add( low, default_buf_size );
    buffer_heap.add( close, default_buf_size );
    buffer_heap.add( tvolume, default_buf_size );
    buffer_heap.add( rvolume, default_buf_size );
    buffer_heap.add( spread, default_buf_size );
    */

    global_actives.active_buffer_heap = &buffer_heap;

}

//void QuantPeriodizationAbstract::setDateRange (
//   QuantTime p_start_date,
//   QuantTime p_end_date
//) {
//    likely_start_date = p_start_date;
//    likely_end_date = p_end_date;
//}

void QuantPeriodizationAbstract::add ( QuantBufferAbstract &buffer ) {
    buffer_heap.add ( buffer );
}

void QuantPeriodizationAbstract::add ( QuantBufferAbstract &buffer, int default_buf_size ) {
    buffer_heap.add ( buffer, default_buf_size );
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

Str QuantPeriodizationAbstract::to_str ()
{
    cerr << "OHLCTV:(" <<
        open << ", " <<
        high << ", " <<
        low << ", " <<
        close << ", " <<
        tvolume << ", " <<
        ask_volume  << ", " <<
        bid_volume << "" <<
        ")";

    return Str("");
}
