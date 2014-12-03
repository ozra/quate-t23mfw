#ifdef SEQDACON_MAIN_INSTANCE

/**
* Created:  2014-11-12
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "boost/program_options.hpp"
#include "QuantProfiling.hh"
#include "QuantTime.hh"

#include "SQDI_Ticks_Dukascopy.hh"
#include "SQDI_Ticks_T23M.hh"

#include "EtcUtils.hh"

namespace po = boost::program_options;

template <class T>
Z run_specific(S broker_id, string symbol, QuantTime start_date, QuantTime end_date,
               bool verify_data_only, bool verbose) {
    // We want to the first even day before/at the given start time point.
    // Same holds to the other extreme on the end
    // ( output format is chunked per day, nothing less... )
    QuantTime aligned_start_ts =
        qts::get_prior_aligned_ts(start_date, pxt::hours(24));
    QuantTime aligned_end_ts =
        qts::get_next_aligned_ts(end_date, pxt::hours(24));

    T source_seq;
    source_seq.set_date_range(aligned_start_ts, aligned_end_ts);
    source_seq.init(broker_id, symbol);

    QuantTick tick;
    QuantTick prev_tick;
    bool ret;

    ActLikeTrueEvery reporting_time(1);

    /*
     * t-delta för USDZAR (2013-01-01 - 01-15)
     * Endast 0.07% under 55ms
     * Endast 15% under 100ms.
     * Endast 75% under 1000ms
     */

    Z t_delta = 0;
    Z t_delta_thresh = 55; // 1 * 1000; //10 * 1000;
    Z tick_count = 0;
    Z above_thresh_tick_count = 0;
    Z avg_t_delta = 0;
    Z max_t_delta = 0;

    SQDI_Ticks_T23M dest_seq_write{
        true
    }; // *TODO* Change to flags, it's clearer ( WRITE_MODE )
    dest_seq_write.set_date_range(aligned_start_ts, aligned_end_ts);
    dest_seq_write.init(broker_id, symbol);
    // dest_seq | dest_seq::FlagsEnum::HISTORY_MODE;
    //dest_seq_write.set_immediate_mode(true);

    while (true) {
        ret = source_seq.readTick(tick);
        if (ret == false) {
            break;
        }
        if (tick.time > aligned_end_ts) {
            break;
        }
        dest_seq_write.writeTick(tick);

        if (++tick_count > 1) {
            // calc some statistics on the data for good measure..
            // 2014-11-19/ORC
            t_delta = qts::ms_from_ptime(tick.time) -
                      qts::ms_from_ptime(prev_tick.time);

            if (t_delta > t_delta_thresh) {
                ++above_thresh_tick_count;
                --tick_count;
                // cerr << "Over thresher was:" <<  prev_tick.time << " -> "
                // << tick.time << " => " << t_delta << "\n";

            } else {
                if (t_delta > max_t_delta) max_t_delta = t_delta;
                avg_t_delta += t_delta;
            }
        }

        prev_tick = tick;

        if (reporting_time) {
            cerr << "Progress, position: " << tick.time << "\n";
        }
    }

    // avg_t_delta /= tick_count
    if (verbose) {
        cout << "Flushing possibly dangling writes.\n";
    }

    // Is handled internally, at destruction
    // dest_seq_write.flush_writes(); // IF any are still pending, cause we
    //                                // ended minutes before a day-end-break
    //                                // for instance - 2014-11-09/Oscar
    //                                // Campbell

    avg_t_delta /= tick_count;

    // // // // // // // // // // // // // // // // // // // // // // // //
    cout << "// // // // // // // // // // // // // // // // // // // // // // "
            "// //\n";
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

Z seqdacon_run(S broker_id, string symbol, pxt::ptime start_date,
               pxt::ptime end_date, bool verify_data_only, bool verbose) {
    if (broker_id == "DUKASCOPY") {
        return run_specific<SQDI_Ticks_Dukascopy>(
            broker_id, symbol, start_date, end_date, verify_data_only, verbose);

    } else {
        throw "Only dukascopy supported for now";
    }
}

//
//
//       ::::    ::::      :::     ::::::::::: ::::    :::
//       +:+:+: :+:+:+   :+: :+:       :+:     :+:+:   :+:
//       +:+ +:+:+ +:+  +:+   +:+      +:+     :+:+:+  +:+
//       +#+  +:+  +#+ +#++:++#++:     +#+     +#+ +:+ +#+
//       +#+       +#+ +#+     +#+     +#+     +#+  +#+#+#
//       #+#       #+# #+#     #+#     #+#     #+#   #+#+#
//       ###       ### ###     ### ########### ###    ####
Z main(Z argc, char** argv) {
    profiler.start(STARTUP);

    std::cerr.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    cerr << "Init SEQDACON for quant sequential data conversions " << argc
         << " " << argv << "\n";

    po::variables_map opts;
    po::options_description desc("Options");

    desc.add_options()("help", "Show help")(
        "verbose", po::value<bool>()->default_value(true), "Verbose output")(
        "verify", po::value<bool>()->default_value(false), "Verify _only_")(
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
    } else if (start_date_str.length() < 13) {
        start_date_str += " 00:00:00.000";
    }

    string end_date_str = opts["end"].as<std::string>();
    if (end_date_str.length() < 10) {
        cerr << desc << "\n";
        return 1;
    } else if (end_date_str.length() < 13) {
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

    cerr << "Broker was set to " << broker_id << ".\n";
    cerr << "Symbol " << symbol << ".\n";
    cerr << "Start was set to " << start_date_str << ".\n";
    cerr << "End was set to " << end_date_str << ".\n";

    pxt::ptime start_date(
        makeTime(start_date_str)); // "2013-09-01 00:00:00.000" ) );
    pxt::ptime end_date(makeTime(end_date_str));

    cerr << "\n";
    cerr << "ptime info - evil cast gives: "
         << *reinterpret_cast<uint64_t*>(&start_date) << "\n";
    pxt::ptime epoch_1900_test(makeTime("1900-01-01 00:00:00.000"));
    pxt::ptime epoch_1970_test(makeTime("1970-01-01 00:00:00.000"));
    cerr << "epoch 1900 - evil cast gives: "
         << *reinterpret_cast<uint64_t*>(&epoch_1900_test) << "\n";
    cerr << "epoch '70 - evil cast gives: "
         << *reinterpret_cast<uint64_t*>(&epoch_1970_test) << "\n";
    cerr << "ptime info - evil cast / (1000*60*60*24*365) : "
         << (*reinterpret_cast<uint64_t*>(&start_date) /
             (1000LL * 60 * 60 * 24 * 365LL)) << "\n";
    cerr << "\n";

    Z ret = seqdacon_run(broker_id, symbol, start_date, end_date,
                         verify_data_only, verbose);

    profiler.end(STARTUP);
    profiler.tell();

    cerr << "Done\n";

    if (verbose) {
        cerr << "Really well done\n";
    }

    return ret;
}

#endif
