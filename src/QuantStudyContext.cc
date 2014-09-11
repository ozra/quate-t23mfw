/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/


#include "QuantStudyContext.hh"


QuantStudyContext::QuantStudyContext (Hash conf)
                        :
                        conf (conf)
{
    // *TODO*

}


QuantStudyContext::~QuantStudyContext () {

    // DELETE SOME STUFF

}


int QuantStudyContext::serialize () {
    return 0;
}

void QuantStudyContext::addFeed ( QuantFeed *feed )
{
    feeds.push_back( feed );
}