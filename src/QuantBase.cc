#ifdef INTERFACE
/**
* Created:  2014-08-07
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

// 141005/ORC - Hint the compiler as to conditional blocks that are more
// or less likely to take place, thereby helping -freorder-blocks explicitly.
#define LIKELY(expr)    (__builtin_expect(!!(expr), 1))
#define UNLIKELY(expr)  (__builtin_expect(!!(expr), 1))

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

typedef pxt::time_duration  QuantDuration;
typedef pxt::ptime          QuantTime;
typedef int                 QuantInt;
typedef unsigned int        QuantUInt;            // which ever is fastest of unsigned and regular
typedef float               QuantFloat;
typedef double              QuantReal;
typedef double              QuantDouble;

typedef double              QuantTypeSized;

union QuantTypesUnion {
    QuantTime   time;
    QuantReal   real;
    QuantUInt   uint;
};

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
class QuantFeedAbstract;
class QuantPeriodizationAbstract;
class QuantBufferSynchronizedHeap;
//template <typename T_A> class QuantBufferAbstract;
class QuantBufferAbstract;


class QuantActiveContainersSetupSingleton {
  public:
    QuantExecutionContext       *run_context = nullptr;
    QuantKeeperJar              *active_jar = nullptr;
    QuantBufferJar              *active_buffer_jar = nullptr;
    QuantFeedAbstract                   *active_feed = nullptr;
    QuantPeriodizationAbstract          *active_periodization = nullptr;
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
    vector<QuantFeedAbstract *>             feeds;
    vector<QuantPeriodizationAbstract *>    periodizations;

    void add ( QuantFeedAbstract *feed );
    void add ( QuantPeriodizationAbstract *periodization );
};

#endif
QuantKeeperJar::QuantKeeperJar () {
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

void QuantKeeperJar::add ( QuantFeedAbstract *feed ) {
    feeds.push_back( feed );
    //run_context->add( feed );
}

void QuantKeeperJar::add ( QuantPeriodizationAbstract *periodization ) {
    periodizations.push_back( periodization );
    //run_context->add( periodization );
}



#ifdef INTERFACE

class QuantBufferJar {
  public:
    QuantBufferJar ();
    void add ( QuantBufferAbstract * );
    vector<QuantBufferAbstract *> buffers;
};

#endif

QuantBufferJar::QuantBufferJar () {
    cerr << "QuantBufferJar::QuantBufferJar - sets 'global_actives.active_buffer_jar'" << "\n";
    global_actives.active_buffer_jar = this;
}

void QuantBufferJar::add ( QuantBufferAbstract * buffer ) {
    buffers.push_back( buffer );
}

