#include "QuantSequentialData_QuledAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-10
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/


/*
 *
 *
 *
 *    *TODO* - general abstract for Quled encodings (Quantitative Length
 *      Encoded Deltas)
 *
 *  - so that it's piece of cake to make specific implementations
 *
 *
 */



#include "QuantSequentialData.hh"

class QuantSequentialData_QuledAbstract : public QuantSequentialDataAbstract
{
  public:
    QuantSequentialData_QuledAbstract () {};
    virtual ~QuantSequentialData_QuledAbstract () = 0;

    //bool readSample( QuantTick &tick );
    const fs::path getFilenameFromLocator ();


    bool loadChunk ();
    virtual void freshChunkInits () = 0;     // Setup primary deltas
    virtual void readUnit () = 0;            // Read and decode one sample

    /*
     *
     *
     *    *TODO*
     *
     */

  private:
    template <typename T> inline T          read_uint32 ();
    template <typename T> inline T          read_uint64 ();
    template <typename T> inline T          read_int32 ();
    template <typename T> inline T          read_int64 ();
    template <typename T> inline T          read_int_var ();
    inline double                           read_double ();
    inline float                            read_float ();
    template <typename T> inline T          read_uint_rel ();
    template <typename T> inline T          read_int_rel ();
    template <typename T, int N> inline T   read_fixed ();
    template <typename T, int N> inline T   read_fixed_rel ();

    //int loadNextChunk();

    pxt::ptime      chunk_time_pos;
    /*
    std::string     symbol;
    double          point_decimals;


    size_t          buf_pos = 0;
    size_t          byte_buffer_size = 0;
    byte            *byte_buffer = nullptr;
    */

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

std::string ThisFugginFuglyIntToStr( int n, int sizing ) {
    std::string out;
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


const fs::path QuantSequentialData_QuledAbstract::getFilenameFromLocator () {
    dt::date                dts { chunk_time_pos.date() };
    dt::greg_year_month_day ymd { dts.year_month_day() };
    fs::path                filename = "BASE_DIR/";
    std::string             symbol = "SYMBOL_NAME";
    std::string             locator_ending = "_some_data.quled";

    filename /= symbol;
    filename /= ThisFugginFuglyIntToStr( ymd.year, 1000 );
    filename /= ThisFugginFuglyIntToStr( ymd.month, 10 );
    filename /= ThisFugginFuglyIntToStr( ymd.day, 10 );
    //filename/= fuglyIntToStr( chunk_time_pos.time_of_day().hours(), 10 );
    filename+= locator_ending;
    //filename+= "h_ticks.quled";
    return filename;

}

