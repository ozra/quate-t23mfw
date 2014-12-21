/**
* Created:  2014-12-02
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

T23MFW / T23M RV
ト23モ ラヲ


OT: 24h clock for Android:
 - [ ] https://play.google.com/store/apps/details?id=info.staticfree.android.twentyfourhour&hl=en
 - [ ] physical clock: http://www.botta-design.de/en/einzeigeruhr-uno-24.html


* MOVE TO RIGHT PLACE:
    2013-12-04 - 2014-12-04 - shows the classic XBT bubble pattern in short term




- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## SOURCE STRUCTURING ##

[ ] http://git-scm.com/book/en/v2/Git-Tools-Submodules



- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## FRAMEWORK BASICS ##

- [x] sequential data format and load utilities
- [x] seqdacon - converter from external data sources to seqdafmt
- [x] retroactive runner ( backtester )
    - [x] tick-feed zipping ( historical data )
- [x] periodization (ohlc chunking)
- [x] Technical Analysis classes (specialized for performace for T23MRV)
    - [x] EMA, ROC, SMA, etc.



- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## MESSAGING / GRAPEWIRE ##

- [ ] Use protocol buffers
- [ ] over ZMQ
- [ ] or WebSocket (web)



- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## VISUALIZATION / UI ##

- [x] TradeGunV8
    - [x] NodeJS + HTML5 / JS / Canvas based solution.

- [x] TradeGunQ7
    - [ ] QT based solution for the swift kind of working a mad scientist needs.
    - [ ] TECHS:
        - [ ] http://www.qcustomplot.com/index.php/tutorials/basicplotting
    - [ ] Android version



- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## LOW LEVEL - OPTED / HARDENED IMPLEMENTATION ALTS ##
- [ ] https://code.google.com/p/gperftools/?redir=1 - for allocating small objs etc.
