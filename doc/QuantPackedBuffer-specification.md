/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

# SEQUENTIAL DATA #


NOTE TODO CLEAN UP - first methodolgy is invalid - one indexed reference-point
 - go with relative reference only (integer offsets ofcourse)


## Real data aquisition -> encoding ##

* A data sample is c columns ( a struct / class )
* Each column is assigned a group-id. Closely related values share group-id (ask, bid,  ohlcm)
* RAM is allocated for the general length of active use buffer.
* A primary 'reference-values'-block (or X 2 of them.. ) is pushed in position - empty.
* New samples are pushed to buffer
* A "min & max-sample" is kept, keeping track of min and max value of each
    value.
* When buffer-page-length is reached - all the values are in-place encoded
    according to chosen ref-values. ref-values are written to the empty RAM-
    blocks.
* The whole buffer can now be written immediately to disk as is.
* Note : more than one page may be stored in each file..


With the above technique, even use-time data could be encoded. However - that
complicates algorithms - NOT GOOD!

So.. instead of above, RLE of ref-value summing might as well be done (since we
have no need to be able to index arbitrarily backwards in the packed variant -
it is simply an intermediate / _sequential_ format)


* PAGE LENGTH
 - should be longest possible to avoid repetetive reference-vals and datatype >
    n bits for columns.
 - should be small enough to be a usable unit (ie, not having to load way more
    data than _normally_ needed)

* USDZAR har t.ex. ca 18000pip diff på en dag. Så säg 30k. = 16bit needed. OM
    man ska behålla full resolution också! (Detta är nu "från single reference")

## FULLY RELATIVE VALUE (except PAGE initial reference) ##

* Since sequential read only is the way for the packed data - it's better to
    use sample-to-sample relativity (signed).

* USDZAR: tick-jump på uppemot 1000pips kan inte uteslutas. Däremot: Gör
    encoding utifrån en ranges values. Detta kräver alltså 10b + 1b sign = 11b

    Algoritm skulle kunna söka upp längst range möjligt med min.window som
    minsta, där allt får plats i: (arbitrary bit-count, that fits the column
    count - last will be padded).

# CAVAETS #

* To get the last possible value in a sequence, the full last page must be
    decoded. It's very uncommon and unlikely to do that - and in such a
    simplistic case - the overhead is nothing.
