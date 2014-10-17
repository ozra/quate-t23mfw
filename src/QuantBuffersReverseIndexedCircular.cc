#include "QuantBuffersReverseIndexedCircular.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/
#include "QuantBase.hh"
#include "QuantBuffersBase.hh"

template <typename T>
class ReversedCircularStructBuffer {
  public:
    ReversedCircularStructBuffer ( int psize )
    :
        capacity { psize },
        buffer { new T[psize] },
        head_ptr { buffer - 1 }      // We increase it by one in first "advance" before reading / writing
    {}

    ~ReversedCircularStructBuffer () {
        delete[] buffer;
    }


    inline operator T& () {
        return *head_ptr;
    }

    inline T& operator () () {
        return *head_ptr;
    }


    inline T& operator[]( int backwards_index ) const {
        #if IS_DEBUG
        if ( backwards_index > size - 1 ) {
            throw _bad_quantbuffer_use_; // *TODO*
        }
        #endif

        int ix = pos - backwards_index;

        if ( ix >= 0 ) {
            return buffer[ix];   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
        } else {
            //ix += capacity;
            return buffer[ix + capacity];
        }
    }

    inline T& operator[]( int backwards_index ) {
        #if IS_DEBUG
        if ( backwards_index > size - 1 ) {
            throw _bad_quantbuffer_use_; // *TODO*
        }
        #endif

        int ix = pos - backwards_index;

        if ( ix >= 0 ) {
            return buffer[ix];   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
        } else {
            //ix += capacity;
            return buffer[ix + capacity];
        }
        //if ( ix < 0) {
        //    ix += capacity;
        //}
        //return buffer[ix];   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    }

    inline T& advance () {    // return reference to [0] from here...
        ++head_ptr;
        if ( ++pos >= capacity ) {
            pos = 0;
            head_ptr = buffer;
        }

        if ( size < capacity ) {
            ++size;
        }

        return *head_ptr;
    }

    inline void reset () {
        pos = 0;
        size = 0;
    }


  //private:
    int capacity;
    int size = 0;

  private:
    int pos = -1;
    T *buffer;
    T *head_ptr;
};

#endif
