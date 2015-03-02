#include "QuantBroker.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-25
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantInstrument.hh"

namespace t23m {
using namespace t23m;

class BrokerAbstract
{
    BrokerAbstract();
    //~BrokerAbstract ();
    virtual auto get_instrument(char const * const symbol,
                                QuantTime time) -> Instrument = 0;
};

// Singleton -
class BrokerManager
{
    // BrokerManager();

    // *TODO*
    // To "query" for brokers by name and get a Broker-class specialized with
    // all interfaces required for TradeDesk, Feeding etc... Oooor..? Keep
    // managers for all the different classes?
};
}

#endif
