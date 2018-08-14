* Created:      2014–08–13
* Rewritten:    2015–04–21
* Author:       Oscar Campbell
* Licence:      MIT (Expat) - http://opensource.org/licenses/mit–license.html



*TODO* all bots should define a `bail–out/safeguard` hook. If the app exits or crashes it is called before shutting down. This way positions can be closed as a safeguard.



# Quate PFW/DB #


## Use case ##

Process huge amounts of quantitative data, like scientific data, most commonly time series.
Primary and driving case: time series of financial data. (x = timestamp).
Possible other uses: non timestamped measurements (x = measurement number), for instance: state of properties in an engine at each revolution. In such a case timestamp could be a secondary variable, temperature of different parts, alignment of ignition coils, voltage tension of coil, whether a knack occurs or not, intensity of knack, lambda composition of exhaust, etc..
Most commonly though, is for timestamp to be the x value.

## Prior art ##

For the DB–part: SQL databases, column based SQL DB's, time–series specific databases (often proprietary, commercial, DB's). For the finance oriented analysis (prominently rudimentary technical analysis) there are hoards of FOSS and commercial systems/frameworks, most implemented in C# and Java, some of the commercial ones in C or C++. In some cases they overlap (like for the Quate solution).

## Motivation ##

- Target: Enable optimized, yet straightforward and simple querying and processing of complex combinations of shitloads of huge data, aka "Big Data".
- Existing DB's are either designed for random access relational records which is much too slow for this application, or are exceedingly expensive proprietary solutions aimed at banks and hedge fund investors (which after studying specs and descriptions don't sound that fucking impressive in innovation anyway - especially the algo–languages which often are retaredly complex (compare KDB/Q)!)
- Existing specialized DB's still opt for fast random access writes, fast random access read, which ends up being... not fast enough. The data processed in situations like this is _always_ gathered in sequential fashion, always sone–source authorative (thus, write once), hence we opt for 'append'-mode writes only, everything else is a grand exception and it's ok for it to cost much more. The number of reads far outweigh writing. (Write once, read shitloads). Obviously the DB should be optimized for reads alone, and sequential append for writes.
- Most existing (FOSS) calc frameworks are implemented without deeper understanding of the needs of a quality algo implementation.
- Most are in C#/Java which are extremely verbose and clumsy languages for writing mathematical expressions and algorithms.
- Most frameworks can't even handle actual trade data, but only course aggregated periodized data with all fidelity removed.
- Almost no frameworks can handle multiple periodizations simulatanously, let alone, actual trade data simultanously.
- Almost no frameworks handle realistic trade–simulation, but rather rely on close price of aggregated low–res data, leading to overly optimistic, unrealistic results.
- Many datafeed/algo frameworks separate backtesting / historical data and liverunning / live–data. Quate always works with the exact same data, can backtest up till edge of data and continue live seamlessly.
- Few of existing solutions compress the data, likely assuming 'storage is cheap'. Well, _transfer_ is not cheap. The super–fast encoding/decoding scheme used by Quate has a average compression ratio of 3:16 (about 18% of original) which is possible by leveraging the entropy of the data, and the decompression is so fast that it basically uses the cycles that otherwise would be idle, waiting for data.
- **Pros of compression:**
    - (below PS means Persistent Storage, abstracting away notions of HDD's, SSD's, NAT->HDD/SDD etc. underlying hardware technologies)
    - Faster fetch from PS to RAM time
    - Faster fetch from RAM to CPU Cache time - data is decompressed in "cache–space", from source–data directly into target–bufs without in–between storing.
    - In algorithm–optimization–runs (millions of back–to–back backtests) the data will be read again and again - compressed data is much more likely to still be in RAM, so no need to go to PS for data for following processing laps.
    - Multiple processes can be run in parallell, they will not align their processing, but uses the same source data, once again, RAM retention of data increases throughput enormously.
    - Since data is memory mapped, all source buffers are shared between parallell processes - no excess create/destroy cycles of duplicate read–buffers.
- Haven't seen _one_ single framework (multi million dollar included) that treat algoritms as actually being _applied math_, they seem to happily and ignorantly pretend it's _theoretical math with infinite sets_. That's not the case. The _algos are applied math_. The _sets are finite_. There _are_ edge cases and _algorithimic instability_ to account for. Algos in Quate automatically communicate stability of calculations. These checks are applied _until_ the whole model is stable, where after the super optimized checks free execution takes over. No effects are carried out based on unstable results.

