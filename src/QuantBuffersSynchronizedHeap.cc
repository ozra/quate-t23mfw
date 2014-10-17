#include "QuantBuffersSynchronizedHeap.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedBufferAbstract.hh"

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

    #ifdef IS_DEBUG
    bool                                is_cut_in_stone = false;
    #endif

  private:
    int                                 default_buf_size;
    QuantTypeSized                      *heap;
    byte                                *heap_end_boundary;
    int                                 total_byte_size = 0;
    int                                 biggest_size = 0;
    QuantBufferJar                      buffer_jar;

};

#endif

QuantBufferSynchronizedHeap::QuantBufferSynchronizedHeap ( int default_buf_size )
:
    default_buf_size  ( default_buf_size )
{
    cerr << "\n\n >> QuantBufferSynchronizedHeap::QuantBufferSynchronizedHeap << - - CONSTRUCTOR! - -" << "\n\n";
}

QuantBufferSynchronizedHeap::~QuantBufferSynchronizedHeap ()
{
    cerr << "\n\nQuantBufferSynchronizedHeap::~QuantBufferSynchronizedHeap - - DESTRUCTOR - -" << "\n";
    cerr << "(" << (int)this << ") Heap ptr is " << (void *)heap << "\n";

    /*
    cerr << "crash heh? " << *(heap_end_boundary - 8);
    cerr << "crash heh? " << *(heap_end_boundary);
    cerr << "crash heh? " << *(heap_end_boundary + 8);
    cerr << "crash heh? " << *(heap_end_boundary + 16);
    cerr << "crash heh? " << *(heap - 8);
    //cerr << "crash heh? " << *(heap - 234234234);
    */

    delete [] heap;

    cerr << "/ QuantBufferSynchronizedHeap::~QuantBufferSynchronizedHeap - - DESTRUCTOR - -" << "\n\n";
}

//auto QuantBufferSynchronizedHeap::add( AbstractQuantBuffer &buf )
//  -> QuantBufferSynchronizedHeap&
void QuantBufferSynchronizedHeap::add ( QuantBufferAbstract &buf )
{
    // *TODO* THESE ADD METHODS SHOULD BE OBSOLETE!

    buf.owning_heap = this;

    if ( buf.capacity == 0 ) {
        buf.capacity = default_buf_size;
    }
}

//auto QuantBufferSynchronizedHeap::add( AbstractQuantBuffer &buf, int size_capacity )
//    -> QuantBufferSynchronizedHeap&
void QuantBufferSynchronizedHeap::add ( QuantBufferAbstract &buf, int size_capacity )
{
    // *TODO* THESE ADD METHODS SHOULD BE OBSOLETE!

    buf.owning_heap = this;
    buf.capacity = size_capacity;
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

        cerr << "Sums buffer of size " << buf_byte_size << ", type-size is " << buf->getDataTypeSize() << "\n";
        biggest_size = MAX( biggest_size, buf_byte_size );
        total_byte_size += buf_byte_size;
    }

    total_byte_size += biggest_size;    // Add the buffer-rotation space with the size of the biggest buffer we have in the heap

    cerr << "Biggest size is " << biggest_size << "\n";
    cerr << "QuantBufferSynchronizedHeap::allocate() total size to alloc: " << total_byte_size <<"\n";

    heap = new QuantTypeSized[ total_byte_size / sizeof(QuantTypeSized) ];

    if ( !heap ) {
        cerr << "\n\n\nCouldn't allocate memmory for Synchronized Heap!\n\n\n";
        return 1;
    }

    heap_end_boundary = reinterpret_cast<byte *>( heap + ( total_byte_size / sizeof(QuantTypeSized) ) );
    QuantTypeSized *ptr = heap;

    cerr << "(" << (int)this << ") Heap ptr is " << (void *)heap << "\n";

    for ( auto buf : buffer_jar.buffers ) {
        buf->relocate_heap_ptr( reinterpret_cast<byte *>( ptr ) );   // 141015/ORC
        ptr += buf->capacity;
        //ptr += buf->getSizeInBytes();  //capacity * sizeof(QuantTime);

        #ifdef IS_DEBUG
        buf->is_cut_in_stone = true;
        #endif

        #ifdef IS_DEBUG
        cerr << "Back step from end is: " << (1 - buf->getZeroOffset()) << " zeroOffset itself is " << buf->getZeroOffset() << "\n";
        cerr << "Buf tail ptr is:" << (void *)buf->tail_ptr << "\n"; // (void *)buf->head_ptr << ", "
        #endif
    }

    #ifdef IS_DEBUG
    is_cut_in_stone = true;
    #endif

    cerr << "QuantBufferSynchronizedHeap::allocate() is done\n";

    return 0;

}

// 4715471517 - 1754747

void QuantBufferSynchronizedHeap::advance () {
    //bool time_for_move = false;
    QuantBufferAbstract *buffer_to_move = nullptr;
    //int typesize;

    for ( auto buf : buffer_jar.buffers ) {
        //typesize = buf->getDataTypeSize();
        //cerr << "QuantBufferSynchronizedHeap::advance - ptrs BEFORE are: " << (void *)buf->head_ptr << ", " << (void *)buf->tail_ptr << "\n";
        //cerr << "Sizeof _ptr_ is: " << sizeof(buf->tail_ptr) << "\n";
        //cerr << "Sizeof ptrtype is: " << typesize << "\n";

        buf->tail_ptr += sizeof(QuantTypeSized);
        buf->head_ptr += sizeof(QuantTypeSized);
        buf->frontier_ptr += sizeof(QuantTypeSized);

        if ( buf->size < buf->capacity ) {
            ++buf->size;
        }
        if ( UNLIKELY( buf->frontier_ptr == heap_end_boundary ) ) {
            buffer_to_move = buf;
        }
    }

    if ( LIKELY( ! buffer_to_move ) ) {   // False is the absolutely most likely branch
        return;

    } else {
        cerr << "\n\n\nQuantBufferSynchronizedHeap::advance() - It's time to move a buf around!!!" << "\n\n\n";

        memcpy( heap, (buffer_to_move->tail_ptr - sizeof(QuantTypeSized)), buffer_to_move->getSizeInBytes() );
        buffer_to_move->relocate_heap_ptr( reinterpret_cast<byte *>( heap ) );
        return;
    }

}

