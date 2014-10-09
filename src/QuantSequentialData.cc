#include "QuantSequentialData.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantTick.hh"

class QuantSequentialDataAbstract
{
  public:
    QuantSequentialDataAbstract ();
    virtual ~QuantSequentialDataAbstract ();


    // *TODO* implementation specials like point_decimals etc. should NOT be
    virtual bool init (
        std::string p_symbol,
        double p_point_decimals,
        QuantTime p_start_date,
        QuantTime p_end_date
    );
    virtual bool doInit(
        std::string p_symbol,
        double p_point_decimals
    ) = 0;

    // *TODO* QuantSequentialData should return a pointer to decoded
    // data-struct - not meddle with application specific data types!
    // - 2014-09-16/ORC(12:33)
    virtual bool readTick ( QuantTick &tick ) = 0;

  //private:
    QuantTime start_date;
    QuantTime end_date;

};

#endif

QuantSequentialDataAbstract::QuantSequentialDataAbstract ()
{}

QuantSequentialDataAbstract::~QuantSequentialDataAbstract ()
{}

bool QuantSequentialDataAbstract::init (
    std::string p_symbol,
    double p_point_decimals,
    QuantTime p_start_date,
    QuantTime p_end_date
) {
    start_date = p_start_date;
    end_date = p_end_date;

    return doInit(
        p_symbol,
        p_point_decimals
    );
}
