#include "QuantPeriodization.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffers.hh"
#include "QuantFeed.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

constexpr int PERIODIZATION_DEFAULT_SIZE = 50000;

class QuantPeriodization {
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
    QuantPeriodization (
        double period,
        QuantPeriodization &quant_period,
        int lookback = PERIODIZATION_DEFAULT_SIZE,
        QuantKeeperJar *the_jar = global_actives.active_jar
    );
    QuantPeriodization (
        double period,
        QuantFeed &quant_feed,
        int lookback = PERIODIZATION_DEFAULT_SIZE,
        QuantKeeperJar *the_jar = global_actives.active_jar
    );

    ~QuantPeriodization ();

    void commonInit ( float p_period );

    void setDateRange (
       QuantTime start_date,
       QuantTime end_date
    );

    void add ( QuantBufferAbstract &buffer );
    void add ( QuantBufferAbstract &buffer, int default_buf_size );

    SwiftSignal onBarClose;

    Str to_str ();

    QuantBufferSynchronizedHeap     buffer_heap;

    QuantBuffer<QuantTime,-1>       time;
    // *TODO*  time could perhaps be implemented as a calculated value instead!
    // We simply store session-breaks times - or a representation for that
    // too ( holidays are special exceptions )

    QuantBuffer<QuantReal,-1>       open;
    QuantBuffer<QuantReal,-1>       high;
    QuantBuffer<QuantReal,-1>       low;
    QuantBuffer<QuantReal,-1>       close;
    //QuantBuffer<QuantReal>      median;   // Use a TA for this!
    QuantBuffer<QuantUInt,-1>       tvolume;
    QuantBuffer<QuantReal,-1>       rvolume;
    QuantBuffer<QuantReal,-1>       spread;

    //QuantReal   *curr_open;   // Not needed - is set only once - on bar open
    QuantReal   *curr_high;
    QuantReal   *curr_low;
    QuantReal   *curr_close;
    QuantUInt   *curr_tvolume;
    QuantReal   *curr_rvolume;
    QuantReal   *curr_spread;

  private:
    // Methods
    void handleTick (
        QuantTime       ttime,
        QuantReal       topen,
        QuantReal       thigh,
        QuantReal       tlow,
        QuantReal       tclose,
        QuantUInt       ttvolume,
        QuantReal       trvolume,
        QuantReal       tspread
    );

    QuantKeeperJar      *the_jar;

    QuantFeed           *quant_feed;
    QuantPeriodization  *period_feed;

    QuantTime           likely_start_date;
    QuantTime           likely_end_date;
    QuantDuration       period;
    QuantDuration       epoch_offset = dt::min_date_time;       // Close the bars earlier (or later) than regular timing (midnight offset)
    QuantTime           next_epoch;
    QuantDuration       epoch_duration;
    QuantTime           next_bar_time = dt::min_date_time;

    int default_buf_size = 0;
    int total_bars = 0;      // total bars processed in running time, may be higher than actual cached bars because of buffer roll-out
    int cached_bars = 0;
    //int open_bar_is_pure_ghost;     // while only ghost ticks has formed the
    // bar - we are open to re-set it to first _real_ tick for open...
    // WE IGNORE this for now...

};

#endif


QuantPeriodization::QuantPeriodization (
    double period,
    QuantPeriodization &period_feed,
    int lookback,
    QuantKeeperJar *the_jar
)
:
    buffer_heap { lookback },
    the_jar { the_jar },
    period_feed { &period_feed },
    default_buf_size { lookback }
{
    commonInit( period );
    period_feed.onBarClose( [this] {
        //cerr << "Periodization::LAMBDA: src_period->onBarClose()\n";
        auto sp = this->period_feed;
        //cerr << "this->period: " << this->period << "\n";
        //cerr << "source_per->period: " << sp->period << "\n";
        //cerr << "sp->time->head_ptr: " << (unsigned int) sp->time.head_ptr << "\n";
        //cerr << "sp->close->head_ptr: " << (unsigned int) sp->close.head_ptr << "\n";
        //cerr << "sp close[-1], close[0]: " << sp->close[-1] << " " << sp->close[0] << "\n";

        this->handleTick(
            sp->time[0],
            sp->open[0],
            sp->high[0],
            sp->low[0],
            sp->close[0],
            sp->tvolume[0],
            sp->rvolume[0],
            sp->spread[0]
        );
    });
}

