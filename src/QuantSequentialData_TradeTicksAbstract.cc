#include "QuantSequentialData_TradeTicksAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantSequentialDataUtils.hh"
#include "QuantTick.hh"

class QuantSequentialDataAbstract {
   public:
    QuantSequentialDataAbstract();
    virtual ~QuantSequentialDataAbstract();

    void setDateRange(QuantTime start_date, QuantTime end_date);

    virtual bool init(std::string broker_id, std::string symbol) = 0;

    // private:
    QuantTime start_date_;
    QuantTime end_date_;
};

#endif

QuantSequentialDataAbstract::QuantSequentialDataAbstract() {}

QuantSequentialDataAbstract::~QuantSequentialDataAbstract() {}

#ifdef FIXELLE
::  ( QuantTime, QuantTime -> void )
QuantSequentialDataAbstract/:set-date-range ( p-start-date, p-end-date ) =>
    some-tmp-var ((std/:strings/:basic-char)) = "47"
    @start-date (= p-start-date
    @end-date (= p-end-date
<=
#endif

void QuantSequentialDataAbstract::setDateRange (
    QuantTime p_start_date,
    QuantTime p_end_date
) {
    start_date_ = p_start_date;
    end_date_ = p_end_date;
}

#ifdef INTERFACE

class QuantSequentialData_TradeTicksAbstract : public QuantSequentialDataAbstract {

   public:
    QuantSequentialData_TradeTicksAbstract() {};
    ~QuantSequentialData_TradeTicksAbstract() {};

    virtual bool readTick(QuantTick& tick) = 0;

};

#endif
