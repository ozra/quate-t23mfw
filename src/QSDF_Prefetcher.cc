#include "QSDF_Prefetcher.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-05
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/**
 * RESURSER:
 *
 * Angående libprefetch:
 * https://www.usenix.org/legacy/event/usenix09/tech/full_papers/vandebogart/vandebogart_html/index.html
 * http://libprefetch.cs.ucla.edu/
 * https://github.com/joyent/libuv
 *
 *
 *
 *
**/

#include <stdint.h>
#include <cstddef>

#include <vector>
using std::vector;

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

#include "rfx11_types.hh"

#include "MutatingBuffer.hh"

/*
 *
 *
 *
 *  *TODO*   *NOTE* ! ! !
 *
 * When performance really is important is during the optimization phase. In
 * this phase however, the data range is read again and again and again.. and
 * so will be reaaaalllly hot in the OS/kernel-cache. Pre-fetching in every
 * parallell optimization process will then be detremiental!
 *
 * So - we should focus our efforts on speeding up decoding of files and
 * minimizing copying / moving of fat data chunks.
 *
 * * We could load all data successively, asynchronously and keep it in -
 * shared memory style...
 *
 *
 */

#include "QuantProfiling.hh"

enum enum_load_result { SUCCESS = 0, EMPTY_FILE = 1, MISSING_FILE = 2 };

template <typename T>
enum_load_result load_file_into_existing_buffer(const char* p_filename,
        MutatingBuffer<T>& buffer);

/*
template <typename T>
T *load_file_into_existing_buffer(
    cApfilename,
    T           *buffer,
    size_t      *buf_byte_size,
    int         *status
);
*/

/*

class FileBuffer {
  public:
    FileBuffer ( byte* buf_ptr, size_t size );
    FileBuffer ( );

    byte*   buf_ptr = nullptr;
    size_t  size = 0;

};

#endif

FileBuffer::FileBuffer( byte* buf_ptr, size_t size )
:
    buf_ptr { buf_ptr },
    size { size }
{}

FileBuffer::FileBuffer() {}

#ifdef INTERFACE

class FilePrefetcher {
  public:

    FilePrefetcher ( size_t cache_size, int max_concurrent )
    :
        cache_size { cache_size },
        max_concurrent { max_concurrent }
    {
        data_heap = new byte[ cache_size ];

    };

    ~FilePrefetcher () {
        delete[] data_heap;
    }

    int pre_fetch ( fs::path file_name );
    / *
     * check if there is space to cache in - otherwise queue
     * /

    bool free ( FileBuffer* buffer );  // Free a file buffer we're done with
    / *
    * see if it was the oldest buffer that was freed - then move up empty-space
    * 'begin' to it - else ERROR (this FilePrefetcher only handles sequential
    * data fetching).
    *
    * check if there are queued prefetches to take care of
    *
    * /

    FileBuffer& load ( int ticket );

    FileBuffer& load ( fs::path file_name);

  private:
    std::queue<fs::path>    filename_queue;
    std::queue<FileBuffer>  currently_cached;

    byte    *data_heap = nullptr;
    size_t  cache_size = 0;
    int     max_concurrent = 8;

};

*/

template <typename T>
enum_load_result load_file_into_existing_buffer(const char* p_filename,
        MutatingBuffer<T>& buffer)
{
    fs::path filename(p_filename);
    if (fs::exists(filename) == false || fs::is_regular(filename) == false) {
        return MISSING_FILE;
    }
    size_t new_size = fs::file_size(filename);
    if (new_size == 0) {
        buffer.resize(0);
        #ifdef IS_DEEPBUG
        cerr << "load_file_into_existing_buffer: returns cause file was empty!"
             << "\n";
        #endif
        return EMPTY_FILE;
    }
    #ifdef IS_DEEPBUG
    cerr << "load_file_into_existing_buffer(\"" << p_filename
         << "\") - file's size: " << new_size
         << " vs existing buf size: " << buffer.size() << ""
         << "\n";
    #endif
    // std::ifstream fin(filename, std::ifstream::binary);
    /*
        try {
    */
    fs::ifstream fin(filename, fs::ifstream::in | fs::ifstream::binary);
    //  | fs::ifstream::ate );
    fin.sync_with_stdio(false);
    if (fin) {
        _DPn("load_file_into_existing_buffer: reads file in to buffer");
        // use this instead??  ->
        // new_size = fin.tellg();
        // fin.seekg( 0, fs::ifstream::beg );
        buffer.resize(new_size); //, false);
        fin.read(reinterpret_cast<char*>(buffer.front()), new_size);
        fin.close();
    }
    /*
        } catch () {

        }
    */
    return SUCCESS;
}

#endif
