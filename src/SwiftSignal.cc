#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <vector>
//#include <functional>
#include <boost/function.hpp>

typedef std::function<void()> VoidFn;
//typedef void (*)(void) VoidFn;
//typedef std::function<void(void)> VoidFn;
//typedef auto(*)() VoidFn;


//template <typename... A>
class SwiftSignal {
  public:
    ~SwiftSignal ();

    void operator() (VoidFn);
    void connect (VoidFn);
    void emit ();

  private:
    std::vector<VoidFn> funcs;

};

#endif

#include "SwiftSignal.hh"

//#include <stdio.h>
#include "QuantBase.hh"

SwiftSignal::~SwiftSignal ( ) {
    cerr << "SwiftSignal::~SwiftSignal - - DESTRUCTOR - -" << "\n";
}

void SwiftSignal::operator() ( VoidFn fn ) {
    cerr << "Adding SwiftSignal via ()\n";
    funcs.push_back(fn);
}

void SwiftSignal::connect ( VoidFn fn ) {
    cerr << "Adding SwiftSignal via connect()\n";
    funcs.push_back(fn);
}

void SwiftSignal::emit () {
    //cerr << "Emit SwiftSignal via ()" << "\n";
    //cerr << sizeof(funcs) << "\n";
    //cerr << funcs.size() << "\n";

    for (auto fn : funcs) {
        //cerr << "SwiftSignal iteration!\n";

        //try {
            fn();

        //} catch (...) {
        //    cerr << "\n\nSome shit went down when calling listener!\n\n\n";
        //    // SHIT HAPPENED - TODO(ORC)
        //}
    }
    //cerr << "Done Emitting SwiftSignal via ()\n";
}
