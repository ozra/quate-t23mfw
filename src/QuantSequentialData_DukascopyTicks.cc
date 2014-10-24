#include "QuantSequentialData_DukascopyTicks.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantSequentialData.hh"

#include "ninety47/dukascopy.h"
//#include <boost/posix_time.hpp>
#include <algorithm>
#include <vector>
#include "ninety47/dukascopy/defs.h"
#include "ninety47/dukascopy/lzma.h"

class QuantSequentialData_DukascopyTicks : public QuantSequentialDataAbstract
{
  public:
    QuantSequentialData_DukascopyTicks () {};
    ~QuantSequentialData_DukascopyTicks () {};

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

    pxt::ptime      chunk_time_pos;
    //pxt::ptime      chunk_epoch;

    size_t          buf_pos = 0;
    size_t          byte_buffer_size = 0;
    byte            *byte_buffer = nullptr;

    /*
    //n47::tick_data  *buffer = nullptr;
    byte *raw_buffer = nullptr;
    size_t raw_buffer_capacity;
    size_t raw_buffer_size;

    byte *decode_buffer = nullptr;
    size_t decode_buffer_capacity;
    size_t decode_buffer_size;
    */

};

#endif




template <typename T>
T *loadToExistingBuffer(fs::path filename, T *buffer, size_t *buf_byte_size, int *status) {
    //T *buffer = 0;
    //fs::path file(filename);

    if ( fs::exists( filename ) && fs::is_regular( filename ) ) {
        size_t new_size = fs::file_size( filename );

        if ( new_size == 0 ) {
            *status = 0;
            *buf_byte_size = 0;
            return 0;
        }

        cerr << "loadToExistingBuffer: " << new_size << "(" << *buf_byte_size << ")" << "\n";
        if ( new_size > *buf_byte_size ) {
            cerr << "loadToExistingBuffer: delete and create new buffer\n";
            delete[] buffer;
            buffer = new T[new_size];
            *buf_byte_size = new_size;
        }

        //std::ifstream fin(filename, std::ifstream::binary);
        fs::ifstream fin(filename, fs::ifstream::binary);
        if ( fin ) {
            cerr << "loadToExistingBuffer: reads file in to buffer\n";
            fin.read(reinterpret_cast<char*>(buffer), *buf_byte_size);
        }
        fin.close();

        *status = 0;

    } else {
        buffer = 0;
        *buf_byte_size = 0;
        *status = -2;
    }

    return buffer;
}



bool QuantSequentialData_DukascopyTicks::doInit (
    std::string p_symbol,
    double p_point_decimals
) {
    symbol = p_symbol;
    point_decimals = p_point_decimals;

    chunk_time_pos = start_date; //makeTime( "2014-01-07 00:00:00.000" );
    // *TODO* *IMPL* spola fram till första tillgängliga fil
    // ASSUMED for now!!!
    return true;   // error *TODO* *IMPL*
}


