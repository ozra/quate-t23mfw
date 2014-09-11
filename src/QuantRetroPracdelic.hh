/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#ifndef QUANTRETROPRACDELIC_HH
#define QUANTRETROPRACDELIC_HH

#include "QuantSuit.hh"

enum retro_mode {
    BACKTEST = 0,
    FULL
};

class RetroPracdelic {
  public:
        RetroPracdelic (
            QuantStudyContext   &study,
            Str                 symbol,
            Str                 start_date,
            Str                 end_date
        );
        ~RetroPracdelic ();

    int
        run ( void );

  private:
    QuantStudyContext & study;
    Str                 symbol;
    Str                 start_date;
    Str                 end_date;
    retro_mode          mode;

};


#endif
