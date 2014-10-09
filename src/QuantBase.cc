#ifdef INTERFACE
/**
* Created:  2014-08-07
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/* This is a class that is passed to Feed, Periodization, etc. it contains
 * a method for multiplying the security/instruments value increasing the
 * part residing in integral (mantissa) and decreasing the part in decimals,
 * thereby:
 *  - increasing precision in float32 operations.
 *  - revealing problems in threshold logic arising from hard coded value
 *    levels.
 *
 * The factor is therefor preferably randomized for each session, thereby
 * increasing the visibility of non normalized calculations.
 */



// *TODO* MOVE TO MAKEFILE SOMEHOW
//#define IS_DEBUG


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

typedef unsigned char byte;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#include <cassert>

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
typedef ptree HashTree;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#include <boost/signals2.hpp>
//#define Emitter boost::signals2::signal
//#define Listener boost::signals2::connection

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;

#define makeTime(A) boost::posix_time::time_from_string(A)

//#define makeDate(A,B,C) boost::gregorian::date((A),(B),(C))

inline boost::gregorian::date makeDate( int y, int m, int d) {
    return boost::gregorian::date(y, m, d);
}

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
//#include <fstream>
namespace fs = boost::filesystem;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#include <sstream>

#include <iostream>

//#include <map>
//#define Hash std::map<string, string>

#include <string>
#define Str std::string


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#include <exception>

class bad_calc: public std::exception {  //  - 2014-08-12/ORC(19:51)
  virtual const char* what() const throw() {
    return "An unrecoverable quant calculation error took place.";
  }
};


/**
    *REMINDER*

  try {
    throw _bad_calc_;
  }
  catch (bad_calc& e) {   // bad_alloc | bad_weak_ptr | ...
    cout << "Alright, it was a calc error : " << e.what() << '\n';
  }
  catch (exception& e) {
    cout << "What happened? A : " << e.what() << '\n';
  }

 **/

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#define DEFAULT_BUFSIZE 32768

//#define QuantTime       long long
#define QuantDuration   pxt::time_duration
#define QuantTime       pxt::ptime
#define QuantInt        int
#define QuantUInt       unsigned int     // which ever is fastest of unsigned and regular
#define QuantFloat      float
//#define QuantReal       double  // float
typedef double          QuantReal;
#define QuantDouble     double

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
using namespace std;


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

/*
class Mantissation {
  public:
    Mantissation ( QuantReal min_factor, QuantReal max_factor );
    Mantissation ( QuantReal fixed_factor );
    ~Mantissation ();

    inline QuantReal mantise( double value );
    inline QuantReal demantise( QuantReal value );

    double mantissation_factor;

};
*/

#endif

#include "QuantBase.hh"


bad_calc _bad_calc_;    // Exception instance

/*
Mantissation::Mantissation ( QuantReal min_factor, QuantReal max_factor ) {
    mantissation_factor = (min_factor + max_factor) * 0.5; // *TODO* *TODO* getRandomValue (min_factor, max_factor);
}

Mantissation::Mantissation ( QuantReal fixed_factor ) {
    mantissation_factor = fixed_factor;
}

Mantissation::~Mantissation () {
}

inline QuantReal Mantissation::mantise( double value ) {
    return value * mantissation_factor;
}

inline QuantReal Mantissation::demantise( QuantReal value ) {
    return value / mantissation_factor;
}
*/


#ifdef INTERFACE

// Forward declarations
class QuantExecutionContext;
class QuantKeeperJar;
class QuantBufferJar;
class QuantFeed;
class QuantPeriodization;
class QuantBufferSynchronizedHeap;
class QuantBufferAbstract;


class QuantActiveContainersSetupSingleton {
  public:
    QuantExecutionContext       *run_context = nullptr;
    QuantKeeperJar              *active_jar = nullptr;
    QuantBufferJar              *active_buffer_jar = nullptr;
    QuantFeed                   *active_feed = nullptr;
    QuantPeriodization          *active_periodization = nullptr;
    QuantBufferSynchronizedHeap *active_buffer_heap = nullptr;
    HashTree                    *active_conf = nullptr;
};

extern QuantActiveContainersSetupSingleton global_actives;

#endif

QuantActiveContainersSetupSingleton global_actives; // Global


#ifdef INTERFACE

class QuantKeeperJar {
  public:
    QuantKeeperJar ();
    QuantKeeperJar ( HashTree &conf );
    ~QuantKeeperJar ();

    QuantExecutionContext           *run_context = nullptr;
    HashTree                        *conf = nullptr;
    vector<QuantFeed *>             feeds;
    vector<QuantPeriodization *>    periodizations;

    void add ( QuantFeed *feed );
    void add ( QuantPeriodization *periodization );
};

#endif
QuantKeeperJar::QuantKeeperJar ()
{
    global_actives.active_jar = this;
}

QuantKeeperJar::QuantKeeperJar ( HashTree &conf )
:
    conf { &conf }
{
    global_actives.active_jar = this;
    global_actives.active_conf = &conf;
}

QuantKeeperJar::~QuantKeeperJar () {
}

void QuantKeeperJar::add ( QuantFeed *feed )
{
    feeds.push_back( feed );
    //run_context->add( feed );
}

void QuantKeeperJar::add ( QuantPeriodization *periodization )
{
    periodizations.push_back( periodization );
    //run_context->add( periodization );
}



#ifdef INTERFACE

class QuantBufferJar
{
  public:
    QuantBufferJar ();
    void add ( QuantBufferAbstract * );
    vector<QuantBufferAbstract *> buffers;
};

#endif

QuantBufferJar::QuantBufferJar ()
{
    cerr << "QuantBufferJar::QuantBufferJar - sets 'global_actives.active_buffer_jar'" << "\n";
    global_actives.active_buffer_jar = this;
}

void QuantBufferJar::add ( QuantBufferAbstract * buffer ) {
    buffers.push_back( buffer );
}

