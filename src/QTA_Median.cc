#include "QTA_Median.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QTABase.hh"

namespace QTA {

class Median : public QTA::ObjectAbstract {
  public:

    Median (
        int length,
        int max_lookback = 0
        /*, QuantBuffer input */,
        QuantPeriodizationAbstract &perian = *global_actives.active_periodization,
        QuantFeedAbstract &feed = *global_actives.active_feed
    );

    ~Median ();

    //inline void init ();

    void operator() ( QuantReal value );
    void operator() ( QuantBufferAbstract &value );
    void operator() ( QTA::ObjectAbstract &value );
    void operator[] ( int reverse_index );

    operator QuantReal() const {
        //return *((QuantReal *)result_ema.head_ptr);
        return result;
    };

  private:
    inline void calculateMedian ();
    //virtual void prime ( QuantReal value );    // prime the lookback with reasonable approximations, default values

    int                 len;
    int                 lookback;

    QuantPeriodizationAbstract  &per;
    QuantFeedAbstract           &feed;

    ReversedCircularStructBuffer<QuantReal> median_collection;
    QuantBuffer<QuantReal,0>                result;

};


}

#endif


namespace QTA {


/*

*TODO*

NOTEWORTHY: See the approach here - EVENTED CALCULATION! That is it takes to
event sources and hooks in to them, reacts directly on their events and base
the periodic sequentiation upon it.

Think through the implications of mixing this style with "user space" evented
sequential QTAI value feeding.

*NOTE* - It is _probably_ better to leave this eventing and feeding to the
_user space_ and supply TWO feeding methods - one for the ticks and one for
the periodic summation. It will require user-annotation - but will also provide
faster execution (less event reaction patterns) and versatility (exotic value
sources)

*TODO*

*/



/*
 *
 *

Median::Median (
    int length,
    int max_lookback,
    QuantPeriodization &per,
    QuantFeedAbstract &feed
) :
    len ( length ),
    lookback ( MAX( max_lookback, length) ),
    per ( per ),
    feed ( feed ),
    median_collection ( 10000 ) //,
    //result { lookback }
{
    per.add( result, lookback );
    //init();

    feed.onRegulatedTick( [this] {
        if ( this->feed.ticks[0].isGhostTick() ) {
            return;
        }
        this->median_collection.advance() = ( this->feed.ticks[0] ).last_price;
    } );

    per.onBarClose( [this] {
        this->calculateMedian();
    } );

}

Median::~Median ()
{}

void Median::calculateMedian ()
{
    QuantReal median = 0.0;
    //
    // *TODO* sort all the values / find the median that is....
    //

    int len = median_collection.size;

    for ( int i = 0; i < len; ++i ) {
        median += median_collection[i];
    }

    median /= len;
    //
    // *TODO* above
    //

    median_collection.reset();

    result[0] = median;
}
*/

/*
Median::Median ( int length, QuantPeriodization &perian, QuantFeed &feed )
: len( length )
, lookback( length )
, per( perian )
, feed( feed )
, median_collection( 10000 )
{
    init();
}
*/

/*
inline void Median::init () {
    per.add( result, lookback );

}
*/

// 140926 - QuantFlux ( margin of error in markets) - thought through earlier
// today (~1400) - time is now 21:17 - and the radio (P1 ?) is talking about
// Quantum Fluctuations during the first 400KY after The Big, captured by the
// Planck camera (which I thought was deemed to be cosmic dust and completely
// moot - already. Anyways. Coincidence de luxe. And the pseudo scientific
// writer of the book the programmed is centered around annoys me immensly -
// hate his voice! Well, and pseudo babble.
//
// Well - the definition for the flamboyant term in market space is "The
// variation of market price of a financial instrument amongst all available
// - realisitcally suitable tradeable - markets. The search for the
// gravitational multi body system, or 0-is-surface based or freely geometrical
// space is based on the assumption that any deviations within Quant Flux is
// of no importance (other than that the relative magnitude of QF may be
// indiciative / confirming of impending (!) change.



}
