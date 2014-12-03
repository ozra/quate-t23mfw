#include "QuantFeed.hh"

#ifdef INTERFACE
/**
* Created:  2014-10-12
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "rfx11_types.hh"
#include "QuantBasic_DESIGN_CHOICES.hh"
#include "QuantFeedAbstract.hh"
#include "HardSignal.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #

// template <class SLOT_T, bool USE_LOOKBACK >
template <class SLOT_T> class QuantFeed : public QuantFeedAbstract {
   public:
    QuantFeed(S broker_id, string symbol_id, Z lookback = 0,
              QuantMultiKeeperJar* the_jar = global_actives.active_jar)
        : QuantFeedAbstract(broker_id, symbol_id, lookback, the_jar) {}

    ~QuantFeed() {}

    /*
    bool readNext () final;
    */

    void emit() { // When in back-testing mode, this is called 'manually' by the
                  // mainloop the keeps track of which feed is next in line
// cerr << "QuantFeed::emit() - regulatedTickSignal" << "\n";

#ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_FEEDS
        //#ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
        //    onRegulatedTick_P.emit( *this );
        //#endif
        onRegulatedTick_T.emit(*this);

//#ifndef DESIGN_CHOICE__USER_CHECKED_GHOST_TICK
//    if ( ticks().isGhostTick() == false ) {
//        //cerr << "QuantFeed::emit() realtick" << "\n";
//        onRealTick_T.emit( *this );
//    }
//#endif

#else
        onRegulatedTick.emit();

//#ifndef DESIGN_CHOICE__USER_CHECKED_GHOST_TICK
//    if ( ticks().isGhostTick() == false ) {
//        onRealTick.emit();
//    }
//#endif

#endif
    }

#ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_FEEDS
    HardSignal<SLOT_T, QuantFeedAbstract&> onRegulatedTick_T;
//#ifndef DESIGN_CHOICE__USER_CHECKED_GHOST_TICK
//    HardSignal<SLOT_T, QuantFeedAbstract & >                onRealTick_T;
//#endif
#endif
};

#endif
