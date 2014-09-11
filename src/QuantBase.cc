/**
* Created:  2014-08-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/* This is a class that is passed to Feed, Periodization, etc. it contains
 * a method for multiplying the security/instruments value increasing the
 * part residing in integral (mantissa) and decreasing the part in decimals,
 * thereby:
 *  - increasing precision in float32 operations.
 *  - revealing problems in threshold logic arising from hard coded value
 *    levels.
 *
 * The factor is therefor preferably randomized for each session, thereby
 * increasing the visibility of non normalized calculations.
 */

#include "QuantBase.hh"


bad_calc _bad_calc_;    // Exception instance

Mantissation::Mantissation ( QuantReal min_factor, QuantReal max_factor ) {
    mantissation_factor = (min_factor + max_factor) * 0.5; // *TODO* *TODO* getRandomValue (min_factor, max_factor);
}

Mantissation::Mantissation ( QuantReal fixed_factor ) {
    mantissation_factor = fixed_factor;
}

Mantissation::~Mantissation () {
}

inline QuantReal Mantissation::mantise( double value ) {
    return value * mantissation_factor;
}

inline QuantReal Mantissation::demantise( QuantReal value ) {
    return value / mantissation_factor;
}

