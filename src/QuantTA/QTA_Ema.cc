/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#if INTERFACE

#include "QuantBase.hh"

namespace QTA {


class Ema : QTAO {
  public:

    /* TODO(ORC) - should we support automatic updating (signalled) from (a)
     * pre-set input buffer(s) ?? Better to be consistent and require manual
     * updating - determinism
     */

    void        Ema ( int length, int max_lookback /*, QuantBuffer input */, QuantPeriodization &perian );
    void        Ema ( int length /*, QuantBuffer input */, QuantPeriodization &perian );
    inline void init ();
    void        ~Ema ();

    void operator() ( QuReal value );
    void operator() ( QuantBuffer value );
    void operator() ( QTAO value );
    void operator[] ( int reverse_index );

  private:
    inline void handleValue ( QuReal value );
    virtual void prime ( QuReal value );    // prime the lookback with reasonable approximations, default values

    int                 len;
    int                 lookback;

    QuantPeriodization  &per;

    QuantBuffer         *cache_buf;
    QuantBuffer         *result;

}


}

#endif

#include "QTA_Ema.hh"

namespace QTA {


Ema::Ema ( int length, int max_lookback, QuantPeriodization &perian )
    :
        len(length),
        lookback(max_lookback),
        per(perian)
{
    init();

}

Ema::Ema ( int length, QuantPeriodization &perian )
    :
        len(length),
        lookback(length),
        per(perian)
{
    init();

}

inline void Ema::init () {
    cache_buf = per.newRealBuffer(len, "ema_cache_");
    result = per.newRealBuffer(lookback, "ema");

}

}