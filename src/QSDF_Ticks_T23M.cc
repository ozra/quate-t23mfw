#include "QSDF_Ticks_T23M.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-12
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
* Code Cls: Blackbox Some Metal
**/

#include <cerrno>
#include <cstring>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <boost/format.hpp>

#include "QuantBase.hh"
#include "QuantTime.hh"
#include "MutatingString.hh"
#include "MutatingBuffer.hh"

#include "QSDF_Ticks_Abstract.hh"
#include "QSDF_Sys_T23M_Abstract.hh"

//#define MONTHLY

/*
                        #### ##    ## #### ########
                         ##  ###   ##  ##     ##
                         ##  ####  ##  ##     ##
                         ##  ## ## ##  ##     ##
                         ##  ##  ####  ##     ##
                         ##  ##   ###  ##     ##
                        #### ##    ## ####    ##
*/

typedef QSDF_Sys_T23M_Abstract<QuantTick, QuantProfileEnum::LOADING_FILES_T23M>
QSDF_T23M_Quant_Abstract;

class QSDF_Ticks_T23M : public QSDF_T23M_Quant_Abstract,
    public QSDF_Ticks_Abstract
{
    friend class QSDF_Sys_T23M_Abstract;

  public:
    QSDF_Ticks_T23M(bool write_mode = false)
        : QSDF_T23M_Quant_Abstract(write_mode,
                                   2)   // *TODO* - 2 => session_break_duration_in_chunk_units
        , QSDF_Ticks_Abstract()
        , write_mode_(write_mode)
          //   #ifdef MONTHLY
          // , chunk_unit_duration(1)
          //   #endif
    {
    }

    ~QSDF_Ticks_T23M() {};

    bool init(string p_broker_id, string p_symbol) final;

    // 2014-12-03/Oscar Campbell - seeking to correct start point is done at every startup and thus needs special attention cycle shaving... So we implement a specific "just spool through values" method.
    // - NOPE. No can do. Need to accumulate all values..
    //inline void seek() final;
    inline bool readTick(QuantTick & tick) final;
    inline bool writeTick(const QuantTick & tick);

  protected:
    // const char *    generate_locator_path ( pxt::ptime time_pos );


    //  *TODO*
    //void advance_page();


    auto generate_locator_path() -> string;
    auto read_page_header(byte *) -> byte*;
    auto write_page_header(byte *) -> byte*;

  private:
    void calculate_point_factors();
    bool make_case_for_getting_more_data();
    void make_case_for_new_page();

    bool write_mode_ = false;
    string broker_id;
    string symbol;
    boost::format filename_formatter_;
    pxt::ptime chunk_time_pos;
    pxt::ptime next_chunk_time_pos;
    // #ifdef MONTHLY
    // const natural session_break_duration_in_chunk_units = 0;
    // boost::gregorian::months chunk_unit_duration; // pxt::hours(7*24);
    // #else
    const natural session_break_duration_in_chunk_units = 2;
    pxt::time_duration chunk_unit_duration;
    // #endif

    real price_point_factor;
    real volume_point_factor;
    real price_point_div_factor;
    real volume_point_div_factor;

    QuantTickFixed prev_raw_tick;

    qts::time_scale current_time_resolution =
        qts::MILLIS; // qts::enum_resolution::MILLIS;
    natural current_time_uncertainty;
    natural current_price_decimal_count;
    natural current_volume_decimal_count;
};


