#include "QuantTradeDesk.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-22
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantInstrument.hh"

namespace t23m {
using namespace t23m;

typedef int OrderId;
typedef String OrderForeignId;
// typedef int     OrderType;
// typedef int     TransactionType;

// typedef uint64_t SecurityNumberCode;

class BrokerAbstract {
    BrokerAbstract();
    //~BrokerAbstract ();
};

struct CostModel {
    bool free_market_terms = false;
    bool contractual_position_based = true;
    QuantReal fee_percentage = 0;
    QuantReal fee_fixed = 0;
    bool pay_spread = true;

    // *TODO* we need leverage on every symbol (?) or just make more costmodels,
    // ALSO varies with time!!!
    // "inherit"
    QuantReal leverage = 0;
};

// Singleton -
class BrokerManager {
    // BrokerManager();

    // *TODO*
    // To "query" for brokers by name and get a Broker-class specialized with
    // all interfaces required for TradeDesk, Feeding etc... Oooor..? Keep
    // managers for all the different classes?
};

enum OrderType {
    NONE = 0,
    // Buys
    BUY_TAKE = 1,
    BUY_MAKE = 2,
    BUY_STOP = 3,
    BUY_TRAILSTOP = 4,

    // Sells
    SELL_TAKE = 11,
    SELL_MAKE = 12,
    SELL_STOP = 13,
    SELL_TRAILSTOP = 14,

    // Loans, except associated fees
    LOAN = 21,
    REPAY = 22,

    // Fees and stuff
    FEES_LEVEL_ABSTRACT = 60,
    // - - -
    TRANSACTION_FEE = 61,
    VOLUME_BASED_FEE = 62,
    LOAN_INTEREST = 63,

    // Think about these - perhaps they shouldn't be part of the transaction
    // chain
    // FICTUAL_FEES_LEVEL_ABSTRACT = 60,
    // SPREAD_COST = 71,       // If buying/selling market (TAKE) in 'real'
    // markets, or any buy/sell in pay-spread-markets.
    FOO = 247
};

typedef OrderType TransactionType;

/*
enum TransactionType {
    NONE = 0,
    // Regular buys, sells
    BUY_TAKE = 1,
    BUY_MAKE = 2,
    SELL_TAKE = 3,
    SELL_MAKE = 4,
    // Loans, except associated fees
    LOAN_FUNDS = 11,
    LOAN_SECURITY = 12,
    // Fees and stuff
    FEES_LEVEL_ABSTRACT = 60,
    TRANSACTION_FEE = 61,
    LOAN_INTEREST = 62,

    // Think about these - perhaps they shouldn't be part of the transaction
    // chain
    // FICTUAL_FEES_LEVEL_ABSTRACT = 60,
    // SPREAD_COST = 71,       // If buying/selling market (TAKE) in 'real'
    // markets, or any buy/sell in pay-spread-markets.
    FOO = 247
};
*/

class Order {
    // *TODO*
};

class Transaction {
   public:
    Transaction(TransactionType type, CurrencyNumberCode base_currency,
                QuantReal base_currency_change, SecurityNumberCode instrument,
                QuantReal instrument_change);

   private:
    TransactionType type;
    CurrencyNumberCode base_currency; // In case of brokers allowing multiple
                                      // BaseCurrency-BrokerAccounts  in the
                                      // TradingBrokerAccount
    QuantReal base_currency_change;
    SecurityNumberCode instrument;
    QuantReal instrument_change;
    // SecurityNumberCode    security_symbol;
    // QuantReal           fund_change;
    // QuantReal           security_change;
};

Transaction::Transaction(TransactionType type, CurrencyNumberCode base_currency,
                         QuantReal base_currency_change,
                         SecurityNumberCode instrument,
                         QuantReal instrument_change)
    : type{ type }
    , base_currency{ base_currency }
    , instrument{ instrument }
    , base_currency_change{ base_currency_change }
    , instrument_change{ instrument_change } {}

// Represents an account on a broker
class BrokerAccount {
   public:
    BrokerAccount();
    ~BrokerAccount();

    QuantReal get_balance(CurrencyNumberCode currency); //  = primary_currency_);
    QuantReal get_equity();

    // // //
    vector<Order> open_orders_;
    vector<Order> orders_;
    vector<Transaction> transactions_;

    CurrencyNumberCode primary_currency_;
    // *TODO* proper datatype...
    std::map<CurrencyNumberCode, QuantDouble> balances_;

    // QuantReal                           positioned_funds;
    // QuantReal                           positions_value;
};

class TradeDeskAbstract {
   public:
    // TradeDeskAbstract ();
    // virtual ~TradeDeskAbstract () = 0;

    virtual OrderId testing_buy(QuantReal qty) = 0;
    virtual OrderId testing_sell(QuantReal qty) = 0;
    virtual bool testing_close(OrderId order_id) = 0;

    virtual OrderId placeOrder(OrderType order_type, double price, double qty,
                               QuantTime deadline) = 0;

    virtual OrderId buyMarket() = 0;
    virtual OrderId sellMarket() = 0;
    virtual OrderId buyLimit() = 0;
    virtual OrderId sellLimit() = 0;
    virtual OrderId buyStop() = 0;
    virtual OrderId sellStop() = 0;
    virtual OrderId buyTrailStop() = 0;
    virtual OrderId sellTrailStop() = 0;

    virtual int orderStatus(OrderId order_id) = 0;
    virtual bool cancelOrder(OrderId order_id) = 0;
    virtual vector<OrderId>* getOpenOrders() = 0;

    QuantReal get_balance();
    QuantReal get_usable_funds();

    // // //
    vector<Transaction> transactions;
    QuantReal funds;
    QuantReal positioned_funds;
    QuantReal positions_value;
};

class TradeDeskSimulator : public TradeDeskAbstract {
   public:
    virtual OrderId testing_buy(QuantReal qty);
    virtual OrderId testing_sell(QuantReal qty);
    virtual bool testing_close(OrderId order_id);

    int level_of_pessimism = 3; // How hard we are on criterions on the ticks to
                                // decide if a level would reach a deal or not -
                                // 2014-10-21/ORC
    QuantDouble find_worst_price_time_window =
        500; // How long past the order do we look for a worst price?

    QuantDouble fee_percentage_taker;
    QuantDouble fee_percentage_maker;
    QuantDouble fee_fixed;
    bool pay_the_spread;
};
}

#endif

namespace t23m {

QuantReal calculate_the_actual_buy_price() {
    // Just fucking bogus to remind of what to do - *TODO*
    return 47.47;
}

QuantReal calculate_the_actual_sell_price() {
    // Just fucking bogus to remind of what to do - *TODO*
    return 47.47;
}

OrderId TradeDeskSimulator::testing_buy(QuantReal qty) {
    QuantReal p = calculate_the_actual_buy_price();
    /*
    *TODO*
    transactions.push_back(Transaction(BUY_TAKE, -qty, qty / p));
    */
    return 4747;
}

OrderId TradeDeskSimulator::testing_sell(QuantReal qty) {
    QuantReal p = calculate_the_actual_sell_price();

    /*
    *TODO*
    transactions.push_back(Transaction(SELL_TAKE, qty * p, -qty));
    */

    return 4747;
}

bool TradeDeskSimulator::testing_close(OrderId order_id) {

    // *TODO*

    if (order_id == (0 + order_id + 47 - 47)) {
        // Close a matching position
        return true;
    } else {
        return false;
    }
}
}
