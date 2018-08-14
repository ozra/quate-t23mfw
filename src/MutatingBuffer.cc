#include "MutatingBuffer.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-07
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/*
 * * Resources * *
 *
 * http://nadeausoftware.com/articles/2012/05/c_c_tip_how_copy_memory_quickly
 *
 */

#include "rfx11_lib_debug.hh"
#include "rfx11_types.hh"

#include <cstring> // std::memcpy
#include <iostream>


template <typename T> class MutatingBuffer
{
  public:
    MutatingBuffer(ff_size_t minimum_size = 0, ff_size_t fixed_margin = 0)
        : fixed_margin_{ fixed_margin }
        , minimum_size_{ minimum_size }
    {
        _Dn("MutatingBuffer::constructor - A BONA FIDE CONSTRUCTION\n");
        if (minimum_size) {
            reserve(minimum_size); //, false);
        }
    }

    MutatingBuffer(bool is_just_a_reference)
        : is_just_a_reference_{ is_just_a_reference }
    {
        _Dn("MutatingBuffer::constructor - JUST AN EMPTY REFERENCE\n");
    }

    MutatingBuffer(MutatingBuffer<T>& other)
        : buffer_front_{ other._buffer_front_ }
        , capacity_{ other.size_ }
        , size_{ other.size_ }
        , fixed_margin_{ 0 }
        , minimum_size_{ 0 }
        , is_just_a_reference_{ true }
    {
        _Dn("MutatingBuffer::COPY constructor - A CLONED REFERENCE\n");
        _Dn("buffer_ = " << (void*)buffer_front_ << "\n"
            << "capacity_ = " << capacity_ << "\n"
            << "size_ = " << size_ << "\n"
            << "is_just_a_reference_ = " << is_just_a_reference_ << "\n"
            << "\n");
    }

    MutatingBuffer(MutatingBuffer<T>&& other)
        : buffer_front_{ other.buffer_front_ }
        , capacity_{ other.size_ }
        , size_{ other.size_ }
        , fixed_margin_{ 0 }
        , is_just_a_reference_{ true }
    {
        _Dn("MutatingBuffer::MOVE constructor - A CLONED REFERENCE\n");
        _Dn("buffer_ = " << (void*)buffer_front_ << "\n"
            << "capacity_ = " << capacity_ << "\n"
            << "size_ = " << size_ << "\n"
            << "is_just_a_reference_ = " << is_just_a_reference_ << "\n"
            << "\n");
    }

    ~MutatingBuffer()
    {
        _Dn("\n\nMutatingBuffer::DESTRUCTOR: clean_uses = " << clean_uses_
            << " mutations = " << mutations_ << "\n");
        if (is_just_a_reference_ == false) {
            _Dn("Is not a reference only, we delete "
                << (void*)buffer_front_);
            //delete[] buffer_front_;
            free(buffer_front_);
        }
    }

    T* resize(ff_size_t size)  // , bool maintain_current_data = true)
    {
        reserve(size); //, maintain_current_data);
        size_ = size;
        buffer_end_ = buffer_front_ + size_;
        buffer_cursor_ = buffer_front_;
        return buffer_front_;
    }

    T* reserve(ff_size_t size)  //, bool maintain_current_data = true)
    {
        // *TODO* as of 2014-12-29  it always maintains (changed from new/delete to realloc..)
        _Dn("MutatingBuffer::reserve" << size << "\n");
        assert(is_just_a_reference_ == false);
        if (size <= capacity_) {
            ++clean_uses_;
            buffer_end_ = buffer_front_ + size_;
            buffer_cursor_ = buffer_front_; // *TODO* - right?
            return buffer_front_;
        }
        else {
            ++mutations_;
            capacity_ =
                std::max(minimum_size_,
                         ff_size_t(size + (size / 4) + fixed_margin_)); // the ( size / 4 ) will
            // likely be optimized by
            // gcc et all to ( size >> 2
            // ) while still being clear
            // in code.. - 2014-11-07 /
            // Oscar Campbell
            _Dn("mutation(): new size is " << size << " new capacity is "
                << capacity_ << "\n");
            buffer_front_ = (T*) realloc(buffer_front_,
                                         capacity_ * sizeof(T)); // nullptr safe
            /*

            T* curr_buf = buffer_front_;
            buffer_front_ = new T[capacity_]; // Allocate with 25% margin and
                                              // possible fixed margin on top,
                                              // or at least the minimum size.

            if (maintain_current_data && curr_buf != nullptr) {
                std::memcpy((void*)buffer_front_, (void*)curr_buf, size_);
            }

            delete[] curr_buf;
            */
            buffer_end_ = buffer_front_ + size_;
            buffer_cursor_ = buffer_front_; // *TODO* - right?
            buffer_capacity_end_ = buffer_front_ + capacity_;
            return buffer_front_;
        }
    }

    //! Set the buffer specified limit.
    /** This is part of the utility methods for keeping track of fulfillment
     *  of a specific buffer length, and consuming it, and automatically
     *  maintaining overflows.
     */
    inline void set_specified_buffer_size(ff_size_t size)
    {
        specified_buffer_limit_ = size;
    }

    inline bool is_at_end()
    {
        return (buffer_cursor_ >= buffer_end_);
    }

    inline bool is_specification_filled()
    {
        _DP("is_specification_filled() -  size = "
            << size_ << " limit = " << specified_buffer_limit_ << "\n");
        return (size_ >= specified_buffer_limit_);
    }

    inline int buffered_count()
    {
        return (std::min(size_, specified_buffer_limit_));
    }

    inline T* consume_specified_buffer()
    {
        rewind_shuffle(buffered_count());
        return buffer_front_;
    }

    inline void shrink_to_fit()
    {
        // We simply ignore this and keep the currently allocated amounts..
        return;
    }

    void wipe()   // deliberate free / delete
    {
        ++mutations_;
        //delete[] buffer_front_;
        free(buffer_front_);
        buffer_cursor_ = buffer_front_ = buffer_end_ = buffer_capacity_end_ =
                                             nullptr;
        capacity_ = size_ = 0;
        _Dn("MutatingBuffer::wipe(): deliberately!\n");
    }

    inline void clear() { size_ = 0; }

    //! "Rewind the buffer" count T.
    /** Ie: copy what's above count up til buffer size (not capacity) to the
     *  beginning of buffer. Reset size to the amount of copied bytes.
     *  Basically, remove all data that has been handled / consumed and reset
     *  positions to beginning of buffer.
     */
    void rewind_shuffle(int count)
    {
        _Dn("rewind_shuffle with " << count << " where size is " << size_);
        assert(count <= size_);
        memmove(buffer_front_, buffer_front_ + count,
                size_ - count); // We need to use memmove to be really sure.
        size_ = size_ - count;
        buffer_end_ = buffer_front_ + size_;
        buffer_cursor_ -= count;
        _Dn("size_ after rewind shuffle = " << size_);
    }

    //! The same as rewind_shuffle ( int count ) but calculates the count
    //  from the pointer compared to front(). Excluding pointer position!
    void rewind_shuffle(T* buf_ptr)
    {
        _Dn("rewind_shuffle with ptr");
        rewind_shuffle((int)(buf_ptr - buffer_front_));
    }

    /*
     *  The ++ should be on the iterator ofcourse!
    inline MutatingBuffer<T> & operator++ () {
    */

    inline int size() { return size_; }

    inline int capacity() { return capacity_; }

    inline void push(T val)
    {
        if (size_ >=
            capacity_) {   // *TODO* `==` should logically suffice - this is an overprotective class-system non-trustive measure..
            // We use +1, but much more will probably be allocated
            reserve(size_ + 1); //, true);
        }
        acc(val);
    }

    inline void operator << (T val)
    {
        push(val);
    }

    inline void append(T val)   // (2014-12-28 Oscar Campbell)
    {
        push(val);
    }

    inline void append(void* buf, ff_size_t len)    // (2014-12-28 Oscar Campbell)
    {
        // *TODO* keep track of largest appended block, as well as largestspan of
        // loose ptr running to estimate future block add sizes for better prediction
        // of pre-allocation
        if (size_ + len > capacity_) {
            reserve(size_ + len); //, true);
        }
        std::memcpy(static_cast<void*>(buffer_cursor_), static_cast<void*>(buf),
                    len);
        buffer_cursor_ += len;
    }

    inline void acc(T val)
    {
        *buffer_cursor_ = val;
        ++buffer_cursor_;
        ++buffer_end_;
        ++size_;
    }

    inline void reset_write_cursor() { buffer_cursor_ = buffer_end_; }

    inline void reset_read_cursor() { buffer_cursor_ = buffer_front_; }

    #ifdef IS_DEBUG
    inline T* on_free_leash_for(int expected_count)
    {
        expected_free_leash_movement_ = expected_count;
    #else
    inline T* on_free_leash_for(int)
    {
    #endif
        return buffer_cursor_;
    }

    inline void catch_up_reads(int count)
    {
        _Dn("catch_up_reads, with count int");
        catch_up_reads(buffer_end_ + count);
    }
    inline void catch_up_reads(T* buf_ptr)
    {
        #ifdef IS_DEBUG
        ff_size_t count = ff_size_t(buf_ptr - buffer_cursor_);
        //_DPn("catch_up_reads, actual count is "
        //     << count << " expected max was " << expected_free_leash_movement_);
        assert(count <= expected_free_leash_movement_);
        assert(buf_ptr <= buffer_capacity_end_);
        //_Dn("ptr: " << (void*)buf_ptr << " and end: " << (void*)buffer_end_ << " ptr-'pos': " << (buf_ptr - buffer_front_));
        assert(buf_ptr <= buffer_end_);
        assert(buf_ptr >= buffer_cursor_);
        assert(buf_ptr > buffer_front_);
        expected_free_leash_movement_ = 0;
        #endif
        buffer_cursor_ = buf_ptr;
    }

    inline void catch_up_writes(int count)
    {
        _Dn("catch_up_writes, with count int");
        catch_up_writes(buffer_end_ + count);
    }
    inline void catch_up_writes(T* buf_ptr)
    {
        int count = (int)(buf_ptr - buffer_cursor_);
        #ifdef IS_DEBUG
        _DPn("catch_up_writes, actual count is "
             << count << " expected max was " << expected_free_leash_movement_);
        assert(count <= expected_free_leash_movement_);
        assert(buf_ptr <= buffer_capacity_end_);
        assert(buf_ptr >= buffer_cursor_);
        assert(buf_ptr > buffer_front_);
        expected_free_leash_movement_ = 0;
        #endif
        size_ += count;
        buffer_end_ = buffer_cursor_ = buf_ptr;
        assert(size_ == int(buffer_end_ - buffer_front_));
        _DPn("catch_up: size_ " << size_ << " buffer_end_ - buffer_front_ "
             << (buffer_end_ - buffer_front_));
    }

    #ifdef IS_DEEPBUG
    inline bool verify_pointer(T* buf_ptr)
    {
        if (buf_ptr > buffer_capacity_end_) {
            _Dn("discrepancy: " << (buf_ptr - buffer_front_)
                << " vs capacity: " << capacity_);
        }
        if (buf_ptr < buffer_front_) {
            _Dn("discrepancy: " << (buf_ptr - buffer_front_));
        }
        assert(buf_ptr <= buffer_capacity_end_);
        assert(buf_ptr >= buffer_front_);
        return true;
    }
    #else
    inline bool verify_pointer(T*) { return true; }
    #endif

    /*
    //! Returns true if re-allocations took place ( = user cached pointers must
    be updated )
    inline bool grow ( int size ) {
        T* prev_buf = buffer_;

        if ( prev_buf != allocate( size, true ) ) {
            return true;
        } else {
            return false;
        }
    }
    */

    /*
    inline operator T * () {
        cerr << "auto cast";
        return buffer_;
    }
    */
    inline T* front()
    {
        // cerr << "begin()";
        return buffer_front_;
    }
    inline T* begin()
    {
        // cerr << "begin()";
        return buffer_front_;
    }
    inline T* end()
    {
        // cerr << "end()";
        return buffer_end_;
    }
    inline T* back()
    {
        // cerr << "end()";
        return buffer_end_;
    }
    inline T* limit()
    {
        // cerr << "limit()";
        return buffer_capacity_end_;
    }
    inline T* spec_limit()
    {
        // cerr << "limit()";
        return buffer_front_ + specified_buffer_limit_;
    }

  private:
    T* buffer_front_ = nullptr;
    T* buffer_end_ = nullptr;
    T* buffer_capacity_end_ = nullptr;
    T* buffer_cursor_ = nullptr;

    ff_size_t capacity_ = 0;
    ff_size_t size_ = 0;
    ff_size_t specified_buffer_limit_ = 0;

    ff_size_t fixed_margin_ = 0;
    ff_size_t minimum_size_ = 0;
    ff_size_t clean_uses_ = 0;
    ff_size_t mutations_ = 0;

    bool is_just_a_reference_ = false;

    #ifdef IS_DEBUG
    ff_size_t expected_free_leash_movement_ = 0;
    #endif
};

// Playing with functionalism for aestethics
template <typename T> inline auto front_of(MutatingBuffer<T>& buf) -> T* {
    return buf.begin();
}

#endif
