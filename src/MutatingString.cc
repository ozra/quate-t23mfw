#include "MutatingString.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-07
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <vector>
using std::vector;
#include <cstring>
#include <string>
#include <limits>
#include <iostream>

void fuglyIntIntoStr(char* buf, int n, int sizing);

//! Create a string with fixed size variables that can be modified.
/*! Saves on allocations and conversions for particular cases like slowly
 *  changing keys/paths where only parts are updated at a time, and when this
 *  will happen a shitload of times.
 */
class MutatingString
{
  public:
    ~MutatingString()
    {
        std::cerr << "< < MutatingString DESTRUCTOR < <\n";
        delete[] buf_ptr;
    }
    void init(std::string format_str)
    {
        buf_ptr = new char[format_str.size() + 1];
        format_str.copy(buf_ptr, format_str.size(), 0);
        buf_ptr[format_str.size()] = '\0';
    }

    MutatingString& addVar(const char* symbol, int type = -1)
    {
        char* var_ptr = strstr(buf_ptr, symbol);
        int support_val = 0;
        if (type < 0) {
            // Assume numeric and automate the length
            type = strlen(symbol);
        }
        if (type > 0) {
            support_val = 1;
            while (--type) {
                support_val *= 10;
            }
        }
        else {
            support_val = 0;
        }
        /*
         * *TODO* make internal struct
         */
        var_support_data.push_back(support_val);
        var_compare_values.push_back(std::numeric_limits<int>::max());
        var_position_ptrs.push_back(var_ptr);
        ++var_count;
        return *this;
    }

    inline const char* mutated_str()
    {
        // inline const char * mutated_str ( ) {
        var_pos = 0;
        return buf_ptr;
    }

    inline MutatingString& operator<<(int val) { return var(val); }

    inline MutatingString& var(int val)
    {
        if (var_compare_values[var_pos] != val) {
            var_compare_values[var_pos] = val;
            if (var_support_data[var_pos] == 0) { // String...
                throw "Fucktard - we were passed an int to a string-var!\n";
                // strcpy( var_position_ptrs[ var_pos ], reinterpret_cast<const
                // char *>(val) );
            }
            fuglyIntIntoStr(var_position_ptrs[var_pos], val,
                            var_support_data[var_pos]);
        }
        ++var_pos;
        return *this;
    }

  private:
    char* buf_ptr = nullptr;
    vector<char*> var_position_ptrs;
    vector<int> var_support_data;
    vector<int> var_compare_values;
    int var_count = 0;
    int var_pos = 0;
    // Vector<Byte[16]>    var_compare_vals; For arbitrary comparing based on
    // input type in args list...
};

#endif

void fuglyIntIntoStr(char* buf, int n, int sizing)
{
    while (sizing > 0) {
        int foo = n / sizing;
        *buf++ = 48 + foo;
        n -= foo * sizing;
        sizing /= 10;
    }
}
