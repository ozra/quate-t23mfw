/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef TURBOSIGNALS_HH
#define TURBOSIGNALS_HH

#include <vector>
#include <functional>

//template <typename... A>
class TurboSignal {
  public:
    TurboSignal() {};
    ~TurboSignal() {};

    void operator() (std::function<void(void)>);
    void connect (std::function<void(void)>);
    void emit ();

  private:
    //std::vector<void(A...)> funcs;
    //std::vector<void (*)(void)> funcs;
    std::vector<std::function<void(void)>> funcs;

};

#endif
