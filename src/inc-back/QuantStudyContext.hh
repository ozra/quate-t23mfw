/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef QUANTSTUDYCONTEXT_HH
#define QUANTSTUDYCONTEXT_HH

#include "QuantBase.hh"

class QuantFeed;    // Forward declaration
class QuantPeriodization;

class QuantStudyContext {
  public:
    QuantStudyContext (Hash conf);
    ~QuantStudyContext ();

    virtual void init () = 0;
    virtual void deinit () = 0;
    int serialize ();

    auto add ( QuantFeed *feed ) -> QuantStudyContext &;
    auto add ( QuantPeriodization *periodization ) -> QuantStudyContext &;

    vector<QuantFeed *> feeds;
    vector<QuantPeriodization *> periodizations;
    //vector<AbstractQuantBuffer *> buffers;    // these are stored in the periodizations - safi?

  private:
    Hash conf;

};


#endif
