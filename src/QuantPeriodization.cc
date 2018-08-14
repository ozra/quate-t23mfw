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
#include "QuantBuffersIntertwinedHeap.hh"
#include "QuantBuffersIntertwinedBufferAbstract.hh"
#include "QuantFeed.hh"

#include "HardSignal.hh"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

constexpr int PERIODIZATION_DEFAULT_SIZE =
    547;  // *TODO* *DEBUG* - low value to ensure many buffer rotations for debuggin purposes


/*


*TODO* *GOOD* - an alternative solution is to use a mixin with only a function of a required
signature.. Any class implementing that would then have a typed method that would be
compatible with the slot.
Cavat: It needs to be virtual
*INVESTIGATE* - will it be used in such a way that the virtualization can be optimized
away? Well, it's a function POINTER anyway - so it should be moot. ? Only the _type_
is "virtual".


*/



template <class SLOT_T>
class QuantPeriodization : public QuantPeriodizationAbstract
{
  public:
    /*
    QuantPeriodization(
        double period,
        QuantPeriodizationAbstract & quant_period,
        int lookback = PERIODIZATION_DEFAULT_SIZE,
        QuantMultiKeeperJar * the_jar = global_actives.active_jar
    ) :
        QuantPeriodizationAbstract(
            period,
            quant_period,
            lookback,
            the_jar
        )
    {};
    */

    QuantPeriodization(
        real period,
        natural lookback = PERIODIZATION_DEFAULT_SIZE,
        QuantMultiKeeperJar* the_jar = global_actives.active_jar
    ) :
        QuantPeriodizationAbstract(
            period,
            lookback,
            the_jar
        )
    {};

    QuantPeriodization(
        double period,
        QuantFeedAbstract& quant_feed,
        int lookback = PERIODIZATION_DEFAULT_SIZE,
        QuantMultiKeeperJar* the_jar = global_actives.active_jar
    ) :
        QuantPeriodizationAbstract(
            period,
            quant_feed,
            lookback,
            the_jar
        )
    {};

    //~QuantPeriodization ();

    #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
    HardSignal<SLOT_T> onBarClose_T;
    #endif

    void emit_signal() final {
        //_Dn("<" << getPeriod() << "> EMIT " << time().time_of_day());

        #ifdef DESIGN_CHOICE__HARD_SIGNALS_INSTEAD_OF_LAMBDA_SIGNALS_FOR_PERIODIZATIONS
        //#ifndef DESIGN_CHOICE__USER_DRIVEN_PERIODIZATION_INPUTS
        //    onBarClose_P.emit();
        //#endif
        onBarClose_T.emit();
        #else
        onBarClose.emit();
        #endif

    };

};

#endif
