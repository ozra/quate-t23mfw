#include "QSDF_Sys_T23M_Abstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-12
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
* Code Cls: Blackbox Bare Metal
**/

#include <cerrno>
#include <cstring>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <boost/format.hpp>

#include "EtcUtils.hh"

#include "rfx11_types.hh"
//#include "QuantTime.hh"
//#include "MutatingString.hh"
#include "MutatingBuffer.hh"

#include "QSDF_Prefetcher.hh"
#include "QSDF_Utils.hh"


// const natural WRITE_CHUNK_SIZE = 128;    // In kilobytes
//↓ - good trade off for SSD's/HDD's
const natural WRITE_CHUNK_SIZE = 64; // In kilobytes

/*
                        #### ##    ## #### ########
                         ##  ###   ##  ##     ##
                         ##  ####  ##  ##     ##
                         ##  ## ## ##  ##     ##
                         ##  ##  ####  ##     ##
                         ##  ##   ###  ##     ##
                        #### ##    ## ####    ##
*/
template <typename T, natural Load_Profiling_Enum> // template<ENUM> Load_Profiling_Enum>
class QSDF_Sys_T23M_Abstract
{
  public:
    QSDF_Sys_T23M_Abstract(bool write_mode, natural accepted_missing_files_count)
        : write_mode_(write_mode)
        , accepted_missing_files_count_(accepted_missing_files_count)
    {
        if (write_mode) {
            const natural prel_buf_size = WRITE_CHUNK_SIZE * 1024;
            // void Add 1024k "soft buffer overrun" space...
            raw_buffer.reserve(prel_buf_size +
                               upper_power_of_two<1024>(sizeof(T) * 10));
            raw_buffer.set_specified_buffer_size(prel_buf_size);
            raw_buffer.reset_write_cursor();
        }
        else {
            const natural prel_buf_size = 500000;
            raw_buffer.reserve(prel_buf_size);
            raw_buffer.reset_read_cursor();
        }
    }

    ~QSDF_Sys_T23M_Abstract()
    {
        flush_writes();
        close_output_file();
    };

    inline void set_immediate_mode(bool that = true) { immediate_mode_ = that; }

    /*
    enum FlagsEnum {
        UNDEF = 0,
        HISTORY_MODE,
        LIVE_MODE,

    };

    QSDF_Sys_T23M_Abstract& operator|( FlagsEnum what ) {
        switch ( what ) {
            case HISTORY_MODE:
                immediate_mode_ = true;
                break;
            case LIVE_MODE:
                immediate_mode_ = false;
                break;
            case UNDEF: break;
        }
        return *this;
    }
    */

  protected:
    /*
    ########  ########   #######  ######## ########  ######  ######## ########
    ########
    ##     ## ##     ## ##     ##    ##    ##       ##    ##    ##    ##       ## ##
    ##     ## ##     ## ##     ##    ##    ##       ##          ##    ##       ## ##
    ########  ########  ##     ##    ##    ######   ##          ##    ######   ## ##
    ##        ##   ##   ##     ##    ##    ##       ##          ##    ##       ## ##
    ##        ##    ##  ##     ##    ##    ##       ##    ##    ##    ##       ## ##
    ##        ##     ##  #######     ##    ########  ######     ##    ########
    ########
    */

    MutatingBuffer<byte> raw_buffer;

    inline bool get_more_data()
    {
        assert(write_mode_ == false); // We don't support duplex right now...
        arbitrary_return_code ret = load_next_with_retries();
        if (ret == 0) {
            raw_buffer.reset_read_cursor();
            read_page_header_and_formalia();
        }
        else if (ret == -2) {
            _DPn("no more data - sets max time on tick.time\n");
            return false; // *TODO* NO MORE DATA...
        }
        else {
            assert(false && "Shouldn't happen!");
        }
        return true;
    }

    inline arbitrary_return_code load_next_with_retries()
    {
        // +1 for " one lap in loop if none allowed"..
        natural retries = accepted_missing_files_count_ + 1;
        arbitrary_return_code ret;
        do {
            ret = load_next_page_file();
            if (ret == 0) {
                return 0;
            }
            // else if (ret == -1) { // -1 = "expected 'error'"
            //}
            // else if ( ret == -2 )    // out of data error or similar
        }
        while (--retries);
        return -2;
    }

