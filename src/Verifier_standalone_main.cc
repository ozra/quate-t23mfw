#ifdef VERIFIER_MAIN_INSTANCE

/**
* Created:  2014-11-21
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "boost/program_options.hpp"
#include "QuantProfiling.hh"
#include "QuantTime.hh"

#include "QSDF_Ticks_Dukascopy.hh"
#include "QSDF_Ticks_T23M.hh"

#include "EtcUtils.hh"

namespace po = boost::program_options;

template <class T>
int run_specific(string broker_id, string symbol, QuantTime start_date,
                 QuantTime end_date,
                 bool verify_data_only, bool verbose, natural max_errors)
{
    bool show_originals = true;
    bool show_diff_deltas = true;
    // const auto report_every = std::chrono::seconds(5);
    // We want to the first even day before/at the given start time point.
    // Same holds to the other extreme on the end
    // ( output format is chunked per day, nothing less... )
    QuantTime aligned_start_ts =
        qts::get_prior_aligned_ts(start_date, pxt::hours(24));
    QuantTime aligned_end_ts =
        qts::get_next_aligned_ts(end_date, pxt::hours(24));
    T seq_A;
    seq_A.set_date_range(aligned_start_ts, aligned_end_ts);
    seq_A.init(broker_id, symbol);
    // |V *TODO* Change to flags, it's clearer ( WRITE_MODE )
    QSDF_Ticks_T23M seq_B{ false };
    seq_B.set_date_range(aligned_start_ts, aligned_end_ts);
    seq_B.init(broker_id, symbol);
    QuantTick tick_A;
    QuantTick tick_B;
    QuantTick prev_tick_A;
    bool ret;
    /*
     * t-delta för USDZAR (2013-01-01 - 01-15)
     * Endast 0.07% under 55ms
     * Endast 15% under 100ms.
     * Endast 75% under 1000ms
     */
    natural ticks_ok_count = 0;
    natural ticks_err_count = 0;
    int t_delta = 0;
    int t_delta_thresh = 0; // 55; // 1 * 1000; //10 * 1000;
    int tick_count = 0;
    int above_thresh_tick_count = 0;
    int avg_t_delta = 0;
    int max_t_delta = 0;
    ActLikeTrueEvery reporting_time(1);
    _D("\n\n\nSTART VERIFICATION\n\n\n");
    while (true) {
        ret = seq_A.readTick(tick_A);
        if (ret == false || tick_A.time > aligned_end_ts) {
            break;
        }
        ret = seq_B.readTick(tick_B);
        if (ret == false || tick_B.time > aligned_end_ts) {
            break;
        }
        if (reporting_time) {
            cerr << "Progress, position: " << tick_A.time << "\n";
        }
        if (++tick_count > 1) {
            // calc some delta statistics on the A data for good measure..
            // 2014-11-19/ORC
            t_delta = qts::ms_from_ptime(tick_B.time) -
                      qts::ms_from_ptime(prev_tick_A.time);
            if (t_delta > t_delta_thresh) {
                ++above_thresh_tick_count;
                --tick_count;
                // cerr << "Over thresher was:" <<  prev_tick_A.time << " -> "
                // << tick_B.time << " => " << t_delta << "\n";
            }
            else {
                if (t_delta > max_t_delta) { max_t_delta = t_delta; }
                avg_t_delta += t_delta;
            }
        }
        int time_diff = qts::millis(tick_B.time) - qts::millis(tick_A.time);
        QuantTick diff_tick(tick_A.time, tick_B.ask - tick_A.ask,
                            tick_B.bid - tick_A.bid, tick_B.ask - tick_A.ask,
                            tick_B.ask_volume - tick_A.ask_volume,
                            tick_B.bid_volume - tick_A.bid_volume);
        int allowed_time_diff = 100;
        real allowed_price_diff = 0.00001;
        real allowed_volume_diff = 0.01;
        // correlation between A data and byte data
        if (abs(time_diff) > allowed_time_diff ||
            abs(diff_tick.ask) > allowed_price_diff ||
            abs(diff_tick.bid) > allowed_price_diff ||
            abs(diff_tick.ask_volume) > allowed_volume_diff ||
            abs(diff_tick.bid_volume) > allowed_volume_diff) {
            if (++ticks_err_count == max_errors) {
                break;
            }
            if (show_originals) {
                cerr << "DISCREPANCY =: " << tick_A.time << ": "
                     << tick_A.to_str() << "  -  " << tick_B.time << ": "
                     << tick_B.to_str() << "\n";
            }
            if (show_diff_deltas) {
                cerr << "DISCREPANCY /\: " << time_diff << ": "
                     << diff_tick.to_str() << "\n";
            }
        }
        else {
            if (verbose) {
            }
            ++ticks_ok_count;
        }
    }
    if (tick_count == 0) { tick_count = 1; }
    avg_t_delta /= tick_count;
    // // // // // // // // // // // // // // // // // // // // // // // //
    cout << "\n\n";
    cout << "\n\n";
    cout
            << "// // // // // // // // // // // // // // // // // // // // // \n ";
    cout << "\n\n";
    cout << "Ticks OK = " << ticks_ok_count << "\n";
    cout << "Ticks ERR = " << ticks_err_count << "\n";
    // // // // // // // // // // // // // // // // // // // // // // // //
    cout << "\n\n";
    cout
            << "// // // // // // // // // // // // // // // // // // // // // \n ";
    cout << "Done!\n\n";
    cout << "t_delta: " << t_delta << "\n";
    cout << "tick_count: " << tick_count << "\n";
    cout << "ABOVE THRESH tick_count: " << above_thresh_tick_count << "("
         << (above_thresh_tick_count /
             (double)(above_thresh_tick_count + tick_count) * 100) << ")"
         << "\n";
    cout << "avg_t_delta: " << avg_t_delta << "\n";
    cout << "max_t_delta: " << max_t_delta << "\n";
    cout << "\n\n";
    cout << " - - Time to verify the data - TIME TO VERIFY THE DATA - - "
         "!\n\n\n\n";
    cout << "\n\nEverything super very done. Much ready.\n\n";
    return 0;
}

