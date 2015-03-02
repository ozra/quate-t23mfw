#include "QuantTrader_Paper.hh"
#ifdef INTERFACE
/**
* Created:  2015-02-16
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
*
* - Refactored out from ZarScalperBot...
*
**/

#include <QuantFeed.hh>


template <bool FOO = true>  // Just to keep it compiling though all inlined.. *TEMP*
class PaperTrader
{
  public:
    PaperTrader(QuantFeedAbstract & feed, real balance, natural leverage)
        : feed(feed)
        , initial_deposit(balance)
        , balance(balance)
        , leverage(leverage)
    {
    }

    ~PaperTrader()
    {
        report();
    }

    void deposit(real qty)
    {
        balance += qty;
    }
    real get_equity()
    {
        return balance + (security_qty * ((feed.ticks().bid + feed.ticks().ask) /
                                          2));
    }
    inline void handle_tick()
    {
        if (current_position == 0) {
            return;
        }
        else if (current_position == 1) {
            real p = feed.ticks().bid;
            if (p > last_trail_source_price) {
                calculate_trail_price(p);
            }
            if (current_stop_loss && p < current_stop_loss) {
                cerr << "Closes long because of stop loss\n";
                close_long();
            }
            else if (current_trailing_stop_delta && p < current_trailing_stop) {
                cerr << "Closes long because of trailing stop loss\n";
                close_long();
            }
        }
        else {
            real p = feed.ticks().ask;
            if (p < last_trail_source_price) {
                calculate_trail_price(p);
            }
            if (current_stop_loss && p > current_stop_loss) {
                cerr << "Closes short because of stop loss\n";
                close_short();
            }
            else if (current_trailing_stop_delta && p > current_trailing_stop) {
                cerr << "Closes short because of trailing stop loss\n";
                close_short();
            }
        }
    }
    inline void calculate_trail_price(real price)
    {
        last_trail_source_price = price;
        if (current_position == 1) {
            current_trailing_stop = price * (1 - current_trailing_stop_delta);
        }
        else {
            current_trailing_stop = price * (1 + current_trailing_stop_delta);
        }
    }
    bool buy(real qty_percent = 5, real stop_loss = 0, real trailing_stop = 0)
    {
        if (current_position == 1) {
            //cerr << "buy: Already long!" << "\n";
            return false;
        }
        else if (current_position == -1) {
            //cerr << "buy: Is short, so we close the short before longing!" << "\n";
            close_short();
        }
        // *TODO* make sure minimum lot size is reached or fail
        current_position = 1;
        position_entry_price = feed.ticks().ask;
        // Check if stop_loss is given in percent
        if (stop_loss != 0 && stop_loss < 0.2) {
            stop_loss = feed.ticks().bid * (1 - stop_loss);
        }
        current_stop_loss = stop_loss;
        current_trailing_stop_delta = trailing_stop;
        if (trailing_stop) {
            calculate_trail_price(feed.ticks().bid);
        }
        position_size = qty_percent / 100 * balance;
        security_qty = (position_size * leverage) / position_entry_price;
        balance -= position_size;
        _Dn("Open long at " << position_entry_price << " = size " << security_qty *
            position_entry_price);
        return true;
    }
    bool sell(real qty_percent = 5, real stop_loss = 0, real trailing_stop = 0)
    {
        if (current_position == -1) {
            //cerr << "sell: Already short!" << "\n";
            return false;
        }
        else if (current_position == 1) {
            //cerr << "sell: Is long, so we close the long before shorting!" << "\n";
            close_long();
        }
        // *TODO* make sure minimum lot size is reached or fail
        current_position = -1;
        position_entry_price = feed.ticks().bid;
        if (stop_loss != 0 && stop_loss < 0.2) {
            stop_loss = feed.ticks().ask * (1 + stop_loss);
        }
        current_stop_loss = stop_loss;
        current_trailing_stop_delta = trailing_stop;
        if (trailing_stop) {
            last_trail_source_price = feed.ticks().ask;
            current_trailing_stop = feed.ticks().ask * (1 + current_trailing_stop_delta);
        }
        position_size = qty_percent / 100 * balance;
        security_qty = (position_size * leverage) / position_entry_price;
        balance -= position_size;
        _Dn("Open short at " << position_entry_price << " = size " << security_qty *
            position_entry_price);
        return true;
    }
    bool close_long()
    {
        if (current_position != 1) {
            cerr << "close_long: Not long, so couldn't!" << "\n";
            return false;
        }
        current_position = 0;
        current_stop_loss = 0;
        current_trailing_stop = 0;
        current_trailing_stop_delta = 0;
        real got = security_qty * feed.ticks().bid - security_qty *
                   position_entry_price;
        balance += got;
        transaction_times.push_back(feed.ticks().time);
        transactions.push_back(got);
        cerr << "closed long opened at " << position_entry_price << " on " <<
             feed.ticks().bid << " a diff off: " << (feed.ticks().ask -
                     position_entry_price) / position_entry_price * 100 << "\n";
        return true;
    }
    bool close_short()
    {
        if (current_position != -1) {
            cerr << "close_short: Not short, so couldn't!" << "\n";
            return false;
        }
        current_position = 0;
        current_stop_loss = 0;
        current_trailing_stop = 0;
        current_trailing_stop_delta = 0;
        real got = -(security_qty * feed.ticks().ask - security_qty *
                     position_entry_price);
        balance += got;
        transaction_times.push_back(feed.ticks().time);
        transactions.push_back(got);
        cerr << "closed short opened at " << position_entry_price << " on " <<
             feed.ticks().ask << " a diff off: " << (position_entry_price -
                     feed.ticks().ask) / position_entry_price * 100 << "\n";
        return true;
    }
    void report()
    {
        cerr << "RESULTS OF TRADING" << "\n";
        cerr << "\n";
        for (size_t i = 0; i < transactions.size(); ++i) {
            cerr << "trade: " << transaction_times[i] << " resulted in " << transactions[i]
                 << "\n";
        }
        cerr << "\n";
        cerr << "Finally: " << balance << " which is " << 100 *
             (balance - initial_deposit) /
             initial_deposit << "%" << "\n";
        cerr << "\n";
    }
  private:
    QuantFeedAbstract & feed;
    vector<real>           transactions;
    vector<QuantTime>   transaction_times;
    real initial_deposit = 0;
    real balance = 0;
    natural leverage = 0;
    int current_position = 0;
    real position_entry_price = 0;
    real position_size = 0;
    real security_qty = 0;
    real current_trailing_stop_delta = 0;
    real current_trailing_stop = 0;
    real last_trail_source_price = 0;
    real current_stop_loss = 0;
    real max_drawdown = 0;
    real max_profit = 0;
    real max_loss = 0;
};





#endif
