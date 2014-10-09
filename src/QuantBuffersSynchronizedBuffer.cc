#include "QuantBuffersSynchronizedBuffer.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedAbstractBuffer.hh"


template <typename T, int Z>
class QuantBuffer : public QuantBufferAbstract {
  public:
    QuantBuffer( QuantBufferSynchronizedHeap *owning_heap = nullptr, int size_capacity = 0 )
    :
        QuantBufferAbstract ( owning_heap, size_capacity )
    {
        cerr << "new QuantBuffer(" << capacity << ")\n";
        //if ( owning_heap ) {
        //    owning_heap->add( this );
        //}
        global_actives.active_buffer_jar->add( this );

    };

    ~QuantBuffer() {};

/*
    *TODO* flytta till AbstractQuantBuffer - använd getDatatypeSize() för
    indexering - ELLER -
    Gör en wrapper funktion, operator[x] anropar virtual getAt(x) på nedärvda
    klasserna.

    EEEEEEELLER : Gör en specialisering av QuantHeapBuffer som har QuantReal
    datatyp för alla underbuffrar - no fuzz!

    Detta är den enda datatyp som kommer behövas för alla user buffers
    egentligen. Även tickVolume skulle kunna lagras med det för konsekvent
    lagring.
*/
    operator T() {
        return *( reinterpret_cast<T *>( head_ptr ) );
    };

    T operator() (int backwards_index) {
        return *( reinterpret_cast<T *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    T& operator[] (int backwards_index) {
        //cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        return *( reinterpret_cast<T *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    int getSizeInBytes () final {
        return capacity * sizeof( T );
    }

    inline int getZeroOffset () final {
        return Z;
    }

    inline int getDataTypeSize () final {
        return sizeof( T );
    }

};

#endif

