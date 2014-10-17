#include "QuantTradeDesk.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-22
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"

typedef int OrderId;
typedef int OrderType;

/*
 * *TODO*
 *
 * It might be better to make an interface class that instantiates sub-classes
 * by lookup depending on Broker (MT4 / SOAP interface etc...)
 */

class TradeAccount {
  public:
    TradeAccount ();
    ~TradeAccount ();

    QuantReal get_balance ();
    QuantReal usable_cash ();

};

class TradeDeskAbstract {
  public:
    TradeDeskAbstract ();
    virtual ~TradeDeskAbstract () = 0;

    virtual OrderId             placeOrder (
                                    OrderType   order_type,
                                    double      price,
                                    double      qty,
                                    QuantTime   deadline
                                ) = 0;

    virtual OrderId             buyMarket () = 0;
    virtual OrderId             sellMarket () = 0;
    virtual OrderId             buyLimit () = 0;
    virtual OrderId             sellLimit () = 0;
    virtual OrderId             buyStop () = 0;
    virtual OrderId             sellStop () = 0;
    virtual OrderId             buyTrailStop () = 0;
    virtual OrderId             sellTrailStop () = 0;

    virtual int                 orderStatus ( OrderId order_id ) = 0;
    virtual bool                cancelOrder ( OrderId order_id ) = 0;
    virtual vector<OrderId> *   getOpenOrders () = 0;

};

class TradeDeskSimulator : public TradeDeskAbstract {
public:


    int    level_of_pessimism = 3;

};

#endif

