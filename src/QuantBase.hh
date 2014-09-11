/**
* Created:  2014-08-07
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/* Quantron - Qu*
 * Quantron Finance - QuFi*
 */


#ifndef QUANTBASE_HH
#define QUANTBASE_HH

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#include <boost/signals2.hpp>
#define Emitter boost::signals2::signal
#define Listener boost::signals2::connection

#include <boost/date_time/posix_time/posix_time.hpp>
namespace pxt = boost::posix_time;

//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#include <iostream>

#include <map>
#define Hash std::map<string, string>

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

#define QuantTime       long long
//#define QuantTime       pxt::ptime
#define QuantInt        int
#define QuantUInt       int     // which ever is fastest of unsigned and regular
#define QuantFloat      float
#define QuantReal       double  // float
#define QuantDouble     double


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
using namespace std;


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

class Mantissation {
  public:
    Mantissation ( QuantReal min_factor, QuantReal max_factor );
    Mantissation ( QuantReal fixed_factor );
    ~Mantissation ();

    inline QuantReal mantise( double value );
    inline QuantReal demantise( QuantReal value );

    double mantissation_factor;

};

#endif
