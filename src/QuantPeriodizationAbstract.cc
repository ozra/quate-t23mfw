#include "QuantPeriodizationAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantTime.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedHeap.hh"
#include "QuantBuffersSynchronizedBuffer.hh"
#include "QuantFeed.hh"

#include "HardSignal.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #

/*
 *
 *
 *
 *

* LOOKUP DOXYGEN / SIMILAR BEST SUITED INLINE DOCUMENTATION

 */

typedef QuantReal qreal;
typedef QuantTime quts;

class QuantPeriodizationAbstract {
   public:
    /*
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
    QuantPeriodizationAbstract(
        R period, QuantPeriodizationAbstract& quant_period, N lookback,
        QuantMultiKeeperJar* the_jar = global_actives.active_jar);
    QuantPeriodizationAbstract(
        R period, QuantFeedAbstract& quant_feed, N lookback,
        QuantMultiKeeperJar* the_jar = global_actives.active_jar);

    ~QuantPeriodizationAbstract();

    void commonInit(R p_period);

    void setDateRange(QuantTime start_date, QuantTime end_date);

    void add(QuantBufferAbstract& buffer);
    void add(QuantBufferAbstract& buffer, size_t default_buf_size);

#ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
/*
#ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
    HardSignal<QuantPeriodizationAbstract,int> onBarClose_P;
#endif
*/
#else
    SwiftSignal onBarClose;
#endif

    Str to_str();

    // // // - -  // // //
    // // // VARS // // //
    // // // - -  // // //
    QuantBufferSynchronizedHeap buffer_heap;

    // *TODO*  time could perhaps be implemented as a calculated value instead!
    // We simply store session-breaks times - or a representation for that
    // too ( holidays are special exceptions )
    QuantBuffer<QuantTime> time;
    QuantBuffer<QuantReal> open;
    QuantBuffer<QuantReal> high;
    QuantBuffer<QuantReal> low;
    QuantBuffer<QuantReal> close;
    QuantBuffer<QuantReal> tick_volume; // For same-datatype concept!
                                        // //QuantBuffer<QuantUInt,QuantReal>
                                        // tvolume;
    QuantBuffer<QuantReal> ask_volume;
    QuantBuffer<QuantReal> bid_volume;
    QuantBuffer<QuantReal> spread;

    QuantTime progressing_time;
    QuantReal progressing_open;
    QuantReal progressing_high;
    QuantReal progressing_low;
    QuantReal progressing_close;
    QuantReal progressing_tick_volume; // QuantUInt   *progressing_tick_volume;
    QuantReal progressing_ask_volume;
    QuantReal progressing_bid_volume;
    QuantReal progressing_spread;

    virtual void emit_signal() = 0;

    /*
    inline R progressingCompletion () {
        // *TODO* calculate progressed time between progressing-time-start and
    next-per-time - based on last received tick
        // Return as 0.0 - 1.0
        return 0.4747474747;
    }
    */
    inline quts progressingTime() { return progressing_time; }
    inline qreal progressingOpen() { return progressing_open; }
    inline qreal progressingHigh() { return progressing_high; }
    inline qreal progressingLow() { return progressing_low; }
    inline qreal progressingClose() { return progressing_close; }
    inline qreal progressingTickVolume() { return progressing_tick_volume; }
    inline qreal progressingAskVolume() { return progressing_ask_volume; }
    inline qreal progressingBidVolume() { return progressing_bid_volume; }
    inline qreal progressingSpread() { return progressing_spread; }