### General model / structure ###

- Data is organized in DataSets, which is somewhat like "tables", _thought_ wise.
- There can be many datasets with the exact same, or similar, schema, selection is done on the dataset–name rather than keeping all in one table and filtering by keys in rows. So in a way the dataset–name is the primary index.
- A DataSet has _one_ value index, the X column, which in time series is 'timestamp'. All columns in a DataSet are index synchronized with the authorative X column.
- The Columns of the DataSet are grouped in ColumnSets - there is always at least one ColumnSet - the 'primary', or 'base' ColumnSet, which also holds the X Column.
- Each ColumnSet must be written together and be in sync.
- Different ColumnSets can be differently advanced at the same specific time point, stemming from different sources / calculations, but are always index–canonical in relation to X, and will most oftenly not be differing
in element count for more than a few CPU cycles - but must be taken into account when reading live data asynchronously (well, or even synchronously)!
- The ColumnsSets has no relation in other DB architectures. They are a bit like the 'molecule–jars' in Highwind–Query.
- They serve two purposes:
  - Technically, to separate indexing data for columns that may not be completed at the exact same time (transaction wise)
  - Simplify querying by storing commonly used columns together, allowing glob selection by whole columnsets.

### Storage structure ###

- Storagewise, the DataSet consists of:
    - 1; _DataSet–Specification file_ for the whole DataSet's general properties, name keys etc.
    - 1–n; _ColumnSet index–file_ storing index/meta–data relating to the whole ColumnSet, one for each ColumnSet.
    - 1–n; Pairs of _Chunk–offset–Index–file_ and _Payload–file_ for each Column.
        - the offset–index is only needed when seeking.

  - Napkin calcs tells us:
    - A two column DataSet (x + value) would land in 6 files.
    - A six column DataSet (candle data for instance) would take 14 files.

- The files are stored like so (arbitrary example):
```
chosen–data–root/
    STHLMBORS–ABB–B–trades/         # - the dataset
        dataset.spec                # - its' spec - describes name, X–properties
        base–colset/                # - the base (default/primary) columnset
            columnset.index
            column–time.index       # - actual columns - `time` is the X column
            column–time.payload
            column–price.index
            column–price.payload
            column–volume.index
            column–volume.payload
            column–ordertype.index
            column–ordertype.payload

    STHLMBORS–FERMENTA–A–trades/    # Another dataset with identical schema
        dataset.spec
        base–colset/
            columnset.index
            column–time.index
            column–time.payload
            column–price.index
            column–price.payload
            column–volume.index
            column–volume.payload
            column–ordertype.index
            column–ordertype.payload
```

- A Column is a field or 'one variable'. It's a list of values stored in the payload–file. The values can actually be whole data–structures, records, what–not, but the common case, with good support from Quate itself without writing specialized record–realizers/readers/writers is a list of POD–values (int, double, etc.).
- The following is taken directly from previous T23MFW C++11/JS–V8 implementations, only difference is that they are record (row) based:
- The data is most often encoded. Many encodings are dependent on more elements than the current, so to create immediately indexable points in the (potentially enormous) list, it is encoded in chunks, or chapters.
- A seek into the list consists of:
    - looking up the offset for the chunk start, closest to wanted X.
    - jumping to the chunk start in the payload.
    - reading and discarding values, since every value is used to derive the next, until the specific 'seek point X' is reached.
- The Column specifies an _'official datatype'_ (one can realize the value in another datatype, float instead of double for instance), and an _encoding_. The encoding is definite.

- Example:
  - The _Column_ "X" of _DataSet_ "FXCM:EURUSD:trades" (which is in _ColumnSet_ "FXCM:EURUSD:trades:_base_") is a timestamp with 1ms resolution.

  - It has offical datatype `int64`. Realizing the value in an ES–environment (like io.js) will in practise give it `int53` range (since it will be stored in a double in mem), which is no problem - we'll be long dead before it runs out of dates ;-).

  - Its' encoding is `"Incrementing–Delta–Natural"` (acknowledging that negative timestamps doesn't occur in this Column in this DataSet). This means that the value is stored as a rle–encoded _natural number_ signifying the _delta_ to the _previous value_. Since it is "Incrementing" (note: it can "increment" by 0, so it is not unique), we store it as a natural which takes one bit less space, which may result in _one byte_ less due to the RLE–encoding. For the first value in every chunk/chapter the 'previous value' is set to 'chunk–start–X', _since that is the basis of the index_. Had it been _any_ other column, 'previous value' had most likely simply been set to zero, thus encoding the first value as it is (delta from 0).
  This way the first value in every chunk is decodable immediately without knowledge of prior data.


