
_D = console.log



/*


As a unit-test, a bot utilizing all indicators should be implemented, both
in NJS-MT4 and in MQL4.

* For comparing they behave the same (orders placed)
* For performance reference


*/


console.log 'QuantBot mod'

#    wave-math
musts, \
qmath, \
QuantBuf, \
QuantPeriodization, \
QuantFeed, \
{AvantAmitter} \
DataTyping \
<- define <[
    wpn/must-haves
    quant-math
    QuantBuffer
    QuantPeriodization
    QuantFeed
    wpn/AvantAmitter
    wpn/DataTyping
]>
#    wpn/avants

console.log 'QuantBot mod inside'

musts.globalize!

console.log 'globalize qmath'

globalize qmath

# # # PUBLIC INTERFACE # # #
QuantBotMod = {}

/*
fooo-bot = '''
{
  init: function(done){
    _D('so some stuff in bot init here then');
    aOn('tick', function(){
      var i$, args, cb;
      args = 0 < (i$ = arguments.length - 1) ? slice$.call(arguments, 0, i$) : (i$ = 0, []), cb = arguments[i$];
      console.log('an "ontick" tied in bot userspace "init()", yeay!');
      cb();
    });
    //_D('Tied a on-tick. args are:', arguments);
    done();
  }
};'''
*/
fooo-bot = '''

var foobar = function(x) {
  return x * x;
};

function init(done){
  _D('so some stuff in bot init here then, like calc foobar:', foobar(47));

  aOn('tick', function(){
    var i$, args, cb;
    args = 0 < (i$ = arguments.length - 1) ? slice$.call(arguments, 0, i$) : (i$ = 0, []), cb = arguments[i$];
    console.log('an "ontick" tied in bot userspace "init()", yeay!');
    cb();
  });
  //_D('Tied a on-tick. args are:', arguments);
  done();
}

'''

TradeDesk.create = (feed, conf) ->
    broker = feed.get-broker!

    # - Public Interface
    TD = {}

    TD.place-order = (conf, done) !->

    TD.close-order = (conf, done) !->

    TD.get-orders = (done) !->

    return TD

slice$ = [].slice

QuantBotMod.create = (conf) ->
    _D 'QuantBotMod.create '

    # - - conf
    run-mode = "LIVE"   # "MT4-TESTER",  "T23MF-TESTER"  - MT4-TESTER has lotsa limitations...

    main-period-len = conf.period
    main-security = conf.security
    main-broker = conf.broker


    # - - variables


    # - - holders
    errors = []
    trade-desks = {}
    amitter = new AvantAmitter


    a-on = (ev, fn) !->
        switch ev
        | 'tick'
            main-feed.a-on 'tick' fn

        | _
            amitter.a-on ev, fn

    a-off = (ev, fn) !->
        switch ev
        | 'tick'
            main-feed.a-off 'tick' fn

        | _
            amitter.a-off ev, fn


    # - - Instances
    main-feed = QuantFeed.create do
        gw-conn: conf.gw-conn   # *TODO* this should NOT be here later on
        broker: main-broker
        security: main-security

    main-periodization = QuantPeriodization.create main-feed, main-period-len

    trade-desks[main-feed.get-broker!] = TradeDesk.create main-feed

    # - - Bot Logic Public Functions - -


    # - Bot configuration convenience functions
    security-feed = (broker, security) ->
        main-feed = QuantFeed.create do
                gw-conn: conf.gw-conn   # *TODO* this should NOT be here later on


    periodization = (period, feed) ->   # convenience function for creating periodizations
        feed ||= main-feed

        return QuantPeriodization.create do
            period: period
            feed: feed

    # - Order placing convenience functions
    place-order = (order-type, qty, price, more, stuff, here, todo, broker, security, done) !->
        if !broker
            broker = main-broker
        if !security
            security = main-security

        err, ticket <-! trade-desks[broker].place-order yada, yada, yo, yo

        # NOT FINISHED!
        #*TODO*

    close-order = (order-type, qty, price, more, stuff, here, todo, broker, security, done) !->
        if !broker
            broker = main-broker
        if !security
            security = main-security

        err, success <-! trade-desks[broker].place-order yada, yada, yo, yo

        # NOT FINISHED!
        #*TODO*

    get-orders = (broker, security, magic-number, done) !->
        if !broker
            broker = main-broker
        if !security
            security = main-security

        err, orders <-! trade-desks[broker].get-orders yada, yada, yo, yo

        # NOT FINISHED!
        #*TODO*

    get-all-orders = (done) !->
        all-orders = []
        <-! ache 'own', trade-desks, (t-desk, next) !->
            err, orders <-! t-desk.get-orders yada, yada, yo, yo
            # *TODO* err handling
            all-orders.=concat orders

        done null, all-orders

        # NOT FINISHED!
        #*TODO*


    # # # EVAL! BE AWARE! (we want this! (TM)) # # #


    # *TODO* *HARD* *DEBUG*
    conf.bot-logic = fooo-bot

    logic-code = conf.bot-logic

    #BOT = null
    BOT = {}

    init = on-tick = handle = null  # - 2014-07-24/ORC(16:25)


    #let # shadow closure - to hide some things from user-space-bot-logic...  - 2014-07-21/ORC(19:03)
        #main-period-len = \
        #main-security = \
        #main-broker = \
        #errors = \
        #amitter = \
        #conf = \
        #global = null

        #code = "BOT = " + logic-code
        #eval code
    eval logic-code

    _D 'QuantBotMod : BOT = logic ', BOT



    # # # PUBLIC INTERFACE # # #

    # *TODO* not much needs to be public, seeing that the user-space-bot-logic is
    # evaled here to gain access to all variables in this space...
    # Vars we absolutely don't want bot-logic to access should be shielded via
    # shadow variables in a closure..

    BOT.is-destroyed = false

    BOT.destroy = (done) !->
        BOT.is-destroyed = true
        conf = undefined
        <-! amitter.amit 'destroy'
        amitter.destroy!
        code = undefined
        BOT = undefined
        done! if done

    #BOT.handle-tick = (time, open, high, low, close, vol, mvol, spread, done) !->
    #    type Fn done
    #
    #    console.log 'bot on-tick got:', time, open, high, low, close, vol, mvol, spread
    #
    #    # BOT internals can subscribe to the internal emitted, which is before the "public" (user-space bot-code)
    #    <-! amitter.amit 'tick-internal', time, open, high, low, close, vol, mvol, spread
    #    <-! amitter.amit 'tick', time, open, high, low, close, vol, mvol, spread
    #    done!

    BOT.a-on = a-on
    BOT.a-off = a-off

    #BOT.a-on 'tick', BOT.on-tick if BOT.on-tick
    a-on 'tick', on-tick if on-tick
    a-on 'tick', handle if handle

    BOT.main-feed = main-feed   # *TEMP* *HARD*
    BOT.main-periodization = main-periodization # *TEMP *HARD*

    #

    _D 'QuantBotMod : call BOT.init '

    nicely !->  # let through return so that event-tiers can do it before init...

        # *TODO* timeout if "user-space" bot fails!

        initer = init || BOT.init

        initer (err) !->    # bot-logic supplied
            console.log 'Bot logic initialized. Call done.'
            amitter.amit 'init'

    return BOT


return QuantBotMod