    inline void accumulateFromFeedTick_HARD(QuantFeedAbstract& feed) {
// cerr << "onRegulatedTick HAAARD lambda callback in Periodization\n";

#ifdef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST
#define THE_T feed.ticks()
#else
        QuantTick& t = feed.ticks; // [0];
#define THE_T t
#endif

        // cerr << "Call handleTick with QuantTick-reference\n";
        ackumulateTick(
            // qt.flags,
            THE_T.time, THE_T.ask, THE_T.ask, THE_T.ask, THE_T.ask,
            (THE_T.ask_volume == 0 && THE_T.bid_volume == 0
                 ? 0
                 : 1), // one tick is... 1 tick - unless ghost-tick.
            THE_T.ask_volume,
            THE_T.bid_volume, THE_T.ask - THE_T.bid);
    };

    /*  *TODO* ofcourse periodization should be passed!  */
    inline void
    accumulateFromSourceCandle_HARD() { // QuantPeriodizationAbstract & per ) {
        ackumulateTick(period_feed->time, period_feed->open, period_feed->high,
                       period_feed->low, period_feed->close,
                       period_feed->tick_volume, period_feed->ask_volume,
                       period_feed->bid_volume, period_feed->spread);
    };

    /*
     * *TODO* *BUBBLARE*
     */
    inline N barCount() { return cached_bars; }
    inline N candleCount() { return cached_bars; }
    inline N count() { return cached_bars; }
    /*
     *
     */

    inline const QuantDuration getPeriod() { return period; }

   private:
    // Methods

    inline auto initOpenCandle(
        /*quts p_time, */
        qreal p_open, qreal p_high, qreal p_low, qreal p_close,
        qreal p_tick_vol, qreal p_ask_vol, qreal p_bid_vol,
        qreal p_spread) -> void {
        progressing_time = next_bar_time;
        progressing_open = p_open;
        progressing_high = p_high;
        progressing_low = p_low;
        progressing_close = p_close;
        progressing_tick_volume = p_tick_vol;
        progressing_ask_volume = p_ask_vol;
        progressing_bid_volume = p_bid_vol;
        progressing_spread = p_spread;
    }

    inline auto updateProgressingCandle(
        /* quts p_time, */
        /* qreal p_open, */ qreal p_high, qreal p_low, qreal p_close,
        qreal p_tick_vol, qreal p_ask_vol, qreal p_bid_vol,
        qreal p_spread) -> void {
        // progressing_high = p_high  if p_high > progressing_high
        if (p_high > progressing_high) {
            progressing_high = p_high;
        }
        if (p_low < progressing_low) {
            progressing_low = p_low;
        }
        progressing_close = p_close;
        progressing_tick_volume += p_tick_vol;
        progressing_ask_volume += p_ask_vol;
        progressing_bid_volume += p_bid_vol;

        // progressing_spread = p_spread  if p_spread > progressing_spread
        if (p_spread > progressing_spread) {
            progressing_spread = p_spread;
        }
    }

    inline auto closeVirginCandleValues(
        /* quts p_time,
        qreal p_open, qreal p_high, qreal p_low, */ qreal p_close,
        /* qreal p_tick_vol, qreal p_ask_vol, qreal p_bid_vol, */ qreal
            p_spread) -> void {
        // auto last_close = close[1];
        time |= progressing_time;
        open |= p_close;
        high |= p_close;
        low |= p_close;
        close |= p_close;
        tick_volume |= 0.0;
        ask_volume |= 0.0;
        bid_volume |= 0.0;
        spread |= p_spread;
    }

    inline auto closeProgressingCandleValues(
        /* quts p_time,
        qreal p_open, qreal p_high, qreal p_low, qreal p_close,
        qreal p_tick_vol, qreal p_ask_vol, qreal p_bid_vol, qreal p_spread
        */
        ) -> void {
        time |= progressing_time;
        open |= progressing_open;
        high |= progressing_high;
        low |= progressing_low;
        close |= progressing_close;
        tick_volume |= progressing_tick_volume;
        ask_volume |= progressing_ask_volume;
        bid_volume |= progressing_bid_volume;
        spread |= progressing_spread;
    }

