#include "QuantBuffersSynchronizedBuffer.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-29
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
//#include "QuantBuffersSynchronizedHeap.hh"


class QuantBufferAbstract {
  public:
    //AbstractQuantBuffer () {};

    QuantBufferAbstract( QuantBufferSynchronizedHeap *owning_heap = nullptr, int size_capacity = 0 )
    :
        owning_heap( owning_heap ? owning_heap : global_actives.active_buffer_heap ),
        capacity( size_capacity )
    {};

    virtual ~QuantBufferAbstract ();


    //auto needs( int lookback_length ) -> AbstractQuantBuffer &;
    void needs( int lookback_length );

/*
    virtual operator T() = 0;
    virtual T operator() ( int backwards_index ) = 0;
    virtual T& operator[] ( int backwards_index ) = 0;
*/

    virtual int getSizeInBytes() = 0;
    virtual int getDataTypeSize() = 0;
    virtual int getZeroOffset() = 0;

    QuantBufferSynchronizedHeap *owning_heap;   // More of a dev / debug safety measure than necessity - 2014-09-12/ORC(17:44)
    int             capacity = 0;
    int             size = 0;

    byte            *tail_ptr; // = nullptr;
    byte            *head_ptr; // = nullptr;

};

#endif

QuantBufferAbstract::~QuantBufferAbstract ()
{
    cerr << "AbstractQuantBuffer::~AbstractQuantBuffer - - DESTRUCTOR - - " << "\n";
}

//auto AbstractQuantBuffer::needs ( int lookback_length )
//  -> AbstractQuantBuffer &
void QuantBufferAbstract::needs ( int lookback_length )
{
/*
 * *TODO* circular deps...
    #ifdef NDEBUG
    if ( owning_heap->is_cut_in_stone ) {  // *TODO* NDEBUG-flagging..
        throw "Tried to change lookback needs for AbstractQuantBuffer - but the heap is already alloacted!";    // *TODO*
    }
    #endif
*/

    capacity = MAX( capacity, lookback_length );
    //return *this;   // for chaining
}



