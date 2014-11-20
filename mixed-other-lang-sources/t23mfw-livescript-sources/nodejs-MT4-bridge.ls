/**
* Created:  2014-07-17
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

_D = console.log
_E = console.error

console.log 'MT4-bridge mod'

GWLT, \
QuantBot, \
musts, \
broker-mngr, \
fs \
<- define <[
    grapewire-lt
    QuantBot
    wpn/must-haves
    BrokerInfo
    fs
]>

console.log 'MT4-bridge mod inside'

musts.globalize musts



/*
OP_BUY
0
Buy operation
OP_SELL
1
Sell operation
OP_BUYLIMIT
2
Buy limit pending order
OP_SELLLIMIT
3
Sell limit pending order
OP_BUYSTOP
4
Buy stop pending order
OP_SELLSTOP
5
Sell stop pending order
*/

let
    brokers = fs.read-file-sync './lib/cfg/brokers.json.json'   # *TODO* lsc bug!!
    try
        brokers = JSON.parse brokers
    catch err
        die "Couldn't read brokers configuration from \"cfg/brokers.json\"!"

    for own k, v of brokers => broker-mngr.add k, v

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#// Transport level

#// Bot level
MT4FN_INITBOT = "71"
MT4FN_REINIT = "72"   # If the NJS-BOT crashes, it re-fetches init-info
MT4FN_DEINIT = "73"
MT4FN_MARKETINFO = "74"

#// Tick level
MT4FN_TICK = "1"
MT4FN_TICK_DONE = "2"
MT4FN_GETORDERS = "3"
MT4FN_PLACEORDER = "4"
MT4FN_CLOSEORDER = "5"

MT4FN_SECURITY = "9"
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


mt4-transport-base-path = '/run/t23m-RAMF-RPC/'

transport-conf =
    type: 'RAMF'
    path: mt4-transport-base-path


mt4FW-create-bot-context = (mt4fw-gwmq-broker, pkg) !->
    say 'mt4FW-create-bot-context'

    [
        bot-name,  account-company, account-name
        symbol, period, is-testing, is-opting
        p1,p2,p3,p4,p5,p6,p7,p8

    ] = pkg.msg

    broker-symbol = broker-mngr.get-id-by-name account-company
    mt4-bot-instance-token = [symbol, period, broker-symbol, account-name, bot-name, is-testing, is-opting].join('_').replace /[\s,.-]/g, '-'
    debug-buffers-count = 0

    say 'mt4-bot-instance-token', mt4-bot-instance-token

    gw-bot = GWLT.create-channel(
        transport: transport-conf
        instance-id: mt4-bot-instance-token
    )

    _D 'create QuantBot'

    bot-src-code = '' # *todo*  -  load-bot-code bot-path + '/' + bot-name + '.js'

    qbot = QuantBot.create do
        is-testing: +is-testing
        is-opting: +is-opting
        period: +period
        security: symbol
        bot-logic: bot-src-code

    _D "QuantBot created done, tie GW's"

    qbot.a-on \init, (...args, done) !->
        _D "send MT4INIT with new service token"

        # uses the njsmt4-fw-gw-broker, cause that's the only route MT4 knows so far..
        mt4fw-gwmq-broker.reply pkg, [mt4-bot-instance-token, debug-buffers-count]

        _D 'after send'
        _D 'call done', args, done

        done!

    #gw-bot.rcv MT4FN_DEINIT, (msg) ->       # specific instance
    gw-bot.sub MT4FN_DEINIT, de-init-subber = (msg) !->       # specific instance
        _D 'Received DEINIT message - destroys bot'

        msg = msg.msg

        qbot.destroy!
        #gw-bot.unsub MT4FN_DEINIT, de-init-subber
        #gw-bot.unsub MT4FN_TICK, tick-subber
        <-! gw-bot.destroy!
        _D 'GrapeWire-LT channel destroyed'


    gw-bot.sub MT4FN_TICK, tick-subber = (letter) !->         # specific instance
        _D 'Received TICK'
        msg = letter.msg

        <-! qbot.main-feed.handle-tick +msg.0, +msg.1, +msg.2, +msg.3, +msg.4, +msg.5, +msg.6, +msg.7

        _D 'reply tick msg!' #.yellow.bold, letter

        gw-bot.reply letter, [] # some-buffers

    #RPC.add 'security', (params) ->     # specific instance
    #    # an ordered security, the bar of the same pos as the last bar in 'main' periodization

# # # Public Interface # # #
MT4 = {}

MT4.create = (conf) ->
    gwmt4 = GWLT.create-channel(
        transport: transport-conf
        instance-id: 'T23M-BROKER'
    )

    say 'sub to MT4FN_INIT event'
    gwmt4.sub MT4FN_INITBOT, (pkg) !->         # this one is on the T23M/portal level - creating / tying an instance, and returns a token for identifying.
        say 'sub got MT4FN_INITBOT'

        mt4FW-create-bot-context gwmt4, pkg

return MT4
