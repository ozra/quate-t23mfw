#include "QuantSequentialData_T23MFWTicks.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-12
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <cmath>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "QuantBase.hh"
#include "QuantTime.hh"
#include "MutatingString.hh"
#include "mutatable_buffer.hh"

#include "QuantSequentialData_TradeTicksAbstract.hh"
#include "QuantSequentialDataPrefetcher.hh"

#include <algorithm>
#include <vector>

#include <boost/format.hpp>

class QuantSequentialData_T23MFWTicks
    : public QuantSequentialData_TradeTicksAbstract {
   public:
    QuantSequentialData_T23MFWTicks(bool write_mode = false)
        : write_mode_{ write_mode } {
        if (write_mode) {
            size_t prel_buf_size = 32 * 1024;
            raw_buffer.reserve(
                prel_buf_size +
                1024); // Add 1024k "soft buffer overrun" space...
            raw_buffer.set_specified_buffer_size(prel_buf_size);
            raw_buffer.reset_write_cursor();
        } else {
            size_t prel_buf_size = 500000;
            raw_buffer.reserve(prel_buf_size);
            raw_buffer.reset_read_cursor();
        }
    }

    ~QuantSequentialData_T23MFWTicks() {};

    bool init(std::string p_broker_id, std::string p_symbol) final;
    // void setHistoryMode (); // enables delayed writes / batching
    // void setLiveMode ();    // makes all writes etc. immediate to ensure
    // losslessness
    // void setAutoSave ();
    // inline void setWriteMode ( bool that = true ) {
    //    write_mode_ = true;
    //}
    inline void setBatchMode(bool that = true) { batched_mode_ = that; }

    inline bool readTick(QuantTick& tick) final;
    inline bool writeTick(QuantTick& tick);
    void flush_writes(); // make sure everything is written to disk.

    /*
    enum FlagsEnum {
        UNDEF = 0,
        HISTORY_MODE,
        LIVE_MODE,

    };

    QuantSequentialData_T23MFWTicks& operator|( FlagsEnum what ) {
        switch ( what ) {
            case HISTORY_MODE:
                batched_mode_ = true;
                break;
            case LIVE_MODE:
                batched_mode_ = false;
                break;
            case UNDEF: break;
        }
        return *this;
    }
    */

   private:
    // const char *    generate_filename ( pxt::ptime time_pos );
    auto generate_filename(pxt::ptime time_pos) -> std::string; // const char *;

    bool fetch_more_data();
    arbitrary_numeric_code load_next_chunk();
    void read_chunk_header();

    void init_next_chunk(); // When working with outgoing data
    void write_chunk_header();
    void write_to_disk_if_timely();
    arbitrary_numeric_code write_to_disk();
    arbitrary_numeric_code open_file_for_writing();
    arbitrary_numeric_code close_file_for_writing();
    void calculate_point_factors();

    bool batched_mode_ = true;
    bool write_mode_ = false;
    const N session_break_duration_in_chunk_units = 2;
    std::string broker_id;
    std::string symbol;

    R price_point_factor;
    R volume_point_factor;
    R price_point_div_factor;
    R volume_point_div_factor;

    // MutatingString      filename_mutator;
    boost::format filename_formatter;

    pxt::ptime chunk_time_pos;
    pxt::ptime next_chunk_time_pos;
    pxt::time_duration chunk_unit_duration;

    fs::ofstream out_file;
    mutatable_buffer<u8> raw_buffer;
    // u8* rdptr = nullptr;
    // u8                          *wrpos = nullptr;

    QuantTickFixed prev_raw_tick;

    qts::enum_resolution current_time_resolution =
        qts::MILLIS; // qts::enum_resolution::MILLIS;
    N current_time_uncertainty;
    N current_price_decimal_count;
    N current_volume_decimal_count;
};

#define profiling profiler
inline void start(Profiling& p, ProfileEnum what) { p.start(what); }
inline void stop(Profiling& p, ProfileEnum what) { p.end(what); }

