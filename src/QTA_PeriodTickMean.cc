#include "QTA_PeriodTickMean.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-12 (05:10)
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/* Calculates the mean of all tick prices, weighted or non weighted for volume
 * over the period of one periodization unit, a faster way of establishing a
 * sort of median price for one candle bar.
 */

#include "QTABase.hh"

namespace QTA {

// template <bool WEIGHTED>
class PeriodTickMeanVariations : public QTA::ObjectAbstract
{
  private:
    size_t lookback;

    QuantReal mean_ack = 0.0;
    QuantReal value_weight = 0.0;
    // int         value_weight = 0;

    QuantBuffer<QuantReal> result;

  public:
    PeriodTickMeanVariations(size_t max_lookback = 0
                             //, QuantPeriodizationAbstract &per =
                             //*global_actives.active_periodization
                            ) :
        lookback{ max_lookback }
        , // max_lookback > 0 ? max_lookback : per.default_buf_size ),
        result(lookback)
    {
        // *TODO* LOOK OVER THE ARCHITECTURE - WE PROBABLY *SHOULD* USE per.add
        // _BUT_ - we want QTA's to be nearly as simple to write as QStudies.
        // QTAs, like QStudies, should be seen as a Mathematician/Novice C++
        // USER pattern space.
        //
        //
        // per.add( result, lookback );
        /*
        feed.onRegulatedTick( [this] {
            if ( this->feed.ticks[0].isGhostTick() ) {  // Ghost ticks have no
        place in the summing
                return;
            }
            //this->median_collection.advance() = ( this->feed.ticks[0]
        ).last_price;
            mean += (this->feed.ticks[0]).last_price;
        } );
        */
        /*
        per.onBarClose( [this] {
            this->calculateMedian();
        } );
        */
    }

    ~PeriodTickMeanVariations() {}

    /*
    inline void updateMean ( QuantReal value ) {
        if ( WEIGHTED ) {
            mean_ack += value;
            ++value_weight;
        } else {
            mean_ack += value;
            ++value_weight;
        }
    }
    */
    inline void updateMean(QuantReal value, QuantReal weight)
    {
        mean_ack += value * weight;
        value_weight += weight;
    }
    inline void updateMean(QuantReal value)
    {
        mean_ack += value;
        ++value_weight;
    }

    inline void calculateMean(QuantReal close_value)
    {
        if (value_weight == 0) { // Ghost candle?
            // cerr << "calculateMean: " << mean_ack << " / " << value_weight <<
            // " USES either: " << result[1] << " or " << close_value << "\n";
            // *TODO*
            if (false && result.size >= 2) {
                result |= result[1];
            }
            else {
                result |= close_value;
            }
        }
        else {
            // cerr << "calculateMean: " << mean_ack << " / " << value_weight <<
            // " = " << (mean_ack / value_weight) << "\n";
            result |= (mean_ack / value_weight);
            mean_ack = 0.0;
            value_weight = 0;
        }
    }

    inline void operator<<(QuantReal value) { updateMean(value); }

    inline void operator|=(QuantReal value) { calculateMean(value); }

    inline QuantReal operator[](int reverse_index) const
    {
        return result[reverse_index];
    }

    inline operator QuantReal() const
    {
        return result;
    };

};

// typedef PeriodTickMeanVariations<true> PeriodTickMeanWeighted;
// typedef PeriodTickMeanVariations<false> PeriodTickMean;
typedef PeriodTickMeanVariations PeriodTickMean;
}

#endif
