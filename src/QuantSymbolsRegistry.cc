#include "QuantSymbolsRegistry.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "rfx11_types.hh"
#include <string>

namespace t23m {
using namespace t23m;

typedef std::string String;

const char* const symbol_names[] = {
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

const N TOTAL_SYMBOL_COUNT = sizeof(symbol_names);


typedef String SecuritySymbol; // "ZARJPY", "XAGUSD", etc.
typedef String CurrencySymbol; // "EUR", "XAG", "SEK", etc.

typedef enum {
    NIL = 0,
    AUDUSD = 1,
    EURUSD,
    GBPUSD,
    NZDUSD,
    USDCAD,
    USDCHF,
    USDJPY,
    AUDCAD,
    AUDCHF,
    AUDJPY,
    AUDNZD,
    CADCHF,
    CADJPY,
    CHFJPY,
    EURAUD,
    EURCAD,
    EURCHF,
    EURGBP,
    EURJPY,
    EURNOK,
    EURNZD,
    EURSEK,
    GBPAUD,
    GBPCAD,
    GBPCHF,
    GBPJPY,
    GBPNZD,
    NZDCAD,
    NZDCHF,
    NZDJPY,
    USDNOK,
    USDSEK,
    USDSGD,
    XAGUSD,
    XAUUSD,
    AUDSGD,
    CADHKD,
    CHFPLN,
    CHFSGD,
    EURDKK,
    EURHKD,
    EURHUF,
    EURMXN,
    EURPLN,
    EURRUB,
    EURSGD,
    EURTRY,
    EURZAR,
    HKDJPY,
    MXNJPY,
    NZDSGD,
    SGDJPY,
    USDBRL,
    USDDKK,
    USDHKD,
    USDHUF,
    USDMXN,
    USDPLN,
    USDRUB,
    USDTRY,
    USDZAR,
    ZARJPY,
    BRENTCMDUSD,
    WTICMDUSD,
    CUCMDUSD,
    AUCMDUSD,
    E_SI,
    PDCMDUSD,
    PTCMDUSD,
    USAMJRIDXUSD,
    USA30IDXUSD,
    BRAIDXBRL,
    USATECHIDXUSD,
    USACOMIDXUSD,
    USA500IDXUSD,
    HKGIDXHKD,
    JPNIDXJPY,
    AUSIDXAUD,
    NLDIDXEUR,
    FRAIDXEUR,
    DEUIDXEUR,
    EUSIDXEUR,
    GBRIDXGBP,
    ESPIDXEUR,
    ITAIDXEUR,
    CHEIDXCHF
} SecurityNumberCode,
    CurrencyNumberCode;

SecurityNumberCode get_symbol_number_code(String symbol);
}

#endif

namespace t23m {

SecurityNumberCode get_symbol_number_code(String symbol) {
    for (auto i = 0; i < sizeof(symbol_names); ++i) {
        if (symbol_names[i] == symbol) {
            return SecurityNumberCode(i);
        }
    }
    return NIL;
}
}