/*
                                     #######
         #####  ######   ##   #####     #    #  ####  #    #
         #    # #       #  #  #    #    #    # #    # #   #
         #    # #####  #    # #    #    #    # #      ####
         #####  #      ###### #    #    #    # #      #  #
         #   #  #      #    # #    #    #    # #    # #   #
         #    # ###### #    # #####     #    #  ####  #    #
*/
inline bool QSDF_Ticks_T23M::readTick(QuantTick & tick)
{
    /*
    //_DPn("readTick()");
    */
    //#ifdef IS_DEBUG
    profiler.start(DECODING_FILES_T23M);
    //#endif
    /*
        //_DPn(1);
    */
    // u8* rdptr = raw_buffer.on_free_leash_for(100);
    // if (rdptr == raw_buffer.end()) {
    if (raw_buffer.is_at_end()) {
        if (false == make_case_for_getting_more_data()) {
            tick.time = pxt::max_date_time;
            return false;
        }
    }
    byte * rdptr = raw_buffer.on_free_leash_for(100);
    /*
        //_DPn(2);

        _DP("raw_buffer is\n"
            << " size: " << raw_buffer.size() << "\n"
            << " capacity: " << raw_buffer.capacity() << "\n"
            << " start: " << (V*)raw_buffer.front() << "\n"
            << " end: " << (V*)raw_buffer.end() << "\n"
            << " limit: " << (V*)raw_buffer.limit() << "\n");
    */
    // raw_buffer.verify_pointer(rdptr);
    prev_raw_tick.time += read_varilen_natural<uint64_t>(
                              rdptr); // Time moves forward only - hence unsigned_varint
    //_DPn(22);
    prev_raw_tick.ask += read_varilen_integer<int>(rdptr);
    //_DPn(23);
    prev_raw_tick.bid += read_varilen_integer<int>(rdptr);
    //_DPn(24);
    prev_raw_tick.ask_volume += read_varilen_integer<int>(rdptr);
    //_DPn(25);
    prev_raw_tick.bid_volume += read_varilen_integer<int>(rdptr);
    // raw_buffer.verify_pointer(rdptr);
    raw_buffer.catch_up_reads(rdptr);
    // raw_buffer.verify_pointer(rdptr);
    //_DPn(3);
    // tick.time = pxt::millisec( prev_raw_tick.time );
    // tick.time = chunk_time_pos + pxt::milliseconds( prev_raw_tick.time *
    // current_time_uncertainty );
    tick.time =
        qts::ptime_from_ms(prev_raw_tick.time * current_time_uncertainty);
    tick.last_price = tick.ask = prev_raw_tick.ask * price_point_div_factor;
    tick.bid = prev_raw_tick.bid * price_point_div_factor;
    tick.ask_volume = prev_raw_tick.ask_volume * volume_point_div_factor;
    tick.bid_volume = prev_raw_tick.bid_volume * volume_point_div_factor;
    //      cerr << "Specific decode Tick is: " << tick.ask << ", " << tick.bid
    // <<
    // ", " << tick.ask_volume << " at " << 47 << "/" << byte_buffer_size <<
    // "\n";
    // cerr << "QSDF_Ticks_T23M::readTick: " << tick.time
    //     << " ask:" << tick.ask << " bid: " << tick.bid << "\n";
    //#ifdef IS_DEEPBUG
    // cerr << symbol << "::readTick(): " << tick.to_str() << " (end - ptr) = "
    // <<
    // (byte_buffer_end - rdptr) << "\n";
    //#endif
    //#ifdef IS_DEBUG
    profiler.end(DECODING_FILES_T23M);
    //#endif
    return true;
}
/*
                                      #######
         #    # #####  # ##### ######    #    #  ####  #    #
         #    # #    # #   #   #         #    # #    # #   #
         #    # #    # #   #   #####     #    # #      ####
         # ## # #####  #   #   #         #    # #      #  #
         ##  ## #   #  #   #   #         #    # #    # #   #
         #    # #    # #   #   ######    #    #  ####  #    #

*/
inline bool QSDF_Ticks_T23M::writeTick(const QuantTick & tick)
{
    _D("writeTick()"
       << "\n");
    //#ifdef IS_DEBUG
    start(profiling, ENCODING_DATA);
    // profiler.start( ENCODING_DATA );
    //#endif
    _DP(" 1 ");
    if (tick.time >= next_chunk_time_pos) {
        _DPn("\n\nPassed chunk time break - init a new chunk aight!\n\n");
        make_case_for_new_page();
    }
    _DP(" 2 ");
    // *TODO* hardcoded for millis.. - 2014-11-10/Oscar Campbell
    auto raw_tick = QuantTickFixed(
                        qts::millis(tick.time) / current_time_uncertainty,
                        tick.ask * price_point_factor, tick.bid * price_point_factor,
                        tick.ask * price_point_factor, tick.ask_volume * volume_point_factor,
                        tick.bid_volume * volume_point_factor);
    _DP(" 3 ");
    // cerr << "Plain-tick: " << tick.to_str() << "\n";
    // cerr << "The muddafuckin raw_tick = " << raw_tick.to_str() << "\n";
    /*
    cerr << "The muddafuckin deltas: "
        << raw_tick.time - prev_raw_tick.time << ", "
        << raw_tick.ask - prev_raw_tick.ask << ", "
        << raw_tick.bid - prev_raw_tick.bid << ", "
        << raw_tick.ask_volume - prev_raw_tick.ask_volume << ", "
        << raw_tick.bid_volume - prev_raw_tick.bid_volume
        << "\n";
    */
    byte * wrpos = raw_buffer.on_free_leash_for(100);
    raw_buffer.verify_pointer(wrpos);
    _DP(" 4.1 ");
    // Time moves forward only - hence natural number, unsigned,_varint
    write_varilen_natural<uint64_t>(wrpos, raw_tick.time - prev_raw_tick.time);
    //_DP(" 4.2 ");
    // raw_buffer.verify_pointer(wrpos);
    write_varilen_integer(wrpos, raw_tick.ask - prev_raw_tick.ask);
    //_DP(" 4.3");
    // raw_buffer.verify_pointer(wrpos);
    write_varilen_integer(wrpos, raw_tick.bid - prev_raw_tick.bid);
    //_DP(" 4.4 ");
    // raw_buffer.verify_pointer(wrpos);
    write_varilen_integer(wrpos,
                          raw_tick.ask_volume - prev_raw_tick.ask_volume);
    //_DP(" 4.5 ");
    // raw_buffer.verify_pointer(wrpos);
    write_varilen_integer(wrpos,
                          raw_tick.bid_volume - prev_raw_tick.bid_volume);
    //_DP(" 4.6 ");
    _DP(" 5 ");
    raw_buffer.verify_pointer(wrpos);
    raw_buffer.catch_up_writes(wrpos);
    _DP(" 6 ");
    raw_buffer.verify_pointer(wrpos);
    // By using pointers we could simply swap the pointers in the vars here for
    // further efficiancy...
    prev_raw_tick = raw_tick;
    _DP(" 7 ");
    persist_maybe();
    _DP(" 8 ");
    raw_buffer.verify_pointer(wrpos);
    //      cerr << "Specific decode Tick is: " << tick.ask << ", " << tick.bid
    // << ", " << tick.ask_volume << " at " << 47 << "/" << byte_buffer_size <<
    // "\n";
    // cerr << "QSDF_Ticks_T23M::readTick: " << tick.time << "
    // ask: " << tick.ask << " bid: " << tick.bid << "\n";
    //#ifdef IS_DEEPBUG
    // cerr << symbol << "::readTick(): " << tick.to_str() << " (end - ptr) = "
    // << (byte_buffer_end - rdptr) << "\n";
    //#endif
    //#ifdef IS_DEBUG
    stop(profiling, ENCODING_DATA);
    //#endif
    //_DPn("writeTick() DONE");
    return true;
}

