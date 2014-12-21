// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #

#include "UziScalperBot.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
#ifdef INTERFACE

#include "QuantBasic_DESIGN_CHOICES.hh"
#include "QuantSuit.hh"
#include "QTA.hh"

#undef DEBUG_BUF_ONEOFF
#undef TEST_ONLY_ONE_BASE_PERIODIZATION

template <class T>
inline bool has(N val, T & obj)
{
    return N(val) <= obj.count();
}

template <class T>
inline bool not_yet(N val, T & obj)
{
    return N(val) > obj.count();
}

typedef QuantReal QR;




template <bool FOO = true>  // Just to keep it compiling though all inlined.. *TEMP*
class FooTrader
{
  public:
    FooTrader(QuantFeedAbstract & feed, R balance, N leverage)
        : feed(feed)
        , balance(balance)
        , initial_deposit(balance)
        , leverage(leverage)
    {
    }

    ~FooTrader()
    {
        report();
    }

    void deposit(R qty)
    {
        balance += qty;
    }
    R get_equity()
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
            R p = feed.ticks().bid;
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
            R p = feed.ticks().ask;
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
    inline void calculate_trail_price(R price)
    {
        last_trail_source_price = price;
        if (current_position == 1) {
            current_trailing_stop = price * (1 - current_trailing_stop_delta);
        }
        else {
            current_trailing_stop = price * (1 + current_trailing_stop_delta);
        }
    }
    bool buy(R qty_percent = 5, R stop_loss = 0, R trailing_stop = 0)
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
    bool sell(R qty_percent = 5, R stop_loss = 0, R trailing_stop = 0)
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
        R got = security_qty * feed.ticks().bid - security_qty * position_entry_price;
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
        R got = -(security_qty * feed.ticks().ask - security_qty *
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
        for (N i = 0; i < transactions.size(); ++i) {
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
    vector<R>           transactions;
    vector<QuantTime>   transaction_times;
    R initial_deposit = 0;
    R balance = 0;
    N leverage = 0;
    Z current_position = 0;
    R position_entry_price = 0;
    R position_size = 0;
    R security_qty = 0;
    R current_trailing_stop_delta = 0;
    R current_trailing_stop = 0;
    R last_trail_source_price = 0;
    R current_stop_loss = 0;
    R max_drawdown = 0;
    R max_profit = 0;
    R max_loss = 0;
};


using namespace QTA;

template <bool PLOTTING_USED>
class ZarScalperBot final : public QuantStudyContext<PLOTTING_USED>
{
    using parent = QuantStudyContext<PLOTTING_USED>;
    using self = ZarScalperBot<PLOTTING_USED>;

    using parent::plot;
    using parent::plot_ohlc;
    using parent::close_plot_lap;

  public:
    string                      main_symbol;
    string                      symbol2;

    QuantFeed<self>             main_feed;
    FooTrader<true>             trader;
    QuantPeriodization<self>    ps;

    QuantPeriodization<self>    pm;
    Ema                         ema_ask;
    Ema                         ema_bid;
    Ema                         ema_ask_l;
    Ema                         ema_bid_l;
    Ema                         ema50pm;
    Ema                         ema200pm;
    // PeriodTickMean         meanpm;
    PeriodTickMean              mean_ask;
    PeriodTickMean              mean_bid;
    Highest                     highestpm;
    Lowest                      lowestpm;
    Highest                     highestpmlong;
    Lowest                      lowestpmlong;

    QuantPeriodization<self>    pl;

    QuantFeed<self>             feed2nd;

    // QuantFeed           *main_joint_feed;
    N handled_ticks_count = 0;
    N handled_ticks_count2 = 0;
    QR avg_roc = 0;
    QR max_roc = 0;
    Z one_byter_roc_delta = 0;
    Z two_byter_roc_delta = 0;
    Z three_byter_roc_delta = 0;
    Z four_byter_roc_delta = 0;
    Z b12_byter_roc_delta = 0;
    QR floatingEURUSD = 0;

    #ifdef IS_DEBUG
    bool do_debug_the_time = false;
    #else
    constexpr static bool do_debug_the_time = false;
    #endif

    // *TODO* (StrategyInstance)
    ZarScalperBot(HashTree & conf)
        : parent(conf)
        , main_symbol(
              param("main_symbol", "_CRASH_ME_", "Main instrument"))
        , symbol2(param("symbol2", "EURUSD", "Complementary instrument"))
        , main_feed(param("main_broker_id", "_CRASH_ME_", "Main broker"),
                    main_symbol, 10)
        , trader(main_feed, 40000, 300)
        , ps(param("fast_period", 0.47, "'Fast' period"), main_feed)
        , pm(param("pm_period_len", 0.50, "'1 minute' period"))
          // *TODO* the tick-aggregation length should be "abrubtly" changed
          // depending on if "in or out of prime time" - thus dynamic.
        , ema_ask(param("ask_tick_ema_len", 20000, "Ask Tick EMA length"))
        , ema_bid(param("bid_tick_ema_len", 20000, "Bid Tick EMA length"))
        , ema_ask_l(param("ask_tick_ema_len", 5600 /*1600*/,
                          "Ask Tick EMA long length"))
        , ema_bid_l(param("bid_tick_ema_len", 5600 /*1600*/,
                          "Bid Tick EMA long length"))
        , ema50pm(param("fast_ema_len", 50 * 10, "Fast EMA length"))
        , ema200pm(param("mid_ema_len", 200 * 10, "Mid EMA length"))
        , mean_ask()
        , mean_bid()
        , highestpm(param("highest_len", 5, "Highest length"))
        , lowestpm(param("lowest_len", 5, "Lowest length"))
        , highestpmlong(param("highest_long_len", 100, "Highest Long length"))
        , lowestpmlong(param("lowest_long_len", 100, "Lowest Long length"))
        , pl(param("pl_period_len", 240.0, "'4 hour' period"))
        , feed2nd { "DUKASCOPY", symbol2, 3 }   // DUKASCOPY_RAW will _not_ be supported from strategies any more...
    {
        cerr << "\nZarScalperBot::ZarScalperBot constructor"
             << "\n\n";
    }

    ~ZarScalperBot()
    { print_summary(); }

    void init() final {
        //main_feed.setRegulatedInterval(ps.getPeriod());
        main_feed.onRegulatedTick_T(this, &self::every_raw_tick);
        feed2nd.onRegulatedTick_T(this, &self::every_raw_tick2);
        ps.onBarClose_T(this, &self::every_ps_close);
        pm.onBarClose_T(this, &self::every_pm_close);
        pl.onBarClose_T(this, &self::every_pl_close);
    }


    void every_raw_tick2(QuantFeedAbstract & feed)
    {
        const QuantTick & tick = feed.ticks.last_as_const(); // [0];
        if (tick.isGhostTick()) { return; }
        ++handled_ticks_count2;
        floatingEURUSD = floatingEURUSD * 0.9 + tick.ask * 0.1;
        if (not_yet(2, feed.ticks)) // feed.ticks.size < 2 )
        { return; }
    }
    // void handleTick_T ( QuantFeed< self > &feed ) {
    void every_raw_tick(QuantFeedAbstract & feed)
    {
        const QuantTick & tick = feed.ticks.last_as_const(); // [0];
        if (tick.isGhostTick()) {
            every_ghost_tick(tick);
        }
        else {
            every_real_tick(tick);
        }
    }
    inline void every_ghost_tick(const QuantTick & tick)
    {
        //_Dn("--> GHOST " << tick.time.time_of_day() << " :: " << tick.ask);
        ps.accumulate_from_feed_tick_ask(tick);
    }
    inline void every_real_tick(const QuantTick & tick)
    {
        //_Dn("--> TICK " << tick.time.time_of_day() << " :: " << tick.ask);
        trader.handle_tick();   // tick);
        ps.accumulate_from_feed_tick_ask(tick);
        ++handled_ticks_count;
        mean_ask << tick.ask;
        mean_bid << tick.bid;
        ema_ask << tick.ask;
        ema_bid << tick.bid;
        ema_ask_l << tick.ask;
        ema_bid_l << tick.bid;
    }
    void every_ps_close()
    {
        //dlog_candle_time_and_close(ps);
        //do_candle_validations(ps);
        #ifndef TEST_ONLY_ONE_BASE_PERIODIZATION
        pm.accumulate_from_source_candle(ps);
        #endif
    }
    void every_pm_close()
    {
        //dlog_candle_time_and_close(pm);
        //do_candle_validations(pm);
        pl.accumulate_from_source_candle(pm);
        if (not_yet(2, pm)) { return; }
        //#define close pm.close
        //#define open pm.open
        //#define high pm.high
        //#define low pm.low
        #if IS_DEEPBUG
        // if ( open < 20 ) {
        if (do_debug_the_time) {
            cerr << "ZarScalperBot::every_pm_close - ";
            // cerr << "#" << pm.barCount() << " - ";
            cerr << (pm.time().date().month()) << "-"
                 << (pm.time().date().day()) << " ";
            cerr << (pm.time().time_of_day()) << " - " << pm.to_str();
            cerr << " ticks: " << main_feed.real_tick_count << " / "
                 << main_feed.ghost_tick_count;
            main_feed.reset_debug_counts();
            cerr << " fltEURUSD: " << floatingEURUSD << " / "
                 << handled_ticks_count2;
            handled_ticks_count2 = 0;
            cerr << "\n";
        }
        else {
            // if ( pm.time > pxt::ptime( dt::date( 2014, 4, 10 ), pxt::hours( 0
            // ) ) )
            // {
            //    do_debug_the_time = true;
            //}
        }
        #endif
        ema_ask.produce(); // *BUBBLARE* *TODO*
        ema_bid.commit();  // *BUBBLARE* *TODO*
        ema_ask_l.commit(); // *BUBBLARE* *TODO*
        ema_bid_l.commit();  // *BUBBLARE* *TODO*
        ema50pm |= pm.close;
        ema200pm |= pm.close;
        // meanpm |= pm.close;
        mean_ask |= main_feed.ticks().ask;
        mean_bid |= main_feed.ticks().bid;
        highestpm |= pm.high;
        lowestpm |= pm.low;
        highestpmlong |= pm.high;
        lowestpmlong |= pm.low;
        // assert( meanpm[0] != 0 );
        // assert( mean_ask[0] != 0 );
        // assert( mean_bid[0] != 0 );
        QR dta_50_200 = ema50pm - ema200pm;
        QR dta_c_50 = pm.close - ema50pm;
        // QR angl30 = angle(ema30[0]);
        // cerr << "50-200: " << dta_50_200 << " dta_c_50: " << dta_c_50 <<
        // "\n";
        QR hlc3 = (pm.close + pm.high + pm.low) * (1. / 3);
        auto granular_close = main_feed.ticks().ask;
        #ifdef DEBUG_BUF_ONEOFF
        #ifdef IS_DEEPBUG
        if (has(2, pm) && has(4, main_feed.ticks)) {
            // if ( pm.count() >= 2 && main_feed.ticks.count() >= 4 ) {
            cerr << "\n"
                 << "pm.close " << pm.close << "  ps.close " << ps.close
                 << "  ps.close[1] " << ps.close[1] << "  feed.ask[0..3] = ["
                 << main_feed.ticks[0].ask << ", " << main_feed.ticks[1].ask
                 << ", " << main_feed.ticks[2].ask << ", "
                 << main_feed.ticks[3].ask << "]"
                 << "";
        }
        #endif
        #endif
        /*
        assert( pm.close == ps.close );
        assert( granular_close == pm.close );
        */
        /*

        */
        QR midp = 1.003 * (main_feed.ticks().bid + main_feed.ticks().ask) / 2;
        //bool is_sell = ema_ask < ema_ask_l && ema_bid < ema_bid_l;
        //bool is_buy = ema_ask > ema_ask_l && ema_bid > ema_bid_l;
        bool is_sell = ema_ask_l - ema_ask_l[1] <
                       -0.0001; // < ema_ask_l && ema_bid < ema_bid_l;
        bool is_buy = ema_ask_l  - ema_ask_l[1] >
                      0.0001; // ema_ask > ema_ask_l && ema_bid > ema_bid_l;
        if (is_sell) {
            if (trader.sell(5, 0, 0.005)) {
                plot(midp * (1 - 0.001), "trade");
            }
            else {
                plot(midp, "trade");
            }
        }
        else if (is_buy) {
            if (trader.buy(5, 0, 0.005)) {
                plot(midp * (1 + 0.001), "trade");
            }
            else {
                plot(midp, "trade");
            }
        }
        else {
            plot(midp, "trade");
        }
        /*

        */
        if (PLOTTING_USED) { // && is_primed ) {
            plot_ohlc(pm.open, pm.high, pm.low, pm.close, "main_instrument", "aaffaa",
                      "ffaaaa", "005500", "550000", true);
            plot(pm.high, "high", "337733", 1, LINE);
            plot(pm.low, "low", "773333", 1, LINE);
            //plot(pm.close, "close", "ffaaaa", 4, LINE);
            //plot(pm.close[0], "close[0]", "bbffbb", 3, LINE);
            //plot(pm.close(), "close())", "ccccff", 2, LINE);
            /*
            plot(hlc3, "hlc3-c-D",
                 (pm.close > pm.close[1] ? "003300" : "000033"), 1, CIRCLE,
                 DASHED, true);
            */
            // plot( hlc3, "hlc3", "0000ff", 1, CIRCLE, DASHED, true );
            // plot( ema50pm, "ema50", "0000ff", 2, DIAMOND, DOTTED );
            plot(mean_ask, "mean_ask", "000000", 3, PLUS);
            plot(mean_bid, "mean_bid", "555555", 1, PLUS);
            plot(ema_ask_l, "mean_ask_l", "ff0000", 3, CIRCLE);
            plot(ema_bid_l, "mean_bid_l", "ff5555", 1, CIRCLE);
            plot(ema_ask, "var_ema_ask", "0000ff", 3, SQUARE);
            plot(ema_bid, "var_ema_ask", "5555ff", 1, SQUARE);
            // plot( ( ema_ask + ema_bid ) / 2 , "ema_ask_bid_midpoint",
            // "000055", 2,
            // DASH, DOTTED );
            plot(ema200pm, "ema200", "ff0000", 3, LINE, DASHED);
            // plot( highestpm, "highest_high", "00aa00", 1, DASH, DASHED );
            // plot( lowestpm, "lowest_low", "aa0000", 1, DASH, DASHED );
            // plot( ( highestpm + lowestpm ) * ( 1.0 / 2.0 ), "mid", "dddddd",
            // 0,
            // CROSS, NONE pl
            // this->close_plot_lap( pm.time );
            // pltext<PLOTTING_USED>::close_plot_lap( pm.time );
            // plot( ( 1.0 / feed2nd.ticks().ask ) * 15.24, "EURUSD p", "ffaaaa"
            // );
            plot(granular_close * 1.000, "granular close", "000000", 3);
            plot(main_feed.ticks().ask * 1.000, "ticks.ask", "444444", 2);
            close_plot_lap(pm.time);
        }
    }
    void every_pl_close()
    {
        //cerr << "ZarScalperBot::handle<" << pl.getPeriod()
        //     << "> - : " << pl.time() << " " << pl.to_str() << "\n";
        /*
        assert( pl.close == pm.close );
        */
    }


    inline void dlog_candle_time_and_close(QuantPeriodizationAbstract &
                                           periodization)
    {
        _Dn("--> candle(" << periodization.getPeriod() << ") " <<
            periodization.time().time_of_day() << " :: " << periodization.close);
    }
    inline void do_candle_validations(QuantPeriodizationAbstract & periodization)
    {
        #ifdef IS_DEBUG
        if (has(2, periodization)) {
            if (periodization.time[1] == periodization.time[0]) {
                _Dn("\n\n\nDuplicated timestamp in periodization<" << periodization.getPeriod()
                    << "!!! : " << periodization.time << "\n\n\n");
            }
        }
        #endif
    }
    void print_summary()
    {
        cerr << "\n\n";
        cerr << "- - - - - -- - - - - - - - - - - - - "
             << "\n";
        cerr << "ZarScalperBot >> >>> DESTRUCTOQR <<< <<"
             << "\n";
        cerr << "floatingEURUSD = " << floatingEURUSD << "\n";
        cerr << "Avg ROC: " << avg_roc * 100000 << "\n";
        cerr << "Max ROC: " << max_roc * 100000 << "\n";
        cerr << "\n";
        cerr << "Amount of handled ticks: " << handled_ticks_count << "\n";
        cerr << "Amount of raw bytes / ticks: " << (handled_ticks_count * 40) /
             1024 / 1024 << "MB\n";
        cerr << "Amount of handled 2nd feed ticks: " << handled_ticks_count2
             << "\n";
        cerr << "Amount of raw bytes / 2nd feed ticks: "
             << (handled_ticks_count2 * 40) / 1024 / 1024 << "MB\n";
        cerr << "both in raw bytes: "
             << ((handled_ticks_count + handled_ticks_count2) * 40) / 1024 /
             1024 << "MB\n";
        cerr << "\n";
        cerr << "No. of one_byter_roc_delta: " << one_byter_roc_delta << "\n";
        cerr << "No. of two_byter_roc_delta: " << two_byter_roc_delta << "\n";
        cerr << "No. of three_byter_roc_delta: " << three_byter_roc_delta
             << "\n";
        cerr << "No. of four_byter_roc_delta: " << four_byter_roc_delta << "\n";
        cerr << "Total byte-size compressed: "
             << (one_byter_roc_delta + two_byter_roc_delta * 2 +
                 three_byter_roc_delta * 3 + four_byter_roc_delta * 4) /
             1024 / 1024 << "MB"
             << "\n";
        cerr << "Total byte-size RAW: "
             << (one_byter_roc_delta * 8 + two_byter_roc_delta * 8 +
                 three_byter_roc_delta * 8 + four_byter_roc_delta * 8) /
             1024 / 1024 << "MB"
             << "\n";
        // cerr << "No. of 1.5_byter_roc_delta: " << b12_byter_roc_delta <<
        // "\n";
        cerr << "- - - - - -- - - - - - - - - - - - - "
             << "\n";
    }
};

#endif

QuantStudyContextAbstract * createStrategy(QuantExecutionContext & exec_space,
        HashTree conf)
{
    QuantStudyContextAbstract * bot;
    if (exec_space.isBuffersOutputEnabled()) {
        _Dn("Create Plotting ZarScalperBot<true>")
        bot = new ZarScalperBot<true>(conf);
    }
    else {
        _Dn("Create Optimized ZarScalperBot<false>")
        bot = new ZarScalperBot<false>(conf);
    }
    return bot;
}

void destroyStrategy(QuantStudyContextAbstract * strategy)
{
    delete strategy;
}

#define QUANT_STUDY
#include "../ext/T23MFW/src/QuantExecution_standalone_main.cc"