    inline void setup_new_output_page()
    {
        //#ifdef IS_DEEPBUG
        _DPn("setup_new_output_page() - INIT CHUNK");
        //#endif
        // pxt::ptime chunk_epoch(boost) = makeTime( "2014-01-07
        // 00:00:00.000"
        // ); //
        // pxt::date( 2014, 01, 07 ) );
        // cerr << "start_date is " << to_iso_string(start_date) << "\n";
        // cerr << "end_date is " << to_iso_string(end_date) << "\n";
        flush_writes(); // make sure everything for former chunk is done
        close_output_file();
        open_output_file_to_current_location();
        _DPn("BEFORE any writings byte size is: "
             << raw_buffer.size() << "capacity is " << raw_buffer.capacity()
             << " ptr is " << (void *)raw_buffer.front());
        cerrbug_a_buffer(raw_buffer.front(), raw_buffer.size());
        write_page_header_and_formalia();
    }

    inline void persist_maybe()
    {
        //_DPn("QuSeqDaCon::T23MFWTicks::persist_maybe()");
        //_DP("raw_buffer.is_specification_filled() ? = " <<
        // raw_buffer.is_specification_filled()
        //    << " spec_limit - front = " << raw_buffer.spec_limit() << "\n"
        //);
        if (immediate_mode_ == true || raw_buffer.is_specification_filled()) {
            persist_to_storage();
        }
    }

    void flush_writes()
    {
        _DP("QuSeqDaCon::T23MFWTicks::flush_writes()\n");
        persist_to_storage();
        /*
         *
         *
         * *TODO*
         *
         *
         */
    }
    /*
    ########  ########  #### ##     ##    ###    ######## ########
    ##     ## ##     ##  ##  ##     ##   ## ##      ##    ##
    ##     ## ##     ##  ##  ##     ##  ##   ##     ##    ##
    ########  ########   ##  ##     ## ##     ##    ##    ######
    ##        ##   ##    ##   ##   ##  #########    ##    ##
    ##        ##    ##   ##    ## ##   ##     ##    ##    ##
    ##        ##     ## ####    ###    ##     ##    ##    ########
    */
  private:
    // const char *    generate_locator_path ( pxt::ptime time_pos );
    virtual auto generate_locator_path() -> string = 0;
    virtual auto read_page_header(byte *) -> byte * = 0;
    virtual auto write_page_header(byte *) -> byte * = 0;

