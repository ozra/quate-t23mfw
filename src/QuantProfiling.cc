#include "QuantProfiling.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <ctime>
#include <iostream>
#include <cassert>

#include <chrono>


using namespace std;


enum ProfileEnum {
    SCRAP = 0,
    STARTUP,
    GENERAL,
    LOADING_FILES,
    DECODING_FILES,
    ENCODING_DATA,
    WRITING_DATA,
    PLOTTING,
    LAST_ITEM
};

class Profiling {
  public:
    Profiling();

    void start ( ProfileEnum what );
    void end ( ProfileEnum what );
    void tell ();

  private:
    ProfileEnum profile_what[100]; // Will not grow even this big..
    int stack_pos = 0;

    std::chrono::high_resolution_clock::time_point past;
    std::chrono::high_resolution_clock::time_point now;

    double the_times[ ProfileEnum::LAST_ITEM ];

};


extern Profiling profiler;

#endif

Profiling::Profiling () {
    past = std::chrono::high_resolution_clock::now();
    profile_what[ 0 ] = SCRAP;
}

void Profiling::start ( ProfileEnum what ) {
    now = std::chrono::high_resolution_clock::now();
    the_times[ profile_what[ stack_pos ] ] += std::chrono::duration_cast<std::chrono::duration<double>>(now - past).count(); // difftime( now, past );
    past = now;

    ++stack_pos;
    profile_what[stack_pos] = what;
}

void Profiling::end ( ProfileEnum what ) {
    assert( what == profile_what[ stack_pos ] );

    now = std::chrono::high_resolution_clock::now();
    the_times[ profile_what[ stack_pos ] ] += std::chrono::duration_cast<std::chrono::duration<double>>(now - past).count(); // difftime( now, past );
    past = now;

    --stack_pos;
}

void Profiling::tell () {
    assert ( stack_pos == 0 );

    cerr << "- - - Profiling - - -" << "\n";

    if ( the_times[ProfileEnum::STARTUP] )
        cerr << "Startup: " << the_times[ProfileEnum::STARTUP] << "\n";

    if ( the_times[ProfileEnum::GENERAL] )
        cerr << "General: " << the_times[ProfileEnum::GENERAL] << "\n";

    if ( the_times[ProfileEnum::PLOTTING] )
        cerr << "Plotting: " << the_times[ProfileEnum::PLOTTING] << "\n";

    if ( the_times[ProfileEnum::LOADING_FILES] )
        cerr << "File loading: " << the_times[ProfileEnum::LOADING_FILES] << "\n";

    if ( the_times[ProfileEnum::DECODING_FILES] )
        cerr << "File decoding: " << the_times[ProfileEnum::DECODING_FILES] << "\n";

    if ( the_times[ProfileEnum::ENCODING_DATA] )
        cerr << "File decoding: " << the_times[ProfileEnum::ENCODING_DATA] << "\n";

    if ( the_times[ProfileEnum::WRITING_DATA] )
        cerr << "File decoding: " << the_times[ProfileEnum::WRITING_DATA] << "\n";

    double all_time = 0.0;

    for( int i = 0; i < ProfileEnum::LAST_ITEM; ++i) {
        all_time += the_times[i];
    }

    cerr << "Total time consumed: " << all_time << "\n";
    cerr << "- - -    -    - - -" << "\n\n";

}

Profiling profiler;

