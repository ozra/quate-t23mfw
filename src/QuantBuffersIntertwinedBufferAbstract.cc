#include "QuantBuffersIntertwinedBufferAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-29
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
//#include "QuantBuffersIntertwinedHeap.hh"

//template <typename T_A>
class QuantBufferAbstract
{
  public:
    //AbstractQuantBuffer () {};

    //QuantBufferAbstract( QuantBufferIntertwinedHeap *owning_heap = nullptr, int size_capacity = 0 )
    //:
    //    owning_heap( owning_heap ? owning_heap : global_actives.active_buffer_heap ),
    //    capacity( size_capacity )
    QuantBufferAbstract(
        ff_size_t size_capacity, // = 0,
        QuantBufferJar * owning_buf_jar // = global_actives.active_buffer_jar //nullptr
    ) :
        owning_buf_jar(
            owning_buf_jar),   //? owning_buf_jar : global_actives.active_buffer_jar ),
        capacity(size_capacity)
    {
        cerr << "new QuantBuffer() :: QuantBufferAbstract(" << capacity << ")\n";
        //owning_buf_jar->add( this );
        cerr << "/ new QuantBuffer() :: QuantBufferAbstract()\n";
    };

    virtual ~QuantBufferAbstract();


    //auto needs( ff_size_t lookback_length ) -> AbstractQuantBuffer &;
    void needs(ff_size_t lookback_length);

    /*
        virtual operator T() = 0;
        virtual T operator() ( int backwards_index ) = 0;
        virtual T& operator[] ( int backwards_index ) = 0;
    */

    inline void relocate_heap_ptr(QuantTypeSized * ptr)
    {
        // Set head_ptr to point to the [0] element - this may be the last
        // element in the buffer, or 2nd last - if [-1] is allowed as last
        // element indexer (used for [0] = last_closed, [-1] = open_non_complete)
        tail_ptr = ptr;
        head_ptr = ptr + capacity - 1;
    }

    inline ff_size_t count()
    {
        return size;
    }

    //virtual int getSizeInBytes () = 0;
    //virtual int getDataTypeSize () = 0;
    //virtual int getZeroOffset () = 0;

    QuantBufferIntertwinedHeap *
    owning_heap;  // *TODO* *DUMP*?? Set from outside by **Heap.. More of a dev / debug safety measure than necessity - 2014-09-12/ORC(17:44)

    QuantBufferJar *
    owning_buf_jar;   // More of a dev / debug safety measure than necessity - 2014-09-12/ORC(17:44)

    #ifdef IS_DEBUG
    bool                                is_cut_in_stone = false;
    bool                                datum_value_has_been_set = false;
    #endif

    ff_size_t             capacity = 0;
    ff_size_t             size = 0;

    QuantTypeSized   *  tail_ptr; // = nullptr;
    QuantTypeSized   *  head_ptr; // = nullptr;

};

#endif

QuantBufferAbstract::~QuantBufferAbstract()
{
    cerr << "AbstractQuantBuffer::~AbstractQuantBuffer - - DESTRUCTOR - - " <<
         "\n";
}

//auto AbstractQuantBuffer::needs ( ff_size_t lookback_length )
//  -> AbstractQuantBuffer &
void QuantBufferAbstract::needs(ff_size_t lookback_length)
{
    #ifdef IS_DEBUG
    assert(! is_cut_in_stone);
    #endif
    capacity = max(capacity, lookback_length);
    //return *this;   // for chaining
}
