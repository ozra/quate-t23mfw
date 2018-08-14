#include "QuantSymbolsRegistry.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "rfx11_types.hh"
#include <string>
#include <cstring>

namespace t23m {
using namespace t23m;

typedef std::string String;

char const* const symbol_names[] = {
    "_NIL_",

    // Cryptos, and reserved space for future cryptos
    "BTCUSD",      "LTCUSD",    "BTCCNY",        "LTCCNY",

    "FOO1", "FOO2", "FOO3", "FOO4", "FOO5", "FOO6", "FOO7",
    "FOO8", "FOO9", "FOO10", "FOO11", "FOO12", "FOO13", "FOO14",
    "FOO15", "FOO16", "FOO17", "FOO18", "FOO19", "FOO20",

    // Common forex pairs
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

constexpr int TOTAL_SYMBOL_COUNT = (int) sizeof(symbol_names);


typedef String SecuritySymbol; // "ZARJPY", "XAGUSD", etc.
typedef String CurrencySymbol; // "EUR", "XAG", "SEK", etc.

typedef enum {
    NIL = 0,
    BTCUSD = 1,
    LTCUSD,
    BTCCNY,
    LTCCNY,
    FOO1,
    FOO2,
    FOO3,
    FOO4,
    FOO5,
    FOO6,
    FOO7,
    FOO8,
    FOO9,
    FOO10,
    FOO11,
    FOO12,
    FOO13,
    FOO14,
    FOO15,
    FOO16,
    FOO17,
    FOO18,
    FOO19,
    FOO20,

    AUDUSD,
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

} SecurityNumberCode, CurrencyNumberCode;

//SecurityNumberCode get_symbol_number_code(String symbol);
int search_string_list_for_matching_row(char const* const list[],
                                        int list_len, char const* const symbol);
SecurityNumberCode get_symbol_number_code(char const* const symbol);
char const* get_symbol_name(SecurityNumberCode code);

}

#endif

namespace t23m {

int search_string_list_for_matching_row(char const* const list[],
                                        int list_len, char const* const symbol)
{
    for (auto i = 0; i < list_len; ++i) {
        if (strcmp(list[i], symbol) == 0) {
            return i;
        }
    }
    return 0;   // *NOTE* - 0 is considered to be a "_NIL_" post, we do not return -1.
}

SecurityNumberCode get_symbol_number_code(char const* const symbol)
{
    return SecurityNumberCode(search_string_list_for_matching_row(symbol_names,
                              TOTAL_SYMBOL_COUNT, symbol));
}

char const* get_symbol_name(SecurityNumberCode code)
{
    if (int(code) >= TOTAL_SYMBOL_COUNT) {
        code = NIL;
    }
    return symbol_names[code];
}

}
