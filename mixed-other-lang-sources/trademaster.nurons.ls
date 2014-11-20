<- define []
!->
# 130616/ORC
    me = @
    {nuron} = me
    W.TRM = TRM = me    # *TODO*
    {_L, _D, _E, _W, _T} = createLoggers 'TRM'



    BC.set 'trm.referenceCurrencyRid',  'USD'        # *TODO* default only..

    TRM_FLAGS =
        '0':    'unknown'
        '1':    'marketprice'
        '2':    'limitprice'
        #4:

    nuron 'trm_trade_abstract', {    # *CS2LS*
        abstract: true
        superSilent: true    # 130718/ORC - fuling
        fields:
            tid:    i: 'int'                # markets own id
            ts:        i: 'ts'                    # timestamp
            p:        i: 'double'            # price
            q:        i: 'double'            # quantity
            bs:        i: 'int'                # buy/sell 0=buy,1=sell
            f:        i: 'int'                # flags
            r:        i: 'double'            # rate against norm-currency (currently USD as of # 130718/ORC)
    }#  *CS2LS*
    nuron 'trm_tradedaysum_abstract', {    # *CS2LS*
        abstract: true
        superSilent: true    # 130718/ORC fuling
        fields:
            ts:        i: 'ts'                    # timestamp
            pavg:    i: 'double'            # average price during day

            bidv:    i: 'int'                # bid-volume traded
            askv:    i: 'int'                # ask-volume traded
            mktv:    i: 'int'                # market-price traded volume
            lmtv:    i: 'int'                # limit-price traded volume

            plo:    i: 'double'            # low price during day
            phi:    i: 'double'            # high price during day
            pwlo:    i: 'double'            # weighted low price during day - USING the lowest subset in specified percentage of daily market-size, weighted per trade (if for instance 100BTC was traded, the lowest 3BTC is used to calculate a weighted low from those posts
            pwhi:    i: 'double'            # weighted high price during day - USING the lowest subset in specified percentage of daily market-size, weighted per trade
            wtper:    i:'int'                # weight-window-percentage used for above
            wtmin:    i:'int'                # weight-window, minimum BTC-vol included (adapted to current arbitrage capital)
    }#  *CS2LS*
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    haxBase = 9900

    # 130718/ORC - create all required nurotopes
    for own mid, mkt of TRM.markets
        mkt.rid = mid

        # *TODO*   haxor måste incrementas för cur-pairs också!
        haxOfs = mkt.haxOfs * 2

        IS_DEV && _D 'MARKETSX:', mid
        for own pid, pair of mkt.pairs
            IS_DEV && _D 'PAIRSX:', pid
            pair.rid = pid

            nuron TRM.getTradesNutope(mid, pair.cur1, pair.cur2), {    # *CS2LS*
                i: 'trm_trade_abstract'
                haxor: haxBase + haxOfs
            }#  *CS2LS*
            nuron TRM.getTradesDaySumNutope(mid, pair.cur1, pair.cur2), {    # *CS2LS*
                i: 'trm_tradedaysum_abstract'
                haxor: haxBase + haxOfs + 1
            }#  *CS2LS*




