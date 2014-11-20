# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# TRADE-GUN MUDDA FUGGA
# 2013-06-16/ORC
# 2014-05-18/ORC(09:58)



GW, QM, TU, hashing, musts, cpo <- define <[ GW qmath-fns timely hash-bashing must-haves cpo ]>
(cfg) !->
    {_L, _D, _E, _W, _T} = createLoggers 'TRM'

    TU.globalize!   # we consider these "language like utils" 	- 2014-07-13/ORC(16:18)

    /*

    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    ## STRUCTURE ##

    */

    TRM = exports = {}



    TRM.voodoo = !->
        # Gör lite practicality tests av indikatorer, buffer-hantering etc..	- 2014-07-12/ORC(02:44)
        indicator-count = 250   # let's say, a shitload of simulated automated Magic Plasma Clouds
        buffer-length = 65535
        theoretical-mem = (indicator-count * buffer-length / (1024 * 1024))
        # (15.6MB...)

        serieses = []

        for f from 0 til indicator-count
            serieses.push new Series buffer-len: buffer-length

        # *TODO* loopa dem, utför EMA, etc.. kolla throughput



    /*

      PSEUDO, STRUCTING, per 2014-07-14


    StudyContext:

    Bot:
        handle: ->  # new data arrived - arbitrary time frame at this point





    /*
    # TESTING AND BACKTESTING
    # performance of backtest vs real-time - a avg ratio could be made, thereafter
    # a controlled ratio (bit slower for cycles reserves) can be done, where all
    # timeout etc. similar values are scaled equally. This way, even timeouts /
    # network-problems can be replayed, but at higher speed, to backtest the
    # foundation, not only the quantitative analysis    - 2014-06-15/ORC(02:52)




    # DATA STRUCTURE, ETC, Nuron-related
    #
    # OHLC+V+T +SPX SPN SPA
    # Too get a full representation of market dynamics - either of these two:
    #t, fl,  v,  o,  h,  l,  c,   sx, sn, sa          - spread-max, spread-min, spread-average - ohlc being mean of ask-bid
    #t, fl,  v,  oa, ha, la, ca,  ob, hb, lb, cb      - open-ask, high-ask... open-bid, high-bid...
    # - fl = flags. 3 flags per value + 8 general.
    #
    #       By storing values as pips instead of decimal, int32 could
    #       probably be used. EURUSD needs 10^6 if half pips (som brokers)
    #       shall be retained, otherwise 10^5 (good for me), most others at
    #       10^4 fix-pointing to pip.
    #
    #       We should also store record-creation time - for lag-calculation
    #       (if > x ms "it's not lag, it's catch up"... OR FLAG THAT.. OR
    #       make sure that the 'caught up' data is fake-lagged when closing
    #       in to fresh data, thereby separating the catch-up from possible
    #       future heavy lags, which are important to document. Then also,
    #       down-time of system compared to net-/broker-lag has to be
    #       considered.
    #
    #    Let's say that there are 5s trading data, 10 years worth, that's
    #    63,072,000 records. That's 4.57GB, given o, h, l, c @ 64bit, b-o, b-h, b-l, b-c @ 64bit, t @64b, v @32b, flags@32b
    #    Add 24B id (mysql-id is 24b, since it's stored as a string) and we've got 6GB - that's a 1.43GB markup!!!
    #    Instead an on-the-fly created id made from nutope-base-id + t would serve
    #    the uniqueness purpose.
    #
    #    Let's revisit it at fixed point (pips stored), and time-stamp in
    #    seconds (fits in 32bit):
    #
    #    Gives 11 fields, all at 4B = 44B = 2.65GB
    #
    #    - Now - doing work on 5 sec data for 10 years, _might_ be moot
    #    point, but, might also be the be-all-end-all analysis. So we want
    #    it to work. But not make it to cumbersome to do the regular work -
    #    extreme tests like these may take more time simply...
    #
    #       Ofcourse, further: the o,h,l,c could be MID-VALUES, and
    #       os,hs,ls,cs are _spread_ for those values - thereby fitting in 8
    #       bits _no doubt_. Thus giving: 1.92GB - that compared to the
    #       original proposed 6GB...

    # Create a couple of base-nuron-setups for OHLC-X nurons. We want some
    # specifics in some cases:
    #
    # Brokers that supply bid and ask prices, as mentioned above, we store
    # both, or the spread to keep down size (but don't do that at the
    # expense of ease of use!!!
    #
    # Where the orderbook is available, we can take snapshots for each
    # candle of the order book in the format: ask/bid for 0.5BTC, ask/bid
    # for 1BTC, ask/bid for 5BTC, ask/bid for 10BTC, ask/bid for 100BTC -
    # this way a sort-of spread profile can be made, and possible hinting as
    # to mentality of participants. Also stability/range of
    # trade-by-trade-tick scalper.
    # So: 0.5, 1, 5, 10, 100
    #
    #   or - pow(2.71828, n) / 2 for n in 0 to 5
    # giving  => [ 0.50, 1.36, 3.70, 10.00, 27.30, 201.70 ] (2.71828... being Eulers number)
    #
    #  or  for n from 0 to 6 => v = pow(3.1415, n) / 3.1415
    # giving: [ 0.32, 1.00, 3.14, 9.87, 31.00, 97.40, 305.00 ]


    # THE ORDERBOOK DATA COULD BE PUT IN ITS OWN TABLE
    # ULTIMATELY, EVERY ORDER PUT/CANCELED/FILLED SHOULD BE STORED! GET
    # AAAALL DATA POSSIBLE!!!!!!!!!!! REDUCE AND GROUP LATER IF NEEDED



    * User-centrism ska vara en självklarhet. Flera users kan använda systemet,
        förbered löst för multi-user - on the web - usage i grund-tänk.


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    ## TIME FRAMES / PERIODIZATIONS / TIME LINES ##

    Any time frames can be used at the same time. Every Series is connected to a
    specific periodization. The current state of the series can be checked with:

    series.is-complete!

    If the series is updated with incomplete data, because our handlind function
    is called because of a new unit/bar/candle in a shorter time frame, it then
    obviously returns false, if we're on a timeframe where a specific series just
    _was_ updated (in non-correlating length periods) or just _is_ updated (in
    length divisible periodizations).

    That is, if the smallest used periodization is 5m, but we also have 23m
    periodizations, the 23m will not complete on even updates, since our handling
    function will be called on the smallest timeframe update.

    It should ofcourse be possible to configure the StudyContext to call
    'handle_candle' on _all_ periodization completions.


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    ## DEPENDENCY CHAINS FOR CALCULATIONS ##

    Series and QS-calculations work with 'inputs' from named sources. These have
    dependants listed. When a certain Series[Matrix] is completed with updated
    data - it updates all it's dependants.

    When a series, as dependant, gets new values, it checks if it's gotten _all_
    it's dependencies - in that case it can perform it's own calcs, and propagate
    further, if having dependants.


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    ## MACHINE LEARNING ##

    Om vi har, säg, 12 indikatorer vi tror starkt ger vettig information (därför
    flera derivat, som roc av grund-indikatorer etc.), så kan de kombineras (ned
    t.om till bara en av dem använd) i 133 kombinationer.

    Antal kombinationer = n ^ 2 - (n - 1)

    En SVM startas med varje kombination för att se vilken som ger bäst prediction

    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    ## TIPS ##

    * "Delaunay" funktionen i process.js-code beskriver matchning av cirkel i
    liksidig triangel - bra för auto fib circles analysen

    * good for flashy demonstration of artifical neurologic network:  http://p5p.cecinestpasparis.net/sketches/p5p.html?sketch=line/quellebruit/quellebruit.pde&lib=line/_plib/line.pde&title=Quelle%20Bruit!

    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #





    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # TECH CONSIDERATIONS #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


    * retrieve tick/ohlc-data from oasmi API, etc API, btc API, MT4 (actual trade data), and / or yahoo and google

    * MT4:
        * use HTTP module to send data to "trademaster"
        * alternatively - use ZMQ for communication
        * use MySQL module to store tick/ohlc-data directly

    * make trades through API or MT4
        * MT4 polls HTTP if not ZMQ possible


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #



    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    #       - 2014-05-17/ORC(18:36)
    # The quantitative calculation language compiler Q
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    #
    # *OBS* ! ! FUCK IT! Straight LS with good fn's does fine! :)
    #
    #Q = (src) ->

    # Example:
    /*
     - FIRST - IN STRAIGHT LS WITH UTIL FNS...

    backlen(200);
    v.set 'series:transed_macd', mapo v('base_macd'), (part) -> add(mul(part, 1.2), div(cos(x) - sin(y), v('close')));
    backlen!

     - ELLER...

    v.set 'transed_macd{}[200]', -> v('base_macd').map (part) -> part.mul(1.2).add(div(cos(x) - sin(y), v('close')));

    MORE:
        foo = ((a `_gt` b) `_and` (b `_lte` 0)) `_or` (c.basemacd.k `_eq` c.emafast)


    (uses new SeriesCollection(..) and new Series - instead of plain Arrays... (set ::isArray = -> true - on inherited fn...




    StudyContext QO
        Indicator QO
            SMatrix QO
                Series QO


        Indicator
            Series QO

        Bot QO
            Indicator QO
                Series QO
            Indicator QO
                Series QO
                Series QO
                Series QO
                Indicator QO
                    Series QO
                Indicator QO
                    Series QO
        Series QO
        Series QO


    ## MODEL I ##

    init
        me = @
        {indicator} = me
        indicator 'macd-base', do
            k-len: 14
            d-len: 28
            s-len: 9

        me.set-lookback 1 + highest [ me.c.macdbase_k_len, ..._d_len, ..._s_len ]

    handle-unit
        me = @
        me.macd-base meiji-base
        me.ema50 me.macd-base.hist
        me.ema100 me.macd-base.hist
        ema_50_100_d = abs(delta(me.ema100, me.ema50))

        #me.ema_delta_ema15 ema_50_100_d

        #if ema_50_100_d > me.ema_delta_ema15!
        #if ema_50_100_d > me.ema_delta_ema15.val and

        if ema_50_100_d > me.ema_delta_ema15(ema_50_100_d).val and me.ema_delta_ema15.val > 1.27
            me.ma50_100_signal ema_50_100_d


    ## MODEL II ##

    look further down..

    */

    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    #UTILS:
    # *TODO* *MOVE*
    Err = (e) -> new Error e

    combine-errors = (...args) ->
        out = ''
        for e in args
            out += e + '\n\n' if e

        return (e and (Err e)) or null

    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    /*
    ## IDEA ##
        - DITCH THIS FOR NOW! MAKE IT WORK FIRST!
        - use rotating buffers for speed!
        - an assert operation makes sure indexing is not performed past buffer size
        - an assert makes sure the buffers are big enough for a users needs (very
            long EMA for instance)
        - asserted / debug-version of code is run for buf-len * 1.5 instances -
            after that, with no assertion errors, it switches automatically to
            optimized, un-asserted / non-debug compiled version
    */


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    StudyContextMixins =
        join-periodization: (periodization) ->
            # *UNTESTED*
            periodization.add-dependant me




    TRM.create-bot = (conf, cb) !->
        err, Bot <- TRM.locate-and-fetch-bot conf.name
        return cb err if err

        bot = new Bot conf
        cb err, bot

    # *TODO* virr-varr av dessa!!!

    DEFAULT_LOOKBACK_BARS = 4096    # *TODO*
    TRM.DEFAULT_MAX_LEN = TRM.DEFAULT_BUF_LEN   # *TODO*
    #DEFAULT_BUF_LEN = 8192  # This is enough for 5.7 days of 1m candles, 4 weeks of 5m, 3 months of 15m, a full year of 60m. Yeah, it's pretty much enough
    DEFAULT_BUF_LEN = 4096  # This is enough for 2.85 days of 1m candles (TV handles MAX 2.3 on screen - in slooow motion), 2 weeks of 5m, 6 weeks of 15m, 6 months of 60m. Yeah, it's pretty much enough - eh, and




    TRM.InstrumentFeed = GW.dub-me class InstrumentFeed
    #   *TODO*
    #   security-feed
    #   orderbook-feed

        (conf) ->
            me = @

            me.is-security-feed = true

            if conf.security
                [me.provider, me.symbol] = conf.security.split /:/

            else
                me.provider = conf.provider
                me.symbol = conf.symbol

            me.period = conf.period
            [me.period-unit, me.period-length, me.period-duration] = TRM.dissect-period-code me.period
            me.period-offset = conf.period-offset || 0
            # etc etc *TODO*

            me.period-ranges = conf.period-ranges || [
                [now! - me.period-duration * DEFAULT_LOOKBACK_BARS, 0]    # default lookback periods, and to 'infinity' (live tick)
            ]

        init: (cb) ->
            # *UNTESTED*
            me = @


            # *TODO* FIRST FIRST - IF we're gonna live-listen - start listening
            # immediately - push to its own buffer until historical data is loaded
            # and resolved - THEN join them at the right candle and start ticking
            # off - continuing to buffer live candles that come in the mean-time
            # *TODO* - collect all ranges, "join" with "PERIOD_RANGE_BREAK_META_CANDLE"

            #   OR   use NuronStash - with live-update - when the security-fetcher
            # for the market runs it will create nurons and thus update the live
            # stash - so as long as we're looping it we'll get all candles - and
            # then we hang on to on('change') on the stash to get sub sequent
            # updates

            err <-! ache me.period-ranges, (range, next-range) !->
                err, data <-! me.request-period-range range[0], range[1]

                return next-range err if err



                err <-! ache data, (candle, next-candle) !->
                    #return next-candle err if err
                    err <-! me.on-data err, candle
                    next-candle! err

                next-range err


            do-some-shit! if err



        request-period-range: (start, end, cb) !->


            # *UNTESTED*
            me = @

            qy =
                start: start
                provider: me.provider
                symbol: me.symbol
                period: me.period
                offset: me.early-offset

            qy.end = end if end # if no end - select to last available

            err, ret <-! me.query qy
            # SHALL BE [O, H, L, C, V,  T]


        query: -> die 'InstrumentFeed::query not implemented!'

        exchange-time-offset: -> die 'InstrumentFeed::exchange-time-offset not implemented'

        on-data: (err, data, cb) ->
            # *UNTESTED*
            me = @
            me.send 'data', body:
                err: err
                data: data

            <-! me.rcv   # we wait for ack before continuing
            cb!

    # *TODO* in it's own file ? :
    InstrumentFeed::query = (qy, cb) ->
        outqy = cpo qy

        # *TODO* 1m, 15m, 1H, 1D, 1W, 1M    - the rest is calculated on
        # the fly (?) - or even less stored (candle calcing is faaaast!
        # 15 minute recalc = 96 bars / day
        # + WE MUST SUPPORT 'EARLY CANDLES' (x minutes early offset)

        qy.nt = "bars_#{THE_symbol}_   "
        err, qres <-! NM.query qy

    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    TRM.TradeDesk = class
    #   # The trading interface - is part of every security-feed - and can be
    # set to different methods, API, MT4, PaperTrader - depending on different
    # reasons - PaperTrading being an obvious one...
    #
    # Note also, that some security feeds don't have TradeDesks of their own -
    # Yahoo, Google, etc. generic services
        (conf) ->

        get-balance: (cb) !-> yada yada

        order: (order, cb) !->
            # *UNTESTED*
            switch order.type
            case 'buymarket'
                do-stuff!

            case 'sellmarket'
                do-stuff!

            case 'asklimit'
                do-stuff!

            case 'bidlimit'
                do-stuff!

            else
                die 'Are there any more to consider?'

            cb && cb err, ret

    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


    # *TODO* should probably be a Nuron - or preferably a FlightControl class!!!
    # *TODO* instead of using implements and mixins in nuron etc. perhaps a
    # 'decorate-with-bla-bla-feature' should be used instead!
    #TRM.RetroPractic = class RetroPractic implements GrapeWire
    TRM.RetroPractic = GW.dub-me class RetroPractic
    #   # Contains the setup for a BACKTEST, including period-ranges and the actual script code used, for reference
        (conf) ->
            me = @
            me.is-retro-practic = true   # set V8 optimization on course immediately
            me.max_lotsize = conf.max_lotsize || 9999999999     # a bot most likelt choose to use less according to a plan, this is the amount the bot has been given to work with
            me.spread = conf.spread || 0
            me.fee-percent = conf.fee_percent || 0
            me.fee-fixed = conf.fee_fixed || 0
            me.fee-calc = conf.fee_calc || me.default_fee_calculator
            me.bot-conf = bot-conf = conf.bot-conf || throw Err "A Bot Conf is necessary for a backtest to be performed!"
            me.join-separate-periods-in-result = true   # will be separated with meta info-"candle" - for the viewer client or analytics this is not necessary - they will notice the time-gap automatically - but - a market-closed gap and a deliberate analytics-window-gap differ. Alternatively it will create separate outputs.
            me.script-sources = []  # *TODO* when the CaseStudy/BOT class has been inited - request used resources from it and store them here.

            bot-conf.is-back-test = true    # Makes StudyContext add PapaerTrader TradeDesk to all security-feeds instead of one of their own (API, MT4, etc.)
            #bc.order-desk = new PaperTrader do
            #    fee-calc: me.fee-calc


            err, bot <-! TRM.create-bot me.bot-conf
            me.script-sources = bot.request-used-resources!
            me.bot-gwid = bot.get-gwid!
            me.sub me.bot-gwid, topic: 'signal', cb: (body, msgo) ->
                me.on-bot-terminated if body is 'finished'

        on-bot-terminated: (msgbody, msgo) ->
            # *UNTESTED*

        default_fee_calculator: (order) -> me.fee_fixed + order.qty * order.price * me.fee_percent

        # *TODO*


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    /*

    ## MODEL II ##

    class some-processing-thing extends Indicator
        init: (conf) ->
            # *UNTESTED*
            me = @
            [me.v, me.indicator] = me.get-quant-ops! 'my-processing-name'
            {indicator} = me


            v \meiji-base = v.main-security

            #v \macd-base v.macd meiji-base

            # note: no values - just class creation..
            indicator 'macd-base', do
                k-len: 14
                d-len: 28
                s-len: 9

            me.set-lookback 1 + highest [ me.c.macdbase_k_len, ..._d_len, ..._s_len ]

        handle-unit: (conf) ->
            # *UNTESTED*
            me = @
            {v
            me.macd-base meiji-base
            me.ema50 me.macd-base.hist
            me.ema100 me.macd-base.hist
            ema_50_100_d = abs(delta(me.ema100, me.ema50))

            #me.ema_delta_ema15 ema_50_100_d

            #if ema_50_100_d > me.ema_delta_ema15!
            #if ema_50_100_d > me.ema_delta_ema15.val and

            if ema_50_100_d > me.ema_delta_ema15(ema_50_100_d).val and me.ema_delta_ema15.val > 1.27
                me.ma50_100_signal ema_50_100_d

    */

    QuantitativeContextMixins =
        get-quant-ops: (id) ->
            # *UNTESTED*
            me = @

            # *TODO* these should simply be created in init-quant-ops(), then
            # they're bound and can easily be referenced, and copied to locals as
            # desired

            val-fn = me._value
            security-fn = me._security
            indicator-fn = me._indicator
            param-fn = me._parameter

            v = -> val-fn.apply me, arguments
            param = -> param-fn.apply me, arguments
            indicator = -> indicator-fn.apply me, arguments
            security = -> security-fn.apply me, arguments

            return [ v, param, indicator, security ]

        push-name: (id) !->
            # *UNTESTED*
            I = @
            I.study-context.name-space.push id
            I.my-id = I.name-space.join '_'


            # *TODO*

            # *everything - on a name base ("one row" mostly) will be
            # called in the same sequence - we can therefor use name-space-context-
            # enums for uniquely and _exactly_ naming child-items.

            # * a flag is set on a quantCtxt on creation (dynamic-init = false)
            #   - when process() has been called the first time, it's set to true

            # * quant-ops only do initating things on dyn-init == false. After that
            #   they reference the created and cached q-objects upon meeting any
            #   q-context generating object. (ex. macd(inputs, conf) is just a
            #   quick-n-dirty coding style for looking up indicator class macd,
            #   creating it with the conf - _if first pass_. In any pass - the
            #   class is then used to process a unit. The class is cached according
            #   to namespaced name - which is constructed from every Q-C's self id
            #   in combination with the namespaced name given through the result
            #   variable name, _and_ finally, an enum. The enum could restart in
            #   every space. Or, possibly, we could have a StudyContext-global
            #   index - simplifying access in ui's by using a specific number as
            #   shortcut.. Such an ordinal is also helpful in locating the specific
            #   QC-item in code (source positionally)!

            # * fundera över när class-str-id används och om closured free vars kan
            #   användas (macd, rsi, etc...) - effektivast är nog:
            #       indicator \my-name, \cls-id, conf
            # * Fler variationer:
            #       qvar \my-mcad macd(inputs, conf) - creates macd-instance if first pass, process
            #       indicator macd conf - creates a macd-instance only
            #       some-macd-ref(inputs) - calculates a macd only

            # * TWO namings should be good : composite-indexing-id, and
            #   composite-display-id

        pop-name: (id) !->
            # *UNTESTED*
            I = @
            I.study-context.name-space.pop!

        prepare-add: (id, conf) !->
            # *UNTESTED*
            if typeof id is \object
                conf = id
                id = conf.id


            # *TODO* - different routes depending on if already created or not, etc.


            # conf to new child quantitative context operator
            conf = deco conf
            conf.study-context = me.study-context
            conf.parent = TODO_TODO
            return [id, conf]

        teardown-all: !->
            # *UNTESTED*
            I = @
            for qtype in <[ qvals series securities ]>
                list = I[qtype]
                for own k, v of list
                    list[k].tear-down!

        _value: (id, conf) ->
            # *UNTESTED*
            me = @
            conf = _prepare-add id, conf

            if typeof id is \string and !conf   # create slot, and name-space operations
                me.push-name id

                bufs = me.study-context.buffers

                return (val) ->
                    bufs[me.my-id] = val
                    me.pop-name!
                    return val

            else    # used to _create_ a buffer, not just a slot for data
                if typeof id is \object
                    conf = id
                    id = conf.id

                me.push-name id
                me.study-context[me.my-id] = s = new Series conf
                me.pop-name!
                return s

        _parameter: (id, conf) ->        # add a parameter
            # *TODO*
            add-here-ok!

        _security: (id, conf) ->         # add a security
            # *TODO*
            conf = cpo conf
            conf.study-context = me.study-context

            # *TODO*
            # THIS ONE SHALL PASS ON PARAMS TO security feeder about PERIOD-WINDOWS
            # and wether to continously send live data or just fixed windows and
            # finish off by sending a 'END OF DATA' unit.
            add-here-ok!

        _indicator: (id, conf) ->        # add an indicator
            me = @
            conf = deco conf
            conf.study-context = me.study-context
            me.unprimed-indicators++
            # *TODO*
            add-here-ok!

        #analysis: (conf) ->        # add an analyser
        #    me = @
        #    conf = deco conf
        #    conf.study-context = me.study-context
        #    me.unprimed-indicators++
        #    # *TODO*
        #    add-here-ok!

        _matrix: (id, conf) ->           # *TODO* matrix instead of collection - not fully right...
            me = @
            conf = deco conf
            conf.study-context = me.study-context
            s = me.matrices.add new SeriesCollection/SMatrix conf
            return s

        _series: (id, conf) ->
            # *UNTESTED*
            me = @
            conf = deco conf
            conf.study-context = me.study-context
            s = me.series.add new Series conf
            return s


    TRM.StudyContext = class implements StudyContextMixins, QuantitativeContextMixins
    #   # Contains default settings etc. that can be used by indicators etc. classes
        (conf) ->
            me = @
            me.is-case-study-context = true   # set V8 optimization on course immediately
            me.ready = (me.ready || 0) - 1
            me.errors = []
            me.unprimed-indicators = 0

            if IS_DBG
                die 'A main-periodization is highly recommended for a TradeContext / TradeBot.' if not conf.main-periodization

            ## *TODO* ! ! ! JUST PASS 'me' AS PARAM TO Series etc.!!! We do use 'add-series', 'add-sercoll', 'add-indicator' etc.!
            #cls = me.SeriesCollection = class MySeriesCollection extends SeriesCollection
            #cls.trade-context = me
            #
            #cls = me.Series = class MySeries extends Series
            #cls.trade-context = me

            err <- me.init!
            if err
                do-some-shit!

            die "A CaseStudy needs to subscribe to at least SOME market data or event to trigger 'handle'" if not me.subscriptions
            die 'TradeBot::init-super must be called!' if not me.case-study-super-called

            me.ready++

        error: (...args) !->
            # *UNTESTED*
            me = @
            err = _DF args
            me.errors.push [now!, err]
            me.send \error, err # message sending
            me.fire \error err

        do-handle: (cb) !->
            # *UNTESTED*
            me = @
            err <-! me.handle
            got-err = false
            er = -> got-err := true; me.error.apply me, arguments

            er "Error in 'handle': ", err if err
            er "Neglections in ::handle!!", handle-err if handle-err = me.sanity-check!

            cb combine-errors err, handle-err

        sanity-check: ->
            # *UNTESTED*
            # *TODO* - do sanity checks on integrity after handle, etc. events
            return null

        get-periodization: (periodization) -> periodization || @main-periodization

        is-ready: -> @ready is 0    # *TODO*

        is-primed: -> @unprimed-indicators is 0 # *TODO*

        init: (cb) !->   # *TODO* cb on init might be overkill, we simply 'state' our needs in it - do we really need to be able to do arbitrary async/server requests in it?
            @case-study-super-called = true
            cb!           # default init







    # *TODO* TRADEBOT INHERITS _STUDYCONTEXT / STUDYCASE_

    #TRM.TradeBot = class TradeBot extends Indicator implements QuantitativeContextMixins
    TRM.TradeBot = class extends TRM.StudyContext implements QuantitativeContextMixins
    #   # The basis for a tradebot - bots inherit this class
    #   # TradeBots has trading abilities aside from just 'inputs' from the market
    #   # which sets it aside from a plain StudyContext
        (conf) !->
            me = @
            me.is-trade-bot = true   # set V8 optimization on course immediately
            <-! super conf
            me.ready--
            me.conf = me.context.conf = conf

            #err <-! me.init!
            die 'TradeBot::init-super must be called!' if not me.bot-super-called

            me.ready++

        do-init: (cb) !->
            # *UNTESTED*
            me = @

            # putting an array in type to allow more types
            # first args in 'allowed range' here is global - allow 0 or more..
            # second args are redundant in this case, but to show: settings for the types them selves, in the same order
            me.parameter \notify-errors-to, 'Notify errors to:', [TYPE.EMAIL, TYPE.PHONE], [0, null, [0, null], [0, null]]    # - *TODO* - should be DEFAULT PARAM FOR ALL BOTS - FILL IN ADRESS AND IT FIRES AWAY! emails yields that, numbers can be entered mixed in the field, then sms will be sent

            err <- super    # calls me.init() - the user function...
            cb err

        do-handle: (cb) !->
            # *UNTESTED*
            err <-! super
            cb err

        serialize: -> @context      # default serializer

        init: (cb) !->                  # *TODO* - not necessary - the work is done in do-init and do-init-super()
            @bot-super-called = true
            super cb
            cb!           # default init

        run-criteria: -> true       # criteria for actually running the analysis can be put here, if not in handle alone...

        rest-criteria: -> false

        handle: (data, cb) !->                  # default handler
            cb Err 'TradeBot::handle Not Implemented'

        destroy: ->                 # default destroy


    TRM.dissect-period-code = (period) ->
        ret = period.match /^(\d*)(\w?)$/

        qty = ret[1] || 1
        unit = ret[2] || 'm'

        duration = switch unit
        | 'T', 'tick'   => 0
        | 'minutes'     => qty |> minutes
        | 'H'           => qty |> hours
        | 'D'           => qty |> days
        | 'W'           => qty |> weeks
        | 'M'           => qty |> months

        return [unit, qty, duration]


    TRM.Periodization = class implements StudyContextMixins
        # A periodization with current period tracking - for bots with several periodizations and
        # perhaps tick update, but working on higher timeframes, to maintain series
        # length integrity
        #
        # A periodization collects all external data (within timeout limits) before
        # calling handle(). The StudyContexts handle is called then, from the
        # periodizations handle(). SC's handle is therefor called several times
        # from.

        (duration, offset, conf) ->
            me = @
            me.is-time-line = true   # set V8 optimization on course immediately

            me.deps = []
            me.dependants = []
            me.count = 0

            me.period = duration
            me.offset = offset

            me.max-len = conf.max-len || TRM.DEFAULT_MAX_LEN

        new-period: (something-here-ke) ->  # *TODO*
            me.count++
            while me.count > me.max-len
                me.do-shift!

        post-handle-sanity-checks: ->
            # *UNTESTED*
            if IS_DBG
                return 47 is 47
            # *TODO* - make sure all dependants of ours have the same lengths so
            # that no series or series-matrix has been missed in updating in handle

        destroy: !->
            # *UNTESTED*
            me = @
            me.deps = null
            # *TODO* for d in deps : d.remove-dependency ?   ? - think about order of events...

        add-dependant: (dep) !->
            # *UNTESTED*
            @dependants.push dep

        do-shift: !->   # (silent) !->
            # *UNTESTED*
            me = @
            me.count--
            for dep in me.dependants
                dep.do-shift!



    #QM.Series = class Series extends Array
    #    value-of: -> @[@length - 1]
    #    v: (n) -> @[@length - 1 - if typeof(n) is \number then n else 0]



    # Look at SeriesBuffer defined in t23m/src/      - 2014-07-18/ORC(13:47)
    TRM.Series = class implements StudyContextMixins # extends Array # ditches Array	- 2014-07-13/ORC(16:11)
        (id, values, conf) ->
            me = @
            me.is-series = true   # set V8 optimization on course immediately

            # *TODO* SERIES ONLY TAKE SINGLE VALUES TOO! ITERATIVE CALCS ALL OVER!
            # (loop and call update on this)
            values ||= []
            me.values = values

            if not conf
                conf = id
                id = conf.id

            conf = cpo conf    # deep decouple as to not mutate source


            # *TODO* ideally we return a function - methods are added upon it to
            # enable setting values etc. otherwise the series is used as s! (
            # returns head), s(3) (returns fourth element in tail, head is 0)
            # s1.input diff(s2) / absd(s2, ma1), s2.is-complete

            me.id = id
            me.buf-len = conf.buf-len || DEFAULT_BUF_LEN   # *TODO* *ASSERT check* must be power of two, 2^n
            #me.buf-mask = ~me.buf-len
            #me.head-pos = 0

            return (i, v) ->
                i ||= 0

                if IS_DBG
                    if i >= me.buf-len
                        die 'Buffer Length was over-run! Increase it one binary bit! Now:', conf.buf-len, 'increase to:', conf.buf-len * 2

                # *TODO* rotating buf is over-opting.. might not even be faster. KISS!  # 	- 2014-07-13/ORC(16:14)
                #i = (me.head-pos + i) .&. me.buf-mask     # wrap position around "edge" if necessary


                # *TODO* we just might store the values in reverse order instead..
                ix = me.values.length - 1 - i
                if is-def v     # "set-mode"
                    values[ix] = v

                return values[ix]




        update: (value, conf) ->



            # *UNTESTED*
            me = @
            # *TODO*  set vs update etc..

            # perhaps the VALUE isn't necessary for 'is-complete', or rather:
            # is-new-value-position - check. We only feed values of the same
            # periodization as the Series - therefore it only has to look at it's
            # Periodization. (me.periodization.is-new-value is true)

            me.advance-pos! if me.periodization.has-new-value

            if value.is-series
                value = value.v()



        do-shift: !->   # (silent) !->


            # *UNTESTED*
            me = @
            me.values.shift!

        sanity-check: !->
            # *UNTESTED*
            return if not IS_DBG    # *TODO* should be me.verbosity < SANITY_CHECKS

            # do checks on series lengths etc.
            die "Wow - didn't see that coming!" if not 47 is 47


    TRM.SMatrix = class SMatrix implements StudyContextMixins
        # Each case should inherit this simply to configure it with main-case-context for defaults
        (id, conf) ->
            me = @
            me.is-series-matrix = true   # set V8 optimization on course immediately

            if not conf
                conf = id
                id = conf.id
            conf = cpo conf    # deep decouple as to not mutate source

            me.$meta = apply conf, do
                name: id
                study-context: conf.study-context
                periodization: conf.periodization || conf.study-context.periodization
                plotstyles: {}   # *TODO* - this should be stored on each Series # for setting up default plot styles for series in the matrix
                sids: []

            if IS_DBG then me.validate-conf me.$meta #conf

            # remove meta data
            #conf.id = null
            #conf.periodization = null

            per = @periodization

            for series in conf.series
                continue if not series

                if is-str series
                    sid = series
                    series = sid: series
                else
                    series = deco series    # decouple
                    sid = series.sid

                #series.periodization = per # taken care of by periodization.Series (factory fn)
                me[sid] = per.Series series
                me.$meta.sids.push sid

            # *TODO* like in Series : return a closuring fn that accepts (k, i) || (k, i, val)
            me

        update: (values, conf) ->
            # *UNTESTED*
            me = @
            # *TODO*
            for k, v of values
                #me[k].update v, conf[k] || conf # *TODO*
                me[k](v, conf[k] || conf) # *TODO*
            me

        do-shift: !->   # (silent) !->
            # *UNTESTED*
            me = @
            for sid in me.$meta.sids
                me[sid].do-shift!

        validate-conf: (conf) !->
            # *UNTESTED*
            die 'No study context passed - required!', if not conf.study-context

        sanity-check: !->
            # *UNTESTED*
            return if not IS_DBG    # *TODO* should be me.verbosity < SANITY_CHECKS

            # do checks on series lengths etc.
            die "Wow - didn't see that coming!" if not 47 is 47


    TRM.OHLC = class extends SMatrix
        (id, conf) ->
            me = @
            me.is-ohlc-series-matrix = true   # set V8 optimization on course immediately

            me.meta-series = conf.periodization.Series id + '_meta', null, WHATEVER_SOMETHING
            # *TODO*
            #me.time = values.time || conf.time
            #me.ghost ?= false           # ghost candles are placed in when there are no candles in the underlying market - they are retroactively interpolated
            #me.complete ?= true         # the last candle is updated dynamically if the case is updated before the 'true' end of the candle.
            #me.off-session ?= false     # if the candle is in off-session hours, for sequentiality, this will be discarded deending on visualization options
            #me.early-close-data = {}    # The case can be setup to mark candle as complete before it really is, namely _moments_ before it 'should' close, to get an edge on the market in determining action, especially when it comes to the last of the day/week for non-24/7 markets
            #
            conf.series = <[ time open high low close volume typical spread ]>    # typical spread, for even more detailed info, use "orderbook"
            super id, conf

        update: (values, conf) ->
            # *UNTESTED*
            do-some-additional-checks-to-validate-the-data!
            super values, conf


    TRM.Indicator = class implements StudyContextMixins
    #   This is going to return a function, straight n fair - to be clean to use in calcs
    #
    #   The basis for indicators, to keep them cached etc. They are always connected
    #   to a Periodization. Defaults to 'main periodization' of not specified
    #
    #   An indicator must self determine when it has processed enough data to
    #   consider it self primed for precise use and then report it to the Periodization
    #   or the StudyContext - WHICH ? *TODO* - the trade-functions will not be
    #   activated until such time..
        (id, conf) ->
            me = @
            me.is-indicator = true   # set V8 optimization on course immediately
            me.is-primed = false
            me.id = id

            # add required series-buffers here

        update: (values) ->     # will be THE fn used by the 'user' after creation
            me = @
            if not me.is-primed and me.count > me.back-periods-needed
                me.study-context.report-primed me


    TRM.StatisticalAnalysis = class
    #   # For analysis of Series that is not iterative like indicators - linear regressions for an example

        ->
            me = @
            me.is-statistical-analysis = true   # set V8 optimization on course immediately


















    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # *TODO* move to own file       - 2014-05-18/ORC(10:01)


    # Proof of Concept descendants while building the above


    TRM.analysis.TheilSenEstimator = class extends StatisticalAnalysis
        # Theil–Sen estimator - a better alternative than Linear Regression, concerning market data
        # http://en.wikipedia.org/wiki/Theil%E2%80%93Sen_estimator


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    TRM.CrossMarketDeviationScalper = class extends TradeBot
        init: (ctxt, cb) ->
            # *UNTESTED*
            me = @
            #{add-ind, add-par, add-sec} =~ me
            #param = add-par
            #security = add-sec
            #indicator = add-ind
            {security, parameter, indicator} = me
            {conf, ctxt} = me

            err <-! super ctxt

            # example of dynamic behaviour
            parameter \period, "Scalping period", TRM.INT, [1, 60], 1
            parameter \scalp-market-symbol, "Scalping market", TRM.SYMBOL, [/.*(BTC|XBT).*/], \BITFINEX:BTCUSD

            # example of dynamic behaviour
            parameter \ma-count, "Number of MA's", TRM.INT, [1, null], 5    # *TODO* one could 'reasonable range' and 'actually allowed range' - to help neural networks work within reasonable realms and not waste CPU cycles

            indicator \baseMACD, \MACD, do   # the params below are also added to "params"
                k_length: 47
                f_length: 13

            for i from 1 to conf.ma-count
                indicator \ema_ + i, \EMA, do   # the params below are also added to "params"
                    k_len: 28
                    f_len: 13

            # Elaborate way of doing things:
            #me.on 'error', (err) !-> me.notify 'mail', conf.trader-emails, subject: 'BOT ERROR' + me.name, body: err
            #parameter \notify-errors-to    - *TODO* - should be DEFAULT PARAM FOR ALL BOTS - FILL IN ADRESS AND IT FIRES AWAY! emails yields that, numbers can be entered mixed in the field, then sms will be sent


            # By default, 'security' adds listening to the symbol AND adds it to
            # the requirement to be fulfilled BEFORE 'handle' is called.
            # It's possible to specify that a certain security isn't required to
            # trigger 'handle'. In such a case waiting or discarding is handled
            # manually in the handle method. Ex.: me.sec.huobiH1.await-partial (err, candle) -> do-stuff!
            #
            security \scalpMarket, conf.scalp-market-symbol, (1 |>minutes), time-out: (20 |>seconds), pre-complete: (0 |>minutes)     # offset: 0 is kinda moot - here for examples sake
            security \huobi, \HUOBI:BTCCNY, (1 |>minutes), time-out: (20 |>seconds), pre-complete: (0 |>minutes)
            security \cnh, \FX:USDCNH, (1 |>minutes), time-out: (20 |>seconds), pre-complete: (0 |>minutes)

            me.fresh-errors = 0

        #serialize: ->
            # *UNTESTED*
        handle: (errs, cb) !->
            # *UNTESTED*
            me = @

            if errs and er = errs.find 'security'

                # We could look what securities are failing, and continue anyway, etc...
                me.log 'Error fetching one or more securities'
                if me.fresh-errors++ > 3
                    CLOSE ANY OPEN POSITIONS AS SECURITY MEASURE!
                    me.send-notification "Serious error - three minutes of scalper down time - closes any open positions!"
                    me.fresh-errors = 0
                return


            #r = new R
            # One way of doing it a bit heavier:
            #me.sec.huobi.await-bar {timeout: 20 |>seconds}, r.add!
            #me.sec.cnh.await-bar {timeout: 20 |>seconds}, r.add!
            #
            #<- r.then


    TRM.ForexPerformance = class extends Indicator

        init: (done) !-> # *TODO* context? shouldn't it just be on the class instance at this point?
            me = @
            {security, parameter: param, indicator} = me
            {conf, ctxt} = me

            # *TODO* *EXAMPLE*
            preferred-brokers =
                'OANDA': 1
                'FXCM': 2
                'THE_RYSSARNA': 3
                'GOOFY_SHIT': 98

            err, fxpairs <-! query do
                nt: 'securities_reference'
                $unique: 'symbol'
                tag: 'FX'
                $order-by-int: (nd) -> preferred-brokers[nd._broker!] || 99

            # *TODO* *GOOD* ! a list of brokers, in preferred order, with flag of wish
            # "fastest-delivery" for instance, let's "security" take care of
            # picking security-feed source instead..

            param \emalen1, "EMA 1 length", Number, [1, Infinity], 8  # TRM.INT

            me.fxpairs = []

            for fxpair in fxpairs
                #query 'preferred fxpair, order by exchange by preference'
                secid = fxpair # exchange + '_' + fxpair



                me.fxpairs.push secid  # *TODO* well, they're probably all available in me.securities, buuut.. there might be more, so it's nice to simply be able to sort out the list of pair-names for easy iteration..
                foo = (me.currencies[fxpair.symbol-parts.a] ||= paired: {})
                foo.paired = fxpair.symbol-parts.b

                # *TODO* - and vice versa... hmm. symbol should be stored also,
                # for ordering of the pairs (the relation specification is
                # order-less - just specifies a relation between the currencies
                # exist... )


                security secid, [fxpair._broker!, fxpair._symbol], (1 |>minutes)   # we don't bother with tick-level on all these buggers... the science isn't that exact anyway.. OOOOR?   - 2014-07-13/ORC(18:21)
                # the securities are added, and handle won't be called until all
                # candles have been collected for the first bar, per every bar,
                # so there's no need to wait in any way at this point..

            done err

        handle: (errs, done) !->
            me = @

            # *GOOD* *TODO*
            # While the indicator / study / whatever is still priming (until
            # all sub-indicators etc. has flagged "is-primed", and, as such,
            # itself doesn't have enough data to give a correct value, it's -
            # you guessed it! un-primed! - order/trade-desk functions are not
            # activated until priming is done, ehhh.. until we're at current
            # time bar actually, yeah.)

            # *TODO* perhaps handle should be for main timeframe - always
            # to handle tick-level, add:
            # handle-tick(...)
            # handle-orderbook(...)

            if me.periodization.is-new-candle!  # main periodization candle?
                do-some-shit!
                # *TODO*

                err <-! calculate-performance
                done err

            else
                done!

        handle-orderbook: (errs, data, done) !->


        calculate-performance: (done) !->
            me = @

            {pars} = me

            total-weight = 0
            total-pairs = 0

            me.gather-series 'perf-series'

            for fxpair in me.fxpairs
                ins = me.securities[fxpair]

                # *TODO* when/where is the additional 'fxstrength'-series added
                # to the instrument SMatrix?

                cur_ma = ema 'ma_'+fxpair, ins.close, pars.emalen1
                weight = diff cur_ma(0), cur_ma(1)
                ins.fxstrength.update 0, weight
                total-weight += weight
                total-pairs++

            for fxpair in me.fxpairs
                # *TODO* normalize the weights by totals...
                normalize-them-and-stuff

            for currency in me.currencies
                # *TODO* weight stuff through individual currency perspective stuff..
                indivudual-relations-weighing-and-stuff

            # *TODO* this stage is probably totally unnecessary.. we just calc
            # on the shitty numbers, no problem, and sets am-primed! when all
            # underlying are primed.. (auto-func for such a thing thank you!)
            ser = me.collect-series 'perf-series'
            if not is-primed ser    # all series in ser-collection *not* primed?
                done!
                return

            me.am-primed!

            do-some-summing-that-can-now-be-done-cause-we-have-got-all-the goods
            for blabla
                do-stuff!

            done null   # we now contain all weighting in the SMatrix props named
                        # after the currency-pairs symbol...

            return
