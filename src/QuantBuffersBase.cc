#include "QuantBuffersBase.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/
#include "QuantBase.hh"

// Exception:
class bad_quantbuffer_use: public std::exception {  //  - 2014-08-12/ORC(19:51)
  public:
    virtual const char* what() const throw();
};

extern bad_quantbuffer_use _bad_quantbuffer_use_;

#endif

const char* bad_quantbuffer_use::what() const throw() {
    return "You can't add buffers after allocating a t23mqf:: / quantron:: BufferHeap!";
}

bad_quantbuffer_use _bad_quantbuffer_use_;

