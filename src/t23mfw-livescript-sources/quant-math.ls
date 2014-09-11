# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Created:  2009-??-??
# Modified: 2014-06-08
# Author:   Oscar Campbell
# Licence:  MIT/Expat
# - http://opensource.org/licenses/mit-license.html
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

<- define []




/*




# *TODO* is just cut n paste... should contain ONLY QBuffer related functions
like 'highest', 'lowest', 'ema', etc. etc.




*/



QM = Quantitative-Math = {}

QM.max = (a, b) -> # 140110/ORC - convenience overload - max applied for each element in vector - and/or for simple values
    if a && a.length
        out = []
        for i in [0 til a.length]
            if b.length
                out.push maxv a[i], b[i]
            else
                out.push maxv a[i], b
    else
        out = maxv a, b
    out

QM.min = (a, b) -> # 140110/ORC - convenience overload - convenience overload - min applied for each element in vector - and/or for simple values
    if a && a.length
        out = []
        for i in [0 til a.length]
            if b.length
                out.push minv a[i], b[i]
            else
                out.push minv a[i], b
    else
        out = minv a, b
    out

# *TODO* handle bars count
QM.highest = (series, bars) -> _.max series    # 140121/ORC - I much prefer 'highest' to find the highest value in series, 'max' instead works in parallell, on each value individually together with another series vector

# *TODO* handle bars count
QM.lowest = (series, bars) -> _.min series    # 140121/ORC - I much prefer 'lowest' to find the lowest value in series, 'min' instead works in parallell, on each value individually together with another series vector

QM.mean = (...args) ->    # 140111/ORC - takes any amount of lists and calculates the mean average of every position and returns a new list. Simple values can be used in any position and is used statically for each iteration of the vectors
    lists = args.length
    out = (foo = args.shift()).slice?(0) || [foo]   # kopiera första..
    len = out.length
    for i in [0 til len]
        for a in args
            out[i] += if typeof a is 'number' then a else a[i]
        out[i] /= lists
    return out

QM.last = (list, count) ->     # works as underscores _.last, with the addition of overloading, handling objects by running last on all vectors in the obeject (instrument for instance), also the count can be put first, which simplifies some composite functional expressions - 140121/ORC
    type = typeof list
    if 'number' is type then foo = list; list = count; count = list
    if 'object' is type and not list.length? # 'lastify' all props on an obj
        out = {}
        for own k, v of list
            out[k] = last v, count
        return out
    else
        return _.last.apply _, arguments

QM.first = (list, n) ->    # most of what applies to last applies to first
    if typeof list is 'object' and not list.length?
        out = {}
        for own k, v of list
            if v.length
                out[k] = first v
        return out
    else
        list[0]

QM._g = (series, pos) -> # gets a value in a vector, indexed from the END, both orders of args are allowed for convenience 140110/ORC
    if typeof series is 'number' then foo = series; series = pos; pos = foo
    return (series[series.length - 1-pos] || series[0]) # first in series as backup if we buffer overflow


return QM