    inline auto closeTheCandle(
        quts p_time,
        /*qreal p_open, qreal p_high, qreal p_low, */ qreal p_close,
        qreal p_tick_vol,
        /*qreal p_ask_vol, qreal p_bid_vol, */ qreal p_spread) -> void {
        // cerr << "Periodization::addNewBar() - gonna advance heap - next_epoch
        // is: " << next_epoch << " next_bar_time is: " << next_bar_time <<
        // "\n";

        if (p_time - next_bar_time > period) {
            cerr << "(" << period
                 << ") - Missing bars since last. Reasonable? time = " << p_time
                 << ", expected = " << next_bar_time
                 << ", delta = " << (p_time - next_bar_time)
                 << " which is > period"
                 << "\n";

            // if ( next_bar_time == dt::min_date_time ) {
            //}
        }

        // // Create a new closed bar to insert the currently open values
        buffer_heap.advance();

        if (progressing_tick_volume >
            0) { // Have we gotten any ticks for the progressing bar?
            closeProgressingCandleValues(
                /*  p_time, p_open, p_high, p_low, p_close, p_tick_vol, p_ask_vol, p_bid_vol, p_spread */);
        } else { // It's a complete ghost bar - set all prices to previous close
            closeVirginCandleValues(
                /* p_time, p_open, p_high, p_low, */ p_close,
                /*p_tick_vol, p_ask_vol, p_bid_vol, */ p_spread);
        }

        ++total_bars;
        if (cached_bars < capacity) {
            ++cached_bars;
        }

#ifdef IS_DEEPBUG
        if (p_tick_vol && period_feed == nullptr) {
            cerr << "\n\n (" << period
                 << ") WE ACTUALLY GOT A NON GHOST-TICK AS BAR-CLOSER!!! at "
                 << p_time << "\n\n";
            ++real_tick_closes;
        } else {
            ++ghost_tick_closes;
        }

#endif

        // // // - If we have at least one _closed_ bar - emit! // // //
        // *TODO* *INVESTIGATE* - one off error????
        if (cached_bars >= 1) { // +1 for the open bar, +1 for closed bar
            emit_signal();
        }
    }

    inline auto openFreshBar(
        /* quts p_time, */
        qreal p_open, qreal p_high, qreal p_low, qreal p_close,
        qreal p_tick_vol, qreal p_ask_vol, qreal p_bid_vol,
        qreal p_spread) -> void {

        if (p_tick_vol == 0) { // Ghost-tick (most common case)
            progressing_time = next_bar_time;
            progressing_tick_volume = 0;

        } else {
            initOpenCandle(/* p_time, */ p_open, p_high, p_low, p_close,
                           p_tick_vol, p_ask_vol, p_bid_vol, p_spread);
        }

        // // // - Calculate time movement - // // //
        next_bar_time += period;

        // // //
        // *TODO* *INVESTIGATE* Look this logic over!
        // // //
        if (next_bar_time > next_epoch) {
            cerr << "\n\n - - - > (" << period
                 << ") Past an PERIODIZATION EPOCH! " << next_epoch << " -  "
                 << next_bar_time << "\n\n";
            if (next_bar_time <
                next_epoch + period) { // In case of uneven periodization units,
                                       // the day-breaking one will be shortened
                                       // so that the next begins on day break
                cerr << "Had to adjust next_bar_time at periodization epoch "
                        "crossing! " << next_bar_time << " -> "
                     << next_epoch + period << "\n";
                next_bar_time = next_epoch + period;
            }

            next_epoch += epoch_duration;
        }
        // // //
        // // //
    }

