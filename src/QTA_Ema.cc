#include "QTA_Ema.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QTABase.hh"

namespace QTA {

class Ema : public QTA::ObjectAbstract
{
  public:

    Ema (
        int length,
        int max_lookback = 0
        /*, QuantBuffer input */,
        QuantPeriodizationAbstract &perian = *global_actives.active_periodization
    );
    /*
    Ema (
        int length
        / *, QuantBuffer input * /,
        QuantPeriodization &perian = *global_actives.active_periodization
    );
    */
    ~Ema ();
    //inline void init ();



    inline bool is_primed () final {
        return ( ++prime_count > prime_minimum );
    }

    //inline void prime ( QuantReal value ) final {
    //    prevMA = value;
    //}



    inline void operator<< ( QuantReal value ) {
        calcValue( value );
    }

    inline void operator|= ( QuantReal value ) {
        handleValue( value );
    }
    //inline void operator() ( QuantReal value ) {
    //    handleValue( value );
    //}
    //
    /*
    inline void operator() ( QuantBuffer<QuantReal> &value ) {
        handleValue( value );
    }
    */
    inline QuantReal operator[] ( int reverse_index ) const {
        return result[ reverse_index ];
    }
    inline QuantReal operator[] ( int reverse_index ) {
        return result[ reverse_index ];
    }
    inline operator QuantReal() const {
        //return *((QuantReal *)result.head_ptr);
        return result;
    };

    // *BUBBLARE* *TODO*
    inline void produce () {
        result |= prevMA;
    }

    // *BUBBLARE* *TODO*
    inline void commit () {
        result |= prevMA;
    }

  private:
    inline void calcValue ( QuantReal in_value ) {


        // *TODO*  replace with the is_primed stuff..


        if ( dynamically_inited ) {
            prevMA = ((in_value - prevMA) * ema_k) + prevMA;
        } else {
            prevMA = in_value;
            dynamically_inited = true;
        }

    }



    //    *TODO*  - all buffers need to implement "is_primed()"  !!


    inline void handleValue ( QuantReal in_value ) {
        /*
         *   *TODO* in the verify_buffers() function instead - aight!
         *
        if ( result.size < stable_sample_count ) {
            // *TODO*
            is_primed = false;
            //per.is_primed = false;    // *TODO*
        }
         *
         */

        calcValue( in_value );
        commit();
        //result |= prevMA;

        /*
        double tempReal, prevMA;
        int i, today, outIdx, lookbackTotal;

        lookbackTotal = LOOKBACK_CALL(EMA)( optInTimePeriod );

        if( startIdx < lookbackTotal )
            startIdx = lookbackTotal;

        if( startIdx > endIdx ) {
            VALUE_HANDLE_DEREF_TO_ZERO(outBegIdx);
            VALUE_HANDLE_DEREF_TO_ZERO(outNBElement);
            return ENUM_VALUE(RetCode,TA_SUCCESS,Success);
        }

        VALUE_HANDLE_DEREF(outBegIdx) = startIdx;

        if( TA_GLOBALS_COMPATIBILITY == ENUM_VALUE(Compatibility,TA_COMPATIBILITY_DEFAULT,Default) ) {
            today = startIdx-lookbackTotal;
            i = optInTimePeriod;
            tempReal = 0.0;
            while( i-- > 0 )
                tempReal += inReal[today++];
            prevMA = tempReal / optInTimePeriod;
        } else {
            prevMA = inReal[0];
            today = 1;
        }

        while ( today <= startIdx )
            prevMA = ((inReal[today++]-prevMA)*optInK_1) + prevMA;

        outReal[0] = prevMA;
        outIdx = 1;

        while ( today <= endIdx ) {
            prevMA = ((inReal[today++]-prevMA)*optInK_1) + prevMA;
            outReal[outIdx++] = prevMA;
        }

        VALUE_HANDLE_DEREF(outNBElement) = outIdx;
        return ENUM_VALUE(RetCode,TA_SUCCESS,Success);
        */
    }
    //virtual void prime ( QuantReal value );    // prime the lookback with reasonable approximations, default values

    int                 len;
    int                 lookback;
    QuantReal           ema_k;

    bool                dynamically_inited = false;
    int                 prime_count = 0;
    int                 prime_minimum = 0;

    QuantReal           prevMA = 0;

    QuantPeriodizationAbstract  &per;

    QuantBuffer<QuantReal>      result;

};


}

#endif

namespace QTA {

Ema::Ema ( int length, int max_lookback, QuantPeriodizationAbstract &per )
:
    ObjectAbstract (),
    len( length ),
    ema_k( 2.0 / ( length + 1.0 ) ),
    lookback( MAX( length, max_lookback ) ),
    prime_minimum { len * 4 },   //MAX( len * 4, lookback ) }
    per( per ),
    result { lookback }
{
    //per.add( result, lookback );
}

Ema::~Ema ()
{}


}
