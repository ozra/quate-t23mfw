#include "QTA_Highest.hh"
#ifdef INTERFACE
/**
* Created:  2014-10-12 (05:10)
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/* Find the highest value over a given length of periods
 */

#include "QuantBasic_DESIGN_CHOICES.hh"
#include "QTABase.hh"

namespace QTA {

class Highest : public QTA::ObjectAbstract
{
  public:
    /* *TODO*
     * BUFFER_JAR should have "is_primed" flag!!!
     * We should have reference to BUFFER_JAR - NOT PERIODIZATION!
     */

    Highest(ff_size_t length, ff_size_t max_lookback = 0
            //, QuantPeriodizationAbstract &per =
            //*global_actives.active_periodization
           )
        : length(length)
        , lookback{ max(max_lookback, length) }
        , // max_lookback > 0 ? max_lookback : per.default_buf_size ),
          result(lookback) {}

    ~Highest() {}

    inline void handleAndCommitValue(QuantBuffer<QuantReal>& in_buf)
    {
        ff_size_t usable_length = length;
        // cerr << "length = " << length << ", in_buf.size = " << in_buf.size <<
        // ", ";
        if (in_buf.size - 1 < usable_length) {
            usable_length = in_buf.size - 1;
            // cerr << "sets usable length to " << usable_length << ", ";
            if (usable_length < 2) { // +1 for CLOSED bar, +1 for highest_so_far
                // fallen out compare
                result |= 0.0;
                // *TODO*
                // owning_jar.is_primed = false;
                return;
            }
        }
        // *TODO*  count statistics for how many times different modes of
        // calculation are incorporated
        // cerr << "highest_so_far " << highest_so_far << ", ";
        if (in_buf > highest_so_far) {
            // cerr << "new highest_so_far gotten. ";
            highest_so_far = in_buf;
        }
        else if (in_buf[usable_length + 1] >=
                 highest_so_far) { // If value falling out of zone is lower,
            // we're good with what we have
            // cerr << "highest_so_far must be calculated again. ";
            highest_so_far = std::numeric_limits<QuantReal>::min(); //  = 0.0;
            #ifdef DESIGN_CHOICE__TA_PTR_ARITH_INSTEAD_OF_BUF_ACCESSOR
            int i = usable_length;
            QuantReal* ptr = in_buf.getPtrTo(i);
            while (--i >= 0) {
                if (*ptr > highest_so_far) {
                    highest_so_far = *ptr;
                }
                ++ptr;
            }
            #else
            for (int i = usable_length; i >= 0; --i) {
                if (in_buf[i] > highest_so_far) {
                    highest_so_far = in_buf[i];
                }
            }
            #endif
        }
        // cerr << "highest_so_far after " << highest_so_far << ", ";
        // cerr << "\n";
        result |= highest_so_far;
    }

    inline void operator|=(QuantBuffer<QuantReal>& in_buf)
    {
        // inline void operator|= ( QuantReal value ) {
        handleAndCommitValue(in_buf);
    }

    inline QuantReal operator[](int reverse_index) const
    {
        return result[reverse_index];
    }

    inline operator QuantReal() const
    {
        return result;
    };

  private:
    ff_size_t length;
    ff_size_t lookback;

    QuantReal highest_so_far = std::numeric_limits<QuantReal>::min();
    QuantBuffer<QuantReal> result;
};
}

#endif
