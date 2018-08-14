/**
* Created:  2014-08-16
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/


2014-10-23 - There are bugs around: OHLC-candles, EMA(1) on tick basis and
 periodicalMean doesn't line up in time. They differ by 1 bar and 2 bars
 respectively. ALSO, making a selection of time to render gives unexpected
 results: Selecting 8:59... vs 09:00... gives completely different sets of
 data - not one minutes diff!!!??? WTF gives??


 - ** PERHAPS - the date set (15:57) becomes the epochian date, even though it
 isn't.


2014-11-04 - Been re-structuring and testing a lot the last couple days.
Time-stomping different periodizations, time-stones (mini epochs), ticks, etc
is getting to where it should be. Needs to be verified even more with edge
cases (however esoteric they may be).
The one-off mystery (no more off-by-two's anyway) is still lurking.

2014-12-03 - The one-offs for periodizations is fixed. See git log instead.
This file isn't maintained well.