int verify_run(string broker_id, string symbol, pxt::ptime start_date,
               pxt::ptime end_date,
               bool verify_data_only, bool verbose, natural max_errors)
{
    if (broker_id == "DUKASCOPY") {
        return run_specific<QSDF_Ticks_Dukascopy>(broker_id, symbol, start_date,
                end_date, verify_data_only,
                verbose, max_errors);
    }
    else {
        throw "Only dukascopy supported for now";
    }
}

void doTimeTests()
{
    cerr << "\nTEST DATING METHODS:\n\n";
    pxt::ptime epoch_1900_test(makeTime("1900-01-01 00:00:00.000"));
    pxt::ptime epoch_1970_test(makeTime("1970-01-01 00:00:00.000"));
    pxt::ptime test_date(makeTime("2013-12-01 23:14:46.123456"));
    cerr << "ptime info - evil cast gives: "
         << *reinterpret_cast<uint64_t *>(&test_date) << "\n";
    cerr << "epoch 1900 - evil cast gives: "
         << *reinterpret_cast<uint64_t *>(&epoch_1900_test) << "\n";
    cerr << "epoch '70 - evil cast gives: "
         << *reinterpret_cast<uint64_t *>(&epoch_1970_test) << "\n";
    cerr << "ptime info - evil cast / (1000*60*60*24*365) : "
         << (*reinterpret_cast<uint64_t *>(&test_date) /
             (1000LL * 60 * 60 * 24 * 365LL)) << "\n";
    cerr << "\n\n";
    qts::EpochS epoch_x(qts::EpochS::hours(5));
    qts::TsS ts_s(qts::TsS::minutes(447));
    cerr << "EPOCH_MN etc.\n\n" << epoch_x << " (" << epoch_x.to_minutes()
         << "), " << ts_s << " ( " << ts_s.to_minutes() << "), ";
    epoch_x += ts_s;
    cerr << epoch_x << " (" << epoch_x.to_minutes() << "), ";
    epoch_x += epoch_x.minutes(25);
    cerr << epoch_x << " (" << epoch_x.to_minutes() << "), ";
    cerr << "ep + ts: " << (epoch_x + ts_s).to_minutes() << "\n";
    // cerr << "ep + ts + ts: " << ((epoch_x + ts_s + ts_s).to_minutes());
    cerr << "ep + ts + ts: " << ((epoch_x + ts_s + ts_s));
    cerr << "\n\n\n";
}

