#include "QTA_ObjectAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"

#include <exception>
#include <cmath>

class bad_calc : public std::exception { //  - 2014-08-12/ORC(19:51)
    virtual const char* what() const throw() {
        return "An unrecoverable quant calculation error took place.";
    }
};

extern bad_calc _bad_calc_;

#endif

bad_calc _bad_calc_; // Exception instance

#ifdef INTERFACE

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

namespace QTA {

class ObjectAbstract {
   public:
    ObjectAbstract() {};
    virtual ~ObjectAbstract() {};

    virtual bool verify_integrity();
    virtual bool is_primed();
};
}

#endif

namespace QTA {

// ObjectAbstract::ObjectAbstract ()
//{}

// ObjectAbstract::~ObjectAbstract ()
//{}

bool ObjectAbstract::verify_integrity() { return true; }

bool ObjectAbstract::is_primed() { return true; }
}
