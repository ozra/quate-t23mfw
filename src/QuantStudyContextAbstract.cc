#include "QuantStudyContextAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantExecutionContext.hh"

enum enum_plot_type {
    // 1 value
    LINE = 0,
    CIRCLE,
    DIAMOND,
    SQUARE,
    CROSS,
    PLUS,
    DASH,

    // 2 values
    ZONE,

    // 4 values
    OHLC,

    // 5 values
    OHLCM
};

enum enum_line_style { NONE = 0, SOLID, DASHED, DOTTED, DOTDASH };

// // // // // // // // // // // // // // // // // // // // // // // // // //

int param(Str key, int default_value, Str description = "",
          HashTree & conf = *global_actives.active_conf);
double param(Str key, double default_value, Str description = "",
             HashTree & conf = *global_actives.active_conf);
String param(Str key, Str default_value, Str description = "",
             HashTree & conf = *global_actives.active_conf);

#endif

int param(Str key, int default_value, Str description, HashTree & conf)
{
    cerr << "StudyContext::param: " << key << " - " << description
         << "val = " << conf.get(key, default_value) << "\n";
    return conf.get(key, default_value);
}

double param(Str key, double default_value, Str description, HashTree & conf)
{
    cerr << "StudyContext::param: " << key << " - " << description
         << "val = " << conf.get(key, default_value) << "\n";
    return conf.get(key, default_value);
}

String param(Str key, Str default_value, Str description, HashTree & conf)
{
    cerr << "StudyContext::param: " << key << " - " << description
         << "val = " << conf.get(key, default_value) << "\n";
    return conf.get(key, default_value);
}

#ifdef INTERFACE

#define plot_stream std::cout

class QuantStudyContextAbstract
{
  public:
    QuantStudyContextAbstract(HashTree & conf);
    virtual ~QuantStudyContextAbstract();

    virtual void init() = 0;
    virtual void prepareRun() = 0;
    // int serialize ();
    void setRunContext(QuantExecutionContext * p_run_context);
    void add(QuantFeedAbstract * feed);
    void add(QuantPeriodizationAbstract * periodization);

    bool isBuffersOutputEnabled();
    bool isOptimization();

    // - - Somewhat private
    QuantMultiKeeperJar the_jar;
    // *TODO* remove - we rely on the global / put it in jar also..
    QuantExecutionContext * run_context;
    // - -
};

#endif

QuantStudyContextAbstract::QuantStudyContextAbstract(HashTree & conf)
    : the_jar{ conf } {}

QuantStudyContextAbstract::~QuantStudyContextAbstract() {}

void
QuantStudyContextAbstract::setRunContext(QuantExecutionContext *
        p_run_context)
{
    // *TODO* remove - we rely on the global / put it in jar also..
    run_context = p_run_context;
    run_context->add(this);
}

bool QuantStudyContextAbstract::isBuffersOutputEnabled()
{
    return run_context
           ->isBuffersOutputEnabled(); // *TODO* *OPT* cache in class..
}

bool QuantStudyContextAbstract::isOptimization()
{
    return run_context->isOptimization(); // *TODO* *OPT* cache in class..
}

// auto QuantStudyContextAbstract::add ( QuantFeed *feed ) ->
// QuantStudyContextAbstract &
void QuantStudyContextAbstract::add(QuantFeedAbstract * feed)
{
    the_jar.add(feed);
    // return *this;
}

// auto QuantStudyContextAbstract::add ( QuantPeriodization *periodization) ->
// QuantStudyContextAbstract &
void QuantStudyContextAbstract::add(QuantPeriodizationAbstract *
                                    periodization)
{
    the_jar.add(periodization);
    // return *this;
}