void doDataTypeTests()
{
    cerr << "DATA SIZES:"
         << "\n"
         << "int: " << sizeof(int) << "\n"
         << "long: " << sizeof(long) << "\n"
         << "long long: " << sizeof(long long) << "\n"
         << "float: " << sizeof(float) << "\n"
         << "double: " << sizeof(double) << "\n"
         << "long double: " << sizeof(long double) << "\n"
         << "dt::date: " << sizeof(dt::date) << "\n"
         << "pxt::ptime: " << sizeof(pxt::ptime) << "\n"
         << "pxt::time_duration: " << sizeof(pxt::time_duration) << " "
         << "\n";
}

/*
           ::::    ::::      :::     ::::::::::: ::::    :::
           +:+:+: :+:+:+   :+: :+:       :+:     :+:+:   :+:
           +:+ +:+:+ +:+  +:+   +:+      +:+     :+:+:+  +:+
           +#+  +:+  +#+ +#++:++#++:     +#+     +#+ +:+ +#+
           +#+       +#+ +#+     +#+     +#+     +#+  +#+#+#
           #+#       #+# #+#     #+#     #+#     #+#   #+#+#
           ###       ### ###     ### ########### ###    ####
*/
int main(int argc, char ** argv)
{
    profiler.start(STARTUP);
    std::cerr.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);
    doTimeTests();
    doDataTypeTests();
    _Dn("Init SEQDACON for quant sequential data conversions " << argc << " "
        << argv);
    po::variables_map opts;
    po::options_description desc("Options");
    desc.add_options()("help", "Show help")(
        "verbose", po::value<bool>()->default_value(true), "Verbose output")(
            "verify", po::value<bool>()->default_value(false),
            "Verify _only_")("limit", po::value<int>()->default_value(10),
                             "Limit of errors/diversions before stopping ")(
                                 "broker", po::value<std::string>()->default_value("DUKASCOPY"),
                                 "Which brokers data to work upon")(
                                     "symbol", po::value<std::string>()->default_value("EURUSD"),
                                     "Which Symbol to convert")(
                                         "start", po::value<std::string>()->default_value("2013-12-01"),
                                         "Start timepoZ ( YYYY-MM-DD[ HH:MM[:SS]] )")(
                                                 "end", po::value<std::string>()->default_value("2014-01-01"),
                                                 "End timepoZ ( YYYY-MM-DD[ HH:MM[:SS]] )");
    po::store(po::parse_command_line(argc, argv, desc), opts);
    po::notify(opts);
    // pxt::ptime start_date( dt::date( 2013, 12, 1 ) ); // "2013-09-01
    // 00:00:00.000" ) );
    // pxt::ptime end_date( makeTime( "2014-01-01 23:59:59.999" ) );
    string start_date_str = opts["start"].as<std::string>();
    if (start_date_str.length() < 10) {
        cerr << desc << "\n";
        return 1;
    }
    else if (start_date_str.length() < 13) {
        start_date_str += " 00:00:00.000";
    }
    string end_date_str = opts["end"].as<std::string>();
    if (end_date_str.length() < 10) {
        cerr << desc << "\n";
        return 1;
    }
    else if (end_date_str.length() < 13) {
        end_date_str += " 23:59:59.999";
    }
    if (opts.count("help")) {
        cerr << desc << "\n";
        return 1;
    }
    bool verify_data_only = opts["verify"].as<bool>();
    bool verbose = opts["verbose"].as<bool>();
    string broker_id = opts["broker"].as<std::string>();
    string symbol = opts["symbol"].as<std::string>();
    _Dn("Broker was set to " << broker_id);
    _Dn("Symbol " << symbol);
    _Dn("Start was set to " << start_date_str);
    _Dn("End was set to " << end_date_str);
    pxt::ptime start_date(
        makeTime(start_date_str)); // "2013-09-01 00:00:00.000" ) );
    pxt::ptime end_date(makeTime(end_date_str));
    int ret = verify_run(broker_id, symbol, start_date, end_date,
                         verify_data_only, verbose, opts["limit"].as<int>());
    profiler.end(STARTUP);
    profiler.tell();
    cerr << "Done\n";
    if (verbose) {
        cerr << "Really well done\n";
    }
    return ret;
}

#endif