inline bool QuantSequentialData_T23MFWTicks::readTick(QuantTick& tick) {
/*
//_DPn("readTick()");
*/
#ifdef IS_DEBUG
    profiler.start(DECODING_FILES);
#endif

    /*
        //_DPn(1);
    */
    // u8* rdptr = raw_buffer.on_free_leash_for(100);
    // if (rdptr == raw_buffer.end()) {
    if (raw_buffer.is_at_end()) {
        if (fetch_more_data() == false) {
            tick.time = pxt::max_date_time;
            return false;
        }
    }

    u8* rdptr = raw_buffer.on_free_leash_for(100);

    /*
        //_DPn(2);

        _DP("raw_buffer is\n"
            << " size: " << raw_buffer.size() << "\n"
            << " capacity: " << raw_buffer.capacity() << "\n"
            << " start: " << (void*)raw_buffer.front() << "\n"
            << " end: " << (void*)raw_buffer.end() << "\n"
            << " limit: " << (void*)raw_buffer.limit() << "\n");
    */

    // raw_buffer.verify_pointer(rdptr);

    prev_raw_tick.time += read_unsigned_only_varint<N64>(
        rdptr); // Time moves forward only - hence unsigned_varint
    //_DPn(22);
    prev_raw_tick.ask += read_varint<Z>(rdptr);
    //_DPn(23);
    prev_raw_tick.bid += read_varint<Z>(rdptr);
    //_DPn(24);
    prev_raw_tick.ask_volume += read_varint<Z>(rdptr);
    //_DPn(25);
    prev_raw_tick.bid_volume += read_varint<Z>(rdptr);

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
    tick.bid_volume = prev_raw_tick.ask_volume * volume_point_div_factor;

//      cerr << "Specific decode Tick is: " << tick.ask << ", " << tick.bid
// <<
// ", " << tick.ask_volume << " at " << 47 << "/" << byte_buffer_size <<
// "\n";

// cerr << "QuantSequentialData_T23MFWTicks::readTick: " << tick.time
//     << " ask:" << tick.ask << " bid: " << tick.bid << "\n";

//#ifdef IS_DEEPBUG
// cerr << symbol << "::readTick(): " << tick.to_str() << " (end - ptr) = " <<
// (byte_buffer_end - rdptr) << "\n";
//#endif

#ifdef IS_DEBUG
    profiler.end(DECODING_FILES);
#endif

    return true;
}

