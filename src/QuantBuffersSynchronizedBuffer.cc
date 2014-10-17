#include "QuantBuffersSynchronizedBuffer.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedBufferAbstract.hh"


template <typename T, int Z>
class QuantBuffer : public QuantBufferAbstract {
  public:
    QuantBuffer(
        int size_capacity = 0,
        QuantBufferJar *owning_buf_jar = global_actives.active_buffer_jar // nullptr
    ) :
        QuantBufferAbstract ( size_capacity, owning_buf_jar )
    {
        cerr << "new QuantBuffer(" << capacity << ")\n";
        //if ( owning_heap ) {
        //    owning_heap->add( this );
        //}
        //global_actives.active_buffer_jar->add( this ); // 141012/ORC - moved to **Abstract
        owning_buf_jar->add( this );
        cerr << "/ new QuantBuffer()\n";

    };

    ~QuantBuffer() {};

    inline void operator << ( T val ) {     // 141015/ORC
        *( reinterpret_cast<T *>( head_ptr ) ) = val;
    };

    inline void operator () ( T val ) {
        *( reinterpret_cast<T *>( head_ptr ) ) = val;
    };

    inline operator T & () {
        return *( reinterpret_cast<T *>( head_ptr ) );
    };

    inline operator T () const {
        return *( reinterpret_cast<T *>( head_ptr ) );
    };

    inline T operator () ( ) {
        return *( reinterpret_cast<T *>( head_ptr ) );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    inline T operator() ( int backwards_index ) {
        assert( backwards_index < capacity + Z );
        return *( reinterpret_cast<T *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    inline T operator[] ( int backwards_index ) const {
        //cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert( backwards_index < capacity + Z );
        return *( reinterpret_cast<T *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    inline T& operator[] ( int backwards_index ) {
        //cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert( backwards_index < capacity + Z );
        return *( reinterpret_cast<T *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    inline T* getPtrTo ( int backwards_index ) {
        //cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert( backwards_index < capacity + Z );
        return ( reinterpret_cast<T *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    /*
    *TODO* flytta till AbstractQuantBuffer - använd getDatatypeSize() för
     indexering
    - ELLER : Gör en wrapper funktion, operator[x] anropar virtual getAt(x) på
     nedärvda klasserna.

    - EELLER : Gör en specialisering av QuantHeapBuffer som har QuantReal
     datatyp för alla underbuffrar - no fuzz!

    - EEELLER: Gör advance() etc. till del av denna som VIRTUAL FUNC (kommer ej
     kunna inlinas pga att den används från virtual/abstract context

    - EEEELLER: Gör QuantBuffer till union (double based) så att varje item
     alltid är 8 byte - TimeBuffers är också 8 byte.

    Detta är den enda datatyp som kommer behövas för alla user buffers
    egentligen. Även tickVolume skulle kunna lagras med det för konsekvent
    lagring.
    */

    int getSizeInBytes () final {
        return capacity * sizeof( T );
    }
    inline int getZeroOffset () final {
        return Z;
    }
    inline int getDataTypeSize () final {
        return sizeof( T );
    }
    inline void relocate_heap_ptr ( byte * ptr ) final {
        // Set head_ptr to point to the [0] element - this may be the last
        // element in the buffer, or 2nd last - if [-1] is allowed as last
        // element indexer (used for [0] = last_closed, [-1] = open_non_complete)
        tail_ptr = ptr;
        frontier_ptr = ptr + ((capacity - 1) * sizeof( T ));
        head_ptr = frontier_ptr - ( sizeof( T ) * (-Z) );
    }

};

#endif

