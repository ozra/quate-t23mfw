#include "EtcUtils.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <cstring>
#include "rfx11_types.hh"
#include "rfx11_lib_debug.hh"

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

constexpr unsigned int codify_str_to_int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

uint64_t codify_string_to_base26_int(const char* string) {
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
            else if ( str[i] >= '0') { str[i] = str[i] - '0' + 15 } // map to
higher letters corresonding number (increase diversing)
            else { str[i] = 16 }    // numerical of "Q" - argueably the most
uncommon mapping
            result += (str[i] + 1) + i * 26;
        }
        return result;
    }
}

*/
}
}

#include <chrono>

// using namespace std::chrono;
// using namespace std::literals::chrono_literals;
// const auto report-every = 5s;

class ActLikeTrueEvery
{
  public:
    ActLikeTrueEvery(real seconds)
    // : repetition(std::chrono::nanoseconds(seconds * 1000000000)) {}
        : previous_time(std::chrono::seconds(0)),
          repetition(std::chrono::nanoseconds(int(seconds * 1000000000))) {}

    operator bool()
    {
        if (std::chrono::high_resolution_clock::now() - previous_time >
            repetition) {
            previous_time = std::chrono::high_resolution_clock::now();
            return true;
        }
        else {
            return false;
        }
    }

  private:
    std::chrono::high_resolution_clock::time_point previous_time;
    // std::chrono::high_resolution_clock::duration
    // repetition(std::chrono::seconds(0));
    std::chrono::high_resolution_clock::duration repetition;
};

template <int minv>
inline int upper_power_of_two(int v)
{
    if (v < minv) { return minv; }
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}


#endif
