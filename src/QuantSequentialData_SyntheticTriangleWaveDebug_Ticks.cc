#include "QuantSequentialData_SyntheticTriangleWaveDebug_Ticks.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-22
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantSequentialData.hh"


/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *     *TODO* - for debugging purposes - generate a alternately uphill/downhill
 *              sawtand price tick signal
 *
 *
 *
 *
 */




class QuantSequentialData_TriangleWaveTicks : public QuantSequentialDataAbstract
{
  public:
    QuantSequentialData_TriangleWaveTicks () {};
    ~QuantSequentialData_TriangleWaveTicks () {};

    bool doInit(
        std::string p_symbol,
        double p_point_decimals
    ) final;

    bool readTick( QuantTick &tick ) final;

  private:
    int loadNextChunk();

    const int       session_break_duration_in_chunk_units = 24 * 2;
    std::string     symbol;
    double          point_decimals;

    pxt::ptime      time_pos;

    size_t          buf_pos = 0;
    size_t          byte_buffer_size = 0;
    byte            *byte_buffer = nullptr;

};

#endif




bool QuantSequentialData_TriangleWaveTicks::doInit (
    std::string p_symbol,
    double p_point_decimals
) {
    symbol = p_symbol;
    point_decimals = p_point_decimals;
    time_pos = start_date; //makeTime( "2014-01-07 00:00:00.000" );
    return true;
}


bool QuantSequentialData_TriangleWaveTicks::readTick ( QuantTick &tick ) {


    /*
     *
     *
     * *TODO*
     *
     *
     */


    tick.time = time_pos;
    tick.last_price = tick.ask =  47;
    tick.bid =  47;
    tick.ask_volume =  47;
    tick.bid_volume =  47;

    return true;
}

