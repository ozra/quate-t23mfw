/**
* Created:  2014-07-17
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

_D = console.log

console.log 'grapewire lt mod'

cpo, musts, DBG, ache, DataTyping, transports, {EventEmitter} <- define <[
    wpn/cpo
    wpn/must-haves
    wpn/DBG
    wpn/Ache
    wpn/DataTyping
    grapewire-lt-transports
    events
]>

console.log 'grapewire lt mod inside'

musts.globalize musts

_D "DataTyping", DataTyping

globalize DataTyping

# # # PUBLIC GW-MOD INTERFACE # # #
GW = {}

GW.create-channel = (conf) ->
    if IS_DEV
        if !conf
            die 'NO CONF FOR GWlt-instance-id-creation! WTF!'

    # - class instances
    transport = null
    #emitter = new EventEmitter


    # - Variables
    msg-serial = 1

    # - Holders
    at-destroy = []
    subbers = {}
    rcvers = {}

    # matches against a list, the format of a list-entry must be
    # [adr(, data, or whatever)] (adr in index 0)

    # *UNTESTED* adress-matchers
    #
    #match-adr = (list, adr-matching) -> # if everything in matching matches list-item, we're ok
    #    for v, i in list
    #        adr = v[0]
    #
    #        for own k,v of adr-matching
    #            continue if (not is-def(adr[k])) || adr[k] isnt adr-matching[k]
    #            return i
    #
    #    return null
    #
    #match-adr-reverse = (list, adr-matching) -> # if everything in list-item matches props in matching, we're ok
    #    for v, i in list
    #        adr = v[0]
    #
    #        for own k,v of adr
    #            continue if (not is-def(adr-matching[k])) || adr[k] isnt adr-matching[k]
    #            return i
    #
    #    return null

    route-message-package = (letter) !->
        _D 'route message package' #, letter

        adr = letter.adr

        #_D 'Cmd, msgid:', letter.topic, adr.msg-id #, 'msg', console.dir(letter.msg) #, 'function:', subbers, rcvers

        if fns = subbers[letter.topic]
            for fn in fns
                #_D 'calls sub', fn.to-string!
                #try
                fn letter
                #catch e
                #    _E 'Subscribing function failed. msg:', e.message, topic, msg-id, msg #, 'function:', fn.to-string!

        if fn = rcvers[adr.msg-id]
            delete rcvers[adr.msg-id]
            #try
            fn letter
            #catch e
            #    _E "Rcv'ing function failed. msg:", e.message, topic, msg-id, msg #, 'function:', fn.to-string!

    do-send = (adr, topic, msg) ->
        #if i = match-adr-reverse transmission-lines, adr
        #    _D 'matched transmission line'
        #else
        #    die 'couldnt match transmission line for', adr, msg

        _D 'do-send', adr, topic, msg

        letter =
            sent: now!
            from:
                msg-id: msg-serial++
            adr: adr
            topic: topic
            msg: msg

        transport.transmit letter

        return letter.from.msg-id

    # # # PUBLIC INTERFACE # # #
    GWch = {}

    # public vars
    GWch.is-working = 0
    GWch.is-destroyed = false

    # public methods
    GWch.reply = (adr, topic, msg) ->
        adr = cpo(adr.from || adr)   # it's the msg-id we're after...
        if not msg
            msg = topic
            topic = 'reply'
        return do-send adr, topic, msg

    GWch.send = (adr, topic, msg) ->    # *TODO* allow wait for delivery?
        adr = cpo(adr.from || adr)
        delete adr.msg-id  # we don't keep this in send operations

        if not msg
            msg = adr-modifications
            adr-modifications = null

        else if is-str adr2
            letter.topic = adr-modifications

        else
            for own k,v of adr-modifications
                adr[k] = v

        return do-send adr, topic, msg

    # every subscriber gets the message, it's not consumed by first reveiver
    GWch.sub = (topic, fn) !->
        type Str topic; type Fn fn

        foo = (subbers[topic] ||= [])
        foo.push fn

    GWch.unsub = (topic, fn) !->
        type Str topic; type Fn fn

        if not foo = subbers[topic]
            return
        while i = foo.index-of fn
            foo.splice i, 1, 0

    # a rcv'ed message is consumed
    GWch.rcv = (msg-id, fn) !->
        type Str msg-id; type Fn fn

        if IS_DEV
            if rcvers[msg-id]
                die 'Some-ones already listening for message with id:', msg-id

        rcvers[msg-id] = fn
        #rcvers[msg-id] = !->
        #    delete rcvers[msg-id]
        #    fn.apply this, arguments


    GWch.destroy = (done) !->
        GWch.is-destroyed = true
        <-! ache at-destroy, (destructor, next) -> destructor next
        done!

    GWch.is-working++

    let
        if ! (transport-obj = transports[conf.transport.type])
            die 'Unknown transport-type!! : ', conf.transport.type

        transport-conf = cpo conf.transport
        transport-conf.instance-id = conf.instance-id
        transport-conf.incoming-handler = route-message-package

        _D 'call transport.create with', transport-conf

        # *TODO* create should ALWAYS be synchronous, instead we use on('ready'), well or cb, but the instance should be RETURNED - NOT CALLBACKED
        transport := transport-obj.create transport-conf, !->
            GWch.is-working--

        #transport.on-data route-message-package
        at-destroy.push (next) !-> transport.destroy next

    return GWch

return GW
