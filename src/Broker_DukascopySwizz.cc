#include "Broker_DukascopySwizz.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/*
 *
 * ZMQ for the JAVA interfacing lib we'll need to trade on dukascopy...
 *
 * http://www.dukascopy.com/wiki/#JForex_SDK
 * http://zeromq.org/bindings:java
 *
 */

#include "rfx11_types.hh"

#include "QuantBroker.hh"
#include "QuantInstrument.hh"

#include "QuantBase.hh" // Move QuantTime to QuantTime.hh - include that from base..

namespace t23m {
// namespace DUKASCOPY {

using namespace t23m;

class DukascopyGnrlBroker : BrokerAbstract
{
  public:
    // *TODO* - temp, for reference only for napkin calcs
    static const natural TRADED_SYMBOLS = 86;

    auto get_instrument(const char* symbol,
                        QuantTime time) -> Instrument override;
};

class DukascopyEurope : DukascopyGnrlBroker
{
  public:
    // auto get_instrument(const char* symbol, QuantTime time) -> Instrument final;
};
//}
}

#endif

namespace t23m {
// namespace DUKASCOPY {


//  *TODO*  switch to hashmap! - (2014-12-27 Oscar Campbell)

char const* const traded_symbols[] = {
    "_NIL_",
    "AUDUSD",      "EURUSD",    "GBPUSD",        "NZDUSD",       "USDCAD",
    "USDCHF",      "USDJPY",    "AUDCAD",        "AUDCHF",       "AUDJPY",
    "AUDNZD",      "CADCHF",    "CADJPY",        "CHFJPY",       "EURAUD",
    "EURCAD",      "EURCHF",    "EURGBP",        "EURJPY",       "EURNOK",
    "EURNZD",      "EURSEK",    "GBPAUD",        "GBPCAD",       "GBPCHF",
    "GBPJPY",      "GBPNZD",    "NZDCAD",        "NZDCHF",       "NZDJPY",
    "USDNOK",      "USDSEK",    "USDSGD",        "XAGUSD",       "XAUUSD",
    "AUDSGD",      "CADHKD",    "CHFPLN",        "CHFSGD",       "EURDKK",
    "EURHKD",      "EURHUF",    "EURMXN",        "EURPLN",       "EURRUB",
    "EURSGD",      "EURTRY",    "EURZAR",        "HKDJPY",       "MXNJPY",
    "NZDSGD",      "SGDJPY",    "USDBRL",        "USDDKK",       "USDHKD",
    "USDHUF",      "USDMXN",    "USDPLN",        "USDRUB",       "USDTRY",
    "USDZAR",      "ZARJPY",    "BRENTCMDUSD",   "WTICMDUSD",    "CUCMDUSD",
    "AUCMDUSD",    "E_SI",      "PDCMDUSD",      "PTCMDUSD",     "USAMJRIDXUSD",
    "USA30IDXUSD", "BRAIDXBRL", "USATECHIDXUSD", "USACOMIDXUSD", "USA500IDXUSD",
    "HKGIDXHKD",   "JPNIDXJPY", "AUSIDXAUD",     "NLDIDXEUR",    "FRAIDXEUR",
    "DEUIDXEUR",   "EUSIDXEUR", "GBRIDXGBP",     "ESPIDXEUR",    "ITAIDXEUR",
    "CHEIDXCHF"
};

Instrument DukascopyGnrlBroker::get_instrument(char const* const symbol,
        QuantTime time)
{
    if (search_string_list_for_matching_row(traded_symbols, TRADED_SYMBOLS + 1,
                                            symbol) == 0) {
        return Instrument("_NIL_", 0, 0);
    }
    else {
        return Instrument(symbol, 100 /* *TODO* - user and time dependant */,
                          5 /* *TODO* symbol dependent */);
    }
}

//! Compose an object containing both static and time dependent data for a
// security
/** Since several factors of a security may change with time - daily, weekly or
 * non periodially for different reasons - this is a necessity.
*/
//}
}
