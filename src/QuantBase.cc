#include "QuantBase.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-07
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#

//#define MAX(a, b) (((a) > (b)) ? (a) : (b))
//#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#include <algorithm>
using std::min;
using std::max;

/*
// FFSake 2014-11-14/Oscar Campbell - have no internet and can't find where the fuck max/min funs are avail. Fuck it. Why bother..
template<typename T> inline auto max(T a, unsigned T b) -> T {
    return a > b ? a : b;
}

template<typename T> inline auto max(T unsigned a, T b) -> T {
    return a > b ? a : b;
}

template<typename T> inline auto min(T a, unsigned T b) -> T {
    return a > b ? a : b;
}

template<typename T> inline auto min(unsigned T a, T b) -> T {
    return a > b ? a : b;
}
*/

// 141005/ORC - Hint the compiler as to conditional blocks that are more
// or less likely to take place, thereby helping -freorder-blocks explicitly.
#define LIKELY(expr) (__builtin_expect(!!(expr), 1))
#define UNLIKELY(expr) (__builtin_expect(!!(expr), 0))  //2014-11-14/ORC - was ",1" (!!)

typedef unsigned char byte;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
#include <vector>
using std::vector;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
typedef ptree HashTree;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
//#include <boost/signals2.hpp>
//#define Emitter boost::signals2::signal
//#define Listener boost::signals2::connection

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#

#include <chrono>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pxt = boost::posix_time;
namespace dt = boost::gregorian;

#define makeTime(A) boost::posix_time::time_from_string(A)

//#define makeDate(A,byte,C) boost::gregorian::date((A),(byte),(C))

inline boost::gregorian::date makeDate(int y, int m, int d)
{
    return boost::gregorian::date(y, m, d);
}

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
//#include <fstream>
namespace fs = boost::filesystem;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
#include <sstream>
#include <iostream>

//#include <map>
//#define Hash std::map<string, string>

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
#include <exception>

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
#define DEFAULT_BUFSIZE 32768

#include "QuantBasicTypes.hh"
typedef string Str;
typedef string String;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
#include "rfx11_lib_debug.hh"

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#

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

/*
Mantissation::Mantissation ( QuantReal min_factor, QuantReal max_factor ) {
    mantissation_factor = (min_factor + max_factor) * 0.5; // *TODO* *TODO*
getRandomValue (min_factor, max_factor);
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
class QuantMultiKeeperJar;
class QuantBufferJar;
class QuantFeedAbstract;
class QuantPeriodizationAbstract;
class QuantBufferIntertwinedHeap;
// template <typename T_A> class QuantBufferAbstract;
class QuantBufferAbstract;

class QuantActiveContainersSetupSingleton
{
  public:
    QuantExecutionContext* run_context = nullptr;
    QuantMultiKeeperJar* active_jar = nullptr;
    QuantBufferJar* active_buffer_jar = nullptr;
    QuantFeedAbstract* active_feed = nullptr;
    QuantPeriodizationAbstract* active_periodization = nullptr;
    QuantBufferIntertwinedHeap* active_buffer_heap = nullptr;
    HashTree* active_conf = nullptr;
};

extern QuantActiveContainersSetupSingleton global_actives;

#endif

QuantActiveContainersSetupSingleton global_actives; // Global

#ifdef INTERFACE

class QuantMultiKeeperJar
{
  public:
    QuantMultiKeeperJar();
    QuantMultiKeeperJar(HashTree& conf);
    ~QuantMultiKeeperJar();

    QuantExecutionContext* run_context = nullptr;
    HashTree* conf = nullptr;
    vector<QuantFeedAbstract*> feeds;
    vector<QuantPeriodizationAbstract*> periodizations;

    void add(QuantFeedAbstract* feed);
    void add(QuantPeriodizationAbstract* periodization);
};

#endif

QuantMultiKeeperJar::QuantMultiKeeperJar() { global_actives.active_jar = this; }

QuantMultiKeeperJar::QuantMultiKeeperJar(HashTree& conf)
    : conf{ &conf }
{
    global_actives.active_jar = this;
    global_actives.active_conf = &conf;
}

QuantMultiKeeperJar::~QuantMultiKeeperJar() {}

void QuantMultiKeeperJar::add(QuantFeedAbstract* feed)
{
    feeds.push_back(feed);
    // run_context->add( feed );
}

void QuantMultiKeeperJar::add(QuantPeriodizationAbstract* periodization)
{
    periodizations.push_back(periodization);
    // run_context->add( periodization );
}

#ifdef INTERFACE

class QuantBufferJar
{
  public:
    inline QuantBufferJar()
    {
        cerr << "QuantBufferJar::QuantBufferJar - sets "
             "'global_actives.active_buffer_jar'"
             << "\n";
        global_actives.active_buffer_jar = this;
    }

    inline void add(QuantBufferAbstract* buffer) { buffers.push_back(buffer); }

    vector<QuantBufferAbstract*> buffers;
};

#endif
