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


class Instrument
{
  public:
    Instrument() {};
    Instrument(char const * const symbol, natural leverage_ratio,
               natural significant_decimals)
        : symbol_name_(symbol)
        , symbol_code_(get_symbol_number_code(symbol))
        , leverage_ratio_(leverage_ratio)
        , significant_decimals_(significant_decimals) {};
    /*
    String getSymbol() {
        get_symbol_from_code(symbol_code_)
    }
    */

    char const * const symbol_name_ = nullptr;
    SecurityNumberCode symbol_code_ = NIL;
    natural leverage_ratio_ = 1;
    natural significant_decimals_ = 0;
};

}

#endif