QuantPeriodization::QuantPeriodization (
    double period,
    QuantFeed &quant_feed,
    int lookback,
    QuantKeeperJar *the_jar
)
:
    buffer_heap { lookback },
    the_jar { the_jar },
    quant_feed { &quant_feed },
    default_buf_size { lookback }
{
    global_actives.active_periodization = this;

    commonInit( period );
    quant_feed.setRegulatedInterval( period );
    quant_feed.onRegulatedTick( [this] {
        //cerr << "onRegulatedTick lambda callback in Periodization\n";
        QuantTick & qt = this->quant_feed->ticks[0];
        //cerr << "Call handleTick with QuantTick-reference\n";
        this->handleTick(
            //qt.flags,
            qt.time,
            qt.ask,
            qt.ask,
            qt.ask,
            qt.ask,
            (qt.last_qty == 0 ? 0 : 1),                  // one tick is... 1 tick - unless ghost-tick.
            qt.last_qty,
            qt.ask - qt.bid
            //qt.swap_long,
            //qt.swap_short
        );
    });
}

QuantPeriodization::~QuantPeriodization ()
{
    cerr << "QuantPeriodization::~QuantPeriodization - - DESTRUCTOR - -\n";
}

void QuantPeriodization::commonInit ( float p_period ) {
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

void QuantPeriodization::setDateRange (
   QuantTime p_start_date,
   QuantTime p_end_date
) {
    likely_start_date = p_start_date;
    likely_end_date = p_end_date;
}

void QuantPeriodization::add ( QuantBufferAbstract &buffer ) {
    buffer_heap.add ( buffer );
}

void QuantPeriodization::add ( QuantBufferAbstract &buffer, int default_buf_size ) {
    buffer_heap.add ( buffer, default_buf_size );
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void QuantPeriodization::handleTick (
    QuantTime       ttime,
    QuantReal       topen,
    QuantReal       thigh,
    QuantReal       tlow,
    QuantReal       tclose,
    QuantUInt       ttvolume,
    QuantReal       trvolume,
    QuantReal       tspread
) {
    //cerr << "QuantPeriodization<" << period << ">::handleTick():  " << ttime;
    //cerr << " " << topen << " " << thigh << " " << tlow << " " << tclose << " " << ttvolume << " " << trvolume << " " << tspread << "\n";

    if ( ttime >= next_bar_time ) {
        cerr << "Periodization::addNewBar() - gonna advance heap - next_epoch is: " << next_epoch << " next_bar_time is: " << next_bar_time << "\n";

        if ( ttime - next_bar_time > period ) {
            cerr << "Missing bars since last - HOW LONG TIME? SESSION BREAK?" << "\n";

            //if ( next_bar_time == dt::min_date_time ) {
            //
            //}

        }

        buffer_heap.advance();
        cached_bars = MIN( open.capacity, ++total_bars );

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
        if ( cached_bars >= 3 ) {

            // *TODO* - one off error????

            onBarClose.emit();
        }


    } else {
        // It's just an update
        //flags[-1] =
        *curr_high = MAX( *curr_high, thigh );
        *curr_low = MIN( *curr_low, tlow );
        *curr_close = tclose;
        (*curr_tvolume) += tvolume;
        (*curr_rvolume) += rvolume;
        *curr_spread = MAX( *curr_spread, tspread );

    }

}

Str QuantPeriodization::to_str ()
{
    cerr << "OHLCTV:(" <<
        open[0] << ", " <<
        high[0] << ", " <<
        low[0] << ", " <<
        close[0] << ", " <<
        tvolume[0] << ", " <<
        rvolume[0]
        << ")";

    return Str("");
}