inline bool QuantSequentialData_T23MFWTicks::writeTick(QuantTick& tick) {
    _D("writeTick()"
       << "\n");
    //#ifdef IS_DEBUG
    start(profiling, ENCODING_DATA);
    // profiler.start( ENCODING_DATA );
    //#endif

    _DP(" 1 ");

    if (tick.time >= next_chunk_time_pos) {
        cerr << "\n\nPassed chunk time break - init a new chunk aight!\n\n";
        init_next_chunk();
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

    u8* wrpos = raw_buffer.on_free_leash_for(100);

    raw_buffer.verify_pointer(wrpos);

    _DP(" 4.1 ");

    // Time moves forward only - hence unsigned_varint
    write_unsigned_only_varint<N64>(wrpos, raw_tick.time - prev_raw_tick.time);
    _DP(" 4.2 ");
    raw_buffer.verify_pointer(wrpos);
    write_varint(wrpos, raw_tick.ask - prev_raw_tick.ask);
    _DP(" 4.3");
    raw_buffer.verify_pointer(wrpos);
    write_varint(wrpos, raw_tick.bid - prev_raw_tick.bid);
    _DP(" 4.4 ");
    raw_buffer.verify_pointer(wrpos);
    write_varint(wrpos, raw_tick.ask_volume - prev_raw_tick.ask_volume);
    _DP(" 4.5 ");
    raw_buffer.verify_pointer(wrpos);
    write_varint(wrpos, raw_tick.bid_volume - prev_raw_tick.bid_volume);
    _DP(" 4.6 ");

    _DP(" 5 ");

    raw_buffer.verify_pointer(wrpos);

    raw_buffer.catch_up_writes(wrpos);

    _DP(" 6 ");

    raw_buffer.verify_pointer(wrpos);

    // By using pointers we could simply swap the pointers in the vars here for
    // further efficiancy...
    prev_raw_tick = raw_tick;

    _DP(" 7 ");

    write_to_disk_if_timely();

    _DP(" 8 ");

    raw_buffer.verify_pointer(wrpos);

    //      cerr << "Specific decode Tick is: " << tick.ask << ", " << tick.bid
    // << ", " << tick.ask_volume << " at " << 47 << "/" << byte_buffer_size <<
    // "\n";
    // cerr << "QuantSequentialData_T23MFWTicks::readTick: " << tick.time << "
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

void clear(QuantTickFixed& tick) {
    tick.time = tick.ask = tick.bid = tick.ask_volume = tick.bid_volume =
        tick.last_price = 0;
}

bool QuantSequentialData_T23MFWTicks::init(std::string p_broker_id,
                                           std::string p_symbol) {
    broker_id = p_broker_id;
    symbol = p_symbol;
    chunk_unit_duration = pxt::hours(24);
    chunk_time_pos =
        qts::get_prior_aligned_ts(start_date_, chunk_unit_duration) -
        chunk_unit_duration; // makeTime( "2014-01-07 00:00:00.000" );
    next_chunk_time_pos = chunk_time_pos + chunk_unit_duration;

#ifdef IS_DEEPBUG
    cerr << "QuantSequentialData_T23MFWTicks::init():"
         << "\n"
         << "start_date: " << start_date_ << "\n"
         << "chunk_time_pos: " << chunk_time_pos << "\n";
#endif

    /*
    filename_mutator.init( string("/usr/local/lib/T23MFW/tickdata/").append(
    broker_id ).append("/").append( symbol ).append( "/YYYY/MM/DD_ticks.t23mtf"
    ) );
    filename_mutator
        .addVar( "YYYY" )
        .addVar( "MM" )
        .addVar( "DD" )
        //.addVar( "HH" )
    ;
    */

    std::string base_format =
        (boost::format("/usr/local/lib/T23MFW/tickdata/%s/%s/%%04d/%%02d/"
                       "%%02d_ticks.t23mtf") %
         broker_id % symbol).str();
    cerr << "std::string base_format = boost::format = " << base_format << "\n";
    filename_formatter = boost::format(base_format);

    // *TODO* *IMPL* spola fram till första tillgängliga fil
    // ASSUMED for now!!!

    return true; // error *TODO* *IMPL*
}

inline void QuantSequentialData_T23MFWTicks::calculate_point_factors() {
    price_point_factor = pow(10, current_price_decimal_count);
    price_point_div_factor = 1.0 / price_point_factor;

    volume_point_factor = pow(10, current_volume_decimal_count);
    volume_point_div_factor = 1.0 / volume_point_factor;
}

auto QuantSequentialData_T23MFWTicks::generate_filename(pxt::ptime time_pos)
    -> std::string { // const char * {
    auto ymd = dt::date(time_pos.date()).year_month_day();
    return (filename_formatter % ymd.year % (Z)ymd.month % ymd.day)
        .str(); // .c_str();
                /*
return ( filename_mutator
<< ymd.year
<< ymd.month
<< ymd.day
).mutated_str();
*/
}

bool QuantSequentialData_T23MFWTicks::fetch_more_data() {
    assert(write_mode_ == false); // We don't support duplex right now...

    N retries = session_break_duration_in_chunk_units + 1; // session break...
    arbitrary_numeric_code ret =
        0; // silence compiler warnings.. could be done with pragmas too
           // though..

    while (--retries > 0) {
        ret = load_next_chunk();

        if (ret == 0) {
            break;

        } else if (ret == -1) { // -1 = "expected 'error'"
            ++retries;
        }
        // else if ( ret == -2 )    // out of data error or similar
    }

    if (ret == -2) {
        cerr << "no more data - sets max time on tick.time\n";
        return false; // *TODO* NO MORE DATA...
    }

    return true;
}

arbitrary_numeric_code QuantSequentialData_T23MFWTicks::load_next_chunk() {
    //#ifdef IS_DEEPBUG
    cerr << "load_next_chunk()"
         << "\n";
    //#endif

    // pxt::ptime chunk_epoch(boost) = makeTime( "2014-01-07 00:00:00.000" ); //
    // pxt::date( 2014, 01, 07 ) );

    // cerr << "start_date is " << to_iso_string(start_date) << "\n";
    // cerr << "end_date is " << to_iso_string(end_date) << "\n";

    chunk_time_pos += chunk_unit_duration;
    next_chunk_time_pos = chunk_time_pos + chunk_unit_duration;

    //_DPn("chunk_time_pos = " << to_iso_string(chunk_time_pos));
    string fname = generate_filename(chunk_time_pos);

    //_DPn("The data chunk filename is '" << fname);

    cerr << "calls loadFileToExistingBuffer: '" << fname << "'\n";
    enum_load_result ret = loadFileToExistingBuffer(fname.c_str(), raw_buffer);
    cerr << "back from call to loadFileToExistingBuffer\n";

    _DP("raw_buffer is\n"
        << " size: " << raw_buffer.size() << "\n"
        << " capacity: " << raw_buffer.capacity() << "\n");

    if (ret == MISSING_FILE) {
        cerr << "No file found to read - shit fucked up"
             << "\n";
#ifdef IS_DEEPBUG
        throw 474747;
#endif
        return -2;

    } else if (ret == EMPTY_FILE) {
#ifdef IS_DEEPBUG
        cerr << "File was empty - may be expectable"
             << "\n";
#endif
        return -1;
    }
// else status == 0 som det ska vara...

#ifdef IS_DEEPBUG
    cerr << "BEFORE decoding byte size is: " << raw_buffer.size() << " ptr is "
         << (void*)raw_buffer.front() << "\n";
    cerrbug_a_buffer(raw_buffer.front(), raw_buffer.size());

#endif

    // rdptr = raw_buffer.begin();
    // wrpos = raw_buffer.end() - 1;

    clear(prev_raw_tick);
    raw_buffer.reset_read_cursor();
    read_chunk_header();
    calculate_point_factors();

    return 0;
}

void QuantSequentialData_T23MFWTicks::init_next_chunk() {
    //#ifdef IS_DEEPBUG
    cerr << "init_next_chunk() - INIT CHUNK"
         << "\n";
    //#endif
    // pxt::ptime chunk_epoch(boost) = makeTime( "2014-01-07 00:00:00.000" ); //
    // pxt::date( 2014, 01, 07 ) );
    // cerr << "start_date is " << to_iso_string(start_date) << "\n";
    // cerr << "end_date is " << to_iso_string(end_date) << "\n";

    // *TODO* - This feels slightly malplaced
    flush_writes(); // make sure everything for former chunk is done
    close_file_for_writing();

    chunk_time_pos = next_chunk_time_pos;
    next_chunk_time_pos += chunk_unit_duration;

    _DPn("in INIT chunk: chunk_time_pos = " << to_iso_string(chunk_time_pos));

    // const char * fname = generate_filename( chunk_time_pos );
    string fname = generate_filename(chunk_time_pos);

    _DPn("The data chunk filename is '" << fname);

    /*
     *  *TODO*  - open file for writing HERE!
     */

    // raw_buffer.clear();

    _DPn("BEFORE any writings byte size is: "
         << raw_buffer.size() << "capacity is " << raw_buffer.capacity()
         << " ptr is " << (void*)raw_buffer.front());
    cerrbug_a_buffer(raw_buffer.front(), raw_buffer.size());

    // rdptr = nullptr;
    // wrpos = raw_buffer.begin();

    clear(prev_raw_tick);
    write_chunk_header();
    calculate_point_factors();
}

// const Z64 THE_BIG_NEG = -4747474747474747477LL;
// const Z64 THE_BIG_NEG = -2147483647LL;
const Z64 THE_BIG_NEG = -474747474747474747LL;

void QuantSequentialData_T23MFWTicks::read_chunk_header() {
    cerr << "read_chunk_header()\n";

    Bp rdptr = raw_buffer.on_free_leash_for(100);

    Z64 check = 0;

    check = read_unsigned_only_varint<Z>(rdptr);
    assert(check == 47);
    check = read_varint<Z>(rdptr);
    assert(check == -47);
    check = read_unsigned_only_varint<Z>(rdptr);
    assert(check == 71);
    check = read_varint<Z64>(rdptr);
    _Dn("check -BIG = " << check << " should be " << THE_BIG_NEG);
    assert(check == THE_BIG_NEG);

    cAp foo = nullptr;
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
        (qts::enum_resolution)read_unsigned_only_varint<N>(rdptr);
    current_time_uncertainty = read_unsigned_only_varint<N>(rdptr);
    current_price_decimal_count = read_unsigned_only_varint<N>(rdptr);
    current_volume_decimal_count = read_unsigned_only_varint<N>(rdptr);

    check = read_unsigned_only_varint<Z>(rdptr);
    assert(check == 0);
    check = read_unsigned_only_varint<Z>(rdptr);
    assert(check == 47);

    raw_buffer.catch_up_reads(rdptr);

    cerr << "read_chunk_header() DONE\n";
}

void QuantSequentialData_T23MFWTicks::write_chunk_header() {
    _DP("QuSeqDaCon::T23MFWTicks::write_chunk_header()\n");

    u8* wrpos = raw_buffer.on_free_leash_for(100);

    current_time_resolution = qts::enum_resolution::MILLIS;
    current_time_uncertainty = 100; // 50;
    current_price_decimal_count = 5;
    current_volume_decimal_count = 2;

    write_unsigned_only_varint(wrpos, 47);
    write_varint(wrpos, -47);
    write_unsigned_only_varint(wrpos, 71);
    // write_varint( wrpos, -4747474747474747L );
    write_varint<Z64>(wrpos, THE_BIG_NEG);

    write_string(wrpos, broker_id.c_str());
    write_string(wrpos, symbol.c_str());

    write_unsigned_only_varint<N>(wrpos, (N)current_time_resolution);
    write_unsigned_only_varint(wrpos, current_time_uncertainty);
    write_unsigned_only_varint(wrpos, current_price_decimal_count);
    write_unsigned_only_varint(wrpos, current_volume_decimal_count);

    write_unsigned_only_varint(wrpos, 0);
    write_unsigned_only_varint(wrpos, 47);

    raw_buffer.catch_up_writes(wrpos);

    cerr << "QuSeqDaCon::T23MFWTicks::write_chunk_header() DONE\n";
}

void QuantSequentialData_T23MFWTicks::write_to_disk_if_timely() {
    //_DPn("QuSeqDaCon::T23MFWTicks::write_to_disk_if_timely()");

    //_DP("raw_buffer.is_specified_buffer_filled() ? = " <<
    // raw_buffer.is_specified_buffer_filled()
    //    << " spec_limit - front = " << raw_buffer.spec_limit() << "\n"
    //);

    if (batched_mode_ == false || raw_buffer.is_specified_buffer_filled()) {
        write_to_disk();
    } else {
#ifdef IS_DEEPBUG
// cerr << "write_to_disk_if_timely() - Was NOT timely\n";
#endif
    }
}

void QuantSequentialData_T23MFWTicks::flush_writes() {
    _DP("QuSeqDaCon::T23MFWTicks::flush_writes()\n");

    write_to_disk();

    /*
     *
     *
     * *TODO*
     *
     *
     */
}

arbitrary_numeric_code
QuantSequentialData_T23MFWTicks::open_file_for_writing() {
    // const char * filename = generate_filename( chunk_time_pos );
    string filename = generate_filename(chunk_time_pos);

    _DP("QuSeqDaCon::T23MFWTicks::open_file_for_writing,");

    if (true) {
        // *TODO* optimize so that checks only are made for path-parts that has
        // changed!!!
        fs::path dirs(filename);
        dirs.remove_filename();
        _DPn("open_file_for_writing - create missing directories if any "
             << dirs << "\n");
        fs::create_directories(dirs);
    }

    _DP("open_file_for_writing - filename = \"" << filename << "\"\n");

    try {
        out_file.open(filename, fs::ofstream::binary | fs::ofstream::out);

        if (!out_file.is_open()) {
            cerr << "Possible error for fopen: " << strerror(errno) << "\n";
        }
        assert(out_file.is_open());

        // out_file.sync_with_stdio(false);
    }
    catch (ifstream::failure e) {
        e.what();
        assert(true == false);
    }

    _DP("open_file_for_writing - DONE\n");

    return 0;
}

arbitrary_numeric_code
QuantSequentialData_T23MFWTicks::close_file_for_writing() {
    _DP("QuSeqDaCon::T23MFWTicks::close_file_for_writing \n");

    if (out_file.is_open()) {
        out_file.close();
    }
    return 0; // *TODO* remove. throw exception on error instead
}

arbitrary_numeric_code QuantSequentialData_T23MFWTicks::write_to_disk() {
    _DP("QuSeqDaCon::T23MFWTicks::write_to_disk()\n");

    size_t bytes_to_write = raw_buffer.buffered_count(); // non_filed_ptr;    //
    // front_of(raw_buffer); //
    // raw_buffer.begin();

    if (bytes_to_write == 0) return 0;

    if (out_file.is_open() == false) {
        open_file_for_writing();
    }

    // *TEMP*
    raw_buffer.verify_pointer(raw_buffer.front() + bytes_to_write);

    out_file.write(reinterpret_cast<char*>(raw_buffer.begin()), bytes_to_write);
    raw_buffer.consume_specified_buffer();
    // wrpos = raw_buffer.begin();
    raw_buffer.reset_write_cursor();
    out_file.flush();
    cerr << "wrote " << bytes_to_write << "bytes\n";

    return 0;
}
