#include "QuantPeriodization.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantPeriodizationAbstract.hh"
#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersSynchronizedHeap.hh"
#include "QuantBuffersSynchronizedBufferAbstract.hh"
#include "QuantFeed.hh"

#include "HardSignal.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

constexpr int PERIODIZATION_DEFAULT_SIZE = 547;  // *TODO* *DEBUG* - low value to ensure many buffer rotations for debuggin purposes


template <class SLOT_T>
class QuantPeriodization : public QuantPeriodizationAbstract {
  public:
    QuantPeriodization (
        double period,
        QuantPeriodizationAbstract &quant_period,
        int lookback = PERIODIZATION_DEFAULT_SIZE,
        QuantKeeperJar *the_jar = global_actives.active_jar
    ) :
        QuantPeriodizationAbstract (
            period,
            quant_period,
            lookback,
            the_jar
        )
    {};

    QuantPeriodization (
        double period,
        QuantFeedAbstract &quant_feed,
        int lookback = PERIODIZATION_DEFAULT_SIZE,
        QuantKeeperJar *the_jar = global_actives.active_jar
    ) :
        QuantPeriodizationAbstract (
            period,
            quant_feed,
            lookback,
            the_jar
        )
    {};

    //~QuantPeriodization ();

    #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
        HardSignal<SLOT_T,int> onBarClose_T;
    #endif

    void emit_signal() final {
        #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
            #ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
                onBarClose_P.emit( 0 );
            #endif
            onBarClose_T.emit( 0 );
        #else
            onBarClose.emit();
        #endif

    };

};

#endif
