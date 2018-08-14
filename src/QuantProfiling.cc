#include "QuantProfiling.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-04
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "Profiling.hh"

enum QuantProfileEnum {
    SCRAP = 0,
    STARTUP,
    GENERAL,
    LOADING_FILES_T23M,
    DECODING_FILES_T23M,
    LOADING_FILES_DUKAS,
    DECODING_FILES_DUKAS,
    ENCODING_DATA,
    WRITING_FILE,
    PLOTTING,
    LAST_ITEM
};

#endif

const char* QuantProfileLabels[] = {
    "-NULL-",                       "Startup",
    "General",                      "Loading files - T23M-format",
    "Decoding data - T23M-format",  "Loading files - Dukas-format",
    "Decoding data - Dukas-format", "Encoding data - T23M",
    "Writing file - T23M",          "Generating plotting data"
};


#ifdef INTERFACE

typedef Profiling<QuantProfileEnum, QuantProfileEnum::LAST_ITEM, 1024>
QuantProfiler;
extern QuantProfiler profiler;

#define profiling profiler
inline void start(QuantProfiler& p, QuantProfileEnum what) { p.start(what); }
inline void stop(QuantProfiler& p, QuantProfileEnum what) { p.end(what); }

#endif

QuantProfiler profiler(QuantProfileLabels);
