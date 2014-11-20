/**
* Created:  2014-07-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

console.log 'QuantPeriod mod'

musts, create-QuantBuffer, TU, {AvantAmitter}, Ripper <- define <[
    wpn/must-haves
    QuantBuffer
    wpn/timely
    wpn/AvantAmitter
    wpn/Ripper
]>

console.log  'QuantPeriod inside'

musts.globalize!
globalize TU

# # # Public Module Interface # # #
PM = {}

PM.defaults =
    buf-size: 4700  # Good for 6.5 months of 1h data for instance

let
    period-match-re = /^(\d*)(\w?)$/

    PM.dissect-period-code = (period) ->
        ret = period.match period-match-re

        qty = ret.1 || 1
        unit = ret.2 || 'm'

        duration = switch unit
        | 'T', 'tick'   => 0
        | 'minutes'     => qty |> minutes
        | 'H'           => qty |> hours
        | 'D'           => qty |> days
        | 'W'           => qty |> weeks
        | 'M'           => qty |> months
        | _             => null

        return null if duration is null

        return [unit, qty, duration]

dissect-period-code = PM.dissect-period-code

PM.set-defaults = (conf) !->
    apply PM.defaults, conf

PM.create = (quant-feed, period, conf) -> # We use factories for everything where the instance count is reasonably low (because of the additional memory required), ie not for quant-data etc. ofc. - 2014-07-18/ORC(22:48) = class implements StudyContextMixins
    # A periodization with current period tracking - for bots with several periodizations and
    # perhaps tick update, but working on higher timeframes, to maintain series
    # length integrity
    #
    # A periodization collects all external data (within timeout limits) before
    # calling handle(). The StudyContexts handle is called then, from the
    # periodizations handle(). SC's handle is therefor called several times
    # from.

    # - - confs
    conf ||= {}

    if is-str period
        period = dissect-period-code period .2

    if IS_DEV
        if period is null
            throw new Error "Unknown periodization time!"

        if ((v = 1440 / period) != Math.floor(v)) &&
            ((v = period / 1440) != Math.floor(v))

            out = "The periodization \"" + period + "\" (gave " + v + ") is not evenly divided over a day - we need that at the moment!\n Choose from:"
            for i from 1 to 1440
                v = 1440.0 / i
                v2 = 1440.0 / i
                if v == Math.floor v
                    out += '\n' + i + ' giving: ' + v + ' periods/d'

            throw new Error out

    default-buf-size = conf.buf-size || PM.defaults.buf-size
    offset = conf.offset || 0    # well, simply "midnight", midnight point can be offset in minutes, to trig one minute earlier than the rest of the world for instance
    period-in-micros = period * 1000000
    ghost-bars = conf.ghost-bars
    ghost-bars ?= true    # We don't want to fail geometry!



    ghost-bars = false  # *TODO* not implemented yet!




    # - - various variables
    total-bars = 0
    ghost-bars = 0  # if we had to fake any cached-bars for geometry's sake.
    cached-bars = 0
    last-bar-time = 0

    # - - holders
    amitter = new EventEmitter

    buffers = [
        time-buf = create-QuantBuffer default-buf-size
        open-buf = create-QuantBuffer default-buf-size
        high-buf = create-QuantBuffer default-buf-size
        low-buf = create-QuantBuffer default-buf-size
        close-buf = create-QuantBuffer default-buf-size
        volume-buf = create-QuantBuffer default-buf-size
        mvolume-buf = create-QuantBuffer default-buf-size
        spread-buf = create-QuantBuffer default-buf-size
        flag-buf = create-QuantBuffer default-buf-size

    ]    # buffers of this periodization


    # This might be called several time in one tick because of faked cached-bars in low volume markets - even though it's faked data, it's often desirable to maintain market geometry for analysis /ORC
    handle-new-bar = (us-time, open, high, low, close, volume, mvolume, spread, flags, done) !->
        rr = Ripper!

        if cached-bars > 1  # the first bar is incomplete and shouldn't be handled
            amitter.amit 'bar-close', PER, rr.add!    # I like closed bars - if I scored /ORC

        <-! rr.then
        total-bars++
        cached-bars := min default-buf-size, total-bars

        # update buffers...
        for buffers => ..inc!   #[..inc! for buffers]   # This does the exact same thing, the active one just being a bit more clear on what happens imh-procedural-o

        last-bar-time += period #-in-micros

        # Initiate new bar in the standard OHLC-buffers
        time-buf 0, last-bar-time
        open-buf 0, open
        high-buf 0, high
        low-buf 0, low
        close-buf 0, close
        spread-buf 0, spread
        flag-buf 0, flags

        <-! amitter.amit 'bar-open', PER   # I always like open bars /ORC
        done!

    # # # Public Interface # # #
    PER = {}


    # *TODO* it would be preferable if a tick-source could be passed in the conf
    # so we could privatize that shit!  - 2014-07-19/ORC(00:35)


    #tick-handler: (time, ask, bid, open, high, low, close, volume, mvolume, spread, leverage, swap-long, swap-short) !->
    quant-feed.a-on 'internal-tick', PER.tick-handler = (us-time, open, high, low, close, volume, mvolume, spread, done) !->



        # *TODO*
        console.log 'in Periodization on internal-tick - returns early for now'



        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

        # Interpolate / extrapolate / cheat when missing information
        flags = 0

        # new bar?
        if last-bar-time is 0   # very first tick and bar..
            # *TODO* flag for 'incomplete bar'
            last-bar-time = Math.floor (us-time / (1440 * 60))

        time-delta = time - last-bar-time - period-in-micros

        # asynchronous while loop
        while-fn = (done) !->
            if time-delta >= period-in-micros # have we missed one or more cached-bars?!
                return done!

            time-delta -= period-in-micros
            last-bar-time += period

            if ghost-bars
                # *TODO*
                calculate fake-times and shit!

                <-! handle-new-bar new-bar-time, fake-open, fake-high, fake-low, fake-close, fake-volume, fake-mvolume, fake-spread, fake-flags
                while-fn done
            else
                while-fn done

        <-! while-fn

        if time-delta > 0
            <-! handle-new-bar bar-time, open, high, low, close, volume, mvolume, spread, flags
            done!

        else    # update last, open, bar
            high-buf 0, max high, high-buf 0
            low-buf 0, min low, low-buf 0
            close-buf 0, close
            volume-buf 0, volume + volume-buf 0
            mvolume-buf 0, mvolume + mvolume-buf 0  # tick count - so this should be the same as + 1 if everythings what it should be...
            spread-buf 0, max spread, spread-buf 0
            # *TODO* think about this logic:
            flag-buf 0, flags || flag-buf 0
            <-! amitter.amit 'bar-update', PER
            done!

    PER.on = amitter~a-on
    PER.off = amitter~a-off

    PER.buffer = (size) ->
        buffers.push buf = create-QuantBuffer size || default-buf-size
        return buf

    PER.get-shift-from-time = (time) ->

        #*TODO*
        #if time is in seconds then do some shit
        #calculate the index from the time
        return the index


    return PER


return PM
