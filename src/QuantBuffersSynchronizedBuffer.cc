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


//template <typename T_A, typename T_B>
template <typename T_B>
class QuantBuffer : public QuantBufferAbstract {
  public:
    QuantBuffer (
        int size_capacity = 0,
        QuantBufferJar *owning_buf_jar = global_actives.active_buffer_jar // nullptr
    ) :
        QuantBufferAbstract ( size_capacity, owning_buf_jar )
    {
        cerr << "new QuantBuffer(" << capacity << ")\n";

        //static_assert( sizeof(T_A) == sizeof(T_B) );


            //if ( owning_heap ) {
        //    owning_heap->add( this );
        //}
        //global_actives.active_buffer_jar->add( this ); // 141012/ORC - moved to **Abstract
        owning_buf_jar->add( this );
        cerr << "/ new QuantBuffer()\n";

    };

    ~QuantBuffer() {};

    inline void set ( T_B val ) {
        assert( size > 0 );
        assert( datum_value_has_been_set == false );

        *( reinterpret_cast<T_B *>( head_ptr ) ) = val;

        #ifdef IS_DEBUG
            datum_value_has_been_set = true;
        #endif

    }

    //inline void operator << ( T_B val ) {     // 141015/ORC
    //    set( val );
    //};

    inline void operator= ( T_B val ) {
        set( val );
    };

    inline void operator|= ( T_B val ) {
        set( val );
    }

    //inline void operator() ( T_B val ) {
    //    set( val );
    //};

    inline operator T_B & () {
        return *( reinterpret_cast<T_B *>( head_ptr ) );
    };

    inline operator T_B () const {
        return *( reinterpret_cast<T_B *>( head_ptr ) );
    };

    inline T_B operator() ( ) {
        return *( reinterpret_cast<T_B *>( head_ptr ) );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    inline T_B operator[] ( int backwards_index ) const {
        //cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert( backwards_index < capacity );
        return *( reinterpret_cast<T_B *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    inline T_B& operator[] ( int backwards_index ) {
        //cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert( backwards_index < capacity );
        return *( reinterpret_cast<T_B *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    inline T_B* getPtrTo ( int backwards_index ) {
        //cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert( backwards_index < capacity );
        return ( reinterpret_cast<T_B *>( head_ptr ) - backwards_index );   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
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


    /*
    int getSizeInBytes () final {
        return capacity * sizeof( T_A );
    }
    inline int getDataTypeSize () final {
        return sizeof( T_A );
    }
    */

    /*
     * Is now moved to the Abstract
    inline void relocate_heap_ptr ( QuantTypeSized * ptr ) final {
        // Set head_ptr to point to the [0] element - this may be the last
        // element in the buffer, or 2nd last - if [-1] is allowed as last
        // element indexer (used for [0] = last_closed, [-1] = open_non_complete)
        tail_ptr = ptr;
        //frontier_ptr = ptr + ((capacity - 1) * sizeof( T_A ));
        head_ptr = ptr + capacity - 1;

        / *

        *TODO* head_ptr fel? ska vara som frontier_ptr (som ska bort).
        obs. Att vi på nåt sätt ska se till att Heap och Buffers har samma
        datatype-LÄNGD (dvs. 8 i "finans-fallet")

        typedefa QuantBufferHeap<QuantReal> QuantHeap
        -""-     QuantBuffer<QuantReal> QuantRealBuffer

        Vooore ju _kanske_ bra med union { QuantTime, QuantReal } - annars får
        vi behålla abstraktionen som den nu är men räkna ptrs med BufferHeaps
        base-datatype (QuantReal) som enda skillnad emot nu.

        * /

    }
     *
     */

};

#endif

