/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantRetroPracdelic.hh"

RetroPracdelic::RetroPracdelic (
    QuantStudyContext   &study,
    Str                 symbol,
    Str                 start_date,
    Str                 end_date
) :
        study (study),
        symbol (symbol),
        start_date (start_date),
        end_date (end_date),
        mode (BACKTEST)
{

}

RetroPracdelic::~RetroPracdelic () {

}

int RetroPracdelic::run ( void ) {
    cerr << "RetroPracdelic::run - init\n";
    study.init();

    auto study_feeds( study.feeds );  // so that we can remove feeds as they reach their last values - might change the implementation to check them every loop - will only affect computations in the very last minute of trading data...  - 2014-09-11/ORC(20:51)

    // Prime feeds by fetching next available tick in each of them
    for ( auto f : study_feeds ) {
        cerr << "calls readNext for priming\n";
        f->readNext();               // This will cause loads for each feed, unfortunately waiting sequentially for each before calling fs for the next..     - 2014-09-11/ORC(20:54)
    }

    //if (state == HISTORICAL )     // There's ALWAYS historical data reading at first, nothing else makes sense really (the pre-load may be short, but will always be there) -  - 2014-09-11/ORC(20:56)

    //# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    // Historical / backtest event loop  - 2014-09-11/ORC(21:03)
    QuantFeed *nearest_feed = NULL;
    //QuantTimer *nearest_timer = null_ptr;     // *TODO(ORC)*
    QuantTime lowest_time = 0;
    //int ret;
    bool b_ret;

    for (;;) {
        lowest_time = 0;


        cerr << "Find closest tick in time\n";

        // Find the tick closest in time
        for ( auto f : study_feeds ) {
            if ( !lowest_time || f->ticks[0].time < lowest_time ) {
                lowest_time = f->ticks[0].time;
                nearest_feed = f;
            }
        }

        cerr << "Got it. emit it\n";

        nearest_feed->emit();            // Let it kick of its' chain of events
        b_ret = nearest_feed->readNext();  // Update to next value

        if ( b_ret == false ) {         // No more _historical_ data in the feed

            // Do it the vector<> way... look it up!
            //study_feeds.erase( nearest_feed );

            if ( study_feeds.size() == 0 ) {
                break;  // Break out of history/backtest event-loop
            }

        }

    }

    //# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    // Change "mode" to live _if_ not pure backtest that is
    if ( mode == BACKTEST ) {
        return 0;
    }

    for ( auto f : study.feeds ) {
        f->setLiveMode();
    }

    //# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    // Live event loop  - 2014-09-11/ORC(21:10)

    for (;;) {

        if ( true ) {   // AKA "TODO"
            break;
        }
    }

    /*

    const int MULTER = 1;

    QuantTime start = 1409986376737L * MULTER;
    QuantTime end = 1410301589077L * MULTER;
    QuantTime duration = end - start;

    QuantTime pos, prev_pos = 0;
    QuantTime quarterly = 7776000 * MULTER;

    cerr << duration << "\n";

    for (QuantTime t = start; t < end; ++t) {
        // run zmq-polls and such

        if(t % quarterly == 0) {
            pos = ((t - start) * 100) / (end - start);
            if(prev_pos != pos)
                cerr << pos << "\n";
            prev_pos = pos;
        }
    }
    */

    cerr << "RetroPracdelic::run deinit";
    study.deinit();
    cerr << "RetroPracdelic::run ends";
    return 0;
}