### Actually Performing a query and acting on the data ###

To begin with, the simple case, without _complex queries_ or _grouping_.

```coffeescript
# is actually livescript but... highlight support

# Here we query some forex exchange rate data, (previously) grouped
# into 5 minute candles ("bars"), which is also the primary/base
# columnset, so the 'bars' part would've been implicit if left out.
#
# Further, we limit the data to a range of x from 7 days ago from now, ending at 1 day ago from know.
# The seeking (range selection) could've been done in the query function call too.

ret = Quate.select "*", "DUKASCOPY:EURUSD:5m:bars"
#ret.seek now() - 7 `days`   # since this code is in an ES–environment, and we know the timestamp X col is in (unixtime * 1000) (kif kif), we can seek by pure number
#ret.stop–at QuTime.FOREVER  # now() - 1 `days`

# Time to do something with the data.

ret.on–data !->  # {symbol: "EURUSD"}, !->  - we've got only one dataset in the resultset, so filtering sources for the handler is moot
    trader = new PaperTrader ret.DUKASCOPY.EURUSD, capital: 100000, lot–risk: 0.05, crude–simulation, pessimism: 9  # - trader gets a whole range of datasets to choose from, it will pick the most fine grained it can, in order: _:_:depth | _:_:trades | _:_:smallest–candle–period. If datasets passed contains multiple symbols, a flag must be passed stating that it's the intention, otherwise an error will be raised.

    # examples of possible referencings to the result dataset sources
    instr = ret.DUKASCOPY.EURUSD
    hlc3 = (instr.high + DUKASCOPY.EURUSD.low + close) / 3 # close used implicitly
    hlc3–m50 = sma(hlc3, 50)
    hlc3–e200 = ema(hlc3, 200)
    hlc3–50–200–d–d1 = angle(hlc3–m50 - hlc3–e200)

    x = hlc3–50–200–d–d1

    case if
        x > 0.03
            trader.buy()
        x < -0.03
            trader.sell()
        else
            chill–dude()

    plot–pane "main"
    plot hlc3
    plot hlc3–m50, "0000ff", linewidth: 2

    plot–pane "derivates"
    plot hlc3–50–200–d–d1, scatter–style: CIRCLES, line–style: NONE


# Now we have jacked in all we want to the dataset. Let's call process, and the range of x (time) we want to process.
# since this code is in an ES–environment, and we know the timestamp X col is in (unixtime * 1000) (kif kif), we can seek by pure number
ret.process(now() - 7 `days`, QuTime.FOREVER)

```

### So, what happened there? ###

#### We start out by stating our query ####

Quate has, on startup, gone through every dir in confed data–root and loaded the data–set specs and their columnsets specs from them.
In the spec is info about key–names and their values for the dataset, and a matching rule to derive them from the filename, and from query–string. It goes through all ColumnSet dirs of the DataSet and loads the columnset–spec/index where there's intell on available columns, their types etc.

So, Quate now dissects our query and seek matching data–sets. Expand our column–selection (when globbed, like in our example), or verify that the names exists in the avail column list for the matched datasets.

It also checks that all queried datasets has the same X–Column–type, otherwise they're not compatible for comparison and joining.

We now have a query–set, a list of datasets and the columns selected. Nothing will be loaded yet, because we'll go through a culling phase first, after user–handlers has been tied.

