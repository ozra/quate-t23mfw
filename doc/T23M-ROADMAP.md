/**
* Created:  2014-12-02
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

T23MFW / T23M RV
ト23モ ラヲ



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

- [ ] Use "protocol buffers"
- [ ] over "ZMQ"
- [ ] over "nanomsg"
- [ ] or "WebSocket" (web)



- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## VISUALIZATION / UI ##

- [x] TradeGunV8
    - [x] NodeJS + HTML5 / JS / Canvas based solution.

- [x] TradeGunQ7
    - [x] QT based solution for the swift kind of working a mad scientist needs.
    - [x] TECHS:
        - [x] http://www.qcustomplot.com/index.php/tutorials/basicplotting
    - [ ] Android version

- [ ] TradeGunOx


- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## LOW LEVEL - OPTED / HARDENED IMPLEMENTATION ALTS ##
- [ ] https://code.google.com/p/gperftools/?redir=1 - for allocating small objs etc.
