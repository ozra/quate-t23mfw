#include "QuantInstrument.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantSymbolsRegistry.hh"

namespace t23m {
using namespace t23m;

//typedef String SecuritySymbol;  // "ZARJPY", "XAGUSD", etc.
//typedef String CurrencySymbol;  // "EUR", "XAG", "SEK", etc.
//typedef int SecurityNumberCode; // numcode for "ZARJPY", "XAGUSD", etc.
//typedef int CurrencyNumberCode; // numcode for "EUR", "XAG", "SEK", etc.

class Instrument {
   public:
    /*
    String getSymbol() {
        get_symbol_from_code(symbol_code_)
    }
    */

    SecurityNumberCode symbol_code_;
    int leverage_ratio_;
    int pip_decimal_;
};
}


#endif
