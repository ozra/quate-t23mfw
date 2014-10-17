/**
* Created:  2014-09-26
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantStudyContextAbstract.hh"
#include "QuantExecutionRetroactive.hh"

#include "boost/program_options.hpp"


extern QuantStudyContextAbstract *  createStrategy ( QuantExecutionContext &exec_space, HashTree conf );
extern void                         destroyStrategy ( QuantStudyContextAbstract * );

namespace po = boost::program_options;




#include "QuantTime.hh"

void doTimeTests () {
    qts::EpochS     epoch_x( qts::EpochS::hours(5) );
    qts::TsS        ts_s( qts::TsS::minutes(447) );

    cerr << "EPOCH_MN etc.\n\n" << epoch_x << " (" << epoch_x.to_minutes() << "), " << ts_s << " ( " << ts_s.to_minutes() << "), ";
    epoch_x += ts_s;
    cerr << epoch_x << " (" << epoch_x.to_minutes() << "), ";
    epoch_x += epoch_x.minutes(25);
    cerr << epoch_x << " (" << epoch_x.to_minutes() << "), ";
    cerr << "ep + ts: " << (epoch_x + ts_s).to_minutes() << ", ";
    //cerr << "ep + ts + ts: " << ((epoch_x + ts_s + ts_s).to_minutes());
    cerr << "ep + ts + ts: " << ((epoch_x + ts_s + ts_s));
    cerr << "\n\n\n";

    uint32_t rgb = 0xffaa55;
    cerr << "rgb:" << rgb << " r " << (rgb & 255) << " g " << ((rgb >> 8) & 255) << " b " << ((rgb >> 16) & 255) << "\n";

    uint32_t rgba = 0xffaa5588;
    cerr << "rgba:" << rgba << " r " << (rgba & 255) << " g " << ((rgba >> 8) & 255) << " b " << ((rgba >> 16) & 255) << "\n";

}

void doDataTypeTests () {
    cerr << "DATA SIZES:"
    << "int: " << sizeof( int ) << " "
    << "long: " << sizeof( long ) << " "
    << "long long: " << sizeof( long long ) << " "
    << "dt::date: " << sizeof( dt::date ) << " "
    << "pxt::ptime: " << sizeof( pxt::ptime ) << " "
    << "pxt::time_duration: " << sizeof( pxt::time_duration ) << " "
    << "\n";

}


int main ( int argc, char **argv )
{
    clock_t prof_start = clock();

    doTimeTests();
    doDataTypeTests();

    cerr << "Init retroactivation of study / strategy" << argc << " " << argv << "\n";

    po::variables_map       opts;
    po::options_description desc("Options");

    desc.add_options()
        ("help", "Show help")
        ("start", po::value<std::string>()->default_value("2013-12-01"), "Start timepoint ( YYYY-MM-DD[ HH:MM[:SS]] )")
        ("end", po::value<std::string>()->default_value("2014-01-01"), "End timepoint ( YYYY-MM-DD[ HH:MM[:SS]] )")
        ("plot", po::value<bool>()->default_value(true), "Enable plotting output of series")
        ("realtime", po::value<bool>()->default_value(false), "Follow ticks realtime (forward testing/running)")
        ("optimization", po::value<bool>()->default_value(false), "Is it an optimization backtest?")
    ;

    po::store( po::parse_command_line(argc, argv, desc), opts );
    po::notify( opts );

    //pxt::ptime start_date( dt::date( 2013, 12, 1 ) ); // "2013-09-01 00:00:00.000" ) );
    //pxt::ptime end_date( makeTime( "2014-01-01 23:59:59.999" ) );
    std::string start_date_str = opts["start"].as<std::string>();
    if ( start_date_str.length() < 10 ) {
        cerr << desc << "\n";
        return 1;
    } else if ( start_date_str.length() < 13 ) {
        start_date_str += " 00:00:00.000";
    }

    std::string end_date_str = opts["end"].as<std::string>();
    if ( end_date_str.length() < 10 ) {
        cerr << desc << "\n";
        return 1;
    } else if ( end_date_str.length() < 13 ) {
        end_date_str += " 23:59:59.999";
    }


    if ( opts.count("help") ) {
        cerr << desc << "\n";
        return 1;
    }

    cerr << "Start was set to " << start_date_str << ".\n";
    cerr << "End was set to " << end_date_str << ".\n";

    if ( opts.count("plot") ) {
        cerr << "Plot was set to " << opts["plot"].as<bool>() << ".\n";
    }
    if ( opts.count("realtime") ) {
        cerr << "Realtime was set to " << opts["realtime"].as<bool>() << ".\n";
    }
    if ( opts.count("optimization") ) {
        cerr << "Optimization was set to " << opts["optimization"].as<bool>() << ".\n";
    }


    pxt::ptime start_date( makeTime( start_date_str ) ); // "2013-09-01 00:00:00.000" ) );
    pxt::ptime end_date( makeTime( end_date_str ) );
    bool enable_plotting = opts["plot"].as<bool>();
    bool is_optimization = opts["optimization"].as<bool>();

    HashTree strategy_conf;  // *TODO* receive through some means, or read...

    cerr << "Init retroactivator\n";
    QuantExecutionRetroactive retro_test(
        start_date,
        end_date,
        enable_plotting,
        is_optimization
    );

    //strategy_conf.add("start_date", start_date);
    //strategy_conf.add("end_date", end_date);

    QuantStudyContextAbstract *strategy = createStrategy( retro_test, strategy_conf );
    cerr << "Init strategy\n";

    cerr << "Connext retroactivator and strategy\n";
    strategy->setRunContext( &retro_test );


    cerr << "Run retro test\n";
    int ret = retro_test.run();

    cerr << "Destroy strategy" << "\n";
    destroyStrategy( strategy );

    cerr << "It all took: " << ( (clock() - prof_start) / (CLOCKS_PER_SEC / 1000) ) << "\n";

    cerr << "Done\n";

    return ret;
}

