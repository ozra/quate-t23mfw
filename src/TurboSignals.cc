/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//#include <stdio.h>
#include "QuantBase.hh"

#include "TurboSignals.hh"

void TurboSignal::operator() ( std::function<void(void)> fn ) {
    cerr << "Adding TurboSignal via ()";
    funcs.push_back(fn);
}

void TurboSignal::connect ( std::function<void(void)> fn ) {
    cerr << "Adding TurboSignal via connect()";
    funcs.push_back(fn);
}

//void TurboSignal::emit (A...) {
void TurboSignal::emit () {
    cerr << "Emit TurboSignal via ()";

    for (auto fn : funcs) {
        try {
            //fn(A...);
            fn();

        } catch (...) {
            // SHIT HAPPENED - TODO(ORC)
        }
    }
}
