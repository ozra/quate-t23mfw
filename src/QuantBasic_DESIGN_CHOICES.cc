#ifdef INTERFACE
/**
* Created:  2014-10-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/*
 * A bunch of defines used to switch between different algorithm / strutural
 * design choices in order to find out which works best in different scenarios.
 * Used for longer term structural optimization choices.
 */


/*
 * As per 2014-11-04, ALL OPTS ON vs ALL OPTS OFF gives a mere 7.27% boost..
 *
 *

uzi_bot --start="2013-11-29 12:29" --end="2014-01-29 14:37" --plot=0

opt_all = ((5523+5188+5527+5370+5203) / 5);
5362.2
opt_none = ((5910+5654+5652+6016+5682) / 5)
5782.8
opt_micro1 = ((5894+6222+5587+5855+5953) / 5)
5902.2
opt_micro3 = ((5666+5739+5905+5791+5748) / 5)
5769.8
opt_none_ref2 = ((5618+5772+5959+5883+6033) / 5)
5853
opt_bigends = ((5819+5980+5650+5669+5892) / 5)
5802
opt_hardsigs = ((5287+5569+5380+5194+5318) / 5)
5349.6
opt_singlehardsigs = ((5629+5147+5206+5157+5199) / 5)
5267.6
opt_singhards_and_user_resps = ((5221+5128+5310+5554+5299) / 5)
5302.4


 */

/*
 *
 *
 * * PROVEN WINNERS * *
 *
 *
 * */
// 141013 (Oscar Campbell)
// Use templated "hard coded" signallers instead of generic BOOST_FUNCTION
// based signalling. (Increases speed by 15% on Tick handling - but increases
// class templating / derivative complexity - can be mediated by making a
// file.t23ms DSL language, or simply generating a study from template code -
// no biggie for the scientific / analytical "end user programmer" )
#define DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_FEEDS

// 141014 (Oscar Campbell)
// Use templated "hard coded" signallers instead of generic BOOST_FUNCTION
// based signalling for periodizations.
#define DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS

// 141014 (Oscar Campbell)
// See how much vector<emitters> vs emitter does in speed.
#define DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT


/*
 *
 *
 * * GOOD CHOICES - NO BOOSTING * *
 *
 *
 */

// 141014 (Oscar Campbell)
// User driven periodization inputs vs periodizations attaching directly to
// event-emitters on passed feeds
// - has been hardcoded in now - 141104/ORC - #define DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS

// 141013 (Oscar Campbell)
// Should the user check for ghost-ticks (when only real ticks are wanted) in
// the "RegulatedTick" event, or should we maintain a signal for "RealTick"
// also. Which is better is not decided yet (too few test cases)
// - has been hardcoded in now - 141104/ORC - #define DESIGN_CHOICE__USER_CHECKED_GHOST_TICK


/*
 *
 *
 * * UNPROVEN * *
 *
 *
 */
// 141013 (Oscar Campbell)
// Should buffers be accessed through accessors, or should a pointer be aquired
// to the memory region, and then pointer arithmetic, in the buffer accessing
// function / object.
#define DESIGN_CHOICE__TA_PTR_ARITH_INSTEAD_OF_BUF_ACCESSOR

// 141014 (Oscar Campbell)
// Tests the difference of using a Tick &t = feed.ticks(), t.doStuff(), ... vs.
// feed.ticks().doStuff(), ...

//#define DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST

//#define DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_2 - has been fixed in code to the true stage

#define DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3


// 141014 (Oscar Campbell)
// Compare specific bigendian32 byte decoding vs N47-lib generics
//#define DESIGN_CHOICE__N47_BIGENDIAN_DECODERS

// 141105 (Oscar Campbell)
// Use float or double for quant-calculations?
//#define DESIGN_CHOICE__USE_FLOAT_FOR_QUANT_CALCULATIONS




// // // // // // // // // // // // // // // // // // // // // // // //
// 141013 (Oscar Campbell)
// Define the thresholds of the machines caches - consider parallell processes
// needs also. Use the "t23mfw-machine-statistics" tool to find
// appropriate levels automatically, per machine.
// These _may_ be used in hot code / data paths for choosing chunk/segment
// sizes of calculations etc.
/*
 *
#undef CACHE_LINE_SIZE                 64
#undef L1_PRACTICAL_CACHE_THRESHOLD    ( ( 16 * 1024 ) / 4 )
#undef L2_PRACTICAL_CACHE_THRESHOLD    ( ( 2 * 1024 * 1024 ) / 4 )
#undef L3_PRACTICAL_CACHE_THRESHOLD    ( 0 )

*/

#endif
