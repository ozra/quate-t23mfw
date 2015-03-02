#include "HardSignal.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-12
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <vector>
#include <iostream>

#include <typeinfo>

/*
 *
 *
 * *TODO* change "typename P" to a param-pack - so that we can have 0 params..
 *
 *
 */

#ifndef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT

template <class T, typename... Ps>
struct MemberPtrPair {
    MemberPtrPair() : class_ptr{ nullptr }, fn_ptr{ nullptr } {}

    MemberPtrPair(T * class_ptr, void (T::*fn_ptr)(Ps... args))
        : class_ptr{ class_ptr }, fn_ptr{ fn_ptr } {}

    T * class_ptr;
    void (T::*fn_ptr)(Ps... args);
};

#endif

/*


*TODO*


simplify hard-signalling

template <typename... Ps>
class HardSignalType { // HardSlot {
   public:
    virtual ~HardSignalType() = 0;
    virtual void slot_member(Ps... args) = 0;
};


*/

template <class T, typename... Ps>
class HardSignal
{
  public:
    ~HardSignal()
    {
        std::cerr << "HardSignal::~HardSignal - - DESTRUCTOR - -"
                  << "\n";
    }

    /*
    template <P>
    inline void operator()<void> ( T *class_ptr, void (T::*fn_ptr)() ) {
        connect( class_ptr, fn_ptr );
    }
    */

    inline void operator()(T * p_class_ptr, void (T::*p_fn_ptr)(Ps... args))
    {
        connect(p_class_ptr, p_fn_ptr);
    }

    inline void connect(T * p_class_ptr, void (T::*p_fn_ptr)(Ps... args))
    {
        std::cerr << "Adding HardSignal\n";
        #ifdef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT
        class_ptr = p_class_ptr;
        fn_ptr = p_fn_ptr;
        #else
        funcs.push_back(MemberPtrPair<T, Ps...>(p_class_ptr, p_fn_ptr));
        #endif
    }

    inline void emit(Ps... args)
    {
// cerr << "Emit HardSignal via ()" << "\n";
// cerr << sizeof(funcs) << "\n";
// cerr << funcs.size() << "\n";
        #ifdef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT
        // if ( class_ptr ) {
        (class_ptr->*(fn_ptr))(args...);
//}
        #else
        for (auto pair : funcs) {
            // try {
            (pair.class_ptr->*(pair.fn_ptr))(args...);
            //} catch (...) {
            //    cerr << "\n\nSome shit went down when calling
            // listener!\n\n\n";
            //}
        }
        #endif
    }

  private:
    #ifdef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT
    T * class_ptr = nullptr;
    void (T::*fn_ptr)(Ps...) = nullptr;
    #else
    std::vector<MemberPtrPair<T, Ps...>> funcs;
    #endif
};

#endif
