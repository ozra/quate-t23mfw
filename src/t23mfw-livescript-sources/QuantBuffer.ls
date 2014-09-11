/**
* Created:  2014-07-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

# *TODO*
# HUOBI API: http://translate.google.com/translate?act=url&depth=1&hl=en&ie=UTF8&prev=_t&rurl=translate.google.com&sl=auto&tl=en&u=http://www.huobi.com/help/index.php%3Fa%3Dmarket_help
# TICKER:
# http://market.huobi.com/staticmarket/ticker_btc_json.js   # just add cache destroyer
# ORDERBOOK: http://market.huobi.com/staticmarket/depth_btc_json.js
# CHART: http://market.huobi.com/staticmarket/btc_kline_001_json.js


<- define []

default-context =
    serial: 1

function create-buffer(size, name, is-internal, styling, periodization, context) # -> # class
    #pow = Math.pow
    ## Normalize buf-size to a power of two   - 2014-07-18/ORC(13:19)
    #for two-power from 8 to 31  # well, we'll probably never have a buffer larger than 16 bits but, you know.. "...ought to be enough for everybody."
    #    if size < (foo = pow(2, two-power))
    #        size = foo
    #        break

    # - Confs
    size++  # space for a undefined at tail
    styling ||= null
    context ||= default-context
    name ||= ('buf-' + (context.serial++) + '-' + size)

    # - Variables
    head = 0    # *TODO* for rapid testing - randomize...

    # - Holders
    data = new Array size

    # # # Public Interface # # #

    Buf = (ix, v) ->
        if IS_DEV
            die 'Buffer overflow!' if ix >= size

        pos = (head + ix)

        if pos > size       # wrap around the rotating buffer
            pos -= size

        if is-def v
            data[pos] = v

        else
            data[pos]

    Buf.is-buffer = true   # set V8 optimization on course immediately

    Buf.inc = !-> head++

    Buf.get-styling = -> styling

    Buf.get-name = -> name

    Buf.destroy = !->
        data = undefined
        periodization = undefined
        size = -1

    return Buf

return create-buffer