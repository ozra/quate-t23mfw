#include "QTA_Ema.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QTABase.hh"

namespace QTA {

class Ema : public QTA::ObjectAbstract {
   public:
    Ema(size_t length, size_t max_lookback = 0
                       /*, QuantBuffer input */
                       //, QuantPeriodizationAbstract &perian =
                       //*global_actives.active_periodization
        );
    /*
    Ema (
        int length
        / *, QuantBuffer input * /,
        QuantPeriodization &perian = *global_actives.active_periodization
    );
    */
    ~Ema();
    // inline void init ();

    inline auto is_primed() -> bool final {
        return (++prime_count > prime_minimum);
    }

    // inline void prime ( QuantReal value ) final {
    //    prevMA = value;
    //}

    inline auto operator<<(QuantReal value) -> void { handleValue(value); }

    inline auto operator|=(QuantReal value) -> void {
        handleAndCommitValue(value);
    }
    // inline void operator() ( QuantReal value ) {
    //    handleValue( value );
    //}
    //
    /*
    inline void operator() ( QuantBuffer<QuantReal> &value ) {
        handleValue( value );
    }
    */
    inline auto operator[](int reverse_index) const -> QuantReal {
        // *TODO* verify that the cell has been set before accessing!
        return result[reverse_index];
    }
    inline auto operator[](int reverse_index) -> QuantReal {
        // *TODO* verify that the cell has been set before accessing!
        return result[reverse_index];
    }
    inline operator QuantReal() const {
        // return *((QuantReal *)result.head_ptr);

        // *TODO* verify that the cell has been set before accessing!

        return result;
    };

    inline auto operator()(QuantReal value, int len)
        -> void { // Dynamically altered period
        ema_k = 2.0 / (len + 1.0);
        handleValue(value);
    }

    // *BUBBLARE* *TODO*
    inline auto produce() -> void { result |= prevMA; }

    // *BUBBLARE* *TODO*
    inline auto commit() -> void { result |= prevMA; }

   private:
    inline auto handleValue(QuantReal in_value) -> void {

        // *TODO*  replace with the is_primed stuff..

        if (dynamically_inited) {
            prevMA = ((in_value - prevMA) * ema_k) + prevMA;
        } else {
            prevMA = in_value;
            dynamically_inited = true;
        }
    }

    //    *TODO*  - all buffers need to implement "is_primed()"  !!

    inline auto handleAndCommitValue(QuantReal in_value) -> void {
        handleValue(in_value);
        commit();
    }

    // virtual void prime ( QuantReal value );    // prime the lookback with
    // reasonable approximations, default values

    size_t len;
    size_t lookback;
    QuantReal ema_k;

    bool dynamically_inited = false;
    size_t prime_count = 0;
    size_t prime_minimum = 0;

    QuantReal prevMA = 0;

    // QuantPeriodizationAbstract  &per;

    QuantBuffer<QuantReal> result;
};
}

#endif

namespace QTA {

Ema::Ema(size_t length,
         size_t max_lookback) //, QuantPeriodizationAbstract &per )
    : ObjectAbstract(),
      len(length),
      lookback(max(length, max_lookback)),
      ema_k(2.0 / (length + 1.0)),
      prime_minimum{ len * 4 }, // MAX( len * 4, lookback ) }
      // per( per ),
      result( lookback ) {
    // per.add( result, lookback );
}

Ema::~Ema() {}
}
