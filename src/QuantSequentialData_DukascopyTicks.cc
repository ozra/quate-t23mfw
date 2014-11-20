#include "QuantSequentialData_DukascopyTicks.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#undef DESIGN_CHOICE__CLEAR_FILING_BUFFERS

#include "QuantBasic_DESIGN_CHOICES.hh"

#include "QuantTime.hh"
#include "QuantSequentialData_TradeTicksAbstract.hh"
#include "QuantSequentialDataPrefetcher.hh"

#include "MutatingString.hh"
#include "mutatable_buffer.hh"

//#include <boost/posix_time.hpp>
#include <algorithm>
#include <vector>

#include "ninety47/dukascopy.h"
#include "ninety47/dukascopy/defs.h"
#include "ninety47/dukascopy/lzma.h"

class QuantSequentialData_DukascopyTicks
    : public QuantSequentialData_TradeTicksAbstract {
   public:
    QuantSequentialData_DukascopyTicks()
        : raw_buffer_((size_t)5000) //,
        // decode_buffer ( (size_t) 10000 )
        {};

    ~QuantSequentialData_DukascopyTicks() {
        // delete[] file_buffer;
        delete[] decode_buffer_ptr;
    };

    void reset_time_pos();
    bool init(std::string p_broker_id, std::string p_symbol);
    bool readTick(QuantTick& tick) final;

   private:
    arbitrary_numeric_code loadNextChunk();

    const N session_break_duration_in_chunk_units = 24 * 2;
    std::string symbol;
    double point_decimals;

    pxt::ptime chunk_time_pos_;
    pxt::time_duration chunk_unit_duration_;
    // pxt::ptime      chunk_epoch;

    size_t byte_buffer_size = 0;
    byte* byte_buffer = nullptr;
    byte* byte_buffer_end = nullptr;
    byte* buf_ptr = nullptr;
    // ofs_t           buf_pos = 0;

    mutatable_buffer<B> raw_buffer_;
    // mutatable_buffer<uint8_t>   decode_buffer;
    Bp decode_buffer_ptr = nullptr;

    MutatingString filename_mutator;
};

#endif

#include "QuantProfiling.hh"

bool QuantSequentialData_DukascopyTicks::init(std::string p_broker_id,
                                              std::string p_symbol) {
    symbol = p_symbol;
    point_decimals =
        0.00001; // *TODO* is 0.0001 for some...   // p_point_decimals;

    chunk_unit_duration_ = pxt::hours(1);
    reset_time_pos();

    _DP("QuantSequentialData_DukascopyTicks::init():"
        << "\n"
        << "start_date: " << start_date_ << "\n"
        << "chunk_time_pos_: " << chunk_time_pos_ << "\n");

    filename_mutator.init((string("/usr/local/lib/T23MFW/rawdata/DUKASCOPY/")
                               .append(symbol)
                               .append("/YYYY/MM/DD/HHh_ticks.bi5")));
    filename_mutator.addVar("YYYY").addVar("MM").addVar("DD").addVar("HH");

    // *TODO* *IMPL* spola fram till första tillgängliga fil
    // ASSUMED for now!!!

    return true; // error *TODO* *IMPL*
}

void QuantSequentialData_DukascopyTicks::reset_time_pos() {
    chunk_time_pos_ =
        qts::get_prior_aligned_ts(start_date_, chunk_unit_duration_) -
        chunk_unit_duration_; // makeTime( "2014-01-07 00:00:00.000" );
}

/*
Str fuglyIntToStr( int n, int sizing ) {
    Str out;
    char one_char[2];
    one_char[1] = 0;

    while ( sizing > 0 ) {
        int foo = n / sizing;
        one_char[0] = 48 + foo;
        out += one_char;
        n -= foo * sizing;
        sizing /= 10;
    }

    return out;
}
*/

