/*

"LANGUAGE" JIT compiled for handling series data
EXAMPLE:

TIQLS - Typed Introspective Quantitative LiveScript
QIVESCRIPT - quantitative script
LIQESCRIPT - live quantitative evaluation script
LIMESCRIPT - live math evaluation script


## IDEA ##
    - use rotating buffers for speed!
    - an assert operation makes sure indexing is not performed past buffer size
    - an assert makes sure the buffers are big enough for a users needs (very
        long EMA for instance)
    - asserted / debug-version of code is run for buf-len * 1.5 instances -
        after that, with no assertion errors, it switches automatically to
        optimized, un-asserted / non-debug compiled version



eval QS '§transed_macd = §base_macd * 1.2 + (cos x - sin y) / §close (~200)'

 - WHICH COMPILES TO:

(function() {                                                               // make sure we contain out temporary variables
    var __out = ctxt.transed_macd || {                                      // fetch or (first time) create output buf, in this case a series-container (multiple named series)
        $meta: {
            primed: false;
        }
    }
    // HERE GOES PRE-COMPILEABLE PARTS
    var _cos_x_$m_sin_y = cos(x) - sin(y);                                  // pre-calc optimized before loop
    var __i_close = ctxt.instrument.close;                                  // cache referenced series

    // HERE GOES EITHER A LOOP FOR series-collections or JUST GO
    for(part in ctxt.base_macd) {                                           // a series-collection obj was passed - iterate it's series
        var __srcs = ctxt.base_macd[part];                                  // cache the current series
        var len = __i_close.length;                                         // cache series length - we use the primary one as length reference

        // this conditional cannot be precompiled, since we iterate a
        // series-collection-object and the type therefor can't be
        // pre-determined

        if(typeof __srcs === 'number') {                                    // plain numbers can be optimized
            var __o = __out[part] = new Array(len);                         // create array for corresponding output part
            var __srcs_$mu_1_2 = __srcs * 1.2;                              // in the case of a plain number, more of the calculation can be pre-compiled

            for(var i = len - 200; i < len; i++) {                          // iterate all, or as in this case, only the last 200 periods in the series
               __o[i] = __srcs_$mu_1_2 + _cos_x_$m_sin_y / __i_close[i];      // perform the final calculation
            }

        } else if(typeof __srcs !== 'object' || !__srcs.length)             // non-arrays are not series in the series collection, but might be meta data etc.
            __out[part] =

        } else {
            __o = __out[part] = new Array(len);                             // create array for corresponding output part

            for(var i = len - 200; i < len; i++) {                          // iterate all, or as in this case, only the last 200 periods in the series
               __o[i] = __srcs[i] * 1.2 + _cos_x_$m_sin_y / __i_close[i];   // perform the final calculation
            }
        }
    }

    ctxt.transed

    return __out;
})()

*/