<- define <[ ]>
out = {}

table-definitions = {}

out.table = (name, conf) ->
    tbl = SomeDefSomeHow

    table-definitions[name] = tbl

out.verify-existance = (done) !->
    for own k, v of table-definitions
        DO-THE-STUFF-FROM-HIGHWIND()

    done!

