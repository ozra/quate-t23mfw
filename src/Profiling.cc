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

template <typename ProfileEnum, N HIGHEST_ENUM, N STACK_SIZE = 1024>
class Profiling
{
  public:
    Profiling(cA * ProfileLabels[]) : ProfileLabels(ProfileLabels)
    {
        past = std::chrono::high_resolution_clock::now();
        profile_what[0] = ProfileEnum(0);
    }

    inline void start(ProfileEnum what)
    {
        now = std::chrono::high_resolution_clock::now();
        the_times[profile_what[stack_pos]] +=
            std::chrono::duration_cast<std::chrono::duration<R>>(now - past)
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

    inline void end(ProfileEnum what)
    {
        assert(what == profile_what[stack_pos]);
        now = std::chrono::high_resolution_clock::now();
        the_times[profile_what[stack_pos]] +=
            std::chrono::duration_cast<std::chrono::duration<R>>(now - past)
            .count(); // difftime( now, past );
        past = now;
        --stack_pos;
    }

    // ↓ Convenience
    inline void start(N what)
    {
        //assert(what < numbers<ProfileEnum>.max_value);
        assert(what < HIGHEST_ENUM);
        start(ProfileEnum(what));
    }

    inline void end(N what)
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
        R all_time = 0.0;
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
    cA ** ProfileLabels;
    ProfileEnum profile_what[STACK_SIZE]; // Will not grow even this big..
    N stack_pos = 0;
    L overflowed_ = false;

    std::chrono::high_resolution_clock::time_point past;
    std::chrono::high_resolution_clock::time_point now;

    R the_times[ProfileEnum::LAST_ITEM];
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

extern R profile_local();

#endif

// 2014-12-03/Oscar Campbell - for simple timing of one-off stuff
R profile_local()
{
    static std::chrono::high_resolution_clock::time_point previous =
        std::chrono::high_resolution_clock::now();
    auto tmp_past = previous;
    previous = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<R>>
           (previous - tmp_past).count();
}
