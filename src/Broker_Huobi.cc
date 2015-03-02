#include "Broker_Huobi.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-14
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/*


#include "rfx11_types.hh"

#include "QuantBroker.hh"
#include "QuantInstrument.hh"

#include "QuantBase.hh" // Move QuantTime to QuantTime.hh - include that from base..

#include "RequestUrl.hh"







*TODO* - this should be just the 'stream' wrapper for sequential data and the trader-process

The rest should be in node-io / V8 world.





namespace t23m {

using namespace t23m;

class Broker_Huobi : BrokerAbstract
{
  public:
    // *TODO* - temp, for reference only for napkin calcs
    static const natural TRADED_SYMBOLS = 2;

    Instrument get_instrument(char const * const symbol,
                              QuantTime time) override;
};

typedef int64_t TickerId;

class BrokerPriceFeed_Huobi : BrokerPriceFeedAbstract
{
  public:
    BrokerPriceFeed_Huobi();
    ~BrokerPriceFeed_Huobi();

    void set_last_gotten_ticker_id(TickerId tid)
    {
        last_gotten_ticker_id_ = tid;
    }

  private:

    catch_up_to_live_feed();
    go_live();

    // NOT NEEDED: One and one only. int outstanding_requests_ = 0;
    TickerId last_gotten_ticker_id_ = 0;

    QuantTime last_query_time_; // QuantTime is a fucking cumbersone data type..
    // Should be switched to some DOUBLE based
    // (seconds, then sub seconds as decimals)
    bool query_ticker_data();

    HardSignal<> on_tick(); // Signals only, read through other fn (?)
};

bool BrokerPriceFeed_Huobi::query_ticker_data()
{
    // *TODO* synchronous or async? We can't make further requests until we now
    // the last tid in an outstanding req, so snchronous is the best simplest
    // option..
    //  BITFINEX:
    // [{"timestamp":1419946025,"tid":4475805,"price":"310.8","amount":"0.1","exchange":"bitfinex","type":"sell"},{"timestamp":1419946014,"tid":4475804,"price":"310.8","amount":"0.1","exchange":"bitfinex","type":"sell"},{"timestamp":1419946013,"tid":4475803,"price":"310.85","amount":"0.375","exchange":"bitfinex","type":"buy"},{"timestamp":1419946001,"tid":4475802,"price":"310.87","amount":"0.1","exchange":"bitfinex","type":"sell"},{"timestamp":1419945999,"tid":4475801,"price":"310.87","amount":"0.3","exchange":"bitfinex","type":"sell"},{"timestamp":1419945998,"tid":4475800,"price":"310.9","amount":"0.2","exchange":"bitfinex","type":"sell"},{"timestamp":1419945995,"tid":4475799,"price":"310.9","amount":"0.1","exchange":"bitfinex","type":"sell"},{"timestamp":1419945993,"tid":4475798,"price":"310.9","amount":"0.5","exchange":"bitfinex","type":"sell"},
    //  do_http_request("https://api.bitfinex.com/v1/trades/BTCUSD?timestamp=" +
    //  ms_timestamp);  // - gets them in reverse time order!
    //  do_http_request("https://api.bitfinex.com/v1/book/BTCUSD");   // gets
    //  the current order book. No history ofc..
    // BTC-CHINA
    // [{"date":"1307942678","price":160,"amount":1,"tid":"2","type":"buy"},{"date":"1307943668","price":170,"amount":1,"tid":"3","type":"buy"},{"date":"1307944076","price":150,"amount":1,"tid":"4","type":"sell"},{"date":"1307944562","price":90,"amount":1,"tid":"5","type":"sell"},{"date":"1307974244","price":120,"amount":1,"tid":"6","type":"buy"},{"date":"1307974275","price":110,"amount":1,"tid":"7","type":"sell"},{"date":"1307974283","price":110,"amount":1,"tid":"8","type":"sell"},{"date":"1307974288","price":110,"amount":1,"tid":"9","type":"sell"},{"date":"1308335925","price":171,"amount":0.01,"tid":"10","type":"buy"},{"date":"1308336001","price":81,"amount":0.01,"tid":"11","type":"sell"},{"date":"1308377236","price":172,"amount":0.1,"tid":"12","type":"buy"},{"date":"1308380070","price":173,"amount":0.1,"tid":"13","type":"buy"},{"date":"1308383255","price":100,"amount":0.03,"tid":"14","type":"buy"},{"date":"1308383255","price":101,"amount":0.47,"tid":"15","type":"buy"},{"date":"1308407383","price":100,"amount":1,"tid":"16","type":"buy"},{"date":"1308407383","price":100,"amount":1,"tid":"17","type":"buy"},{"date":"1308407418","price":101,"amount":0.53,"tid":"18","type":"buy"},{"date":"1308407418","price":105,"amount":1,"tid":"19","type":"buy"},
    // do_http_request("https://data.btcchina.com/data/historydata?since=",
    // max(last_gotten_ticker_id_, 1)
    // [{"amount":"0.274","date":1419677621,"date_ms":1419677621000,"price":"1989.59","tid":86809280,"type":"buy"},{"amount":"0.2","date":1419677621,"date_ms":1419677621000,"price":"1989.6","tid":86809282,"type":"sell"},{"amount":"0.148","date":1419677621,"date_ms":1419677621000,"price":"1989.6","tid":86809284,"type":"sell"},
    do_http_request("https://www.okcoin.cn/api/trades.do?since=" +
                    last_gotten_ticker_id);
    return 0; // All good
}
}

*/

#endif

/*

namespace t23m {
// namespace DUKASCOPY {

//  *TODO*  switch to hashmap! - (2014-12-27 Oscar Campbell)

char const * const traded_symbols[] = { "_NIL_", "BTCCNY", "LTCCNY" };

Instrument Broker_Huobi::get_instrument(char const * const symbol,
                                        QuantTime time)
{
    if (search_string_list_for_matching_row(traded_symbols, TRADED_SYMBOLS + 1,
                                            symbol) == 0) {
        return Instrument("_NIL_", 0, 0);
    }
    else {
        return Instrument(symbol, 3 / * *TODO* - user and time dependant * /,
                          4 / * *TODO* * /);
    }
}

//! Compose an object containing both static and time dependent data for a
// security

// Since several factors of a security may change with time - daily, weekly or
// non periodially for different reasons - this is a necessity.

//}
}


*/
