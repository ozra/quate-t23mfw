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

enum OrderStatus {
    undef = 0,
    open = 1,
    filled = 2,
    canceled = 3,
    partially_filled = 4,
    partially_canceled = 5
};

typedef int OrderId;
typedef String OrderForeignId;
// typedef int     OrderType;
// typedef int     TransactionType;

// typedef uint64_t SecurityNumberCode;


struct CostModel {
    bool free_market_terms = false;
    bool contractual_position_based = true;
    real fee_percentage = 0;
    real fee_fixed = 0;
    bool pay_spread = true;

    // *TODO* we need leverage on every symbol (?) or just make more costmodels,
    // ALSO varies with time!!!
    // "inherit"
    natural leverage = 0;
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

class Order
{
    // *TODO*
};

class Transaction
{
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


// Represents an account on a broker
class BrokerAccount
{
  public:
    BrokerAccount();
    ~BrokerAccount();

    real get_balance(CurrencyNumberCode currency); //  = primary_currency_);
    real get_equity();

    // // //
    string broker_id;
    string user;
    string pass;

    vector<Order> open_orders_;
    vector<Order> historical_orders_;
    vector<Transaction> transactions_;

    CurrencyNumberCode primary_currency_;

    // *TODO* proper datatype...
    //std::map<CurrencyNumberCode, real> balances_;
    real balances_[TOTAL_SYMBOL_COUNT];

    // QuantReal                           positioned_funds;
    // QuantReal                           positions_value;
};

class TradeDeskAbstract
{
  public:
    TradeDeskAbstract();
    TradeDeskAbstract(string broker, string user, string pass);
    // virtual ~TradeDeskAbstract () = 0;

    virtual OrderId testing_buy(real qty) = 0;
    virtual OrderId testing_sell(real qty) = 0;
    virtual bool testing_close(OrderId order_id) = 0;

    virtual OrderId placeOrder(OrderType order_type, real price, real qty,
                               QuantTime deadline) = 0;

    virtual OrderId buyMarket(real qty, natural slippage,
                              natural magic_number_id = 0) = 0;
    virtual OrderId sellMarket(real qty, natural slippage,
                               natural magic_number_id = 0) = 0;
    virtual OrderId buyLimit(real qty, real price, natural slippage,
                             natural magic_number_id = 0) = 0;
    virtual OrderId sellLimit(real qty, real price, natural slippage,
                              natural magic_number_id = 0) = 0;
    virtual OrderId buyStop(real qty, real price, natural slippage,
                            natural magic_number_id = 0) = 0;
    virtual OrderId sellStop(real qty, real price, natural slippage,
                             natural magic_number_id = 0) = 0;
    virtual OrderId buyTrailStop(real qty, real distance, natural slippage,
                                 natural magic_number_id = 0) = 0;
    virtual OrderId sellTrailStop(real qty, real distance, natural slippage,
                                  natural magic_number_id = 0) = 0;

    virtual OrderStatus orderStatus(OrderId order_id) = 0;
    virtual bool cancelOrder(OrderId order_id) = 0;
    virtual vector<OrderId> * getOpenOrders() = 0;

    real get_balance();
    real get_usable_funds();
};

class TradeDeskSimulator : public TradeDeskAbstract
{
  public:
    virtual OrderId testing_buy(QuantReal qty);
    virtual OrderId testing_sell(QuantReal qty);
    virtual bool testing_close(OrderId order_id);

    natural level_of_pessimism =
        3; // How hard we are on criterions on the ticks to
    // decide if a level would reach a deal or not -
    // 2014-10-21/ORC
    real find_worst_price_time_window =
        500; // How long past the order do we look for a worst price?

    real fee_percentage_taker;
    real fee_percentage_maker;
    real fee_fixed;
    bool pay_the_spread;
};
}

#endif

namespace t23m {


Transaction::Transaction(TransactionType type,
                         CurrencyNumberCode base_currency,
                         QuantReal base_currency_change,
                         SecurityNumberCode instrument,
                         QuantReal instrument_change)
    : type{ type }
    , base_currency{ base_currency }
    , instrument{ instrument }
    , base_currency_change{ base_currency_change }
    , instrument_change{ instrument_change } {}


QuantReal calculate_the_actual_buy_price()
{
    // Just fucking bogus to remind of what to do - *TODO*
    return 47.47;
}

QuantReal calculate_the_actual_sell_price()
{
    // Just fucking bogus to remind of what to do - *TODO*
    return 47.47;
}

OrderId TradeDeskSimulator::testing_buy(QuantReal qty)
{
    QuantReal p = calculate_the_actual_buy_price();
    /*
    *TODO*
    transactions.push_back(Transaction(BUY_TAKE, -qty, qty / p));
    */
    return 4747;
}

OrderId TradeDeskSimulator::testing_sell(QuantReal qty)
{
    QuantReal p = calculate_the_actual_sell_price();
    /*
    *TODO*
    transactions.push_back(Transaction(SELL_TAKE, qty * p, -qty));
    */
    return 4747;
}

bool TradeDeskSimulator::testing_close(OrderId order_id)
{
    // *TODO*
    if (order_id == (0 + order_id + 47 - 47)) {
        // Close a matching position
        return true;
    }
    else {
        return false;
    }
}
}
