#include "mutatable_buffer.hh"
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

template <typename T> class mutatable_buffer {
   public:
    mutatable_buffer(size_t minimum_size = 0, size_t fixed_margin = 0)
        : fixed_margin_{ fixed_margin }
        , minimum_size_{ minimum_size } {
        cerr << "mutatable_buffer::constructor - A BONA FIDE CONSTRUCTION\n";

        if (minimum_size) {
            reserve(minimum_size, false);
        }
    }

    mutatable_buffer(L is_just_a_reference)
        : is_just_a_reference_{ is_just_a_reference } {
        cerr << "mutatable_buffer::constructor - JUST AN EMPTY REFERENCE\n";
    }

    mutatable_buffer(mutatable_buffer<T>& other)
        : buffer_front_{ other._buffer_front_ }
        , capacity_{ other.size_ }
        , size_{ other.size_ }
        , fixed_margin_{ 0 }
        , minimum_size_{ 0 }
        , is_just_a_reference_{ true } {
        cerr << "mutatable_buffer::COPY constructor - A CLONED REFERENCE\n";
        cerr << "buffer_ = " << (O*)buffer_front_ << "\n"
             << "capacity_ = " << capacity_ << "\n"
             << "size_ = " << size_ << "\n"
             << "is_just_a_reference_ = " << is_just_a_reference_ << "\n"
             << "\n";
    }

    mutatable_buffer(mutatable_buffer<T>&& other)
        : buffer_front_{ other.buffer_front_ }
        , capacity_{ other.size_ }
        , size_{ other.size_ }
        , fixed_margin_{ 0 }
        , is_just_a_reference_{ true } {
        cerr << "mutatable_buffer::MOVE constructor - A CLONED REFERENCE\n";
        cerr << "buffer_ = " << (O*)buffer_front_ << "\n"
             << "capacity_ = " << capacity_ << "\n"
             << "size_ = " << size_ << "\n"
             << "is_just_a_reference_ = " << is_just_a_reference_ << "\n"
             << "\n";
    }

    ~mutatable_buffer() {
        cerr << "\n\nmutatable_buffer::DESTRUCTOR: clean_uses = " << clean_uses_
             << " mutations = " << mutations_ << "\n";
        if (is_just_a_reference_ == false) {
            cerr << "Is not a reference only, we delete "
                 << (O*)buffer_front_;
            delete[] buffer_front_;
        }
    }

    T* resize(NN size, L maintain_current_data = true) {
        reserve(size, maintain_current_data);
        size_ = size;
        buffer_end_ = buffer_front_ + size_;
        buffer_cursor_ = buffer_front_;
        return buffer_front_;
    }

    T* reserve(NN size, L maintain_current_data = true) {
        cerr << "mutatable_buffer::reserve" << size << "\n";
        assert(is_just_a_reference_ == false);

        if (size <= capacity_) {
            ++clean_uses_;
            buffer_end_ = buffer_front_ + size_;
            buffer_cursor_ = buffer_front_; // *TODO* - right?
            return buffer_front_;

        } else {
            ++mutations_;
            capacity_ =
                std::max(minimum_size_,
                    NN(size + (size / 4) + fixed_margin_)); // the ( size / 4 ) will
            // likely be optimized by
            // gcc et all to ( size >> 2
            // ) while still being clear
            // in code.. - 2014-11-07 /
            // Oscar Campbell
            cerr << "mutation(): new size is " << size << " new capacity is "
                 << capacity_ << "\n";

            T* curr_buf = buffer_front_;
            buffer_front_ = new T[capacity_]; // Allocate with 25% margin and
                                              // possible fixed margin on top,
                                              // or at least the minimum size.

            if (maintain_current_data && curr_buf != nullptr) {
                std::memcpy((O*)buffer_front_, (O*)curr_buf, size_);
            }

            delete[] curr_buf;

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
    inline O set_specified_buffer_size(NN size) {
        specified_buffer_limit_ = size;
    }

    inline L is_at_end() {
        return (buffer_cursor_ >= buffer_end_);
    }

    inline L is_specified_buffer_filled() {
        _DP("is_specified_buffer_filled() -  size = "
            << size_ << " limit = " << specified_buffer_limit_ << "\n");
        return (size_ >= specified_buffer_limit_);
    }

    inline NN buffered_count() {
        return (std::min(size_, specified_buffer_limit_));
    }

    inline T* consume_specified_buffer() {
        rewind_shuffle(buffered_count());
        return buffer_front_;
    }

    inline O shrink_to_fit() {
        // We simply ignore this and keep the currently allocated amounts..
        return;
    }

    O wipe() { // deliberate free / delete
        ++mutations_;
        delete[] buffer_front_;
        buffer_cursor_ = buffer_front_ = buffer_end_ = buffer_capacity_end_ =
            nullptr;
        capacity_ = size_ = 0;
        cerr << "mutatable_buffer::wipe(): deliberately!\n";
    }

    inline O clear() { size_ = 0; }

    //! "Rewind the buffer" count T.
    /** Ie: copy what's above count up til buffer size (not capacity) to the
     *  beginning of buffer. Reset size to the amount of copied bytes.
     *  Basically, remove all data that has been handled / consumed and reset
     *  positions to beginning of buffer.
     */
    O rewind_shuffle(NN count) {
        cerr << "rewind_shuffle with " << count << " where size is " << size_
             << "\n";
        assert(count <= size_);
        memmove(buffer_front_, buffer_front_ + count,
                size_ - count); // We need to use memmove to be really sure.
        size_ = size_ - count;
        buffer_end_ = buffer_front_ + size_;
        buffer_cursor_ = nullptr;
        cerr << "size_ after rewind shuffle = " << size_ << "\n";
    }

    //! The same as rewind_shuffle ( NN count ) but calculates the count
    //  from the pointer compared to front(). Excluding pointer position!
    O rewind_shuffle(T* buf_ptr) {
        cerr << "rewind_shuffle with ptr"
             << "\n";
        rewind_shuffle((N)(buf_ptr - buffer_front_));
    }

    /*
     *  The ++ should be on the iterator ofcourse!
    inline mutatable_buffer<T> & operator++ () {
    */

    inline NN size() { return size_; }

    inline NN capacity() { return capacity_; }

    inline O push(T val) {
        if (size_ >= capacity_) {
            reserve(
                size_ + 1,
                true); // We use +1, but much more will probably be allocated
        }
        acc(val);
    }

    inline O acc(T val) {
        *buffer_cursor_ = val;
        ++buffer_cursor_;
        ++buffer_end_;
        ++size_;
    }

    inline O reset_write_cursor() { buffer_cursor_ = buffer_end_; }

    inline O reset_read_cursor() { buffer_cursor_ = buffer_front_; }

#ifdef IS_DEBUG
    inline T* on_free_leash_for(N expected_count) {
        expected_free_leash_movement_ = expected_count;
#else
    inline T* on_free_leash_for(N) {
#endif
        return buffer_cursor_;
    }

    inline O catch_up_reads(N count) {
        cerr << "catch_up_reads, with count int\n";
        catch_up_reads(buffer_end_ + count);
    }
    inline O catch_up_reads(T* buf_ptr) {
#ifdef IS_DEBUG
        N count = N(buf_ptr - buffer_cursor_);
        //_DPn("catch_up_reads, actual count is "
        //     << count << " expected max was " << expected_free_leash_movement_);
        assert(count <= expected_free_leash_movement_);
        assert(buf_ptr <= buffer_capacity_end_);
        //_Dn("ptr: " << (O*)buf_ptr << " and end: " << (O*)buffer_end_ << " ptr-'pos': " << (buf_ptr - buffer_front_));
        assert(buf_ptr <= buffer_end_);
        assert(buf_ptr >= buffer_cursor_);
        assert(buf_ptr > buffer_front_);
        expected_free_leash_movement_ = 0;
#endif

        buffer_cursor_ = buf_ptr;
    }

    inline O catch_up_writes(NN count) {
        cerr << "catch_up_writes, with count int\n";
        catch_up_writes(buffer_end_ + count);
    }
    inline O catch_up_writes(T* buf_ptr) {
        NN count = (N)(buf_ptr - buffer_cursor_);

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

        assert(size_ == N(buffer_end_ - buffer_front_));
        _DPn("catch_up: size_ " << size_ << " buffer_end_ - buffer_front_ "
                                << (buffer_end_ - buffer_front_));
    }

#ifdef IS_DEEPBUG
    inline L verify_pointer(T* buf_ptr) {
        if (buf_ptr > buffer_capacity_end_) {
            cerr << "discrepancy: " << (buf_ptr - buffer_front_)
                 << " vs capacity: " << capacity_ << "\n";
        }
        if (buf_ptr < buffer_front_) {
            cerr << "discrepancy: " << (buf_ptr - buffer_front_) << "\n";
        }

        assert(buf_ptr <= buffer_capacity_end_);
        assert(buf_ptr >= buffer_front_);

        return true;
    }
#else
    inline L verify_pointer(T*) { return true; }
#endif

    /*
    //! Returns true if re-allocations took place ( = user cached pointers must
    be updated )
    inline L grow ( NN size ) {
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
    inline T* front() {
        // cerr << "begin()";
        return buffer_front_;
    }
    inline T* begin() {
        // cerr << "begin()";
        return buffer_front_;
    }
    inline T* end() {
        // cerr << "end()";
        return buffer_end_;
    }
    inline T* back() {
        // cerr << "end()";
        return buffer_end_;
    }
    inline T* limit() {
        // cerr << "limit()";
        return buffer_capacity_end_;
    }
    inline T* spec_limit() {
        // cerr << "limit()";
        return buffer_front_ + specified_buffer_limit_;
    }

   private:
    T* buffer_front_ = nullptr;
    T* buffer_end_ = nullptr;
    T* buffer_capacity_end_ = nullptr;
    T* buffer_cursor_ = nullptr;

    NN capacity_ = 0;
    NN size_ = 0;
    NN specified_buffer_limit_ = 0;

    NN fixed_margin_ = 0;
    NN minimum_size_ = 0;
    ncount clean_uses_ = 0;
    ncount mutations_ = 0;

    L is_just_a_reference_ = false;

#ifdef IS_DEBUG
    NN expected_free_leash_movement_ = 0;
#endif
};

// Playing with functionalism for aestethics
template <typename T> inline auto front_of(mutatable_buffer<T>& buf) -> T* {
    return buf.begin();
}

#endif
