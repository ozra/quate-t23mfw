#include "SQDI_BaseAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantSequentialDataUtils.hh"
#include "QuantTick.hh"
#include <cstring>

class QuantSequentialDataAbstract {
   public:
    QuantSequentialDataAbstract();
    virtual ~QuantSequentialDataAbstract();

    void set_date_range(QuantTime start_date, QuantTime end_date);

    virtual bool init(std::string broker_id, std::string symbol) = 0;

    // private:
    QuantTime start_date_;
    QuantTime end_date_;
};

#endif

QuantSequentialDataAbstract::QuantSequentialDataAbstract() {}

QuantSequentialDataAbstract::~QuantSequentialDataAbstract() {}

void QuantSequentialDataAbstract::set_date_range(QuantTime p_start_date,
                                               QuantTime p_end_date) {
    start_date_ = p_start_date;
    end_date_ = p_end_date;
}
