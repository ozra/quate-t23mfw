#include "QuantObjectManagers.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "EtcUtils.hh"
//#include "QuantBase.hh" // for String - move all such to "QuantBasicTypes.hh"
#include "QuantBasicTypes.hh"
#include "QuantInstrument.hh"


// *TODO* referencing and file placement...
#include "Broker_DukascopySwizz.hh"
// *TODO* referencing and file placement...

namespace t23m {
using namespace t23m;

//! Compose an object containing both static and time dependent data for a
// security
/** Since several factors of a security may change with time - daily, weekly or
 * non periodially for different reasons - this is a necessity.
*/

/*

*TODO* - should be part of class BrokerAbstract

*/

/*
auto get_instrument(String broker_name, String security_symbol, QuantTime time)
    -> Instrument {
    // switch(utils::string_to_base26_int(broker_name)) {
    //}

    if (broker_name == "DUKASCOPY") {
        return DUKASCOPY::get_dukascopy_instrument(security_symbol, time);

    } else if (broker_name == "FXCM") {
        throw "Not implemented!";

    } else if (broker_name == "OANDA") {
        throw "Not implemented!";
    }
    return Instrument();    // To satisfy compiler
}
*/

}


#endif
