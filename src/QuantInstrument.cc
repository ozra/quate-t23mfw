#include "QuantInstrument.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "rfx11_types.hh"
#include "QuantSymbolsRegistry.hh"

namespace t23m {
using namespace t23m;

// typedef String SecuritySymbol;  // "ZARJPY", "XAGUSD", etc.
// typedef String CurrencySymbol;  // "EUR", "XAG", "SEK", etc.
// typedef int SecurityNumberCode; // numcode for "ZARJPY", "XAGUSD", etc.
// typedef int CurrencyNumberCode; // numcode for "EUR", "XAG", "SEK", etc.

class Instrument {
   public:
    Instrument() {};
    Instrument(std::string symbol, N leverage_ratio, N pip_decimal)
        : symbol_code_(get_symbol_number_code(symbol))
        , leverage_ratio_(leverage_ratio)
        , pip_decimal_(pip_decimal) {};
    /*
    String getSymbol() {
        get_symbol_from_code(symbol_code_)
    }
    */

    SecurityNumberCode symbol_code_ = NIL;
    N leverage_ratio_ = 1;
    N pip_decimal_ = 0;
};
}

#endif