#### Add processors ####
Now, we add a listener to the query–set.
Datahandling is done by adding listeners to "data–available" and "group–done" events, not by reading it in a loop.
The reason for this is that we can query a lot of data that has different x–signatures.
Quate ensures ordered delivery of all the different datasets by X. So dataset A from the query–set might fire about 1000 times, and about every thousanth time we reach the next X in dataset B.
We _could_ implement a "polling", loop based variant, but the user code would have to branch depending on flags for which dataset or xsig–group fired, etc. etc. It would just be unnecessarily error–prone and complicated in user–code. Plus, by listening, we pass the functions to Quate. And Quate does some crazy stuff with our functions. It re–compiles and optimizes them in accordance with the queryset it has formed, integrating user program with the very heart of the DB automatically, optimizing it on the fly. Then the V8 engine optimizes the optimized inlined code further.
Kabang! Faaaast!
One can filter the query–sets for listening to specific sources, so that a handler handles several sources, the listening processor will be instantiated once for every unique source to handle.
One can also listen to x–signature 'fully–available' event, which is fired when all the datasets in the "x–signature group" has data ready. An example would be to handle 20 different forex exchange feeds with the same handler, performing the same calculations and analysis on all of them for every 5m time period. Then a handler for the x–signature:5m event, which will be fired when all those have been fired and handled. So now we can access the calculated values from all 20 fx sets, make some weighted statistics and then make some decisions based on those and place a trade on a chosen broker.

There are plot function defined in the standard library for convenience. Plots of the data can be solved in many ways, with many kinds of software. This is just a basic helpful feature to manage and preset plot–setups quickly. This is the least important thing in this discussion, so we'll leave that aside for now.
Plots are lifted out of the code completely when running backtests and parameter optimizations.


#### Kicking of the processing ####

Now finally we call `process()` on the query–set, which kicks off the whole operation.
This is when things _really_ happen.

