#include "QuantBuffersSynchronizedHeap.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedAbstractBuffer.hh"

class QuantBufferSynchronizedHeap {
  public:
    QuantBufferSynchronizedHeap ( int default_buf_size );
    ~QuantBufferSynchronizedHeap ();

    //auto add( AbstractQuantBuffer &buf ) -> QuantBufferSynchronizedHeap &;
    //auto add( AbstractQuantBuffer &buf, int size_capacity ) -> QuantBufferSynchronizedHeap &;
    void add( QuantBufferAbstract &buf );
    void add( QuantBufferAbstract &buf, int size_capacity );

    int     allocate();     // 0 = ok, else error code
    void    advance();

    #ifdef NDEBUG
    bool                                is_cut_in_stone = false;
    #endif

  private:
    int                                 default_buf_size;
    byte                                *heap;              // TODO(ORC) make sure we're on even 8byte...
    byte                                *heap_end_boundary;
    int                                 total_byte_size = 0;
    int                                 biggest_size = 0;

    //vector<QuantBuffer<QuantReal> *>    real_buffers;
    //vector<QuantBuffer<QuantTime> *>    time_buffers;
    QuantBufferJar                      buffer_jar;

};

#endif

QuantBufferSynchronizedHeap::QuantBufferSynchronizedHeap ( int default_buf_size )
:
    default_buf_size  ( default_buf_size )
{}

QuantBufferSynchronizedHeap::~QuantBufferSynchronizedHeap ()
{
    cerr << "QuantBufferSynchronizedHeap::~QuantBufferSynchronizedHeap - - DESTRUCTOR - -" << "\n";
    delete heap;

    /*
     * Deleting the buffers is the users responsibility (they're most likely
     * created at context construction time, and therefor automatically
     * destroyed...
    cerr << "Will delete " << buffers.size() << " quant buffers" << "\n";

    for ( auto buf : buffers ) {
        delete buf;
    }

     */
}

//auto QuantBufferSynchronizedHeap::add( AbstractQuantBuffer &buf )
//  -> QuantBufferSynchronizedHeap&
void QuantBufferSynchronizedHeap::add ( QuantBufferAbstract &buf )
{
    buf.owning_heap = this;

    if ( buf.capacity == 0 ) {
        buf.capacity = default_buf_size;
    }

    //buffers.push_back( &buf );    *TODO* is auto
    //return *this;
}

//auto QuantBufferSynchronizedHeap::add( AbstractQuantBuffer &buf, int size_capacity )
//    -> QuantBufferSynchronizedHeap&
void QuantBufferSynchronizedHeap::add ( QuantBufferAbstract &buf, int size_capacity )
{
    buf.owning_heap = this;
    buf.capacity = size_capacity;
    //buffers.push_back( &buf );    // *TOOD* auto!
    //return *this;
}

int QuantBufferSynchronizedHeap::allocate ()
{
    cerr << "QuantBufferSynchronizedHeap::allocate()\n";

    int buf_byte_size;

    for ( auto buf : buffer_jar.buffers ) {
        if ( buf->capacity == 0 ) {
            buf->capacity = default_buf_size;
        }

        buf_byte_size = buf->getSizeInBytes();

        cerr << "Sums buffer of size " << buf_byte_size << "type-size is " << buf->getDataTypeSize() << "\n";
        biggest_size = MAX(biggest_size, buf_byte_size);
        cerr << "Biggest size is " << biggest_size << "\n";
        total_byte_size += buf_byte_size;
    }

    total_byte_size += biggest_size;

    cerr << "QuantBufferSynchronizedHeap::allocate() total size to alloc: " << total_byte_size <<"\n";

    heap = new byte[total_byte_size];
    if ( !heap ) {
        return 1;
    }

    heap_end_boundary = heap + total_byte_size;
    byte *ptr = heap;

    cerr << "ptr (heap copied ptr) is " << (void *)ptr << "end_ptr is: " << (void *)heap_end_boundary << "\n";

    for ( auto buf : buffer_jar.buffers ) {
        buf->tail_ptr = ptr;
        ptr += buf->getSizeInBytes();  //capacity * sizeof(QuantTime);
        // Set head_ptr to point to the [0] element - this may be the last
        // element in the buffer, or 2nd last - if [-1] is allowed as last
        // element indexer (used for [0] = last_closed, [-1] = open_non_complete)
        buf->head_ptr = (ptr - (buf->getDataTypeSize() * (1 - buf->getZeroOffset()) ) );

        cerr << "Back step from end is: " << (1 - buf->getZeroOffset()) << " zeroOffset itself is " << buf->getZeroOffset() << "\n";
        cerr << "Ptrs are:" << (void *)buf->head_ptr << ", " << (void *)buf->tail_ptr << "\n";
    }

    #ifdef NDEBUG
    is_cut_in_stone = true;
    #endif

    cerr << "QuantBufferSynchronizedHeap::allocate() is done\n";

    return 0;

}

void QuantBufferSynchronizedHeap::advance ()
{
    bool time_for_move = false;
    int typesize;

    for ( auto buf : buffer_jar.buffers ) {
        typesize = buf->getDataTypeSize();
        //cerr << "QuantBufferSynchronizedHeap::advance - ptrs BEFORE are: " << (void *)buf->head_ptr << ", " << (void *)buf->tail_ptr << "\n";
        //cerr << "Sizeof _ptr_ is: " << sizeof(buf->tail_ptr) << "\n";
        //cerr << "Sizeof ptrtype is: " << typesize << "\n";

        buf->tail_ptr += typesize;
        buf->head_ptr += typesize;
        buf->size = MAX ( buf->capacity, ++buf->size );

        //cerr << "QuantBufferSynchronizedHeap::advance - ptrs are: " << (void *)buf->head_ptr << ", " << (void *)buf->tail_ptr << "\n";

        if ( (buf->head_ptr + typesize) >= heap_end_boundary ) {
            time_for_move = true;
        }
    }

    if ( time_for_move ) {
        //cerr << "QuantBufferSynchronizedHeap::advance() - It's time to move a buf around\n";

        /*
         *
         * *9* *TODO*  move the last bufs mem area to beginning of heap
         *
        */
    }

}

