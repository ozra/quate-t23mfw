/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef QUANTBUFFERS_HH
#define QUANTBUFFERS_HH

#include "QuantBase.hh"
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


typedef signed char byte;   // signed as not to be interpreted as chaaar



template <typename T>
class ReversedCircularStructBuffer {
  public:
    ReversedCircularStructBuffer ( int size );
    ~ReversedCircularStructBuffer ();

    T& operator[]( int backwards_index );
    T& advance ();    // return reference to [0] from here...

  private:
    int capacity;
    int size;
    int pos;
    T *buffer;
};


template <typename T>
ReversedCircularStructBuffer<T>::ReversedCircularStructBuffer ( int psize )
: capacity(psize),
  size(0),
  pos(0),
  buffer(new T[psize])
{
}

template <typename T>
ReversedCircularStructBuffer<T>::~ReversedCircularStructBuffer ()
{
    delete[] buffer;
}

template <typename T>
T& ReversedCircularStructBuffer<T>::operator[](int backwards_index) {
    if ( backwards_index > size - 1 ) {
        throw 4747; // *TODO*
    }

    int ix = pos - backwards_index;

    if ( ix < 0) {
        ix += capacity;
    }

    return buffer[ix];   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
}

template <typename T>
T& ReversedCircularStructBuffer<T>::advance ()
{
    if ( ++pos >= capacity ) {
        pos = 0;
    }

    if ( ++size > capacity ) {
        size = capacity;
    }

    return buffer[pos];

}










// Exception:
class bad_quantbuffer_use: public std::exception {  //  - 2014-08-12/ORC(19:51)
  public:
    virtual const char* what() const throw();
};

extern bad_quantbuffer_use _bad_quantbuffer_use_;




// Forward declaration
class AbstractQuantBuffer;
template <typename T> class QuantBuffer;


// *TODO* QuantSynchronizedBufferHeap...
class QuantBufferSynchronizedHeap {
  public:
    QuantBufferSynchronizedHeap ();
    ~QuantBufferSynchronizedHeap ();

    //QuantBuffer<QuantTime> *            newTimeBuffer( int size );
    //void    newIntBuffer(int size);
    //QuantBuffer<QuantReal> *            newRealBuffer( int size );
    //void    newBuffer(int size);

    //template <typedef T>
    //QuantBuffer<T> *            newBuffer(int size);

    auto add( AbstractQuantBuffer &buf ) -> QuantBufferSynchronizedHeap &;
    auto add( AbstractQuantBuffer &buf, int size_capacity ) -> QuantBufferSynchronizedHeap &;

    int     allocate();     // 0 = ok, else error code
    void    advance();

    #ifdef IS_DEBUG
    bool                                is_cut_in_stone = false;
    #endif

  private:
    byte                                *heap;              // TODO(ORC) make sure we're on even 8byte...
    byte                                *heap_end_boundary;
    int                                 total_byte_size = 0;
    int                                 biggest_size = 0;

    //vector<QuantBuffer<QuantReal> *>    real_buffers;
    //vector<QuantBuffer<QuantTime> *>    time_buffers;
    vector<AbstractQuantBuffer *>       buffers;

};



class AbstractQuantBuffer {
  public:
    AbstractQuantBuffer( ) {};

    AbstractQuantBuffer( QuantBufferSynchronizedHeap *owning_heap, int size_capacity = 0 )
    : owning_heap( owning_heap ),
      capacity( size_capacity )
    {};

    virtual ~AbstractQuantBuffer() {}; //= 0;

    auto needs( int lookback_length ) -> AbstractQuantBuffer &;

    virtual int getSizeInBytes() = 0;
    virtual int getDataTypeSize() = 0;

    QuantBufferSynchronizedHeap *owning_heap;   // More of a dev / debug safety measure than necessity - 2014-09-12/ORC(17:44)
    int             capacity = 0;

    byte            *tail_ptr; // = nullptr;
    byte            *head_ptr; // = nullptr;

};

template <typename T>
class QuantBuffer : public AbstractQuantBuffer {
  public:
    QuantBuffer() {};

    QuantBuffer( QuantBufferSynchronizedHeap *owning_heap, int size_capacity = 0 )
    : AbstractQuantBuffer ( owning_heap, size_capacity )
    {
        cerr << "new QuantBuffer(" << capacity << ")\n";
    };

    ~QuantBuffer() {};

    operator T() {
        return *(reinterpret_cast<T *>(head_ptr));
    };

    T operator()(int backwards_index) {
        return *(reinterpret_cast<T *>(head_ptr) - backwards_index);   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    T& operator[](int backwards_index) {
        cerr << "QuantBuffer::[] " << backwards_index << "ptr is: " << (reinterpret_cast<unsigned>(head_ptr)) << "\n";
        return *(reinterpret_cast<T *>(head_ptr) - backwards_index);   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

    int getSizeInBytes() final {
        return capacity * sizeof(T);
    }

    inline int getDataTypeSize() final {
        return sizeof(T);
    }

};


#endif