    inline auto ackumulateTick(quts p_time, qreal p_open, qreal p_high,
                               qreal p_low, qreal p_close, qreal p_tick_vol,
                               qreal p_ask_vol, qreal p_bid_vol,
                               qreal p_spread) -> void {
        // cerr << "QuantPeriodization<" << period << ">::handleTick():  " <<
        // p_time;
        // cerr << " " << p_open << " " << p_high << " " << p_low << " " <<
        // p_close << " " << p_tick_vol << " " << trvolume << " " << p_spread <<
        // "\n";

        if (LIKELY(p_time <
                   next_bar_time)) { // It's still an update to the open bar
            if (progressing_tick_volume >
                0) { // We already have ticks in the bar - it's bona fide, not a
                     // ghost candle anymore - let's update it!
                updateProgressingCandle(/*p_time, */ /* p_open, */ p_high,
                                        p_low, p_close, p_tick_vol, p_ask_vol,
                                        p_bid_vol, p_spread);
            } else { // It's still a ghost candle, this is the first tick
                     // reaching it! 2014-11-03/ORC
                initOpenCandle(/*p_time, */ p_open, p_high, p_low, p_close,
                               p_tick_vol, p_ask_vol, p_bid_vol, p_spread);
            }

        } else {
            closeTheCandle(p_time, /* p_open, p_high, p_low, */ p_close,
                           p_tick_vol, /* p_ask_vol, p_bid_vol, */ p_spread);
            openFreshBar(/* p_time, */ p_open, p_high, p_low, p_close,
                         p_tick_vol, p_ask_vol, p_bid_vol, p_spread);
        }
    }

    QuantMultiKeeperJar* the_jar;

    QuantFeedAbstract* quant_feed;
    QuantPeriodizationAbstract* period_feed;

    // QuantTime           likely_start_date;
    // QuantTime           likely_end_date;
    QuantDuration period;
    QuantDuration epoch_offset = dt::min_date_time; // Close the bars earlier
                                                    // (or later) than regular
                                                    // timing (midnight offset)
    QuantTime next_epoch;
    QuantDuration epoch_duration;
    QuantTime next_bar_time = dt::min_date_time;

    // R pretty_period;   // For cerr / debugging

    size_t default_buf_size = 0;
    size_t capacity = 0;
    N total_bars = 0; // total bars processed in running time, may be higher
                      // than actual cached bars because of buffer roll-out
    N cached_bars = 0;
    // Z open_bar_is_pure_ghost;     // while only ghost ticks has formed the
    // bar - we are open to re-set it to first _real_ tick for open...
    // WE IGNORE this for now...

    N real_tick_closes = 0;
    N ghost_tick_closes = 0;
};

#endif

QuantPeriodizationAbstract::QuantPeriodizationAbstract(
    R period, QuantPeriodizationAbstract& period_feed, N lookback,
    QuantMultiKeeperJar* the_jar)
    : buffer_heap{ lookback }
    , the_jar{ the_jar }
    , period_feed{ &period_feed }
    , default_buf_size{ lookback }
    , capacity{ lookback } {
    commonInit(period);

    /*
    #ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
        #ifdef
    DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
            period_feed.onBarClose_P( this,
    &QuantPeriodizationAbstract::handlePeriodBar_HARD );

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
                //cerr << "sp->time->head_ptr: " << (unsigned int)
    sp->time.head_ptr << "\n";
                //cerr << "sp->close->head_ptr: " << (unsigned int)
    sp->close.head_ptr << "\n";
                //cerr << "sp close[-1], close[0]: " << sp->close[-1] << " " <<
    sp->close[0] << "\n";


                this->ackumulateTick(
                    sp->time,
                    sp->open,
                    sp->high,
                    sp->low,
                    sp->close,
                    sp->tick_volume,
                    sp->ask_volume,
                    sp->bid_volume,
                    sp->spread
                );
            });
        #endif
    #endif
    */
}

