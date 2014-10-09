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
    ReversedCircularStructBuffer ( int size );
    ~ReversedCircularStructBuffer ();

    T& operator[]( int backwards_index );
    T& advance ();    // return reference to [0] from here...
    void reset ();

  //private:
    int capacity;
    int size = 0;
    int pos = -1;

  private:
    T *buffer;
};


template <typename T>
ReversedCircularStructBuffer<T>::ReversedCircularStructBuffer ( int psize )
: capacity( psize ),
  buffer( new T[psize] )
{}

template <typename T>
ReversedCircularStructBuffer<T>::~ReversedCircularStructBuffer ()
{
    delete[] buffer;
}

template <typename T>
T& ReversedCircularStructBuffer<T>::operator[](int backwards_index)
{
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

    if ( size < capacity ) {
        ++size;
    }

    return buffer[pos];

}

template <typename T>
void ReversedCircularStructBuffer<T>::reset ()
{
    pos = 0;
    size = 0;
}

#endif