Str fuglyIntToStr(int n, int sizing) {
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

int QuantSequentialData_DukascopyTicks::loadNextChunk ()
{
    cerr << "loadNextChunk()" << "\n";

    //pxt::ptime chunk_epoch(boost) = makeTime( "2014-01-07 00:00:00.000" ); // pxt::date( 2014, 01, 07 ) );

    cerr << "start_date is " << to_iso_string(start_date) << "\n";
    cerr << "end_date is " << to_iso_string(end_date) << "\n";

    dt::date dts( chunk_time_pos.date() );
    dt::greg_year_month_day ymd = dts.year_month_day();

    cerr << "date is " << to_iso_string(dts) << "\n";

    fs::path
    filename = "/usr/local/lib/T23MFW/";
    filename/= "tickdata/";
    filename/= symbol;
    filename/= fuglyIntToStr( ymd.year, 1000 );
    filename/= fuglyIntToStr( ymd.month - 1, 10 );
    filename/= fuglyIntToStr( ymd.day, 10 );
    filename/= fuglyIntToStr( chunk_time_pos.time_of_day().hours(), 10 );
    filename+= "h_ticks.bi5";

    cerr << "The freaking filename is " << filename  << "\n";

    chunk_time_pos += pxt::hours(1);

    buf_pos = 0;
    delete[] byte_buffer;
    byte_buffer = nullptr;
    byte_buffer_size = 0;

    int status = 0;

    size_t ret_buffer_size = 0;
    unsigned char *ret_buffer = nullptr;
    ret_buffer = loadToExistingBuffer<unsigned char>(
        filename,
        ret_buffer,
        &ret_buffer_size,
        &status
    );

    if ( ret_buffer == 0 ) {
        byte_buffer = nullptr;
        byte_buffer_size = 0;

        if ( status == -2 ) {
            cerr << "No file found to read - shit fucked up" << "\n";
            return -2;
        } else {
            cerr << "File was empty - expectable" << "\n";
            return -1;
        }
    }

    if ( n47::lzma::bufferIsLZMA(ret_buffer, ret_buffer_size) ) {
        // decompress
        cerr << "Is LZMA buffer" << "\n";
        size_t decoded_bytes = 0;

        unsigned char *buffer = n47::lzma::decompress(
            ret_buffer,
            ret_buffer_size,
            &status,
            &decoded_bytes
        );

        cerr << "LZMA decompres done!" << "\n";

        delete[] ret_buffer;

        if (status != N47_E_OK) {
            cerr << "LZMA decoding didn't work out!" << "\n";
            decoded_bytes = 0;
            byte_buffer = nullptr;
            return -2;
        }

        byte_buffer = buffer;
        byte_buffer_size = decoded_bytes;

    } else {
        cerr << "Is straight BINARY buffer" << "\n";
        byte_buffer = ret_buffer;
        byte_buffer_size = ret_buffer_size;
    }

    cerr << "All decoding done, size is: " << byte_buffer_size << "\n";

    return 0;
}

bool QuantSequentialData_DukascopyTicks::readTick ( QuantTick &tick )
{
    //cerr << "readTick()" << "\n";

    if ( buf_pos >= byte_buffer_size ) {
        int retries = session_break_duration_in_chunk_units + 1;   // session break...
        int ret;

        while ( --retries > 0 ) {
            ret = loadNextChunk();

            if ( ret == 0 ) {
                break;

            } else if ( ret == -1 ) {   // -1 = "expected 'error'"
                ++retries;
            }
        }

        if ( ret == -2 ) {
            cerr << "no more data\n";
            return false;   // *TODO* NO MORE DATA...
        }

        buf_pos = 0;
    }

    //cerr << "get tick reference" << "\n";

    n47::bytesTo<unsigned int, n47::BigEndian> bytesTo_unsigned;
    n47::bytesTo<float, n47::BigEndian> bytesTo_float;

    byte *buf_ptr = (byte_buffer + buf_pos);
    //unsigned int pos = buf_pos;

    //unsigned int ts = bytesTo_unsigned(buf_ptr);
    // ms = pxt::millisec(ts);

    //tick.time = chunk_epoch + pxt::millisec( bytesTo_unsigned( buf_ptr ) ); // + ms;
    tick.time = chunk_time_pos + pxt::millisec( bytesTo_unsigned( buf_ptr ) ); // + ms;

    // *TODO* HIGHLY FLAWED! Basis stepping on uint size for all kinds of
    // datatypes that are based on FILE FORMAT spec - not architecture!!!
    buf_ptr += sizeof(unsigned int);

    tick.last_price =
    tick.ask = bytesTo_unsigned(buf_ptr) * point_decimals;
    buf_ptr += sizeof(unsigned int);

    tick.bid = bytesTo_unsigned(buf_ptr) * point_decimals;
    buf_ptr += sizeof(unsigned int);

    // ask_vol
    tick.ask_volume = bytesTo_float(buf_ptr);
    buf_ptr += sizeof(unsigned int);

    // bid_vol
    tick.bid_volume = bytesTo_float(buf_ptr);

    // This could ofcourse easily be removed by just keeping the buf_ptr walking
    buf_pos += n47::ROW_SIZE;

    //cerr << "QuantSequentialData_DukascopyTicks::readTick: " << tick.time << " ask: " << tick.ask << " bid: " << tick.bid << "\n";

    return true;
}









/*
tick *tickFromBuffer(
    unsigned char *buffer,
    pxt::ptime epoch,
    double digits,
    size_t offset
) {
    bytesTo<unsigned int, n47::BigEndian> bytesTo_unsigned;
    bytesTo<float, n47::BigEndian> bytesTo_float;

    unsigned int ts = bytesTo_unsigned(buffer + offset);
    QuantDuration ms = pxt::millisec(ts);
    unsigned int ofs = offset + sizeof(ts);

    float ask = bytesTo_unsigned(buffer + ofs) * digits;
    ofs += sizeof(ts);

    float bid = bytesTo_unsigned(buffer + ofs) * digits;
    ofs += sizeof(ts);

    float askv = bytesTo_float(buffer + ofs);
    ofs += sizeof(ts);

    float bidv = bytesTo_float(buffer + ofs);

    return new tick(epoch, ms, ask, bid, askv, bidv);
}
*/

/*
tick_data* read_bin(
    unsigned char *buffer,
    size_t buffer_size,
    pxt::ptime epoch,
    double point_decimals
) {
    std::vector<tick*> *data = new std::vector<tick*>();
    std::vector<tick*>::iterator iter;

    std::size_t offset = 0;

    while ( offset < buffer_size ) {
        data->push_back(tickFromBuffer(buffer, epoch, point_decimals, offset));
        offset += ROW_SIZE;
    }

    return data;
}


tick_data* read_bi5(
    unsigned char *lzma_buffer,
    size_t lzma_buffer_size,
    pxt::ptime epoch,
    double point_decimals,
    size_t *bytes_read
) {
    tick_data *result = 0;

    // decompress
    int status;
    unsigned char *buffer = n47::lzma::decompress(
        lzma_buffer,
        lzma_buffer_size,
        &status,
        bytes_read
    );

    if (status != N47_E_OK) {
        bytes_read = 0;
    } else {
        // convert to tick data (with read_bin).
        result = read_bin(buffer, *bytes_read, epoch, point_decimals);
        delete [] buffer;
    }

    return result;
}


tick_data* read(
    const char *filename,
    pxt::ptime epoch,
    double point_decimals,
    size_t *bytes_read
) {
    tick_data *result = 0;
    size_t buffer_size = 0;




    ladda fil till pooled-raw-buffer

    if is lzma buf
        decode till pooled-decode-buffer
        out-buffer = pooled-decode-buffer
    else
        out-buffer = pooled-raw-buffer

    decode in to passed QuantTick buffer

    return




    unsigned char *ret_buffer = loadToExistingBuffer<unsigned char>(filename, buffer, &buffer_size);

    if ( ret_buffer != 0 ) {
        if ( n47::lzma::bufferIsLZMA(ret_buffer, ret_buffer_size) ) {
            result = read_bi5(ret_buffer, ret_buffer_size, epoch, point_decimals, bytes_read);
            // Reading in as bi5 failed lets double check its not binary
            // data in the ret_buffer.
            if (result == 0) {
                result = read_bin(ret_buffer, ret_buffer_size, epoch, point_decimals);
            }
        } else {
            result = read_bin(ret_buffer, ret_buffer_size, epoch, point_decimals);
            *bytes_read = ret_buffer_size;
        }
        delete [] ret_buffer;

        if (result != 0 && result->size() != (*bytes_read / n47::ROW_SIZE)) {
            delete result;
            result = 0;
        }
    }
    return result;
}

*/
