/**
* Created:  2014-08-10
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBuffers.hh"

const char* bad_quantbuffer_use::what() const throw() {
    return "You can't add buffers after allocating a t23mqf:: / quantron:: BufferHeap!";
}

bad_quantbuffer_use _bad_quantbuffer_use_;

QuantBufferHeap::QuantBufferHeap () {
}

QuantBufferHeap::~QuantBufferHeap () {
    delete heap;

    for ( auto buf : time_buffers ) {
        delete buf;
    }
    for ( auto buf : real_buffers ) {
        delete buf;
    }

}

int QuantBufferHeap::allocate () {
    total_byte_size += biggest_size;

    heap = new char[total_byte_size];
    if ( !heap ) {
        return 1;
    }

    heap_end_boundary = heap + total_byte_size;
    char *ptr = heap;

    for ( auto buf : time_buffers ) {
        buf->tail_ptr = (QuantTime *) ptr;
        ptr += buf->capacity * sizeof(QuantTime);
        buf->head_ptr = (QuantTime *) (ptr - sizeof(QuantTime));
    }

    for ( auto buf : real_buffers ) {
        buf->tail_ptr = (QuantReal *) ptr;
        ptr += buf->capacity * sizeof(QuantReal);
        buf->head_ptr = (QuantReal *) (ptr - sizeof(QuantReal));
    }

    return 0;

}

void QuantBufferHeap::advance () {
    bool time_for_move = false;

    for ( auto buf : time_buffers ) {
        ++buf->tail_ptr;
        ++buf->head_ptr;
        if (buf->head_ptr + sizeof(QuantTime) >= (QuantTime *)heap_end_boundary) {
            time_for_move = true;
        }
    }

    for ( auto buf : real_buffers ) {
        ++buf->tail_ptr;
        ++buf->head_ptr;
        if (buf->head_ptr + sizeof(QuantReal) >= (QuantReal *)heap_end_boundary) {
            time_for_move = true;
        }
    }

    if ( time_for_move ) {
        /*
         *
         * *TODO*  move the last bufs mem area to beginning of heap
         *
        */
    }

}




// *TODO* generalize

QuantBuffer<QuantTime> * QuantBufferHeap::newTimeBuffer (int size) {
    if ( heap ) {
        throw _bad_quantbuffer_use_;
    }
    auto *buf = new QuantBuffer<QuantTime>(size);
    biggest_size = MAX(biggest_size, (int)sizeof(QuantTime) * size);
    total_byte_size += sizeof(QuantTime) * size;

    return buf;
}

QuantBuffer<QuantReal> * QuantBufferHeap::newRealBuffer (int size) {
    if ( heap ) {
        throw _bad_quantbuffer_use_;
    }
    auto *buf = new QuantBuffer<QuantReal>(size);
    biggest_size = MAX(biggest_size, (int)sizeof(QuantReal) * size);
    total_byte_size += sizeof(QuantReal) * size;

    return buf;
}

//template <typedef T>
//QuantBuffer<T> * QuantBufferHeap::newBuffer (int size) {
//    if (heap) {
//        TERROR("You can't add buffers after allocating!");
//    }
//    auto *buf = new QuantBuffer<T>(size);
//    biggest_size = MAX(biggest_size, (int) sizeof(T) * size);
//    total_byte_size += sizeof(T) * size;
//
//    return buf;
//}
