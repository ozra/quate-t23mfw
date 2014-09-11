/**
* Created:  2014-07-17
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
* Does:     Define the tables required in T23M, also the fieldmapping for
            fetching the correct fields when loading in arrayed encoding from
            DB.
**/

DB, {schema, verify-existance: verify-db} <- define <[ mysql-dsi schema-quick-def ]>

out = {}

out.define-schemas = !->
    out.define-base-schemas!
    out.define-all-candle-schemas!


out.define-all-candle-schemas = ->
    err, ret <- DB.query "SELECT broker, security FROM securities" #  WHERE created = 0

    for sec in ret
        for duration in <[ tick 1m ]>
            define-security-data-schema sec.broker, sec.security, duration

out.define-candle-schema = (broker-symbol, security-symbol, duration-symbol) ->
    schema-name = "sec_#{security-symbol}_#{broker-symbol}_{duration-symbol}"

    schema schema-name, fields: [
        * name: "time",         jstype:'number', sqltype: 'BIGINT'        # unix timestamp
        * name: "open",         jstype:'number', sqltype: 'DECIMAL(8, 5)'
        * name: "high",         jstype:'number', sqltype: 'DECIMAL(8, 5)'
        * name: "low",          jstype:'number', sqltype: 'DECIMAL(8, 5)'
        * name: "close",        jstype:'number', sqltype: 'DECIMAL(8, 5)'
        * name: "volume",       jstype:'number', sqltype: 'DECIMAL(8, 5)'
        * name: "mvolume",      jstype:'int', sqltype: 'INT'
        * name: "spread",       jstype:'int', sqltype: 'INT'   # in pips, presumably

        * name: "flags",        jstype:'int', sqltype: 'UNSIGNED SMALLINT'
    ]

out.define-base-schemas = ->
    schema 'brokers', fields: [
        * name: "name",           jstype: 'string', sqltype: 'char(16)'
        * name: "symbol",         jstype: 'string', sqltype: 'char(8)'    # forex pairs really should be the longest (6), stock is 4 (NYSE has a fith extension letter sometimes). Let's begin with 8. Some levay
        * name: "created",        jstype: 'number', sqltype: 'BIGINT'        # unix timestamp

        * name: "time_offset",    jstype:'number', sqltype: 'INT'        # offset in ms from UTC

        #"trust_rating":     jstype: 'number', sqltype: 'double'
        #"wire_rating":      jstype: 'number', sqltype: 'double'     # connection / server response time quality
        #"spread_rating":    jstype: 'number', sqltype: 'double'
        #"depth_rating":     jstype: 'number', sqltype: 'double'
    ]

    schema 'securities', fields: [
        * name: "broker",         jstype: 'string', sqltype: 'char(16)'
        * name: "security",       jstype: 'string', sqltype: 'char(8)'
        * name: "created",        jstype: 'number', sqltype: 'BIGINT'        # unix timestamp
        * name: "part1",          jstype: 'string', sqltype: 'char(3)'    # for exchange pair securities only (EURSEK, XAGUSD, etc.)
        * name: "part2",          jstype: 'string', sqltype: 'char(3)'    # for exchange pair securities only (EURSEK, XAGUSD, etc.)
        * name: "spread_rating",  jstype: 'number', sqltype: 'double'
        * name: "depth_rating",   jstype: 'number', sqltype: 'double'
    ]

out.verify-database-integrity = (done) ->
    verify-db done

out