#endif

#include "QuantProfiling.hh"

void clear(QuantTickFixed & tick)
{
    tick.time = tick.ask = tick.bid = tick.ask_volume = tick.bid_volume =
                                          tick.last_price = 0;
}

/*
  #####  #          #     #####   #####     ### #     # ### #######
 #     # #         # #   #     # #     #     #  ##    #  #     #
 #       #        #   #  #       #           #  # #   #  #     #
 #       #       #     #  #####   #####      #  #  #  #  #     #
 #       #       #######       #       #     #  #   # #  #     #
 #     # #       #     # #     # #     #     #  #    ##  #     #
  #####  ####### #     #  #####   #####     ### #     # ###    #

*/

// #ifdef MONTHLY
//
// bool QSDF_Ticks_T23M::init(string p_broker_id, string p_symbol)
// {
//     broker_id = p_broker_id;
//     symbol = p_symbol;
//     // *TODO* month iterator
//     //chunk_unit_duration = pxt::hours(24 * 30);
//     auto ymd = dt::date(start_date_.date()).year_month_day();
//     chunk_time_pos =
//         pxt::ptime(dt::date(ymd.year, ymd.month, 1), pxt::hours(0));
//     next_chunk_time_pos = chunk_time_pos + chunk_unit_duration;
//     _DPn("QSDF_Ticks_T23M::init():"
//          << "\n"
//          << "start_date: " << start_date_ << "\n"
//          << "chunk_time_pos: " << chunk_time_pos << "\n");
//     string base_format =
//         (boost::format(
//              "/usr/local/lib/T23MFW/tickdata/%s/%s/%%04d/M%%02d_ticks.t23mtf") %
//          broker_id % symbol).str();
//     _DPn("string base_format = boost::format = " << base_format << "\n");
//     filename_formatter_ = boost::format(base_format);
//     // *TODO* *IMPL* spola fram till första tillgängliga fil
//     // ASSUMED for now!!!
//     return true; // error *TODO* *IMPL*
// }
//
// inline auto QSDF_Ticks_T23M::generate_locator_path() ->
// string { // const char * {
//     auto ymd = dt::date(chunk_time_pos.date()).year_month_day();
//     return (filename_formatter_ % ymd.year % (int)ymd.month % ymd.day).str();
// }
//
// #else

