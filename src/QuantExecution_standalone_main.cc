/**
* Created:  2014-09-26
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#define TEST_STRATEGY


#ifdef TEST_STRATEGY

#include "QuantSuit.hh"
#include "QuantExecutionRetroactive.hh"

#include "boost/regex.hpp"


extern QuantStudyContextAbstract *  createStrategy ( HashTree conf );
extern void                         destroyStrategy ( QuantStudyContextAbstract * );

//using boost;


int main ( int argc, char **argv )
{
    cerr << "Init retroactivation of study / strategy" << argc << " " << argv << "\n";
    /*
    if ( argc < 2 ) {
        cerr << "Missing args, need 'symbol' 'start_date' and optionally 'end_date'\n\n";
        return 1;
    }
    */

    bool enable_buffers_output = true;
    bool is_optimization = false;

    const boost::regex d6 {"(\\d\\d)(\\d\\d)(\\d\\d)"};
    const boost::regex d8 {"(\\d\\d\\d\\d)(\\d\\d)(\\d\\d)"};

    while ( argc-- ) {

        /*
        if ( std::strcmp( *argv, "--optimization" ) ) {
            cerr << "Optimization flag" << "\n";
            enable_buffers_output = false;
            is_optimization = true;

        //} else if ( ( auto match = boost::regex_match( argv, d6 ) ) || ( auto match = boost::regex_match( argv, d8 ) ) ) {

        }
         */

        ++argv;
    }

    pxt::ptime start_date( dt::date( 2013, 12, 1 ) ); // "2013-09-01 00:00:00.000" ) );
    pxt::ptime end_date( makeTime( "2014-01-01 23:59:59.999" ) );

    HashTree conf;  // *TODO* receive through some means, or read...

    cerr << "Init retroactivator\n";
    QuantExecutionRetroactive retro_test(
        start_date,
        end_date,
        enable_buffers_output,
        is_optimization
    );

    QuantStudyContextAbstract *strategy = createStrategy( conf );
    cerr << "Init strategy\n";

    cerr << "Connext retroactivator and strategy\n";
    strategy->setRunContext( &retro_test );


    cerr << "Run retro test\n";
    int ret = retro_test.run();

    cerr << "Destroy strategy" << "\n";
    destroyStrategy( strategy );

    cerr << "Done\n";

    return ret;
}

#endif
