Ripper, DB <- define <[ Ripper mysql-dsi ]>
out = {}

normalize-tick = (data) ->
    /*

    *TODO*
    * validate time depending on offset of market (double check for gap and see if
    it is in the expected place (fri - sun 23 or so...).
    * offset it correctly to UTC
    if no volume, spread, mvolume data etc. interpolate/extrapolate, set flags

    */

    return tick

store-in-db = (data, done) !->
    /*

    *TODO*
    check if it already is in db at t, if so, compare them for differences,
    like 'ghost'-flag, 'borrowed'-flag, etc, spit out error if diffs,
    otherwise, skip changing with just a warning.

    */

    done!

strategy-quant-import = (location, broker, security, done) !->

    /*

    *TODO*
    load-dir-files-or-whatever
    arr = split/cut/slice-file-whatever

    */

    rr = Ripper!

    for entry in arr
        tick = normalize-tick entry
        store-in-db tick, rr.add!

    err, ret <-! rr.then
    done!

out.import-ticker-data = (location, resource-type, broker, security, done) !->
    /*
    # TICK DATA SOURCES #
    http://www.strategyquant.com/tickdatadownloader/
    http://eareview.net/tick-data/download-free-tick-data
    http://www.forextester.com/data/FullDataList.html
    http://www.forextester.com/data/datasources.html
    http://www.fxdd.com/us/en/forex-resources/forex-trading-tools/metatrader-1-minute-data/

    * HUOBI API: http://translate.google.com/translate?act=url&depth=1&hl=en&ie=UTF8&prev=_t&rurl=translate.google.com&sl=auto&tl=en&u=http://www.huobi.com/help/index.php%3Fa%3Dmarket_help
    TICKER:
    http://market.huobi.com/staticmarket/ticker_btc_json.js   # just add cache destroyer
    ORDERBOOK: http://market.huobi.com/staticmarket/depth_btc_json.js
    CHART: http://market.huobi.com/staticmarket/btc_kline_001_json.js

    * OK-COIN:
    https://www.okcoin.cn/about/webSocket.do

    */

    switch resource-type
    | 'strategy-quant'  => strategy-quant-import location, broker, security, done
    | 'csv'     => general-csv-import location, broker, security, done
    | _     => die 'Unknown data-source in import-ticker-data!'

out
