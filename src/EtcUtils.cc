#include "EtcUtils.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <cstring>

namespace t23m {
namespace utils {

using namespace t23m;
/*
 * *TODO*
 *
 * http://stackoverflow.com/questions/2111667/compile-time-string-hashing
 *
 */

/*

constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

uint64_t string_to_base26_int(const char* string) {
    uint64_t result = 0;
    int len = strlen(string);

    if (len <= 8) {
        strcpy(reinterpret_cast<char*>(result), symbol);
        return result;

    } else {
        // double stepping = max(1, ((double)len) / (64 / 5); // culling
        // strategy

        for (int i = 0; i < len; ++i) {
            if ( str[i] >= 'a') { str[i] = str[i] - 'a' }
            else if ( str[i] >= 'A') { str[i] = str[i] - 'A' }
            else if ( str[i] >= '0') { str[i] = str[i] - '0' + 15 } // map to higher letters corresonding number (increase diversing)
            else { str[i] = 16 }    // numerical of "Q" - argueably the most uncommon mapping
            result += (str[i] + 1) + i * 26;
        }
        return result;
    }
}

*/

}
}

#endif
