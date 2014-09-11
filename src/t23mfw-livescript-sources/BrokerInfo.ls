/**
* Created:  2014-07-25
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

console.log 'BrokerInfo mod'

#ubiqs, {AvantAmitter} <- define <[
#    wpn/ubiqs
#    wpn/AvantAmitter
#]>
musts, {AvantAmitter} <- define <[
    wpn/must-haves
    wpn/AvantAmitter
    BrokerInfo
]>

console.log  'BrokerInfo inside'

musts.globalize musts

# # # Public Module Interface # # #
mod = {}

brokers = {}
broker-ids-by-name = {}

mod.get = (broker-id) ->
    return broker-id if is-obj broker-id
    return brokers[broker-id]

mod.get-id-by-name = (broker-name) ->
    brokers-ids-by-name[broker-name]

mod.add = (broker-id, broker-conf) !->
    brokers[broker-id] = c = {}
    brokers-ids-by-name[c.name] = broker-id

    c.is-broker = true
    c.name = broker-conf.name || die "Brokers 'name' is missing in conf", broker-conf
    c.time-resolution = broker-conf.time-resolution || die "Brokers 'time-resolution' is missing in conf", broker-conf
    c.utc-offset = broker-conf.utc-offset || die "Brokers 'utc-offset' is missing in conf", broker-conf


    #name: "Distel Enterprise (BTC-e)"
    #security-feeds: <[ MT4 BTCE_API ]>
    #trade-desks:  <[ MT4 BTCE_API ]>
    #time-resolution: 1
    #utc-offset: +3


