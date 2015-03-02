#include "HardClosure.hh"
#ifdef INTERFACE
/**
* Created:  2014-12-06
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/*
 *  Code by codereview.stackexchange.com :: user1095108
 *  http://codereview.stackexchange.com/questions/14730/impossibly-fast-delegate-in-c11
 */


/*



   *TODO*  revise completely - we want ONLY OBJECT MEMBER METHODS POINTERS!!!



*/



#include <cassert>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

template <typename T> class HardClosure;

template<class real, class ...A>
class HardClosure<real(A...)>
{
    using stub_ptr_type = real(*)(void *, A && ...);

    HardClosure(void * const o, stub_ptr_type const m) noexcept :
        object_ptr_(o),
        stub_ptr_(m)
    {
    }

  public:
    HardClosure() = default;

    HardClosure(HardClosure const &) = default;

    HardClosure(HardClosure &&) = default;

    HardClosure(::std::nullptr_t const) noexcept : HardClosure() { }

    template < class C, typename =
               typename ::std::enable_if < ::std::is_class<C> {} >::type >
    explicit HardClosure(C const * const o) noexcept :
        object_ptr_(const_cast<C * >(o))
    {
    }

    template < class C, typename =
               typename ::std::enable_if < ::std::is_class<C> {} >::type >
    explicit HardClosure(C const & o) noexcept :
        object_ptr_(const_cast<C * >(&o))
    {
    }

    template <class C>
    HardClosure(C * const object_ptr, real(C::* const method_ptr)(A...))
    {
        *this = from(object_ptr, method_ptr);
    }

    template <class C>
    HardClosure(C * const object_ptr, real(C::* const method_ptr)(A...) const)
    {
        *this = from(object_ptr, method_ptr);
    }

    template <class C>
    HardClosure(C & object, real(C::* const method_ptr)(A...))
    {
        *this = from(object, method_ptr);
    }

    template <class C>
    HardClosure(C const & object, real(C::* const method_ptr)(A...) const)
    {
        *this = from(object, method_ptr);
    }

    template <
        typename T,
        typename = typename ::std::enable_if <
            !::std::is_same<HardClosure, typename ::std::decay<T>::type> {}
            >::type
        >
    HardClosure(T && f) :
        store_(operator new(sizeof(typename ::std::decay<T>::type)),
               functor_deleter<typename ::std::decay<T>::type>),
        store_size_(sizeof(typename ::std::decay<T>::type))
    {
        using functor_type = typename ::std::decay<T>::type;
        new(store_.get()) functor_type(::std::forward<T>(f));
        object_ptr_ = store_.get();
        stub_ptr_ = functor_stub<functor_type>;
        deleter_ = deleter_stub<functor_type>;
    }

    HardClosure & operator=(HardClosure const &) = default;

    HardClosure & operator=(HardClosure &&) = default;

    template <class C>
    HardClosure & operator=(real(C::* const rhs)(A...))
    {
        return *this = from(static_cast<C *>(object_ptr_), rhs);
    }

    template <class C>
    HardClosure & operator=(real(C::* const rhs)(A...) const)
    {
        return *this = from(static_cast<C const *>(object_ptr_), rhs);
    }

    template <
        typename T,
        typename = typename ::std::enable_if <
            !::std::is_same<HardClosure, typename ::std::decay<T>::type> {}
            >::type
        >
    HardClosure & operator=(T && f)
    {
        using functor_type = typename ::std::decay<T>::type;
        if ((sizeof(functor_type) > store_size_) || !store_.unique()) {
            store_.reset(operator new(sizeof(functor_type)),
                         functor_deleter<functor_type>);
            store_size_ = sizeof(functor_type);
        }
        else {
            deleter_(store_.get());
        }
        new(store_.get()) functor_type(::std::forward<T>(f));
        object_ptr_ = store_.get();
        stub_ptr_ = functor_stub<functor_type>;
        deleter_ = deleter_stub<functor_type>;
        return *this;
    }

    template <real(* const function_ptr)(A...)>
    static HardClosure from() noexcept
    {
        return { nullptr, function_stub<function_ptr> };
    }

    template <class C, real(C::* const method_ptr)(A...)>
    static HardClosure from(C * const object_ptr) noexcept
    {
        return { object_ptr, method_stub<C, method_ptr> };
    }

    template <class C, real(C::* const method_ptr)(A...) const>
    static HardClosure from(C const * const object_ptr) noexcept
    {
        return { const_cast<C *>(object_ptr), const_method_stub<C, method_ptr> };
    }

    template <class C, real(C::* const method_ptr)(A...)>
    static HardClosure from(C & object) noexcept
    {
        return { &object, method_stub<C, method_ptr> };
    }

    template <class C, real(C::* const method_ptr)(A...) const>
    static HardClosure from(C const & object) noexcept
    {
        return { const_cast<C *>(&object), const_method_stub<C, method_ptr> };
    }

    template <typename T>
    static HardClosure from(T && f)
    {
        return ::std::forward<T>(f);
    }