    arbitrary_return_code load_next_page_file()
    {
        //#ifdef IS_DEEPBUG
        _DPn("load_next_page_file()");
        //#endif
        // pxt::ptime chunk_epoch(boost) = makeTime( "2014-01-07
        // 00:00:00.000"
        // ); //
        // pxt::date( 2014, 01, 07 ) );
        // cerr << "start_date is " << to_iso_string(start_date) << "\n";
        // cerr << "end_date is " << to_iso_string(end_date) << "\n";
        //_DPn("chunk_time_pos = " << to_iso_string(chunk_time_pos));
        string fname = generate_locator_path();
        //_DPn("The data chunk filename is '" << fname);
        _DPn("calls load_file_into_existing_buffer: '" << fname);
        profiler.start(Load_Profiling_Enum);
        enum_load_result ret =
            load_file_into_existing_buffer(fname.c_str(), raw_buffer);
        profiler.end(Load_Profiling_Enum);
        _DPn("back from call to load_file_into_existing_buffer");
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
        }
        else if (ret == EMPTY_FILE) {
            #ifdef IS_DEEPBUG
            cerr << "File was empty - may be expectable"
                 << "\n";
            #endif
            return -1;
        }
// else status == 0 som det ska vara...
        #ifdef IS_DEEPBUG
        _DPn("BEFORE decoding byte size is: " << raw_buffer.size() << " ptr is "
             << (void *)raw_buffer.front()
             << "\n");
        cerrbug_a_buffer(raw_buffer.front(), raw_buffer.size());
        #endif
        // rdptr = raw_buffer.begin();
        // wrpos = raw_buffer.end() - 1;
        return 0;
    }

    // const int64_t THE_BIG_NEG = -4747474747474747477LL;
    // const int64_t THE_BIG_NEG = -2147483647LL;
    const int64_t THE_BIG_NEG = -474747474747474747LL;

    void read_page_header_and_formalia()
    {
        _DPn("read_page_header_and_formalia()");
        byte * rdptr = raw_buffer.on_free_leash_for(100);
        int64_t check = 0;
        check = read_varilen_natural<int>(rdptr);
        assert(check == 47);
        check = read_varilen_integer<int>(rdptr);
        assert(check == -47);
        check = read_varilen_natural<int>(rdptr);
        assert(check == 71);
        check = read_varilen_integer<int64_t>(rdptr);
        _Dn("check -BIG = " << check << " should be " << THE_BIG_NEG);
        assert(check == THE_BIG_NEG);
        rdptr = read_page_header(rdptr);
        check = read_varilen_natural<int>(rdptr);
        assert(check == 0);
        check = read_varilen_natural<int>(rdptr);
        assert(check == 47);
        raw_buffer.catch_up_reads(rdptr);
        _DPn("read_page_header_and_formalia() DONE");
    }

    void write_page_header_and_formalia()
    {
        _DP("QuSeqDaCon::T23MFWTicks::write_page_header_and_formalia()\n");
        byte * wrpos = raw_buffer.on_free_leash_for(100);
        write_varilen_natural(wrpos, 47);
        write_varilen_integer(wrpos, -47);
        write_varilen_natural(wrpos, 71);
        // write_varilen_integer( wrpos, -4747474747474747L );
        write_varilen_integer<int64_t>(wrpos, THE_BIG_NEG);
        wrpos = write_page_header(wrpos);
        write_varilen_natural(wrpos, 0);
        write_varilen_natural(wrpos, 47);
        raw_buffer.catch_up_writes(wrpos);
        _DPn("QuSeqDaCon::T23MFWTicks::write_page_header_and_formalia() "
             "DONE");
    }

    /*
    ##     ## ##      ##          ######  ##    ##  ######          ####  #######
    ##     ## ##  ##  ##         ##    ##  ##  ##  ##    ##          ##  ##     ##
    ##     ## ##  ##  ##         ##         ####   ##                ##  ##     ##
    ######### ##  ##  ## #######  ######     ##     ######  #######  ##  ##     ##
    ##     ## ##  ##  ##               ##    ##          ##          ##  ##     ##
    ##     ## ##  ##  ##         ##    ##    ##    ##    ##          ##  ##     ##
    ##     ##  ###  ###           ######     ##     ######          ####  #######
    */

    arbitrary_return_code open_output_file_to_current_location()
    {
        // const char * filename = generate_locator_path( chunk_time_pos );
        string filename = generate_locator_path();
        _DP("QuSeqDaCon::T23MFWTicks::open_output_file_to_current_location,");
        if (true) {
            // *TODO* optimize so that checks only are made for path-parts
            // that
            // has
            // changed!!!
            fs::path dirs(filename);
            dirs.remove_filename();
            _DPn("open_output_file_to_current_location - create missing directories if "
                 "any "
                 << dirs << "\n");
            fs::create_directories(dirs);
        }
        _DP("open_output_file_to_current_location - filename = \"" << filename
            << "\"\n");
        // *TODO*(2014-11-23/Oscar Campbell) *NORM* Formalize how error
        // handling should be approached concerning file opening etc. -
        try {
            out_file_.open(filename, fs::ofstream::binary | fs::ofstream::out);
            if (out_file_.is_open() == false) {
                cerr << "Possible error for fopen: " << strerror(errno) << "\n";
                assert(out_file_.is_open());
                return -2;
            }
            // out_file_.sync_with_stdio(false);
        }
        catch (std::ifstream::failure e) {
            e.what();
            assert(true == false);
        }
        _DP("open_output_file_to_current_location - DONE\n");
        return 0;
    }

    arbitrary_return_code close_output_file()
    {
        _DP("QuSeqDaCon::T23MFWTicks::close_output_file \n");
        if (out_file_.is_open()) {
            out_file_.close();
        }
        return 0; // *TODO* remove. throw exception on error instead
    }

    arbitrary_return_code persist_to_storage()
    {
        _DP("QuSeqDaCon::T23MFWTicks::persist_to_storage()\n");
        natural bytes_to_write = raw_buffer.buffered_count(); // non_filed_ptr;    //
        // front_of(raw_buffer); //
        // raw_buffer.begin();
        if (bytes_to_write == 0) { return 0; }
        if (out_file_.is_open() == false) {
            _Dn("File was not open proper in persist_to_storage()!\n\n");
            return -2;
        }
        // *TEMP*
        raw_buffer.verify_pointer(raw_buffer.front() + bytes_to_write);
        profiler.start(WRITING_FILE);
        out_file_.write(reinterpret_cast<char *>(raw_buffer.begin()),
                        bytes_to_write);
        profiler.end(WRITING_FILE);
        raw_buffer.consume_specified_buffer();
        out_file_.flush();
        _DPn("wrote " << bytes_to_write << "bytes");
        return 0;
    }

    bool immediate_mode_ = false;
    bool write_mode_ = false;
    natural accepted_missing_files_count_ = 0;

    fs::ofstream out_file_;
};

#endif
