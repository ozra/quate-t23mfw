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

// Exception:
class bad_quantbuffer_use: public std::exception {  //  - 2014-08-12/ORC(19:51)
  public:
    virtual const char* what() const throw();
};

extern bad_quantbuffer_use _bad_quantbuffer_use_;


template <typename T>
class QuantBuffer {
  public:
    QuantBuffer(int size) : capacity (size) {};
    ~QuantBuffer() {};

    operator T() {
        return *head_ptr;
    };
    T operator()(int backwards_index) {
        return *(head_ptr - backwards_index);   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };
    T& operator[](int backwards_index) {
        return *(head_ptr - backwards_index);   // Returning a reference is good because it can be assigned to - BUT - we don't want the performance hit. Use () for setting/updating, [] for getting.
    };

  //protected:
    int capacity = 0;
    T   *tail_ptr = nullptr;
    T   *head_ptr = nullptr;

};

//QuantBuffer::QuantBuffer() { }

//QuantBuffer::~QuantBuffer() { }

// Om vi returnerar referens kan man ju tilldela till den...
/*
T & QuantBuffer::operator[] (int backwards_index) {
    return &(*(head_ptr - backwards_index));
}
*/

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


// *TODO* QuantSynchronizedBufferHeap...
class QuantBufferHeap {
  public:
    QuantBufferHeap ();
    ~QuantBufferHeap ();

    QuantBuffer<QuantTime> *            newTimeBuffer(int size);
    //void    newIntBuffer(int size);
    QuantBuffer<QuantReal> *            newRealBuffer(int size);
    //void    newBuffer(int size);

    //template <typedef T>
    //QuantBuffer<T> *            newBuffer(int size);

    int     allocate();     // 0 = ok, else error code
    void    advance();

  private:
    char                                *heap;              // TODO(ORC) make sure we're on even 8byte...
    char                                *heap_end_boundary;
    int                                 total_byte_size = 0;
    int                                 biggest_size = 0;

    vector<QuantBuffer<QuantReal> *>    real_buffers;
    vector<QuantBuffer<QuantTime> *>    time_buffers;

};

#endif
