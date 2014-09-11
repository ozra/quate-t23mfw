/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantPeriodization.hh"

#define PERIODIZATION_DEFAULT_SIZE 50000

void QuantPeriodization::commonInit () {
    int size = PERIODIZATION_DEFAULT_SIZE;

    time = buffer_heap.newTimeBuffer(size);
    //flags = buffer_heap.newFlagsBuffer(size);
    open = buffer_heap.newRealBuffer(size);
    high = buffer_heap.newRealBuffer(size);
    low = buffer_heap.newRealBuffer(size);
    close = buffer_heap.newRealBuffer(size);
    median = buffer_heap.newRealBuffer(size);
    tvolume = buffer_heap.newRealBuffer(size);
    rvolume = buffer_heap.newRealBuffer(size);
    spread = buffer_heap.newRealBuffer(size);
    //longswap = buffer_heap.newRealBuffer(size);
    //shortswap = buffer_heap.newRealBuffer(size);
}


QuantPeriodization::QuantPeriodization (
    int period_duration,
    QuantPeriodization *qu_per
)
  : period (period_duration),
    source_period (qu_per)
{
    commonInit();

    auto sp = source_period;

    //tick_listener =
    source_period->onBarClose.connect( [this, sp] () {
        this->handleTick(
            (*(sp->flags))(0),
            (*(sp->time))(0),
            (*(sp->open))(0),
            (*(sp->high))(0),
            (*(sp->low))(0),
            (*(sp->close))(0),
            (sp->use_median ? (*(sp->median))(0) : (*(sp->close))(0)),
            (*(sp->tvolume))(0),
            (*(sp->rvolume))(0),
            (*(sp->spread))[0]
            //(*(sp->longswap))[0],
            //(*(sp->shortswap))(0)
        );
    });
}

QuantPeriodization::QuantPeriodization (
    int period_duration,
    QuantFeed *qu_feed
)
  : period (period_duration),
    quant_feed (qu_feed)
{
    commonInit();



    // *TODO* this is killed when constructor is done, but is
    // referenced in the lambda!!

    auto qf = quant_feed;

    //tick_listener =
    quant_feed->onPreTick.connect( [this, qf] () {
        QuantTick & qt = qf->ticks[0];

        this->handleTick(
            qt.flags,
            qt.time,
            qt.bid,
            qt.bid,
            qt.bid,
            qt.bid,
            qt.bid,
            1.0,
            qt.last_qty,
            qt.ask - qt.bid
            //qt.swap_long,
            //qt.swap_short
        );
    });
}


QuantPeriodization::~QuantPeriodization ()
{
    tick_listener.disconnect();
}


void QuantPeriodization::closeLastBar () {
    QuantReal tmedian;

    if ( use_median ) {
        tmedian = 47474747; // TODO(ORC) - *TODO* ackumulator::median<QuantReal>( bar_ticks.median );
    } else {
        tmedian = 0;
    }

    median[0] = tmedian;

}


// This might be called several times in one _real_ tick because of faked cached-bars - in low tvolume markets, even though it's faked data, it's often desirable to maintain market geometry for analysis /ORC
void QuantPeriodization::addNewBar (
        QuantDataFlags in_flags,
        QuantTime in_time,
        QuantReal in_open,
        QuantReal in_high,
        QuantReal in_low,
        QuantReal in_close,
        QuantReal in_median,
        QuantReal in_tvolume,
        QuantReal in_rvolume,
        QuantReal in_spread
        //QuantReal in_long_swap,
        //QuantReal in_short_swap
) {
    if (cached_bars > 1) {
        closeLastBar();
        onBarClose();  //time, open, high, low, close, median, tvolume, rvolume, spread, flags);
    }

    ++total_bars;
    cached_bars = min(time->capacity, total_bars);

    buffer_heap.advance();

    last_bar_time += period;   // #-in-micros

    // Initiate new bar in the standard OHLC-buffers
    flags[0] = in_flags;
    time[0] = last_bar_time;
    open[0] = in_open;
    high[0] = in_high;
    low[0] = in_low;
    close[0] = in_close;
    median[0] = 0.0;            // will be updated upon bar closing - is CPU heavy
    tvolume[0] = in_tvolume;
    rvolume[0] = in_rvolume;
    spread[0] = in_spread;
    //longswap[0] = in_long_swap;
    //shortswap[0] = in_short_swap;

    //onBarOpen( *this );
    onBarOpen();

}

//void QuantPeriodization::handleTick (time, open, high, low, close, tvolume, rvolume, spread, flags) {
void QuantPeriodization::handleTick (
    QuantDataFlags  tflags,
    QuantTime       ttime,
    QuantReal       topen,
    QuantReal       thigh,
    QuantReal       tlow,
    QuantReal       tclose,
    QuantReal       tmedian,
    QuantReal       ttvolume,
    QuantReal       trvolume,
    QuantReal       tspread
    //QuantReal       tlongswap,
    //QuantReal       tshortswap
) {

    //# Interpolate / extrapolate / cheat when missing information

    // *TODO* handle scheduled session breaks (weekends on most forex markets, etc.)
    //    - simple solution: if gap > 50 minutes (for night breaks), actually, the threshold could be even smaller - like minutes





    //# new bar?
    if ( last_bar_time == 0 ) { //# very first tick and bar..

        //      # *TODO* flag for 'incomplete bar'

        /*
         *
         **TODO*
        last_bar_time = Math.floor(us-time / (1440 * 60));
        */

    }

    QuantTime time_delta = ttime - last_bar_time - period_in_micros;

    //# asynchronous while loop
    while ( time_delta >= period_in_micros ) {     // have we missed one or more cached-bars?!
        time_delta -= period_in_micros;
        last_bar_time += period;

        //if ( (ghostify_session_breaks && __time_is_out_of_sessions__) || use_ghostbars )
        //
        //   *TODO*  move this responsibility to the QuantFeed! The QuantFeed has the "contact" with the underlying data, either it fills out missing data or not.. (?) Naahhh... Hmmm... Arggg..
        if ( use_ghostbars ) {
            // # *TODO*
  /*
            calculate fake-times and shit!
            handle_new_bar(new_bar_time, fake_open, fake_high, fake_low, fake_close, fake_volume, fake_rvolume, fake_spread, fake_flags);
*/

        }
    }

    if (time_delta > 0) {
        addNewBar(tflags, ttime, topen, thigh, tlow, tclose, tmedian, ttvolume, trvolume, tspread); //, tlongswap, tshortswap);

    } else {    // update last, open, bar
/*
        high_buf 0, max high, high_buf 0
        low_buf 0, min low, low_buf 0
        close_buf 0, close
        tvolume_buf 0, tvolume + tvolume_buf 0
        rvolume_buf 0, rvolume + rvolume_buf 0    // tick count _ so this should be the same as + 1 if everythings what it should be...
        spread_buf 0, max spread, spread_buf 0
        // # *TODO* think about this logic:
        flag_buf 0, flags | flag_buf 0
        // #<-! amitter.amit 'bar-update', PER
*/
        onBarUpdate(); //*this );

    }

}
