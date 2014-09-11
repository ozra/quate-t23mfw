/**
* Created:  2014-07-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/


## IDEA ##
    (this snitched from Q-SCRIPT ideas..)

    - on buffer sizes:

    200EMA, daily, but on 1H candles, is 4800 samles...
    200EMA, daily, but on 5min candles, is 57 600 samles...
    500EMA, daily, but on 1H candles, is 12 000 samles...
    500EMA, daily, but on 5min candles, is 144 000 samles...

    - use rotating buffers for speed!
    - an assert operation makes sure indexing is not performed past buffer size
    - an assert makes sure the buffers are big enough for a users needs (very
        long EMA for instance)
    - asserted / debug-version of code is run for buf-len * 1.5 instances -
        after that, with no assertion errors, it switches automatically to
        optimized, un-asserted / non-debug compiled version
