#include "QSDF_Ticks_Abstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QSDF_BaseAbstract.hh"
#include "QuantElementModel_Tick.hh"

enum calculation_type {
    NOP = 0,
    RECIPROCAL = 1,
    LOG = 2,
    REL_PERCENTAL = 3

};


class QSDF_Ticks_Abstract : public QuantSequentialDataAbstract
{

  public:
    QSDF_Ticks_Abstract() {};
    ~QSDF_Ticks_Abstract() {};

    void set_streaming_calculation(calculation_type stream_calculation);

    //virtual bool seek() = 0;
    virtual bool readTick(QuantTick & tick) = 0;
    virtual bool writeTick(const QuantTick & tick);
};


#endif

bool QSDF_Ticks_Abstract::writeTick(const QuantTick &)
{
    throw "Not implemented in";
    return false;
}
