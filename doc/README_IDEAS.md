/**
* Created:  2014-08-16
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

T23M RW
ト23モ ラヲ


# C++ learning todo: BOOST: #

Boost has an unimaginable number of libraries. Easy ones to get started on are

noncopyable
array
circular_buffer
foreach
operators (one of my personal favorites)
smart_ptr
date_time
More advanced ones include

lambda
bind
iostreams
serialization
threads


# More BOOST #

boost::any and boost::variant are good of you need a variant data type, with two different approaches.
boost::regex if you need some text parsing.
boost::thread and boost::filesystem




# SqLite wrapper header only #

https://github.com/burner/sweet.hpp/blob/master/sweetql.hpp



# C++ - WebUi #

https://github.com/sihorton/appjs-deskshell
https://github.com/rogerwang/node-webkit
http://cppcms.com/wikipp/en/page/main
http://www.awesomium.com/
http://librocket.com/


# HTML5 Canvas Charts jQuery based - pluginable #
http://www.flotcharts.org/flot/examples/


# TradingView API - for ideas / reference #
https://docs.google.com/document/d/1rAigRhQUSLgLCzUAiVBJGAB7uchb-PzFVe0Bl8WTtF0/edit?pli=1#



# DATA VISUALIZATION THEORY #

http://www.tableausoftware.com/public/blog/2013/10/leveraging-color-improve-your-data-visualization-2174



# ALGORITHMIC TRADING AND FORMULAS IN GENERAL #

http://www.castrader.com/2006/09/the_kelly_crite.html
http://papers.nips.cc/paper/5116-a-latent-source-model-for-nonparametric-time-series-classification.pdf
http://arxiv.org/pdf/1410.1231v1.pdf
http://jonathankinlay.com/index.php/2011/04/a-practical-application-of-regime-switching-models/




## MATHS PROPER ##

http://en.wikipedia.org/wiki/Dynamic_Bayesian_network
http://en.wikipedia.org/wiki/Recursive_Bayesian_estimation
https://staff.fnwi.uva.nl/j.m.mooij/libDAI/     // C++ lib for Dynamic Bayesian..
http://en.wikipedia.org/wiki/Forward%E2%80%93backward_algorithm
    http://en.wikipedia.org/wiki/Dynamic_programming
    http://en.wikipedia.org/wiki/Forward_algorithm
        http://en.wikipedia.org/wiki/Hidden_Markov_model



## The idea about creating the 'noise free' price series ##

Might make use of Kalman filters :

http://en.wikipedia.org/wiki/Kalman_filter
http://intelligenttradingtech.blogspot.se/2010/05/kalman-filter-for-financial-time-series.html
http://jonathankinlay.com/index.php/2010/07/learning-the-kalman-filter/

The Kalman filter can update the signal x times in a single value step and likewise, it can estimate x steps without signal. Applying signals from all viable markets to create the selective average...

Unscented Kalman filter


The other idea ofcourse being recursive selective exclusive clustering combining mean value: all possible combinations std-dev calced median/mean-groupings of their means, deciding "error" factor of each market, and adjustement values, and the n finally producing a mean value with ther error corrections applied.





// 140926 - QuantFlux ( margin of error in markets) - thought through earlier
// today (~1400) - time is now 21:17 - and the radio (P1 ?) is talking about
// Quantum Fluctuations during the first 400KY after The Big, captured by the
// Planck camera (which I thought was deemed to be cosmic dust and completely
// moot - already. Anyways. Coincidence de luxe. And the pseudo scientific
// writer of the book the programmed is centered around annoys me immensly -
// hate his voice! Well, and pseudo babble.
//
// Well - the definition for the flamboyant term in market space is "The
// variation of market price of a financial instrument amongst all available
// - realisitcally suitable tradeable - markets. The search for the
// gravitational multi body system, or 0-is-surface based or freely geometrical
// space is based on the assumption that any deviations within Quant Flux is
// of no importance (other than that the relative magnitude of QF may be
// indiciative / confirming of impending (!) change.
