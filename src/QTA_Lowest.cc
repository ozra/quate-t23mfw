#include "QTA_Lowest.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-12 (05:10)
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/* Find the lowest value over a given length of periods
 */

#include "QuantBasic_DesignChoices.hh"
#include "QTABase.hh"

namespace QTA {

class Lowest : public QTA::ObjectAbstract {
  public:


    /* *TODO*
     * BUFFER_JAR should have "is_primed" flag!!!
     * We should have reference to BUFFER_JAR - NOT PERIODIZATION!
     */

    Lowest (
        int length,
        int max_lookback = 0,
        QuantPeriodizationAbstract &per = *global_actives.active_periodization
    ) :
        length ( length ),
        lookback { MAX( max_lookback, length ) }, // max_lookback > 0 ? max_lookback : per.default_buf_size ),
        result ( lookback )
    {
    }

    ~Lowest () {}

    inline void handleValue ( QuantBuffer<QuantReal,-1> & in_buf ) {
        QuantReal usable_length = length;

        //cerr << "length = " << length << ", in_buf.size = " << in_buf.size << ", ";

        if ( in_buf.size - 1 < usable_length ) {
            usable_length = in_buf.size - 1;

            cerr << "sets usable length to " << usable_length << ", ";

            if ( usable_length < 2 ) {  // +1 for CLOSED bar, +1 for lowest_so_far fallen out compare
                result( 0.0 );
                // *TODO*
                // owning_jar.is_primed = false;
                return;
            }
        }

        // *TODO*  count statistics for how many times different modes of
        // calculation are incorporated

        //cerr << "lowest_so_far " << lowest_so_far << ", ";

        if ( in_buf < lowest_so_far ) {
            //cerr << "new lowest_so_far gotten. ";
            lowest_so_far = in_buf;

        } else if ( in_buf[ usable_length + 1] <= lowest_so_far ) {   // If value falling out of zone is lower, we're good with what we have
            //cerr << "lowest_so_far must be calculated again. ";

            lowest_so_far = std::numeric_limits<QuantReal>::max();

            #ifdef DESIGN_CHOICE__TA_PTR_ARITH_INSTEAD_OF_BUF_ACCESSOR
                int i = usable_length; // + 1;
                QuantReal *ptr = in_buf.getPtrTo( i );

                while( --i >= 0 ) {
                    if ( *ptr < lowest_so_far ) {
                        lowest_so_far = *ptr;
                    }
                    ++ptr;
                }

            #else
                for ( int i = usable_length; i >= 0; --i ) {
                    if ( in_buf[i] < lowest_so_far ) {
                        lowest_so_far = in_buf[i];
                    }
                }

            #endif

        }

        //cerr << "lowest_so_far after " << lowest_so_far << ", ";
        //cerr << "\n";

        result( lowest_so_far );
    }

    inline void operator() ( QuantBuffer<QuantReal,-1> &value ) {
        handleValue( value );
    }

    void operator() ( QTA::ObjectAbstract &value ) {
        //handleValue( value[0] ); *TODO* lägg in virtual method i ObjectAbstract
    }

    inline QuantReal operator[] ( int reverse_index ) const {
        return result[ reverse_index ];
    }

    inline operator QuantReal() const {
        return result;
    };

  private:
    int length;
    int lookback;

    QuantReal lowest_so_far = std::numeric_limits<QuantReal>::max();

    QuantBuffer<QuantReal,0>    result;

};

}

#endif
