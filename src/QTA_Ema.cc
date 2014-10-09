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
        QuantPeriodization &perian = *global_actives.active_periodization
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

    void operator() ( QuantReal value );
    void operator() ( QuantBuffer<QuantReal,-1> &value );
    void operator() ( QTA::ObjectAbstract &value );
    QuantReal operator[] ( int reverse_index );

  private:
    inline void handleValue ( QuantReal value );
    //virtual void prime ( QuantReal value );    // prime the lookback with reasonable approximations, default values

    int                 len;
    QuantReal           ema_k;
    int                 lookback;
    int                 stable_sample_count;
    bool                dynamically_inited = false;
    bool                is_primed = false;
    QuantReal           prevMA = 0;
    QuantPeriodization  &per;

    //QuantBuffer         cache_buf;
    QuantBuffer<QuantReal,0>         result_ema;

};


}

#endif

namespace QTA {

Ema::Ema ( int length, int max_lookback, QuantPeriodization &per )
: ObjectAbstract ()
, len( length )
, ema_k( 2.0 / ( length + 1.0 ) )
, lookback( MAX( length, max_lookback ) )
, stable_sample_count( len * 2 )
, per( per )
{
    per.add( result_ema, lookback );
    //init();
}

Ema::~Ema ()
{}

/*
inline void Ema::init () {
    //per.add(cache_buf, len);
    per.add( result_ema, lookback );
}
*/

void Ema::operator() ( QuantReal value ) {
    handleValue( value );
}

void Ema::operator() ( QuantBuffer<QuantReal,-1> &value ) {
    handleValue( value[0] );
}

void Ema::operator() ( QTA::ObjectAbstract &value ) {
    //handleValue( value[0] ); *TODO* lägg in virtual method i ObjectAbstract
}

QuantReal Ema::operator[] ( int reverse_index ) {
    return result_ema[ reverse_index ];
}


void Ema::handleValue (
/*
    int               startIdx,
    int               endIdx,
    const INPUT_TYPE *inReal,
    int               optInTimePeriod,
    double            optInK_1,
    int              *outBegIdx,
    int              *outNBElement,
    double           *outReal
*/
    //QuantBuffer<QuantReal,-1> &src_buf
    QuantReal   in_value
) {
    if ( result_ema.size < stable_sample_count ) {
        is_primed = false;
        //per.is_primed = false;    // *TODO*
    }

    if ( dynamically_inited == false ) {    // initial value
        result_ema[0] = in_value; //src_buf[0];
        dynamically_inited = true;
        return;
    }

    prevMA = ((in_value - prevMA) * ema_k) + prevMA;
    result_ema[0] = prevMA;


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


}
