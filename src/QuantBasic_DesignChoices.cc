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


// 141014 (Oscar Campbell)
// User driven periodization inputs vs periodizations attaching directly to
// event-emitters on passed feeds
#define DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS


// 141013 (Oscar Campbell)
// Should the user check for ghost-ticks (when only real ticks are wanted) in
// the "RegulatedTick" event, or should we maintain a signal for "RealTick"
// also. Which is better is not decided yet (too few test cases)
#define DESIGN_CHOICE__USER_CHECKED_GHOST_TICK


//#if DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS && !DESIGN_CHOICE__USER_CHECKED_GHOST_TICK
//    #error "DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS needs DESIGN_CHOICE__USER_CHECKED_GHOST_TICK"
//#endif


// 141013 (Oscar Campbell)
// Should buffers be accessed through accessors, or should a pointer be aquired
// to the memory region, and then pointer arithmetic, in the buffer accessing
// function / object.
#define DESIGN_CHOICE__TA_PTR_ARITH_INSTEAD_OF_BUF_ACCESSOR


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
// Tests the difference of using a Tick &t = feed.ticks(), t.doStuff(), ... vs.
// feed.ticks().doStuff(), ...
#define DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST
#undef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_2
#undef DESIGN_CHOICE__FEED_TICK_REF_MICRO_OPT_TEST_3


// 141014 (Oscar Campbell)
// See how much vector<emitters> vs emitter does in speed.
#undef DESIGN_CHOICE__HARDSIGNAL__THE_SINGLE_LISTENER_EXPERIMENT



// // // // // // // // // // // // // // // // // // // // // // // //
// 141013 (Oscar Campbell)
// Define the thresholds of the machines caches - consider parallell processes
// needs also. Use the "t23mfw-machine-statistics" tool to find
// appropriate levels automatically, per machine.
// These _may_ be used in hot code / data paths for choosing chunk/segment
// sizes of calculations etc.
#define CACHE_LINE_SIZE                 64
#define L1_PRACTICAL_CACHE_THRESHOLD    ( ( 16 * 1024 ) / 4 )
#define L2_PRACTICAL_CACHE_THRESHOLD    ( ( 2 * 1024 * 1024 ) / 4 )
#define L3_PRACTICAL_CACHE_THRESHOLD    ( 0 )

#endif
