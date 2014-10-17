#include "QuantPeriodizationAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffers.hh"
#include "QuantFeed.hh"

#include "HardSignal.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


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

    QuantBuffer<QuantTime,-1>       time;
    // *TODO*  time could perhaps be implemented as a calculated value instead!
    // We simply store session-breaks times - or a representation for that
    // too ( holidays are special exceptions )
    QuantBuffer<QuantReal,-1>       open;
    QuantBuffer<QuantReal,-1>       high;
    QuantBuffer<QuantReal,-1>       low;
    QuantBuffer<QuantReal,-1>       close;
    //QuantBuffer<QuantUInt,-1>       tvolume;
    QuantBuffer<QuantReal,-1>       tvolume;    // For same-datatype concept!
    QuantBuffer<QuantReal,-1>       rvolume;
    QuantBuffer<QuantReal,-1>       spread;

    //QuantReal   *curr_open;   // Not needed - is set only once - on bar open
    QuantReal   *curr_high;
    QuantReal   *curr_low;
    QuantReal   *curr_close;
    QuantReal   *curr_tvolume;  //QuantUInt   *curr_tvolume;
    QuantReal   *curr_rvolume;
    QuantReal   *curr_spread;


    virtual void emit_signal() = 0;

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
            (THE_T.volume == 0 ? 0 : 1),                  // one tick is... 1 tick - unless ghost-tick.
            THE_T.volume,
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
            period_feed->rvolume,
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
        QuantReal       trvolume,
        QuantReal       tspread
    ) {
        //cerr << "QuantPeriodization<" << period << ">::handleTick():  " << ttime;
        //cerr << " " << topen << " " << thigh << " " << tlow << " " << tclose << " " << ttvolume << " " << trvolume << " " << tspread << "\n";

        if ( UNLIKELY( ttime >= next_bar_time ) )   {
            //cerr << "Periodization::addNewBar() - gonna advance heap - next_epoch is: " << next_epoch << " next_bar_time is: " << next_bar_time << "\n";

            if ( ttime - next_bar_time > period ) {
                cerr << "Missing bars since last - HOW LONG TIME? SESSION BREAK?" << "\n";

                //if ( next_bar_time == dt::min_date_time ) {
                //
                //}

            }

            buffer_heap.advance();

            if ( cached_bars < capacity ) {
                ++cached_bars;
            }
            ++total_bars;
            //cached_bars = MIN( open.capacity, ++total_bars );

            //if ( cached_bars > 0 ) {
            //    CLOSE LAST BAR  - IS ALREADY TRUE SINCE WE SKIPPED MEDIAN...
            //}


            // // // - Open a new bar - // // //
            //flags[-1] = tflags;
            // Get pointers to all values we will be updating till the bar closes
            // *TODO* perhaps just store these last values in VARS and refer to
            // them as special case when [-1] is used - and set [0] at bar close?

            time[-1] = next_bar_time;
            open[-1] = topen;
            curr_high = &(high[-1]);
            *curr_high = thigh;
            curr_low = &(low[-1]);
            *curr_low = tlow;
            curr_close = &(close[-1]);
            *curr_close = tclose;
            curr_tvolume = &(tvolume[-1]);
            *curr_tvolume = ttvolume;
            curr_rvolume = &(rvolume[-1]);
            *curr_rvolume = trvolume;
            curr_spread = &(spread[-1]);
            *curr_spread = tspread;

            // // // - Calculate time movement - // // //
            next_bar_time += period;
            if ( next_bar_time > next_epoch ) {
                cerr << " - - - > Past an PERIODIZATION EPOCH! " << next_epoch << "\n";
                if ( next_bar_time < next_epoch + period ) {  // In case of uneven periodization units, the day-breaking one will be shortened so that the next begins on day break
                    cerr << "Had to adjust next_bar_time at periodization epoch crossing! " << next_bar_time << " -> " << next_epoch + period << "\n";
                    next_bar_time = next_epoch + period;
                }

                next_epoch += epoch_duration;
            }

            // // // - If we have at least one _closed_ bar - emit! // // //
            if ( cached_bars >= 2 ) { // +1 for the open bar, +1 for closed bar
                // *TODO* - one off error????

                emit_signal();
                //onBarClose.emit();
            }


        } else {
            // It's just an update
            //flags[-1] =
            if ( *curr_high < thigh )
                *curr_high = thigh;
            //*curr_high = MAX( *curr_high, thigh );
            if ( *curr_low > tlow )
                *curr_low = tlow;
            //*curr_low = MIN( *curr_low, tlow );
            *curr_close = tclose;
            (*curr_tvolume) += tvolume;
            (*curr_rvolume) += rvolume;

            if ( *curr_spread < tspread )
                *curr_spread = tspread;
            //*curr_spread = MAX( *curr_spread, tspread );

        }

    }

    QuantKeeperJar      *the_jar;

    QuantFeedAbstract   *quant_feed;
    QuantPeriodizationAbstract  *period_feed;

    QuantTime           likely_start_date;
    QuantTime           likely_end_date;
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
                    (THE_T.volume == 0 ? 0 : 1),                  // one tick is... 1 tick - unless ghost-tick.
                    THE_T.volume,
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

void QuantPeriodizationAbstract::setDateRange (
   QuantTime p_start_date,
   QuantTime p_end_date
) {
    likely_start_date = p_start_date;
    likely_end_date = p_end_date;
}

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
        rvolume
        << ")";

    return Str("");
}
