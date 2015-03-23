#include "QuantExecutionContext.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-19
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"

class QuantStudyContextAbstract;
class QuantFeedAbstract;
class QuantPeriodizationAbstract;

class QuantExecutionContext
{
  public:
    QuantExecutionContext(
        QuantTime start_date,
        QuantTime end_date,
        bool buffers_output_enabled,
        bool is_optimization
    );
    virtual ~QuantExecutionContext() = 0;

    virtual void add(QuantStudyContextAbstract * study) = 0;

    bool isBuffersOutputEnabled();
    bool isOptimization();

    QuantTime start_date;
    QuantTime end_date;

  private:
    bool buffers_output_enabled;
    bool is_optimization;

};


#endif

QuantExecutionContext::QuantExecutionContext(
    QuantTime start_date,
    QuantTime end_date,
    bool buffers_output_enabled,
    bool is_optimization
) :
    start_date(start_date),
    end_date(end_date),
    buffers_output_enabled(buffers_output_enabled),
    is_optimization(is_optimization)
{
    global_actives.run_context = this;
}

QuantExecutionContext::~QuantExecutionContext()
{}

bool QuantExecutionContext::isBuffersOutputEnabled()
{
    return buffers_output_enabled;
}

bool QuantExecutionContext::isOptimization()
{
    return is_optimization;
}