QuantPeriodizationAbstract::QuantPeriodizationAbstract(
    R p_period, QuantFeedAbstract& quant_feed, N lookback,
    QuantMultiKeeperJar* the_jar)
    : buffer_heap{ lookback }
    , the_jar{ the_jar }
    , quant_feed{ &quant_feed }
    , default_buf_size{ lookback }
    , capacity{ lookback } {
    global_actives.active_periodization = this;

    commonInit(p_period);
    quant_feed.setRegulatedInterval(p_period);

    /*
    #ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS

        #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_FEEDS
            quant_feed.onRegulatedTick_P( this,
    &QuantPeriodizationAbstract::handleFeedTick_HARD );

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
                this->ackumulateTick(
                    //qt.flags,
                    THE_T.time,
                    THE_T.ask,
                    THE_T.ask,
                    THE_T.ask,
                    THE_T.ask,
                    ( (THE_T.ask_volume == 0 && THE_T.bid_volume == 0) ? 0 : 1
    ),                  // one tick is... 1 tick - unless ghost-tick.
                    THE_T.ask_volume,
                    THE_T.bid_volume,
                    THE_T.ask - THE_T.bid
                    //qt.swap_long,
                    //qt.swap_short
                );
            });
        #endif
    #endif
    */
}

QuantPeriodizationAbstract::~QuantPeriodizationAbstract() {
#ifdef IS_DEBUG
    cerr << "QuantPeriodization::~QuantPeriodization - - DESTRUCTOR - -\n";
    cerr << "Period: " << period << "\n";
    cerr << "Ghost-tick closes: " << ghost_tick_closes << "\n";
    cerr << "Non-Ghost-tick closes: " << real_tick_closes << "\n";
    cerr << "\n";
#endif
}

void QuantPeriodizationAbstract::commonInit(R p_period) {
    the_jar->add(this);

    // Z default_buf_size = PERIODIZATION_DEFAULT_SIZE;

    if (p_period >= 1) {
        period = pxt::minutes(p_period); // Passed in minutes, store as tX
    } else {
        period = pxt::seconds(
            p_period * 100); // From the notation-value of decimal to integer tX
    }

    // pretty_period = ( period.total_seconds() >= 60 ? period.total_seconds() /
    // 60.0 : ( - period.total_seconds() ) );

    if (period <= pxt::hours(24)) {
        next_epoch = qts::get_next_aligned_ts(
            global_actives.run_context->start_date, pxt::hours(24));
        epoch_duration = pxt::hours(24);
        next_bar_time = qts::get_next_aligned_ts(
            global_actives.run_context->start_date, period);

    } else {
        next_epoch = pxt::ptime(
            dt::next_weekday(
                global_actives.run_context->start_date.date(),
                dt::greg_weekday(dt::greg_weekday::weekday_enum::Sunday)),
            pxt::hours(0)); // + pxt::hours( 24 * 7 );
        epoch_duration = pxt::hours(7 * 24);
        next_bar_time = qts::get_next_aligned_ts(
            global_actives.run_context->start_date, period,
            dt::greg_weekday(dt::greg_weekday::weekday_enum::Sunday));
    }

    cerr << "p_period = " << p_period << " period = " << period
         << " seconds of that: " << period.seconds()
         << " epoch_duration = " << epoch_duration.hours() << "\n";

    global_actives.active_buffer_heap = &buffer_heap;
}

// void QuantPeriodizationAbstract::setDateRange (
//   QuantTime p_start_date,
//   QuantTime p_end_date
//) {
//    likely_start_date = p_start_date;
//    likely_end_date = p_end_date;
//}

void QuantPeriodizationAbstract::add(QuantBufferAbstract& buffer) {
    buffer_heap.add(buffer);
}

void QuantPeriodizationAbstract::add(QuantBufferAbstract& buffer,
                                     size_t default_buf_size) {
    buffer_heap.add(buffer, default_buf_size);
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

Str QuantPeriodizationAbstract::to_str() {
    std::stringstream buf;
    buf << "OHLC: " << time << " (" << open << ", " << high << ", " << low
        << ", " << close << ", " << tick_volume << ", " << ask_volume << ", "
        << bid_volume << ""
        << ")";

    return buf.str();
}
