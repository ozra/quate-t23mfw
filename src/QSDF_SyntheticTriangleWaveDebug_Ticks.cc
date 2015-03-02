#include "QSDF_SyntheticTriangleWaveDebug_Ticks.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-22
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QSDF_Utils.hh"
#include "QSDF_Ticks_Abstract.hh"

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

// class QSDF_TriangleWaveTicks : public
// QuantSequentialDataAbstract
class QSDF_TriangleWaveTicks
    : public QSDF_Ticks_Abstract
{
  public:
    QSDF_TriangleWaveTicks() {};
    ~QSDF_TriangleWaveTicks() {};

    bool init(std::string p_broker_id, std::string p_symbol);

    bool readTick(QuantTick & tick) final;

  private:
    int loadNextChunk();

    const int session_break_duration_in_chunk_units = 24 * 2;
    std::string broker_id;
    std::string symbol;
    double point_decimals;

    pxt::ptime time_pos;

    size_t buf_pos = 0;
    size_t byte_buffer_size = 0;
    byte * byte_buffer = nullptr;
};

#endif

bool QSDF_TriangleWaveTicks::init(std::string p_broker_id,
                                  std::string p_symbol)
{
    broker_id = p_broker_id;
    symbol = p_symbol;
    point_decimals = 000001; // point_decimals;
    time_pos = start_date_; // makeTime( "2014-01-07 00:00:00.000" );
    return true;
}

bool QSDF_TriangleWaveTicks::readTick(QuantTick & tick)
{
    /*
     *
     *
     * *TODO*
     *
     *
     */
    tick.time = time_pos;
    tick.last_price = tick.ask = 47;
    tick.bid = 47;
    tick.ask_volume = 47;
    tick.bid_volume = 47;
    return true;
}