arbitrary_numeric_code QuantSequentialData_DukascopyTicks::loadNextChunk() {
    //#ifdef IS_DEEPBUG
    cerr << "loadNextChunk()"
         << "\n";
    //#endif

    // pxt::ptime chunk_epoch(boost) = makeTime( "2014-01-07 00:00:00.000" ); //
    // pxt::date( 2014, 01, 07 ) );

    // cerr << "start_date is " << to_iso_string(start_date) <<
    // "chunk_time_pos_r << "end_date is " << to_iso_string(end_date) << "\n";

    chunk_time_pos_ += chunk_unit_duration_;

#ifdef IS_DEEPBUG
    cerr << "chunk_time_pos = " << to_iso_string(chunk_time_pos_) << "\n";
#endif

    dt::date dts(chunk_time_pos_.date());
    dt::greg_year_month_day ymd = dts.year_month_day();

#ifdef IS_DEEPBUG
    cerr << "date is " << to_iso_string(dts) << "\n";
#endif

    const char* fname =
        (filename_mutator << ymd.year << (ymd.month - 1) << ymd.day
                          << chunk_time_pos_.time_of_day().hours())
            .mutated_str();

#ifdef IS_DEEPBUG
    cerr << "The data chunk filename is " << fname << "\n";
#endif

#ifdef DESIGN_CHOICE__CLEAR_FILING_BUFFERS
    raw_buffer_.wipe();
// decode_buffer.wipe();

#endif

    // int     ret_status      = 0;
    /*
    size_t  ret_buffer.size() = raw_buffer__size;
    uint8_t *ret_buffer     = raw_buffer_;
    */

    // mutatable_buffer<uint8_t> ret_buffer;

    cerr << "calls loadFileToExistingBuffer\n";
    // mutatable_buffer<uint8_t> ret_buffer = loadFileToExistingBuffer(
    enum_load_result ret = loadFileToExistingBuffer(fname, raw_buffer_);
    cerr << "back from call to loadFileToExistingBuffer\n";

    // if ( ret_buffer == nullptr ) {
    //    byte_buffer_end = byte_buffer = nullptr;
    //    byte_buffer_size = 0;

    // <|> ret
    //
    // | MISSING_FILE
    //     do_shit()
    //
    // | EMPTY_FILE
    //     do_other_stuff()

    if (ret == MISSING_FILE) {
        cerr << "No file found to read - shit fucked up"
             << "\n";
#ifdef IS_DEEPBUG
        throw 474747;
#endif
        return -2;

    } else if (ret == EMPTY_FILE) {
#ifdef IS_DEEPBUG
        cerr << "File was empty - expectable"
             << "\n";
#endif
        return -1;
    }
// else status == 0 som det ska vara...

#ifdef IS_DEEPBUG
    cerr << "BEFORE decoding byte size is: " << raw_buffer_.size() << "ptr is "
         << (void*)raw_buffer_.front() << "\n";
    cerrbug_a_buffer(raw_buffer_.front(), raw_buffer_.size());

#endif

    if (decode_buffer_ptr) {
        delete[] decode_buffer_ptr;
    }

    // The n47::lzma::bufferIsLZMA function did NOT turn out all right! It
    // misses small buffers ( leading to devastating results )
    if (true) { // n47::lzma::bufferIsLZMA(ret_buffer, ret_buffer.size()) ) {
// decompress
#ifdef IS_DEEPBUG
        cerr << "Is LZMA buffer"
             << "\n";
#endif

        size_t decoded_bytes = 0;
        arbitrary_numeric_code ret_status = 0;

        decode_buffer_ptr =
            n47::lzma::decompress(raw_buffer_.front(), raw_buffer_.size(),
                                  &ret_status, &decoded_bytes);

#ifdef IS_DEEPBUG
        cerr << "LZMA decompress done!"
             << "\n";
#endif

        if (ret_status != N47_E_OK) {
            cerr << "LZMA decoding didn't work out!"
                 << "\n";
            decoded_bytes = 0;
            decode_buffer_ptr = nullptr;
            byte_buffer = nullptr;
            byte_buffer_end = nullptr;
            return -2;
        }

        byte_buffer = decode_buffer_ptr;
        byte_buffer_size = decoded_bytes;

    } else {
#ifdef IS_DEEPBUG
        cerr << "Is straight BINARY buffer"
             << "\n";
#endif
        decode_buffer_ptr = nullptr;
        byte_buffer = raw_buffer_.front();
        byte_buffer_size = raw_buffer_.size();
    }

    byte_buffer_end = byte_buffer + byte_buffer_size;

#ifdef IS_DEEPBUG
    cerr << "All decoding done, byte size is: " << byte_buffer_size << "\n";
    cerr << "'unit'-size is: " << (byte_buffer_size / (4 * 5.0)) << "\n";
    cerrbug_a_buffer(byte_buffer, byte_buffer_size);
#endif

    return 0;
}

/*
template <typename T>
inline T readBigEndian32 ( const unsigned char *buffer ) {
    T value;
    unsigned char *val_ptr = reinterpret_cast<unsigned char *>(&value);
    val_ptr[0] = buffer[3];
    val_ptr[1] = buffer[2];
    val_ptr[2] = buffer[1];
    val_ptr[3] = buffer[0];

    //cerr << "readBigEndian32() gave: " << value << "\n";

    return value;
}
*/

