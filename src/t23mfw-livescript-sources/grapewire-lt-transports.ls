/**
* Created:  2014-07-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/



/*

# *TODO*
* uppstädning _ska_ skötas korrekt
    * lägg till ErrorDomain / crash-hantering - anropa destroy chains från den.


*/



colors, musts, DBG, ache, {exec, spawn}, fs <- define <[
    colors
    wpn/must-haves
    wpn/DBG
    wpn/Ache
    child_process
    fs
]>

musts.globalize!

#Terror = (t) -> throw new Error t
#!define Terror(t) (throw new Error (t))

transports = {}



RAMF = transports.RAMF = {}

# * Transport Package Format:
# * Index 0 - 4 = adr, 5 - len-2 = msg, len-1 = check-sum
# * ASCII
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# *SENT-TS    *FROM             *TO               *TOPIC  *MSG      *CHECK-SUM
# timestamp,  mid, instance-id  mid, instance-id, topic,  a,b,c,d,  -47
# STIKE - no instance-id    - - - - mid, instance-id,  mid, instance-id,  topic,   a,b,c,d,   -47
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#GW_TRANSPORT_CONNECT = "4747"
#GW_TRANSPORT_CONNECT_ACK = "4748"
#GW_TRANSPORT_UPGRADE_INSTANCE_ID = "4751"

transmitFilePostfix = "_ABTRANSPORT"

_D = console.log
_ALERT = (...args) !->
    args.unshift '\n\nXXXXXXXXXX\n\n'.red.bold
    args.push '\n\nXXXXXXXXXX\n\n'.red.bold
    _D.apply this, args