#     HWE.onReady !->
#         TRM.initTradingMachinery()






    #*TODO* bort med boot, fram med GW.when 'hwe.ready' - etc....


    boot ['hwe.ready'], 'trm', (mainCb) !-> #TRM.initTradingMachinery = !->
        mainCb()    # väntar inte på att ovanstående ska lösas!    # 130719/ORC

        return

    boot ['mine.runScrapers'], 'trm.fetchTrades', (cb) !->
        _D 'RUNS FETCHTRADESX'
        TRM.refreshTrades cb
        return

    boot [], 'trm.fetchOrders', (cb) !->
        _D 'RUNS FETCHTRADESX'
        TRM.refreshOrders cb
        return

    TRM.loopMarkets = (fn) !->
        for own mid, mkt of TRM.markets
            IS_DEV && _D 'MARKETSX:', mid
            for own pid, pair of mkt.pairs
                IS_DEV && _D 'PAIRSX:', pid
                fn mkt, pair



        return

    TRM.refreshTrades = (cb) !->
        rrFetch = new Rr
        TRM.loopMarkets (mkt, pair) !->
            TRM.markets[mkt.rid].fetchTrades pair.cur1, pair.cur2, tidVarKey, rrFetch.sow !->
                _D 'done'

        rrFetch.reap !->
            delay 15*1000, TRM.refreshTrades    # run every 15 seconds!    # 130916/ORC -
            cb && cb()
        return

    TRM.refreshOrders = (cb) !->
        rrFetch = new Rr
        TRM.loopMarkets (mkt, pair) !->
            TRM.markets[mkt.rid].fetchOrders pair.cur1, pair.cur2, rrFetch.sow !->
                _D 'done'

        rrFetch.reap !->
            delay 15*1000, TRM.refreshOrders    # run every 15 seconds!    # 130916/ORC -
            cb && cb()
        return




    request = require 'request'

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        # *TODO* 'binda' in dessa i metoder på varje market..
    TRM.getTradesNutope = (marketId, cur1, cur2) ->
        return "trm_trade_{{marketId}}_{{cur1}}_{{cur2}}"

    TRM.getTradesDaySumNutope = (marketId, cur1, cur2) ->
        return "trm_tradedaysum_{{marketId}}_{{cur1}}_{{cur2}}"


    TRM.catchupDaySums = (market, cur1, cur2) !->
        lastDateKey = "trm.daysums.kapiton.{{cur1}}_{{cur2}}.lastCalced"

        lastDate = getVarVal lastDateKey

        nutope = TRM.getTradesDaySumNutope market, cur1, cur2

    #     NM.query
    #         nt: nutope
    #         $orderby: 'ts, tid'
    #
    #     , (er,nds) !->

    # PSEUDO *TODO*
    #     loopa alla dygn upp t.om. IGÅR (eller idag också, mark 'incomplete'
    #         selecta alla trade-poster mellan 00:00:00.000 - 23:59:59.999 för dygnet
    #             loopa igeom poster
    #                 gör summeringar (se sumTrades)
    #             skapa dag-post
    #
    #
        # done

        return


    TRM.sumTrades = (market, cur1, cur2, days) !->
        _D 'query last five days'
        __profiling_178 = now()

        nutope = TRM.getTradesNutope market, cur1, cur2


        # *TODO* *WTF* - why is an open query done first, and then sub-selection???

        NM.query {    # *CS2LS*
            nt: nutope

        }, !->
            NM.query {    # *CS2LS*
                nt: nutope
                ts:
                    $gte: now() - days * day
                    $lte: now()
                $orderby: 'ts, tid'

            }, (er, nds) !->
                _D 'query done, count = ', nds.length
                (", took "+(now() - __profiling_178)+"ms")

                diver = 1 / nds.length
                avgrate = sekvol = vol = avgp = avgp2 = 0


                __profiling_204 = now()

                for nd in nds
                    avgp +=        nd.get('p') * diver
                    avgp2 +=    nd.get('p')
                    vol +=        nd.get('q')
                    sekvol +=    nd.get('p')*nd.get('q')
                    avgrate+= nd.get('r') * diver

                avgp2 /= nds.length

                _D 'SEK: avgp', avgp, 'avgp2', avgp2, 'total vol', vol, 'total sek', sekvol
                _D 'USD: avgp', avgp*avgrate, 'avgp2', avgp2*avgrate, 'total vol', vol, 'total sek', sekvol*avgrate
                (", took "+(now() - __profiling_204)+"ms")

                return
            return
        return






    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # SPECIFIC MARKETS
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


    minute = 60*1000
    hour = 60*minute
    day = 24*hour



    TRM.exchanges =
        swedbank:
            interval: '13:00'

    # *TODO* ta ifrån webminer och flytta ut hit! Webminer ska ENBART ha underlättande funktioner för att extrahera det man vill!
    #    VISA-Europa-kort: (kräver post-sidor framåt - extrahera post-info) http://www.visaeurope.com/en/cardholders/exchange_rates.asp
    #    Mastercard: (kräver js-based val på sida, leder till XHR, som ej går att köra utanför domain) https://www.mastercard.com/global/currencyconversion/index.html
    #    Handelsbanken: (use, rakt av - ren lista)  http://www.handelsbanken.se/shb/kurslista.nsf/vvalutakurser?OpenView&Count=100
    #    icabanken: (samma som mastercard? +0% fee)


    TRM.markets =
        kapiton:
            name:'Kapiton'
            haxOfs: 0
            pairs:
                SEK_BTC:
                    cur1:'SEK'
                    cur2:'BTC'
                    estimatedTradeTime:    2 * hour
                    lastTid:            0
                    trades:                []
                    lastOrderId:        0
                    orderbook:             []

            cur:
                SEK:
                    withdrawTime:         1 * day
                    depositTime:        1 * day
                BTC:
                    withdrawTime:         1 * hour
                    depositTime:        1 * hour

        mtgox:
            name:'Mt.Gox'
            haxOfs: 1
            pairs:
                USD_BTC:
                    cur1:'USD'
                    cur2:'BTC'
                    estimatedTradeTime:    2 * hour
                    lastTid:            0
                    trades:                []
                    lastOrderId:        0
                    orderbook:             []
            cur:
                USD:
                    withdrawTime:         21 * day
                    depositTime:        1 * day
                BTC:
                    withdrawTime:         1 * hour
                    depositTime:        1 * hour

        btce:
            name:'btc.e'
            haxOfs: 2
            pairs:
                USD_BTC:
                    cur1:'USD'
                    cur2:'BTC'
                    estimatedTradeTime: 2 * hour
                    lastTid:            0
                    trades:                []
                    lastOrderId:        0
                    orderbook:             []
            cur:
                USD:
                    withdrawTime:         14 * day
                    depositTime:        1 * day
                BTC:
                    withdrawTime:         24 * hour
                    depositTime:        1 * hour

        bitstamp:
            name:'BitStamp'
            haxOfs: 3
            pairs:
                USD_BTC:
                    cur1:'USD'
                    cur2:'BTC'
                    estimatedTradeTime:    2 * hour
                    lastTid:0
                    trades:                []
                    lastOrderId:        0
                    orderbook:             []
            cur:
                USD:
                    withdrawTime:         14 * day
                    depositTime:        1 * day
                BTC:
                    withdrawTime:         1 * hour
                    depositTime:        1 * hour



    TRM.markets.mtgox.fetchOrders = (cur1, cur2, cb) !->
        # *TODO*
        cb()
    TRM.markets.mtgox.fetchTrades = (cur1, cur2, tidVarKey, cb) ->
        # *TODO*
        cb()

    TRM.markets.btce.fetchOrders = (cur1, cur2, cb) !->
        # *TODO*
        cb()
    TRM.markets.btce.fetchTrades = (cur1, cur2, tidVarKey, cb) ->
        # *TODO*
        cb()

    TRM.markets.bitstamp.fetchOrders = (cur1, cur2, cb) !->
        # *TODO*
        cb()
    TRM.markets.bitstamp.fetchTrades = (cur1, cur2, cb) ->
        # *TODO*
        cb()

    TRM.markets.kapiton.fetchOrders = (cur1, cur2, cb) !->
        request "https://kapiton.se/api/0/orderbook", (err, res, body) ->
            if err
                dieif 'MUDDAFUCKIN ERR'

            body = dec body


            cb()
        return
    TRM.markets.kapiton.fetchTrades = (cur1, cur2, cb) ->
        #tidKey = "trm.trades.kapiton.#{cur1}_#{cur2}.lastTid"
        #currTid = getVarVal(tidKey,0)
        nutope = TRM.getTradesNutope 'kapiton', cur1, cur2
        nt = NM.getNt nutope

        # *TODO* VILKEN rate-source som används är upp till varje market beroende på troligast växlingsagent. Valutan är gemensam
        rate = 1 / (minedData.swedbankRates.EUR / mminedData.unicreditRates[BC.get('trm.referenceCurrencyRid')])    # unicredits iväxelkurser är emot EUR, så vi måste räkna om den till att stå emot SEK, med swedbanks SEK/EUR-rate.            # (minedData.swedbankRates.EUR / minedData.unicreditRates.USD)

        # *TODO* GÖR QUERY PÅ HÖGSTA TID OCH LIMIT 1, AIGHT! INGE JÄVLA STORE SOM SÅ HÄR. KEEP WITH THE DATA AS A _SOURCE_!

        QND {    # *CS2LS*
            nt: nutope
            $limit: 1
            $orderby: 'tid desc'
        }, (err, nd) !->

            currTid = nd?.get('tid') || 0

            _D 'mem before req', process.memoryUsage()

            request "https://kapiton.se/api/0/trades/?since='+currTid+'", (err, res, body) ->
                _D 'mem before dec', process.memoryUsage()
                body = dec body
                _D 'mem after dec', process.memoryUsage()
                _D 'count of trades:', body.length

                # *TODO* make sure there are no records with the tid in the storage!

                for rs in body
                    _D rs.tid
                    nd := new nt {    # *CS2LS*   # *CS2LS* *SHADOW*:"nd".
                        tid:    rs.tid
                        ts:        rs.date*1000
                        p:        rs.price
                        q:        rs.amount
                        bs:        -1                            # buy/sell 0=buy,1=sell,-1=unknown
                        f:        0                                # flags
                        r:        rate
                    }#  *CS2LS*
                    if rs.tid > currTid
                        currTid := rs.tid # *CS2LS* *SHADOW*:"currTid".


                _D 'mem after nutope-creation', process.memoryUsage()
                body = null
                rs = null

                #_D 'calls setVar ', tidKey, ',', currTid
                #setVar tidKey, currTid

                NM.sync !->
                    _D 'mem after NM.sync', process.memoryUsage()
                    cb()

                return



        return
