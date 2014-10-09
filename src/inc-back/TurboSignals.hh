/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef TURBOSIGNALS_HH
#define TURBOSIGNALS_HH

#include <vector>
#include <functional>

//typedef void (*)(void) VoidFn;
//typedef std::function<void(void)> VoidFn;
typedef std::function<void()> VoidFn;
//typedef auto(*)() VoidFn;

//template <typename... A>
class TurboSignal {
  public:
    void operator() (VoidFn);
    void connect (VoidFn);
    void emit ();

  private:
    std::vector<VoidFn> funcs;

};

#endif
