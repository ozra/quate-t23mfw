/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef QUANTSTUDYCONTEXT_HH
#define QUANTSTUDYCONTEXT_HH

#include "QuantBase.hh"

class QuantFeed;    // Forward declaration

class QuantStudyContext {
  public:
    QuantStudyContext (Hash conf);
    ~QuantStudyContext ();

    virtual void init () = 0;
    virtual void deinit () = 0;
    int serialize ();

    void addFeed ( QuantFeed *feed );

    vector<QuantFeed *> feeds;

  private:
    Hash conf;

};


#endif
