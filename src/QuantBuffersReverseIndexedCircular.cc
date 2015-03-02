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
class ReversedCircularStructBuffer
{
  public:
    ReversedCircularStructBuffer(ff_size_t psize)
        : capacity{ psize }
        , buffer{ new T[psize] }
    , head_ptr{ buffer - 1 }
    // We increase it by one in first "advance" before reading / writing
    {}

    ~ReversedCircularStructBuffer() { delete[] buffer; }

    inline operator T & () { return *head_ptr; }
    //inline operator const T&() { return *head_ptr; }

    inline T & operator()() { return *head_ptr; }
    inline const T & last_as_const() const { return *head_ptr; }

    inline void set(T value) { *head_ptr = value; }

    inline T & operator[](int backwards_index) const
    {
        #if IS_DEBUG
        if (backwards_index > size - 1) {
            cerr << "\nIndex overflow in ReverseBuffer: " << backwards_index
                 << " vs size-1: " << size - 1 << "\n";
            throw _bad_quantbuffer_indexing_; // *TODO*
        }
        #endif
        int ix = pos - int(backwards_index);
        if (ix >= 0) {
            return buffer[ix];
        }
        else {
            // ix += capacity;
            return buffer[ix + capacity];
        }
    }

    inline T & operator[](int backwards_index)
    {
        #if IS_DEBUG
        if (backwards_index > size - 1) {
            cerr << "\nIndex overflow in ReverseBuffer: " << backwards_index
                 << " vs size-1: " << size - 1 << "\n";
            throw _bad_quantbuffer_indexing_; // *TODO*
        }
        #endif
        int ix = pos - int(backwards_index);
        if (ix >= 0) {
            return buffer[ix];
        }
        else {
            // ix += capacity;
            return buffer[ix + capacity];
        }
        // if ( ix < 0) {
        //    ix += capacity;
        //}
        // return buffer[ix];
    }

    inline T & advance()  // return reference to [0] from here...
    {
        ++head_ptr;
        if (int(++pos) >= capacity) {
            pos = 0;
            head_ptr = buffer;
        }
        if (size < capacity) {
            ++size;
        }
        return *head_ptr;
    }

    inline void reset()
    {
        pos = 0;
        size = 0;
    }

    inline ff_size_t count() { return size; }

    // private:
    ff_size_t capacity;
    ff_size_t size = 0;

  private:
    int pos = -1; // We can't make it ofs_t because we need negative number in
    // init *TODO*
    T * buffer;
    T * head_ptr;
};

#endif