RAMF.create = (conf, done) ->    # construct, saying "this is an asynchronous factory"

    _D 'transports.RAMF::create - conf'.yellow.bold, conf

    # - Confs -
    instance-id-path = conf.path + conf.instance-id + '/'
    machine = null

    # - Instances -
    notif = null
    gwlt-incoming-handler = (conf.incoming-handler || throw new Error  "No incoming-handler supplied!")

    # - Variables -
    is-destroyed = false

    # - Holders -
    write-locks = {}
    keep-cool-locks = {}    # Workaround for strange close_write events on files that have been deleted...  - 2014-07-21/ORC(00:05)
    out-queue = []
    in-queue = []

    # - Private functions -

    compare-write-locks = (path) ->
        for own k of write-locks
            #_D 'tests write lock', k, 'vs', path
            if 0 is path.index-of k # we can have files with extensions like _TMP that should match to the same base, therby index-of /ORC
                return path
        return null

    compare-keep-cool-locks = (path) ->
        for own k of keep-cool-locks
            #_D 'tests write lock', k, 'vs', path
            if 0 is path.index-of k # we can have files with extensions like _TMP that should match to the same base, therby index-of /ORC
                return path
        return null

    # # RECEIVING # #
    handle-RAMF-event = (event, package-path) !->
        switch event
        | \DELETE
            #_D 'handle-RAMF-event - is delete event', package-path

            _D 'DELETE: write-locks is', write-locks, 'keep-cool-locks is', keep-cool-locks

            if a-transport-file = compare-write-locks package-path
                    _D 'got DELETE event for a transmission-file - ready to write again!'.red.bold, package-path.yellow.bold
                    delete write-locks[a-transport-file]
                    keep-cool-locks[a-transport-file] = true    # we get signals of CLOSE_WRITE on the file AFTER it's been deleted. dunno why. so let's do it like this for now...  - 2014-07-21/ORC(00:04)

                    _D 'DELETE, post move to keep-cool: write-locks is', write-locks, 'keep-cool-locks is', keep-cool-locks

                    process-out-queue!
                    return
            #_D 'an ordinary cleanup delete event, nothing more'

        | \CLOSE_WRITE
            if compare-write-locks package-path
                _D 'handle-RAMF-event - close_write: was transmission-file - ignore it', package-path
                return

            if compare-keep-cool-locks package-path
                _ALERT 'handle-RAMF-event - close_write: was transmission-file - THAT IS ALREADY DELETED! - ignores it'.red.bold, package-path.yellow.bold
                return

            if 0 <= package-path.index-of "_ABTRANSPORT"
                _ALERT 'handle-RAMF-event - close_write: was GENERIC transmission-file - ignore it', package-path
                return

            _D 'handle-RAMF-event - close_write: fresh message:'.green.bold, package-path.yellow.bold
            err, data <- fs.read-file package-path, encoding: 'ascii' # 'utf16le' # 'utf8'

            if err
                _ALERT 'write-locks is', write-locks, 'keep-cool-locks is', keep-cool-locks
                throw new Error  err.message||err + ' ' + package-path

            ## WTF is this? Microsoft BOM/BOF? Let's remove it before it infects us!
            #pos = 0
            #while 65533 is data.char-code-at pos
            #    pos++
            #data = data.substr pos

            #_D 'read data:', data.char-code-at(0), data.char-code-at(1), data.char-code-at(2), data
            handle-RAMF-package data

            _D 'WILL delete the consumed recieved msg package'.blue.bold, package-path.yellow.bold
            <-! fs.unlink package-path
            _D 'done w deleted the consumed recieved msg package'.red.bold, package-path.yellow.bold

        | _
            47 == 47
            #_D 'handle-RAMF-event - Non accepted event:', event, package-path

    handle-RAMF-package = (data) ->
        _D 'handle-RAMF-package - decodes data:', data

        pkg = data / ','
        check-sum = pkg.pop!

        sent = +pkg.shift!
        pkg.shift!  # separator

        # decode address-parts
        from-to = [{}, {}]
        adr-parts = <[ msgId instanceId unitId unitImplId machineId ]>

        for v in from-to
            part-ix = 0
            while (foo = pkg.shift!) isnt "__"
                v[adr-parts[part-ix++]] = foo

            #pkg.shift!  the separator is taken care of in the match above # separator

        topic = pkg.shift!
        pkg.shift!  # separator

        letter =
            sent: sent
            from: from-to.0
            adr: from-to.1
            topic: topic
            msg: pkg

        if check-sum isnt "-47"
            throw new Error  "Checksum isnt -47 for", letter.topic, letter.adr.instance-id, letter.msg

        #return if transport-level-commands-ka letter

        #_D 'handle-RAMF-package - decoded letter is:', letter
        _D 'calls gw-lts inbound-handler.'

        #for let fn in listeners
        gwlt-incoming-handler letter

    #transport-level-commands-ka = (letter) ->
    #    if letter.topic is GW_TRANSPORT_CONNECT   # transport-connect ?
    #
    #        # store info about this instance-id
    #        inst-id = letter.msg[0]
    #        routes[letter.from.instance-id] = inst-id
    #
    #        #if inst-id.length > 6   # Or something... re-generate a shorter instance-id
    #
    #        *TODO* XXX
    #        skicka connect-ack, tillsammans med instance-id update..

    # # SENDING # #
    transmit-msg = (msg, priority = false) !->    # *TODO*, allow waiting for delivery?
        _D 'transport::transmit', msg
        if priority
            out-queue.unshift msg
        else
            out-queue.push msg

        process-out-queue!

    process-out-queue = !->
        # *TODO* add some timeout stuff, if opposing side hangs or something..
        _D 'process-out-queue', out-queue.length, write-locks
        return if out-queue.length is 0

        letter = out-queue.shift!  # FIFO

        _D 'popped a letter:', letter

        if not letter.adr.instance-id
            throw new Error  "The letter to-adr doesn't signifiy target instance-id! We require that!", letter

        transmission-file-path = instance-id-path + letter.adr.instance-id + transmitFilePostfix

        if write-locks[transmission-file-path]
            _D 'had a writelock, push back the letter'
            out-queue.push letter   # put the letter back in the end of line
            return

        write-msg transmission-file-path, letter

    write-msg = (transmission-file-path, letter) ->
        write-locks[transmission-file-path] = true
        msg-pkg = serialize-msg letter
        send-file-tmp-path = transmission-file-path + "_TMP"
        _D 'writes to tmp out-file', send-file-tmp-path, msg-pkg
        err, ret <-! fs.write-file send-file-tmp-path, msg-pkg, encoding: 'ascii', mode: 438
        _D 'moves to real out-file'
        # Now, it's written and flushed, we _move_ it into location, fully written  - 2014-07-19/ORC(16:22)
        err, ret <-! fs.rename send-file-tmp-path, transmission-file-path

    serialize-msg = (letter) ->
        adr = [
            letter.sent
            letter.from.msg-id
            letter.from.instance-id
            letter.adr.msg-id
            letter.adr.instance-id
        ]

        adr.push letter.topic

        msg-pkg = (adr.concat(letter.msg, "-47")).join ','
        return msg-pkg


    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # ASYNC INITS OF MODULE # #
    nicely !->


        c,o,e <-! exec 'hostname'   # find out our machine name
        machine := o

        say 'machine is', machine.yellow.bold

        cmd = "mkdir -p #{instance-id-path} && chmod 777 #{instance-id-path}"
        _D 'execs cmd:', cmd
        c, o, e <-! exec cmd
        return done(e || c) if c or e

        wanted-events = 'moved_to,moved_from,delete,close_write'
        inotify-params = [ '-mr', '-e', wanted-events, '.' ]
        inotify-root = instance-id-path

        _D 'cmd: inotifywait ' + inotify-params.join ' '

        start-inotify = !->
            notif := spawn "inotifywait", inotify-params, do
                cwd: inotify-root

        start-inotify!

        #filter-events = wanted-events.to-upper-case! / /,/

        notif.stdout.on 'data', (data) !->
            #_D 'stdout: ' + data
            rows = (''+data) / /\n/

            for src-row in rows
                continue if not src-row

                row = src-row.split ' '

                _D 'file-info-row:'.red.bold, src-row.yellow.bold

                path = (row.0 + row.2) - /^.\//
                events = (row.1 || '').split ','

                package-path = instance-id-path + path

                for event in events
                    #if -1 is filter-events.index-of event
                    #    #_D 'event unwanted:', event, filter-events
                    #    continue

                    #_D 'file change:', path, ':', event, 'row:', row
                    handle-RAMF-event event, package-path

        notif.stderr.on 'data', (data) !->
            _E 'inotifywait stderr: ' + data

        notif.on 'close', (code) !->
            _E 'inotifywait closed - RESTART IT!'
            start-inotify! unless RAMF.is-destroyed


        # *TODO* perhaps we should touch ./* to re-read all messages not handled
        # We need timestamp on message-freshness!


        done null, RAMF
        return

    # - Public interface -
    RAMF = {}

    RAMF.is-destroyed = false

    RAMF.destroy = (done) !->
        RAMF.is-destroyed = true
        notif.kill!

        c, o, e <-! exec "rm -rf #{instance-id-path}"
        die c, e if c or e
        done!

    RAMF.transmit = transmit-msg    # *TODO*, allow waiting for delivery?

    return RAMF

return transports
