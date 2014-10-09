#include "QuantTradeDesk.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-22
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"

typedef int OrderId;

/*
 * *TODO*
 *
 * It might be better to make an interface class that instantiates sub-classes
 * by lookup depending on Broker (MT4 / SOAP interface etc...)
 */


class TradeDeskAbstract {
  public:
    TradeDeskAbstract ();
    virtual ~TradeDeskAbstract () = 0;

    virtual OrderId placeOrder (
        double price,
        double qty,
        QuantTime deadline
    ) = 0;
    virtual int                 orderStatus ( OrderId order_id ) = 0;
    virtual bool                cancelOrder ( OrderId order_id ) = 0;
    virtual vector<OrderId> *   getOpenOrders () = 0;

};


#endif