    static HardClosure from(real(* const function_ptr)(A...))
    {
        return function_ptr;
    }

    template <class C>
    using member_pair =
        ::std::pair<C * const, real(C::* const)(A...)>;

    template <class C>
    using const_member_pair =
        ::std::pair<C const * const, real(C::* const)(A...) const>;

    template <class C>
    static HardClosure from(C * const object_ptr,
                            real(C::* const method_ptr)(A...))
    {
        return member_pair<C>(object_ptr, method_ptr);
    }

    template <class C>
    static HardClosure from(C const * const object_ptr,
                            real(C::* const method_ptr)(A...) const)
    {
        return const_member_pair<C>(object_ptr, method_ptr);
    }

    template <class C>
    static HardClosure from(C & object, real(C::* const method_ptr)(A...))
    {
        return member_pair<C>(&object, method_ptr);
    }

    template <class C>
    static HardClosure from(C const & object,
                            real(C::* const method_ptr)(A...) const)
    {
        return const_member_pair<C>(&object, method_ptr);
    }

    void reset() { stub_ptr_ = nullptr; store_.reset(); }

    void reset_stub() noexcept { stub_ptr_ = nullptr; }

    void swap(HardClosure & other) noexcept { ::std::swap(*this, other); }

    bool operator==(HardClosure const & rhs) const noexcept
    {
        return (object_ptr_ == rhs.object_ptr_) && (stub_ptr_ == rhs.stub_ptr_);
    }

    bool operator!=(HardClosure const & rhs) const noexcept
    {
        return !operator==(rhs);
    }

    bool operator<(HardClosure const & rhs) const noexcept
    {
        return (object_ptr_ < rhs.object_ptr_) ||
               ((object_ptr_ == rhs.object_ptr_) && (stub_ptr_ < rhs.stub_ptr_));
    }

    bool operator==(::std::nullptr_t const) const noexcept
    {
        return !stub_ptr_;
    }

    bool operator!=(::std::nullptr_t const) const noexcept
    {
        return stub_ptr_;
    }

    explicit operator bool() const noexcept { return stub_ptr_; }

    real operator()(A... args) const
    {
//  assert(stub_ptr);
        return stub_ptr_(object_ptr_, ::std::forward<A>(args)...);
    }

  private:
    friend struct ::std::hash<HardClosure>;

    using deleter_type = void (*)(void *);

    void * object_ptr_;
    stub_ptr_type stub_ptr_{};

    deleter_type deleter_;

    ::std::shared_ptr<void> store_;
    ::std::size_t store_size_;

    template <class T>
    static void functor_deleter(void * const p)
    {
        static_cast<T *>(p)->~T();
        operator delete(p);
    }

    template <class T>
    static void deleter_stub(void * const p)
    {
        static_cast<T *>(p)->~T();
    }

    template <real(*function_ptr)(A...)>
    static real function_stub(void * const, A && ... args)
    {
        return function_ptr(::std::forward<A>(args)...);
    }

    template <class C, real(C::*method_ptr)(A...)>
    static real method_stub(void * const object_ptr, A && ... args)
    {
        return (static_cast<C *>(object_ptr)->*method_ptr)(
                   ::std::forward<A>(args)...);
    }

    template <class C, real(C::*method_ptr)(A...) const>
    static real const_method_stub(void * const object_ptr, A && ... args)
    {
        return (static_cast<C const *>(object_ptr)->*method_ptr)(
                   ::std::forward<A>(args)...);
    }

    template <typename>
    struct is_member_pair : std::false_type { };

    template <class C>
    struct is_member_pair<::std::pair<C * const,
           real(C::* const)(A...)>> : std::true_type {
           };

    template <typename>
    struct is_const_member_pair : std::false_type { };

    template <class C>
    struct is_const_member_pair<::std::pair<C const * const,
           real(C::* const)(A...) const>> : std::true_type {
    };

    template <typename T>
    static typename ::std::enable_if <
    !(is_member_pair<T> {} ||
      is_const_member_pair<T> {}),
                           real
                           >::type
                           functor_stub(void * const object_ptr, A && ... args)
    {
        return (*static_cast<T *>(object_ptr))(::std::forward<A>(args)...);
    }

    template <typename T>
    static typename ::std::enable_if <
    is_member_pair<T> {} ||
    is_const_member_pair<T> {},
                         real
                         >::type
                         functor_stub(void * const object_ptr, A && ... args)
    {
        return (static_cast<T *>(object_ptr)->first->*
                static_cast<T *>(object_ptr)->second)(::std::forward<A>(args)...);
    }
};

namespace std {
template <typename real, typename ...A>
struct hash<::HardClosure<real(A...)>> {
                                        size_t operator()(::HardClosure<real(A...)> const & d) const noexcept
{
    auto const seed(hash<void *>()(d.object_ptr_));
    return hash<typename ::HardClosure<real(A...)>::stub_ptr_type>()(
               d.stub_ptr_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
                                    };
}


#endif
