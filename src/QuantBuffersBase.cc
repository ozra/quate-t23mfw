#include "QuantBuffersBase.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <exception>

// Exception:
class bad_quantbuffer_use: public std::exception    //  - 2014-08-12/ORC(19:51)
{
  public:
    virtual const char* what() const throw()
    {
        return "Some unspecified error when using a quant::BufferHeap! Check stderr/cerr log / output.";
    }
};

class bad_quantbuffer_indexing: public
    bad_quantbuffer_use    //  - 2014-08-12/ORC(19:51)
{
  public:
    virtual const char* what() const throw()
    {
        return "You've used an index that overflows - or something similar - a quant::BufferHeap!";
    }
};

class bad_quantbuffer_allocation: public
    bad_quantbuffer_use    //  - 2014-08-12/ORC(19:51)
{
  public:
    virtual const char* what() const throw()
    {
        return "You can't add buffers after allocating a quant::BufferHeap!";
    }
};

extern bad_quantbuffer_use _bad_quantbuffer_use_;
extern bad_quantbuffer_allocation _bad_quantbuffer_allocation_;
extern bad_quantbuffer_indexing _bad_quantbuffer_indexing_;

#endif

bad_quantbuffer_use _bad_quantbuffer_use_;
bad_quantbuffer_allocation _bad_quantbuffer_allocation_;
bad_quantbuffer_indexing _bad_quantbuffer_indexing_;