bool QSDF_Ticks_T23M::init(string p_broker_id, string p_symbol)
{
    broker_id = p_broker_id;
    symbol = p_symbol;
    // *TODO* day iterator
    chunk_unit_duration = pxt::hours(24);
    chunk_time_pos =
        qts::get_prior_aligned_ts(start_date_, chunk_unit_duration) -
        chunk_unit_duration; // makeTime( "2014-01-07 00:00:00.000" );
    next_chunk_time_pos = chunk_time_pos + chunk_unit_duration;
    _DPn("QSDF_Ticks_T23M::init():"
         << "\n"
         << "start_date: " << start_date_ << "\n"
         << "chunk_time_pos: " << chunk_time_pos << "\n");
    string base_format =
        (boost::format("/usr/local/lib/T23MFW/quff-data/%s/ticks/%s/%%04d/%%02d/"
                       "%%02d_ticks.t23mtf") %
         broker_id % symbol).str();
    _DPn("string base_format = boost::format = " << base_format << "\n");
    filename_formatter_ = boost::format(base_format);
    // *TODO* *IMPL* spola fram till första tillgängliga fil
    // ASSUMED for now!!!
    return true; // error *TODO* *IMPL*
}

inline string QSDF_Ticks_T23M::generate_locator_path()   // const char * {
{
    auto ymd = dt::date(chunk_time_pos.date()).year_month_day();
    return (filename_formatter_ % ymd.year % (int)ymd.month % ymd.day).str();
}

// #endif

/*
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        ######## ##     ## ########    ######## ########  ######
           ##    ##     ## ##          ##          ##    ##    ##
           ##    ##     ## ##          ##          ##    ##
           ##    ######### ######      ######      ##    ##
           ##    ##     ## ##          ##          ##    ##
           ##    ##     ## ##          ##          ##    ##    ##
           ##    ##     ## ########    ########    ##     ######
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
inline void QSDF_Ticks_T23M::calculate_point_factors()
{
    price_point_factor = pow(10, current_price_decimal_count);
    price_point_div_factor = 1.0 / price_point_factor;
    volume_point_factor = pow(10, current_volume_decimal_count);
    volume_point_div_factor = 1.0 / volume_point_factor;
}

bool QSDF_Ticks_T23M::make_case_for_getting_more_data()
{
    chunk_time_pos += chunk_unit_duration;
    next_chunk_time_pos = chunk_time_pos + chunk_unit_duration;
    if (get_more_data()) {
        clear(prev_raw_tick);
        return true;
    }
    else {
        return false;
    }
}

void QSDF_Ticks_T23M::make_case_for_new_page()
{
    chunk_time_pos = next_chunk_time_pos;
    next_chunk_time_pos += chunk_unit_duration;
    _DPn("in INIT chunk: chunk_time_pos = " << to_iso_string(chunk_time_pos));
    // ↓ *TODO*(2014-11-23/Oscar Campbell) - should be configurable
    // ↓ should only have to be done at class configure / construction time - it
    // won't change.
    current_time_resolution = qts::time_scale::MILLIS;
    current_time_uncertainty = 1;
    current_price_decimal_count = 5;
    current_volume_decimal_count = 2;
    calculate_point_factors();
    clear(prev_raw_tick);
    setup_new_output_page();
}

inline byte * QSDF_Ticks_T23M::read_page_header(byte * rdptr)
{
    _DPn("T23MFWTicks::read_page_header()");
    const char * foo = nullptr;
    foo = read_string(rdptr);
    if (strcmp(foo, broker_id.c_str()) != 0) {
        cerr << "The file is not formatted correctly. Broker-id didn't match: '"
             << broker_id << "' vs '" << foo << "'\n";
        throw 99;
    }
    foo = read_string(rdptr);
    if (strcmp(foo, symbol.c_str()) != 0) {
        cerr << "The file is not formatted correctly. Broker-id didn't match: '"
             << symbol << "' vs '" << foo << "'\n";
        throw 98;
    }
    current_time_resolution =
        (qts::time_scale)read_varilen_natural<int>(rdptr);
    current_time_uncertainty = read_varilen_natural<int>(rdptr);
    current_price_decimal_count = read_varilen_natural<int>(rdptr);
    current_volume_decimal_count = read_varilen_natural<int>(rdptr);
    calculate_point_factors();
    _DPn("T23MFWTicks::read_page_header() DONE");
    return rdptr;
}

inline byte * QSDF_Ticks_T23M::write_page_header(byte * wrpos)
{
    _DP("T23MFWTicks::write_page_header()\n");
    current_time_resolution = qts::time_scale::MILLIS;
    current_time_uncertainty = 100; // 50;
    current_price_decimal_count = 5;
    current_volume_decimal_count = 2;
    write_string(wrpos, broker_id.c_str());
    write_string(wrpos, symbol.c_str());
    write_varilen_natural<int>(wrpos, (int)current_time_resolution);
    write_varilen_natural(wrpos, current_time_uncertainty);
    write_varilen_natural(wrpos, current_price_decimal_count);
    write_varilen_natural(wrpos, current_volume_decimal_count);
    _DPn("T23MFWTicks::write_page_header() DONE");
    return wrpos;
}