/*
template <typename T>
inline void loadBigEndian32To ( T *val_ptr, const unsigned char *buffer ) {
    //T value;
    //unsigned char *val_ptr = reinterpret_cast<unsigned char *>(&value);
    reinterpret_cast<unsigned char *>(val_ptr)[0] = buffer[3];
    reinterpret_cast<unsigned char *>(val_ptr)[1] = buffer[2];
    reinterpret_cast<unsigned char *>(val_ptr)[2] = buffer[1];
    reinterpret_cast<unsigned char *>(val_ptr)[3] = buffer[0];
    //return value;
}
*/

bool QuantSequentialData_DukascopyTicks::readTick(QuantTick& tick) {
    // cerr << "readTick()" << "\n";
    //#ifdef IS_DEBUG
    profiler.start(DECODING_FILES);
    //#endif

    // *TODO* move this stuff out since it'l be used more seldom...
    if (buf_ptr == byte_buffer_end) {
        N retries =
            session_break_duration_in_chunk_units + 1; // session break...
        arbitrary_numeric_code ret = 0; // silence compiler warnings.. could be done with pragmas
                     // too though..

        while (--retries > 0) {
            ret = loadNextChunk();

            if (ret == 0) {
                break;
            } else if (ret == -1) { // -1 = "expected 'error'"
                ++retries;
            }
            // else if ( ret == -2 )    // out of data error or similar
        }

        if (ret == -2) {
            cerr << "no more data - sets max time on tick.time\n";
            tick.time = pxt::max_date_time;
            return false; // *TODO* NO MORE DATA...
        }

        buf_ptr = byte_buffer;
    }

// cerr << "get tick reference" << "\n";

#ifndef DESIGN_CHOICE__N47_BIGENDIAN_DECODERS

    // tick.time = chunk_epocchunk_time_pos_sec( bytesTo_unsigned( buf_ptr ) );
    // // + ms;
    tick.time =
        chunk_time_pos_ + pxt::millisec(readBigEndian32<N32>(buf_ptr));
    buf_ptr += 4;
    tick.last_price = tick.ask =
        readBigEndian32<N32>(buf_ptr) * point_decimals;
    buf_ptr += 4;
    tick.bid = readBigEndian32<N32>(buf_ptr) * point_decimals;
    buf_ptr += 4;
    tick.ask_volume = readBigEndian32<R32>(buf_ptr);
    buf_ptr += 4;
    tick.bid_volume = readBigEndian32<R32>(buf_ptr);
    buf_ptr += 4;

//      cerr << "Specific decode Tick is: " << tick.ask << ", " << tick.bid <<
// ", " << tick.ask_volume << " at " << 47 << "/" << byte_buffer_size << "\n";

// cerr << "QuantSequentialData_DukascopyTicks::readTick: " << tick.time << "
// ask: " << tick.ask << " bid: " << tick.bid << "\n";

#else

    n47::bytesTo<unsigned int, n47::BigEndian>
    bytesTo_unsignechunk_time_pos_tesTo<float, n47::BigEndian> bytesTo_float;

    tick.time = chunk_time_pos_ + pxt::millisec(bytesTo_unsigned(buf_ptr));

    // *TODO* HIGHLY FLAWED! Basis stepping on uint size for all kinds of
    // datatypes that are based on FILE FORMAT spec - not architecture!!!
    buf_ptr += sizeof(unsigned int);

    tick.last_price = tick.ask = bytesTo_unsigned(buf_ptr) * point_decimals;
    buf_ptr += sizeof(unsigned int);

    tick.bid = bytesTo_unsigned(buf_ptr) * point_decimals;
    buf_ptr += sizeof(unsigned int);

    // ask_vol
    tick.ask_volume = bytesTo_float(buf_ptr);
    buf_ptr += sizeof(unsigned int);

    // bid_vol
    tick.bid_volume = bytesTo_float(buf_ptr);

    buf_ptr += sizeof(unsigned int);
//      cerr << "n47::generic decode Tick is: " << tick.ask << ", " << tick.bid
// << ", " << tick.ask_volume << " at " << 47 << "/" << byte_buffer_size <<
// "\n";
// buf_ pos += n47::ROW_SIZE;
// cerr << "QuantSequentialData_DukascopyTicks::readTick: " << tick.time << "
// ask: " << tick.ask << " bid: " << tick.bid << "\n";

#endif

    //#ifdef IS_DEEPBUG
    // cerr << symbol << "::readTick(): " << tick.to_str() << " (end - ptr) = "
    // << (byte_buffer_end - buf_ptr) << "\n";
    //
    //#endif

    //#ifdef IS_DEBUG
    profiler.end(DECODING_FILES);
    //#endif

    return true;
}
