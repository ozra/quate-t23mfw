/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef QUANTPERIODIZATION_HH
#define QUANTPERIODIZATION_HH

#include "QuantBase.hh"
#include "QuantBuffers.hh"
#include "QuantFeed.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

class QuantPeriodization {
  public:
        QuantPeriodization (int period_duration, QuantPeriodization *qu_per);
        QuantPeriodization (int period_duration, QuantFeed *qu_feed);
        ~QuantPeriodization ();
    void
        commonInit ();

    //QuantBuffer<QuantReal>
    //    createSeries ();

    // void (const &QuFiInstrument)
    Emitter<void ()>
        onBarOpen;
    Emitter<void ()>
        onBarUpdate;
    Emitter<void ()>
        onBarClose;

    QuantBufferHeap
        buffer_heap;

    QuantBuffer<QuantDataFlags>
        *flags;

    QuantBuffer<QuantTime>
        *time;

    QuantBuffer<QuantReal>
        *open,
        *high,
        *low,
        *close,
        *median,
        *tvolume,
        *rvolume,
        *spread
        //*longswap,
        //*shortswap
        ;

  private:
    // Methods
    void
        handleTick (QuantDataFlags  tflags,
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
    );

    void
        addNewBar (
            QuantDataFlags,
            QuantTime,
            QuantReal,
            QuantReal,
            QuantReal,
            QuantReal,
            QuantReal,
            QuantReal,
            QuantReal,
            QuantReal
            //QuantReal,
            //QuantReal
    );

    void
        closeLastBar ();

    // Conf-like
    int
        period;
    long long   // *TODO*
        period_in_micros;
    bool
        use_ghostbars           = true;
    bool
        ghostify_session_breaks = true;
    bool
        use_median              = false;
    QuantFeed
        *quant_feed;
    QuantPeriodization
        *source_period;

    // Object/List-like
    //vector<QuantBuffer<QuantReal> *>
    //    buffers;
    Listener
        tick_listener;

    // Variable-like
    int
        total_bars;      // total bars processed in running time, may be higher than actual cached bars because of buffer roll-out
    int
        cached_bars;
    QuantTime
        last_bar_time;

};

#endif
