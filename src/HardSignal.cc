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

template <class T, typename P>
struct MemberPtrPair {
    MemberPtrPair ()
    :
        class_ptr { nullptr },
        fn_ptr { nullptr }
    {}

    MemberPtrPair ( T *class_ptr, void (T::*fn_ptr)(P) )
    :
        class_ptr { class_ptr },
        fn_ptr { fn_ptr }
    {}

    T *                     class_ptr;
    void                    (T::*fn_ptr)(P);
};

template <class T, typename P>
class HardSignal {
  public:
    ~HardSignal () {
        std::cerr << "HardSignal::~HardSignal - - DESTRUCTOR - -" << "\n";
    }

    /*
    template <P>
    inline void operator()<void> ( T *class_ptr, void (T::*fn_ptr)() ) {
        connect( class_ptr, fn_ptr );
    }
    */

    inline void operator() ( T *p_class_ptr, void (T::*p_fn_ptr)(P) ) {
        connect( p_class_ptr, p_fn_ptr );
    }

    inline void connect ( T *p_class_ptr, void (T::*p_fn_ptr)(P) ) {
        std::cerr << "Adding HardSignal\n";
        #ifdef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT
            class_ptr = p_class_ptr;
            fn_ptr = p_fn_ptr;
        #else
            funcs.push_back( MemberPtrPair<T,P>(p_class_ptr, p_fn_ptr ) );
        #endif
    }

    inline void emit ( P val ) {
        //cerr << "Emit HardSignal via ()" << "\n";
        //cerr << sizeof(funcs) << "\n";
        //cerr << funcs.size() << "\n";

        #ifdef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT
            #ifdef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
                (class_ptr->*(fn_ptr))( val );
            #else
                if ( class_ptr ) {
                    ( class_ptr->*(fn_ptr) )( val );
                }
            #endif

        #else
            for ( auto pair : funcs ) {
                //try {
                ( pair.class_ptr->*(pair.fn_ptr) )( val );
                //} catch (...) {
                //    cerr << "\n\nSome shit went down when calling listener!\n\n\n";
                //}
            }
        #endif
    }

  private:
    #ifdef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT
        T *                     class_ptr = nullptr;
        void                    (T::*fn_ptr)(P) = nullptr;
    #else
        std::vector< MemberPtrPair<T, P> > funcs;
    #endif
};

#endif


