/**
* Created:  2014-07-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

console.log 'QuantFeed mod'

#ubiqs, {AvantAmitter} <- define <[
#    wpn/ubiqs
#    wpn/AvantAmitter
#]>
musts, {AvantAmitter}, broker-mngr <- define <[
    wpn/must-haves
    wpn/AvantAmitter
    BrokerInfo
]>

console.log  'QuantFeed inside'

musts.globalize musts

# # # Public Module Interface # # #
PM = {}

PM.defaults =
    buf-size: 4700  # Good for 6.5 months of 1h data for instance

PM.set-defaults = (conf) !->
    apply PM.defaults, conf

PM.create = (broker-id, security-id, options) ->

    # - Confs-ish
    broker = broker-mngr.get broker-id

    # - Variables
    seconds-sub-index = 0
    last-raw-time = 0
    raw-time-multiplier = (1000000 / broker.time-resolution)
    raw-time-offset = -(broker-utc.offset * 60 * 60 * 1000 * 1000)

    # - Instances
    amitter = new AvantAmitter

    # *TODO* ATM the handle-tick method is called from the outside.
    # Naturally this fn shall set up connection required and decode
    # and normalize the data and time to UTC etc...



    # # # Public Interface # # #
    Feed = {}

    #tick-handler: (time, ask, bid, open, high, low, close, volume, mvolume, spread, leverage, swap-long, swap-short) !->
    Feed.handle-tick = (raw-time, open, high, low, close, volume, mvolume, spread, done) !->
        # Massage the time variable to our liking (0.01ms resolution - to handle 100 000 ticks in a second)
        # STRIKE --- NEXT LINE Out time data type therefor requires 47 bits datatype minimum
        # Our time datatype is in microseconds, therefor requires 51 bits datatype minimum

        # *TODO* this should only be used for indexing purpose in db bascially - unique identifier
        if raw-time == last-raw-time
            seconds-sub-index++
        else
            seconds-sub-index := 0

        last-raw-time = raw-time

        # translate time to:
        #   * microseconds since 1970-01-01
        #   * UTC
        time = (raw-time * raw-time-multiplier) + raw-time-offset + seconds-sub-index    # rawtime in is expected to be in seconds (de facto norm)

        <-! amitter.amit 'tick-internal',   time, open, high, low, close, volume, mvolume, spread
        <-! amitter.amit 'tick',            time, open, high, low, close, volume, mvolume, spread
        done!

    Feed.a-on = amitter~a-on
    Feed.a-off = amitter~a-off

    Feed.get-broker = ->
        return broker

    Feed.destroy = !->
        broker = undefined
        amitter.destroy!


    return Feed


return PM
