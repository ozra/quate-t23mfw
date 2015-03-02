#include "QuantBuffersIntertwinedBuffer.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersIntertwinedBufferAbstract.hh"

/*
template <typename T_B> constexpr T_B* castTB( ptr ) {
    return reinterpret_cast<T_B *>( head_ptr );
}
*/

#define castTB(A) (reinterpret_cast<T_B*>(A))

// template <typename T_A, typename T_B>
template <typename T_B>
class QuantBuffer : public QuantBufferAbstract
{
  public:
    QuantBuffer(ff_size_t size_capacity = 0,
                QuantBufferJar * owning_buf_jar =
                    global_actives.active_buffer_jar // nullptr
               )
        : QuantBufferAbstract(size_capacity, owning_buf_jar)
    {
        cerr << "new QuantBuffer(" << capacity << ")\n";
        // static_assert( sizeof(T_A) == sizeof(T_B) );
        // if ( owning_heap ) {
        //    owning_heap->add( this );
        //}
        // global_actives.active_buffer_jar->add( this ); // 141012/ORC - moved
        // to **Abstract
        owning_buf_jar->add(this);
        cerr << "/ new QuantBuffer()\n";
    };

    ~QuantBuffer() {};

    inline void set(const T_B val)
    {
        assert(size > 0);
        #ifdef IS_DEBUG
        assert(datum_value_has_been_set == false);
        #endif
        *(reinterpret_cast<T_B *>(head_ptr)) = val;
        #ifdef IS_DEBUG
        datum_value_has_been_set = true;
        #endif
    }

    // inline void operator << ( T_B val ) {     // 141015/ORC
    //    set( val );
    //};

    inline void operator=(const T_B val) { set(val); };

    inline void operator|=(const T_B val) { set(val); }

    // inline void operator() ( T_B val ) {
    //    set( val );
    //};


    //inline operator const T_B & () const
    //{
    //    return *(reinterpret_cast<T_B *>(head_ptr));
    //};

    inline operator const T_B() const
    {
        // return *( (~) ( head_ptr ) );
        return *(reinterpret_cast<T_B *>(head_ptr));
    };

    inline const T_B & last_as_const() const
    {
        // return *( (~) ( head_ptr ) );
        return *(reinterpret_cast<T_B *>(head_ptr));
    };

    // constexpr typename _get_type() { return T_B };

    inline T_B operator()() const
    {
        return *(reinterpret_cast<T_B *>(head_ptr)); // Returning a reference is
        // good because it can be
        // assigned to - BUT - we
        // don't want the
        // performance hit. Use ()
        // for setting/updating, []
        // for getting.
    };

    inline T_B operator[](ff_size_t backwards_index) const
    {
        // cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " <<
        // (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert(backwards_index < capacity);
        return *(reinterpret_cast<T_B *>(head_ptr) - backwards_index);
    };

    inline T_B & operator[](ff_size_t backwards_index)
    {
        // cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " <<
        // (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert(backwards_index < capacity);
        return *(reinterpret_cast<T_B *>(head_ptr) - backwards_index);
    };

    inline T_B * getPtrTo(ff_size_t backwards_index) const
    {
        // cerr << "QuantBuffer::[] " << backwards_index << " - ptr is: " <<
        // (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        assert(backwards_index < capacity);
        return (reinterpret_cast<T_B *>(head_ptr) - backwards_index);
    };
};

/* Experiments with a functional approach to handling currently "classified"
 * solutions - to reveal if it might be a viably more clearer way of coding.
 * 141107/ORC
 */

template <typename T>
inline void set(QuantBuffer<T> & b, T val)
{
    assert(b.size > 0);
    assert(b.datum_value_has_been_set == false);
    // assert( b.get_type() == T );
    *(reinterpret_cast<T *>(b.head_ptr)) = val;
    #ifdef IS_DEBUG
    b.datum_value_has_been_set = true;
    #endif
}
/*
template <typename T>
inline QuantBuffer<T>& operator|= ( QuantBuffer<T> &b, T val ) {
    set( b, val );
    return b;
}
*/

#endif
