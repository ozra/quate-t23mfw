#include "QuantStudyContextAbstract.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantExecutionContext.hh"


constexpr int MAX_PLOTS = 1000;

enum enum_plot_type {
    // 1 value
    LINE = 0,
    CIRCLE,
    DIAMOND,
    SQUARE,
    CROSS,
    DASH,

    // 2 values
    ZONE,

    // 4 values
    OHLC,

    // 5 values
    OHLCM
};

enum enum_line_style {
    SOLID = 0,
    DASHED,
    DOTTED,
    DOTDASH
};


// // // // // // // // // // // // // // // // // // // // // // // // // //

int     param ( Str key, int default_value,     Str description = "", HashTree &conf = *global_actives.active_conf );
double  param ( Str key, double default_value,  Str description = "", HashTree &conf = *global_actives.active_conf );
Str     param ( Str key, Str default_value,     Str description = "", HashTree &conf = *global_actives.active_conf );

#endif


int param ( Str key, int default_value, Str description, HashTree &conf ) {
    cerr << "StudyContext::param: " << key << " - " << description << "\n";
    return conf.get( key, default_value );
}

double param ( Str key, double default_value, Str description, HashTree &conf ) {
    cerr << "StudyContext::param: " << key << " - " << description << "\n";
    return conf.get( key, default_value );
}

Str param ( Str key, Str default_value, Str description, HashTree &conf ) {
    cerr << "StudyContext::param: " << key << " - " << description << "\n";
    return conf.get( key, default_value );
}



#ifdef INTERFACE

#define plot_stream std::cout


class QuantStudyContextAbstract {
  public:
    QuantStudyContextAbstract ( HashTree &conf );
    virtual ~QuantStudyContextAbstract ();

    virtual void init () = 0;
    virtual void prepareRun () = 0;
    //int serialize ();
    void setRunContext ( QuantExecutionContext *p_run_context );
    void add ( QuantFeed *feed );
    void add ( QuantPeriodization *periodization );

    bool isBuffersOutputEnabled ();
    bool isOptimization ();

    // - - Somewhat private
    QuantKeeperJar the_jar;
    // *TODO* remove - we rely on the global / put it in jar also..
    QuantExecutionContext *run_context;
    // - -


};

#endif

QuantStudyContextAbstract::QuantStudyContextAbstract ( HashTree &conf )
:
    the_jar { conf }
{}

QuantStudyContextAbstract::~QuantStudyContextAbstract () {
}

void QuantStudyContextAbstract::setRunContext ( QuantExecutionContext *p_run_context ) {
    // *TODO* remove - we rely on the global / put it in jar also..
    run_context = p_run_context;
    run_context->add ( this );
}

bool QuantStudyContextAbstract::isBuffersOutputEnabled () {
    return run_context->isBuffersOutputEnabled();   // *TODO* *OPT* cache in class..
}

bool QuantStudyContextAbstract::isOptimization () {
    return run_context->isOptimization();   // *TODO* *OPT* cache in class..
}

//auto QuantStudyContextAbstract::add ( QuantFeed *feed ) -> QuantStudyContextAbstract &
void QuantStudyContextAbstract::add ( QuantFeed *feed ) {
    the_jar.add( feed );
    //return *this;
}

//auto QuantStudyContextAbstract::add ( QuantPeriodization *periodization) -> QuantStudyContextAbstract &
void QuantStudyContextAbstract::add ( QuantPeriodization *periodization) {
    the_jar.add( periodization );
    //return *this;
}