The user–code is traversed as an AST and type is infered for DOAMs and DOSEs (which is straight forward since their symbols are matched in the 'context'-hash passed to the mutiny–function.
**Time for some culling, aka dead code removal.**
Every DOSE (side effect) is followed upwards, marking all its back dependencies code paths as `used = true`.
Now, the AST is traversed again. All nodes where `used is false` are removed from the AST. This may be none, a few, or many, if backtesting mode and there are lots of plots. The plots will be removed, and then anything that was used for just plotting, but no other effects, will be ditched.



*TODO* jack in to the qsdf–readers, cull non used readers ... *TODO*


Opening a dataset:
    - simply open all the associated files with ro access, then mmap them all.
    - The original implementation mmap'ed small buffering regions, and re–mapped per need. This is silly.
    - The new implementation mmap's all the data fully into memory.
    - The upside to this is, the data is accessed like memory, just like any other buffer in the program.
    - The downside is that this rules out 32bit systems completely - they will run out of addressable virtual memory. The files can be GiB sizes, a large Krivo query on trade–ticks could total 100GiB.
    - The actual reserved _physical_ memory is taken care of completely by the O/S. In practise above Krivo exampe would require no more than 100KiB of source data reserved memory!! (Pre–fetching and caching aside, but those do not hog the memory).

We seek to a timepoint 7 days back. The time–value returned by the programming language in this case is compatible with the timestamp format in the dataset, so we can simply pass this value as x–selector.
The X–column specifies datatype info, so in query/seek, we could have textually written date/time based commands for specifying X range values.

We set range–end X to FOREVER. This is where _qualities of Quate become interestingly shiny_:
    - Quate works through data in a highly efficient tempo.
    - When it hits data past the ending–X, it's done.
    - When the ending–X is FOREVER (or, to be generic, actually: (X–datatype::max - 1) ) it will never stop.
        - When Quate hits end of available data, it finishes its hyper–fast read–loop, and goes in to a pretty–hyper–fast–loop that does additional verifications, because it is now officially on the edge of data, and therefor LIVE data, which, depending on sources, may need som coordination consideration because of measuring / network latencies and so on.
        - **The important part:** Your code continues to work exactly the same way without any modifications, on live data feed, as in super optimized historical data analysis.




#### The handlers sugaring, boiler–plate ellimination and optimization ####

The model, algorithm, analysis, whatever, can be written in any language that can be compiled to javascript - _without too much mangling_.

Let's take the example of a Krivo Index like model.
- We query about 20 forex exchange instruments.
- We connect a handler to handle all of these, it performs some technical analysis.
- We connect a handler to the x–signature group 'completion', it uses the results from all of the above in calcs.


```coffeescript

ret = Quate.select "* from FXCM:(EUR.*|USD.*):1h"

# ret.create–group–list 'EUR', {instrument:"EUR.*"}
# ret.create–group–list 'USD', {instrument:"USD.*"}

ret.on {instrument:"*"}, ->
    mid = (open * 0.2 + high * 0.25 + low * 0.25 + close * 0.3)
    midp = (mid - mid[1]) / mid[1]
    # mid = add(mul(open.ofs(0), 0.2), add(mul(high.ofs(0), 0.25), ...)
    # midp = div(sub(mid, mid.ofs(1)), mid.ofs(1))

    midp–m50 = sma(midp, 50)
    midp–e200 = ema(midp, 200)
    midp–50–200–d–d1 = angle(midp–m50 - midp–e200)

ret.h1.on ->
    trader = PaperTrader EURUSD, 0.05

    euros = filter (.instrument == /^EUR.../), result–sets
    dollars = filter (.instrument == /^USD.../), result–sets

    eur–m50–avg = (fold ((a,x)->a+x), (map (.midp–m50), euros), 0)
    usd–m50–avg = (fold ((a,x)->a+x), (map (.midp–m50), dollars), 0)

    wgt = funky–weighting(eur–m50–avg, usd–m50–avg)

    # The part below would likely have to be put in actual handler–cb - the rest is in the model–functor–tree
    # There's a branching, that is incomplete (no default)
    # There are statements / procedures

    if wgt > conf.buy–threshold
        buy trader

    else if wgt < conf.buy–threshold
        sell trader



            *TODO* is this solvable??


```


A L T E R N A T I V E L Y :

```coffeescript

<-! Quate.select "* from FXCM:(EUR.*|USD.*):1h"

# ret.create–group–list 'EUR', {instrument:"EUR.*"}
# ret.create–group–list 'USD', {instrument:"USD.*"}

handle {instrument:"*"}, !->
    mid = (open * 0.2 + high * 0.25 + low * 0.25 + close * 0.3)
    midp = (mid - mid[1]) / mid[1]
    # mid = add(mul(open.ofs(0), 0.2), add(mul(high.ofs(0), 0.25), ...)
    # midp = div(sub(mid, mid.ofs(1)), mid.ofs(1))

    midp–m50 = sma(midp, 50)
    midp–e200 = ema(midp, 200)
    midp–50–200–d–d1 = angle(midp–m50 - midp–e200)

after h1, !->
    trader = PaperTrader EURUSD, 0.05

    euros = filter (.instrument == /^EUR.../), result–sets
    dollars = filter (.instrument == /^USD.../), result–sets

    eur–m50–avg = fold ((a,x)->a+x), (map (.midp–m50), euros), 0
    usd–m50–avg = fold ((a,x)->a+x), (map (.midp–m50), dollars), 0

    wgt = funky–weighting(eur–m50–avg, usd–m50–avg)

    # The part below would likely have to be put in actual handler–cb - the rest is in the model–functor–tree
    # There's a branching, that is incomplete (no default)
    # There are statements / procedures

    if wgt > conf.buy–threshold
        trader.buy()

    else if wgt < conf.buy–threshold
        trader.sell()

#after result–sets, ->
finish !->
    # When the whole process is completed


```

-->

```javascript

// Generated by LiveScript 1.3.1
Quate.select("* from FXCM:(EUR.*|USD.*):1h", function(){
    /*

    TO CREATE IMPLICITLY IN THIS CONTEXT:

    handle(), after(), finish()
    resultSets

    */

  handle({
    instrument: "*"

  }, function(){
    var mid, midp, midp_m50, midp_e200, midp_50_200_d_d1;

    mid = open * 0.2 + high * 0.25 + low * 0.25 + close * 0.3;
    midp = (mid - mid[1]) / mid[1];
    midp_m50 = sma(midp, 50);
    midp_e200 = ema(midp, 200);
    midp_50_200_d_d1 = angle(midp_m50 - midp_e200);
    return;
  });

  after({
    xsignature: "1h"

  }, function(){
    var trader, euros, dollars, eur_m50_avg, usd_m50_avg, wgt;

    trader = PaperTrader(EURUSD, 0.05);
    euros = filter(function(it){
      return /^EUR.../.exec(it.instrument);
    }, resultSets);
    dollars = filter(function(it){
      return /^USD.../.exec(it.instrument);
    }, resultSets);
    eur_m50_avg = fold(function(a, x){
      return a + x;
    }, map(function(it){
      return it.midp_m50;
    }, euros), 0);
    usd_m50_avg = fold(function(a, x){
      return a + x;
    }, map(function(it){
      return it.midp_m50;
    }, dollars), 0);
    wgt = funkyWeighting(eur_m50_avg, usd_m50_avg);
    if (wgt > conf.buyThreshold) {
      return trader.buy();
    } else if (wgt < conf.buyThreshold) {
      return trader.sell();
    }
  });
  //return after(resultSets, function(){});
  finish(function(){});
});


```







#### The execution of the query and handling ####

Ok, now is when it gets a bit grittier and we put the pedal to the metal and shine off the V8 engine.










Now, let's see what the queryt–set MIGHT look like:  (this part is not set in stone yet. convenience for user is of utmost importance here!)

```coffeescript
{
__results__: [
    [{
        selector:
            broker:"DUKASCOPY"
            instrument:"EURUSD"
            grouping:"5m"
            columnset:"bars"

        columns: [   # the actual columns, they are referenced in multiple points in the result hash–tree further down..
          - name: 'time'
            buffer: {... a quate column result buffer instance... }

          - name: 'open'
            buffer: {... a quate column result buffer instance... }
          ...
            more columns cut edit for brevity...

        ]
    }]
]
__xsignatures__: [
    {
        __name__: "DUKASCOPY:5m"
        __results__: [ ... same as above in this case... ]
        EURUSD: {.. the cols, duplicate ref.. }
        #    DUKASCOPY: {.. the cols, duplicate ref.. }  - since the group was formed for (xgroup + broker), otherwise a more generic xsignature with coercion rules must be specified in the query)
    }
]
__userdata__ : {}   # Not populated until we add handlers and such..

# Convenience keyed references:

m5:             .. points referencially to above .. # since there's only one 5m group in the query

dukascopy–m5:   .. points referencially to above ..

EURUSD: { .. the columns, keyed per name as below .. }   # since there was only one instrument, with no name clashes in the query, it's conveniently put on the root

DUKASCOPY:                      # accessible through the broker–name
    EURUSD:                     # columns keyed on their names
        time: buffer–ref...
        open: buffer–ref...
        high: buffer–ref...
        low: buffer–ref...
        close: buffer–ref...
        volume: buffer–ref...
}
```
There's also some methods on the result–set, the subscribe method `on` for instance.

So, there's lots of duplication in the result set. What it will look like exactly will depend on the query.
If we select EURUSD from three different brokers, the instrument cannot be put on the root with a key.. Eh,
well, ofcourse, an array of them.. Something like that.

The exact nature of the logic of "conveniating" the result, while creating some kind of consistency has yet to
be figured out by doing a shitload of use–case tests and see what works.

When we start adding data–handlers, they will store variables and their own series/set–buffers in `__userdata__`
 (facilitated by Quate).














## *TODO* ##

- template (aka schema, but for reuse with parametirization for keys etc. details)
    - `create template trades (blargh.... )`
    - `create dataset from template trades where broker=FXPLUS, instrument=GOLD`   -- defaults for many technical values, `x–chunk–interval` etc. for instance
        - `QSDB.create 'trades', broker:'FXPLUS', instrument:'GOLD'`

## USAGE ##

### Future scenario, text selects ###
 - Needs some light parsing, this is not needed now, will be added later

    `select o h l c v m avg hlc3 from DUKASCOPY:EURUSD:5m:(bars|my)`

    `stream ohlcv(*) from DUKASCOPY:EURUSD:trades group by 5m into _:bars`
        - special functions (like ohlcv) could be defined by the qdataset, in that case target is auto–mapped to keys/tags too auto.
            - ("foo" -> BROK:{src–brok} / SYMB:{src–symb} / GROUP:{src–grouping} / COLLECTION:foo )
        - What about functions defined by col–sets? (is–buy(), is–sell(), for instance)
        - perhaps deduction from target–name to suitable grouping also... (into blargh:5m)
        - Cache/Streams can only be made by users with permission to do so, and "global/central" streams only by those with su–permission. Otherwise it becomes "user specific"..

    `select *, close–ma = ema(@c, 50), inv–close = 1 / c, deriv = c - c[1] from OANDA:USDSEK:1h`
        - list–usage of column has to be specified (see `ema(@c...)`). An `@` for list–use, plain for 'value use' (that is implicit `val[0]`)

    `select|stream|cache|aggregate close–ma = ema(@c, 50), inv–close = 1 / c, deriv = c - c[1] from OANDA:USDSEK:1h into _:extras`

    `select t = t, o = 1/o, h = 1/h, l = 1/l, c = 1/c, v = v from FXCM:AUDJPY:1h into FXCM:_JPY–AUD:1h`
        -- bars is default colset, becomes implicit target colset too, if all cols are covered  :bars (primary)"
        -- same named columns should be qualified maybe? Or does it suffice that we're doing an `into`

    `select vel, ve, a, rect from THECORP:MOONDIV:LABS:RND:REACTOR:0.001`
        - just wanted to try out an identifier with more tokens to figure out how to generalize..


### Programmatic case ###

    ```
    d–eurusd–m5 = security "*", "DUKASCOPY:EURUSD:5m", "now()-5h", "start+2h"
    d–eurusd–m5 = security "*", "DUKASCOPY:EURUSD:5m:*", "now()-5h", ".."

    foobar–t = QSDB.query {
        -- select: ['o', 'h', 'l', 'c', 'v']
        $buffer: 5000       -- ask qsdb to create and maintain buffers for return values with a minimum lookback of 5000 elements. q will choose calc–forward amounts etc.
        $fastmath: true     -- reduce precision to Float32 for added speed
        select: "o, h, l, c, v"
        from: "FOO:BAR:trades"
        group: "5m"
        start: "now() - 5h"
        end: "start + 2h"
    }

    foobar–t.on !->
        do–stuff–with–it()
        hlc3 = (foobar–t.c + foobar–t.h + foobar–t.l) / 3


    -- DSL lang, simply uses LS with some added AST magic, for aggregation, compiles to V8–JS internally (= compiles to machine code soon enough)
    -- see above also where @ refers to column value list, and no prefix refers to value at zero–index (last available value!), and here, where group() is used, plain refers instead to the list of values of each column contained in the group

    QSDB.create–stream {
        group: "1h"
        from:
            broker: "FOO"
            instrument: "BAR"
            xgroup: "trades"
            -- colset: "base"
        aggregate: """
            bars = Col–Set [t, o, h, l, c, v], {primary, auto–type}
            extras = Col–Set [vu, vd, tcu, tcd, mn, md, wmn, se, dyn–ma] --, {auto–type}

            -- explicit qualification allowed?
            bars.t = group–start–x + group–interval - 1      -- x given to group is 'next group x - smallest–value–type–step'

            bars.o = first p
            h = highest p
            l = lowest p
            c = last p
            v = sum q

            vu = sum q `filter–by` f, (f) -> f.&.1 != 0    -- removed "auto–params" for now...  -- auto–param should allow any var names, but certain ones are put in a specific order...
            vd = sum q `filter–by` f, (f) -> f.&.2 != 0     -- if context fns, this could be `-> is–sell()`

            tc = count t
            tcu = count filter–by q, f, (f) -> f.&.1 != 0
            tcd = count filter–by q, f, (f) -> f.&.2 != 0

            mn = avg(p)
            md = median(p)
            wmn = awavg(p, 1.0)
            se = stderr(p)

            dyn–ma = ema @p, 500      -- `@` before a column name signifies access to the full original set, not the group

        """
    }
    ```

    -- column–sets keep track of last–chunk and element–nm since each col–set is written together
    -- could the source define target characteristics? For simple ad–hoc derivation? yes. Ofcourse, datatypes and traits are simply inherited from source as much as possible
    -- Bonus variant would be to be able to pause aggregations, by tags (ad–hoc tags for less used FX–pairs for instance) or conf certain groups for batch runs (during the night / out of office hours, etc), instead of live continous operation..



## TECHNICAL ##

### Considerations ###

- By moving candle–integration / bar–creation into the query–system (grouping), and generalizing it to pure functional reductions, we get the bonus of being able to create the bar series there - and store them, and also to load them from there, without re–creating again and again. And ofcourse to do all kinds of creative operations, not just a "do candles for interval x". Ideally all purely mathematical should be performed in the query pipeline, and only the 'engineering stuff' (complex logics performed on returned series, buy/sell etc. async, slightly random execution, and handling stuff...)

- "Chronologization" should maybe also be moved into query–deliverer - because we can otherwise suffer gaps in trade–feeds that cause trade/bar events to be triggered out of sync (bars catch up with several ghost–bars, or close the bar (on first tick after close–time–line) after another feed has already fired a earlier tick in the new xgroup...
This could be solved separately, so that a "local" chronologizer (external to DB sys) does it, however, it seems right from more perspectives to put it in DB.
Cons for internal is that it gets more "locked in".. trying out different zipping–methods is easy if it's done by external code.

- the select/aggregate part, or rather the flow generating/calling the integrator, when working on group–data, must consider the case of 'no data elements during group interval', common actions then is to repeat certain columns and zero certain columns. Other ops are to insert special value NA / NAN. Optimally NA is supported by using n2–1 (largest value handled by data–type) and sacrificing that from available value range. This lets us stay with using general math without constant special exception checks. It's up to the user to handle NA's, should such data–sets be used. It's fully possible to avoid such sets completely. Ghost–elements (repeated) can be checked via a special meta 'flags' column instead, etc. Calculations totally agnostic to the 'not really a real value' works on like a charm, those who find the distinction important does so explicitly. Everybody happy.

- it should be possible to get a 'hard regularity' tick for the regularizing grouping queries, so values are produced ghostly in absence of actual data events - this is most import in the live scenario. In historical it's important in order to not have columns from feed B fire events before columns that are 'supposed' to close at time T before where a event in B was fired... Eh..

### General flow of operations ###

- User makes query for certain data and columns
- parse the 'semi–ast' query (hash)
- iterate the data–root. load each dir's 'dataset–spec', iterate all column–set' dirs in each. Read the 'colset–spec'
    - Now all info on available datasets, their colset, their columns and datatypes, and also query tag–expansions/matchings/defaults is available
- match the query–ast against the available sets, expand '\*' into the available columns of the selected colgroups (expand them too from * etc.)
    - verify that wanted data / columns actually are available
- setup and 'connect' the query–interface:
    - memmap _offset–index_, _payload_ for each column, _chunk–element–count_ for col–set, dataset–spec shouldn't be needed after matching/finding's been figured out (unless we're a write process - but now we're reading)
    - seek to wanted start position if any, else just hang there with mmapped files..

- user connects event–listeners to feeds, or to x–sync–groups for 'all in group available' events
- alternatively: user adds query–feeds to x–zipper–groupings (chronologizer) manually
    - connects listeners to that one, one of the x–sync–groupings, or to the original feeds
- user calls `run()` on fetcher...

- loop until x–end is reached and realize all data appropriately (if end == QVT–FOREVER, it will always be bigger than QVT–NO–FUTURE and simply stall when out of data until new arrives, on and on, until it's finally shutdown explicitly (live mode).


#### The reading process in more detail ####


qsdataset:
    x–col: cols.time

    col–sets: [
        name: 'base'
        current–chunk–element–nm: 0
        chunks–meta:    MMAP:some–root/some–qset–keys/base/column–set–meta.qsdcet
        columns:
            time:
                index:      MMAP:some–root/some–qset–keys/base/time.qsdix
                payload:    MMAP:some–root/some–qset–keys/base/time.qsdpay
            price:
                index:      MMAP:some–root/some–qset–keys/base/price.qsdix
                payload:    MMAP:some–root/some–qset–keys/base/price.qsdpay
            qty:
                index:      MMAP:some–root/some–qset–keys/base/qty.qsdix
                payload:    MMAP:some–root/some–qset–keys/base/qty.qsdpay
            flags:
                index:      MMAP:some–root/some–qset–keys/base/flags.qsdix
                payload:    MMAP:some–root/some–qset–keys/base/flags.qsdpay
    ,
        name: 'extras'
        current–chunk–element–nm: 0
        chunks–meta:    MMAP:some–root/some–qset–keys/extras/column–set–meta.qsdcet
        columns:
            feed–lag–stats:
                index:      MMAP:some–root/some–qset–keys/base–colset/feed–lag–stats.qsdix
                payload:    MMAP:some–root/some–qset–keys/base–colset/feed–lag–stats.qsdpay
    ]


    peek–x: ->
        for cs in col–sets => return NO–FUTURE  if cs.current–chunk–element–nm == cs.curent–chunk–element–count


    read–values
