#include "Profiling.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
* Code Cls: Blackbox FoolProof
**/

//#include <ctime>
#include <iostream>
using std::cerr;
#include <cassert>
#include <chrono>
//#include <numeric_limits>
#include "rfx11_types.hh"

// using namespace std;

template <typename ProfileEnum, natural HIGHEST_ENUM, natural STACK_SIZE = 1024>
class Profiling
{
  public:
    Profiling(const char * ProfileLabels[]) : ProfileLabels(ProfileLabels)
    {
        past = std::chrono::high_resolution_clock::now();
        profile_what[0] = ProfileEnum(0);
    }

    inline void start(ProfileEnum what)
    {
        now = std::chrono::high_resolution_clock::now();
        the_times[profile_what[stack_pos]] +=
            std::chrono::duration_cast<std::chrono::duration<real>>(now - past)
            .count(); // difftime( now, past );
        past = now;
        if (++stack_pos == STACK_SIZE) {
            #ifdef IS_DEBUG
            assert(stack_pos > STACK_SIZE);
            #else
            --stack_pos; // Continue, but discard silently if used in
            // production.
            overflowed_ = true;
            #endif
        }
        profile_what[stack_pos] = what;
    }

    #ifdef NDEBUG
    inline void end(ProfileEnum)
    #else
    inline void end(ProfileEnum what)
    #endif
    {
        assert(what == profile_what[stack_pos]);
        now = std::chrono::high_resolution_clock::now();
        the_times[profile_what[stack_pos]] +=
            std::chrono::duration_cast<std::chrono::duration<real>>(now - past)
            .count(); // difftime( now, past );
        past = now;
        --stack_pos;
    }

    // ↓ Convenience
    inline void start(natural what)
    {
        //assert(what < numbers<ProfileEnum>.max_value);
        assert(what < HIGHEST_ENUM);
        start(ProfileEnum(what));
    }

    inline void end(natural what)
    {
        //assert(what < numbers<ProfileEnum>.max_value);
        assert(what < HIGHEST_ENUM);
        end(ProfileEnum(what));
    }

    /*
                    ######## ######## ##       ##
                       ##    ##       ##       ##
                       ##    ##       ##       ##
                       ##    ######   ##       ##
                       ##    ##       ##       ##
                       ##    ##       ##       ##
                       ##    ######## ######## ########
    */
    void tell()
    {
        assert(stack_pos == 0);
        cerr << "\n\n";
        cerr << "- -  -    -    -  - -"
             << "\n";
        cerr << "- - - Profiling - - -"
             << "\n";
        if (overflowed_ == true) {
            cerr << "\n\n\nProfiling stack overflowed. Increase it before next "
                 "run, aight! : " << STACK_SIZE << "\n\n\n";
            return;
        }
        real all_time = 0.0;
        for (auto i = 0; i < ProfileEnum::LAST_ITEM; ++i) {
            if (the_times[i])
                cerr << ProfileLabels[i] << ": "
                     << the_times[ProfileEnum::WRITING_FILE] << "\n";
            all_time += the_times[i];
        }
        cerr << "Total time consumed: " << all_time << "\n";
        cerr << "- -  -    -    -  - -";
        cerr << "\n\n";
    }

  private:
    const char ** ProfileLabels;
    ProfileEnum profile_what[STACK_SIZE]; // Will not grow even this big..
    natural stack_pos = 0;
    bool overflowed_ = false;

    std::chrono::high_resolution_clock::time_point past;
    std::chrono::high_resolution_clock::time_point now;

    real the_times[ProfileEnum::LAST_ITEM];
};


#ifdef UNIT_TEST
unit_tester.add([]()
{
    legitimite_use()
    make_it_overflow();
    use_faulty_nums();
    other_shit();
};

#endif

extern real profile_local();

#endif

// 2014-12-03/Oscar Campbell - for simple timing of one-off stuff
real profile_local()
{
    static std::chrono::high_resolution_clock::time_point previous =
        std::chrono::high_resolution_clock::now();
    auto tmp_past = previous;
    previous = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<real>>
           (previous - tmp_past).count();
}
