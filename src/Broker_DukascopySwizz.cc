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

#include "QuantInstrument.hh"

#include "QuantBase.hh" // Move QuantTime to QuantTime.hh - include that from base..

namespace t23m {
namespace DUKASCOPY {

using namespace t23m;

auto get_dukascopy_instrument(String symbol, QuantTime time) -> Instrument;
}
}

#endif

namespace t23m {
namespace DUKASCOPY {

const char* const traded_symbols[] = {
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

//! Compose an object containing both static and time dependent data for a
// security
/** Since several factors of a security may change with time - daily, weekly or
 * non periodially for different reasons - this is a necessity.
*/

/*
PERHAPS:

class DukascopyGnrlBroker : Broker {
    getInstrument(String symbol, QuantTime time) override;


};

class DukascopyEurope : DukascopyGnrlBroker {
    getInstrument(String symbol, QuantTime time) final;


};

*/

auto get_dukascopy_instrument(String symbol, QuantTime time) -> Instrument {
    Instrument instrument;

    return instrument;
}
}
}
