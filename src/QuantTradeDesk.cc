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



class BrokerAbstract {
    BrokerAbstract ();
    //~BrokerAbstract ();

    // *TODO* - or keep these in the TradeDesk specialization?
    QuantReal   fee_percentage;
    QuantReal   fee_fixed;
    bool        pay_spread;
    QuantReal   leverage;

};

// Signleton -
class BrokerManager {
    BrokerManager ();

    // *TODO*
    // To "query" for brokers by name and get a Broker-class specialized with
    // all interfaces required for TradeDesk, Feeding etc... Oooor..? Keep
    // managers for all the different classes?

};


enum enum_transaction_type  {
    NONE = 0,
    LOAN_FUNDS = 1,
    LOAN_SECURITY = 2,
    BUY_TAKE = 3,
    BUY_MAKE = 4,
    SELL_TAKE = 5,
    SELL_MAKE = 6,

    FEES_LEVEL_ABSTRACT = 60,
    TRANSACTION_FEE = 61,
    LOAN_INTEREST = 62,

    // Think about these - perhaps they shouldn't be part of the transaction chain
    //FICTUAL_FEES_LEVEL_ABSTRACT = 60,
    //SPREAD_COST = 71,       // If buying/selling market (TAKE) in 'real' markets, or any buy/sell in pay-spread-markets.

    FOO = 7
};

class TradeAccountTransaction {
  public:
    TradeAccountTransaction (
        enum_transaction_type   type,
        QuantReal               fund_change,
        QuantReal               security_change
        /*
         * *TODO*
         *
         * Any amount of securities can be held positioned, they have to be
         * identified (LTCUSD, BTCUSD, BTCCNY, XAGUSD, USDZAR, whatever..
         *
         */
    );

  private:
    enum_transaction_type   type;
    QuantReal               fund_change;
    QuantReal               security_change;

};

TradeAccountTransaction::TradeAccountTransaction (
    enum_transaction_type   type,
    QuantReal               fund_change,
    QuantReal               security_change
) :
    type { type },
    fund_change { fund_change },
    security_change { security_change }
{}

/*
class TradeAccount {
  public:
    TradeAccount ();
    ~TradeAccount ();

    QuantReal get_balance ();
    QuantReal get_usable_funds ();

    // // //
    vector<TradeAccountTransaction>     transactions;
    QuantReal                           funds;
    QuantReal                           positioned_funds;
    QuantReal                           positions_value;

};
*/

class TradeDeskAbstract {
  public:
    //TradeDeskAbstract ();
    //virtual ~TradeDeskAbstract () = 0;

    virtual OrderId             testing_buy ( QuantReal qty ) = 0;
    virtual OrderId             testing_sell ( QuantReal qty ) = 0;
    virtual bool                testing_close ( OrderId order_id ) = 0;


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




    QuantReal get_balance ();
    QuantReal get_usable_funds ();

    // // //
    vector<TradeAccountTransaction>     transactions;
    QuantReal                           funds;
    QuantReal                           positioned_funds;
    QuantReal                           positions_value;


};



class TradeDeskSimulator : public TradeDeskAbstract {
public:

    virtual OrderId             testing_buy ( QuantReal qty );
    virtual OrderId             testing_sell ( QuantReal qty );
    virtual bool                testing_close ( OrderId order_id );

    int         level_of_pessimism = 3;     // How hard we are on criterions on the ticks to decide if a level would reach a deal or not - 2014-10-21/ORC

    QuantReal   fee_percentage_taker;
    QuantReal   fee_percentage_maker;
    QuantReal   fee_fixed;
    bool        pay_the_spread;

};

#endif

QuantReal calculate_the_actual_buy_price() {
    // Just fucking bogus to remind of what to do - *TODO*
    return 47.47;
}

QuantReal calculate_the_actual_sell_price() {
    // Just fucking bogus to remind of what to do - *TODO*
    return 47.47;
}

OrderId TradeDeskSimulator::testing_buy ( QuantReal qty ) {
    QuantReal p = calculate_the_actual_buy_price();
    transactions.push_back( TradeAccountTransaction( BUY_TAKE, -qty, qty / p ) );
    return 4747;
}

OrderId TradeDeskSimulator::testing_sell ( QuantReal qty ) {
    QuantReal p = calculate_the_actual_sell_price();
    transactions.push_back( TradeAccountTransaction( SELL_TAKE, qty * p, -qty ) );
    return 4747;
}

bool TradeDeskSimulator::testing_close ( OrderId order_id ) {

    // *TODO*

    if ( order_id  ==   (  0 + order_id + 47 - 47  )  ) {
        // Close a matching position
        return true;
    } else {
        return false;
    }
}





