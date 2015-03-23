#include "QuantStudyContextBase.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantStudyContextAbstract.hh"
#include "QuantProfiling.hh"

#define plot_stream std::cout

constexpr int MAX_PLOTS = 100;
// MAX_PLOTS - This is a generous amount - we'll most likely never
// fucking ever reach this high a count of plots in one study (that
// would be the new definition of insane!)

class PlotCollection
{
  public:
    PlotCollection();
    //~PlotCollection ();

    std::string         last_plot_colors[MAX_PLOTS];

    pxt::ptime          buf_epoch; // = dt::min_date_time;
    int                 time_delta_ms_rel_resolution = 1;
    std::stringstream   timestamps;

    std::stringstream   series[MAX_PLOTS];
    std::stringstream   dynamics[MAX_PLOTS];
    std::stringstream   headers[MAX_PLOTS];
};

#endif

PlotCollection::PlotCollection()
    :
    buf_epoch { makeDate(1970, 1, 1) }
{
}

#ifdef INTERFACE

template <bool PLOTTING_USED>
class QuantStudyContext : public QuantStudyContextAbstract
{
  public:
    QuantStudyContext(HashTree & conf);
    virtual ~QuantStudyContext();

    bool verify_buffers();      // 141012/ORC

    void prepareRun();

    // // // non plotting vars // // //
    bool    is_primed = false;
    clock_t profiling_ts;

    // - - Plotting functions
    void init_plotting();
    void finalize_plot();

    inline void close_plot_lap(QuantTime ts);

    inline void plot_separator()
    {
        ++plot_ix;
    }

    inline void set_plot_group(Str group);

    inline void plot_ohlc(
        //QuantPeriodization &per,
        QuantReal o,
        QuantReal h,
        QuantReal l,
        QuantReal c,
        Str buffer_name,
        Str up_body_color = "",
        Str down_body_color = "",
        Str up_border_color = "",
        Str down_border_color = "",
        bool dynamic_color = false
    );

    inline void plot(
        QuantReal value,
        Str buffer_name,
        Str color = "",
        int thickness = 1,
        enum_plot_type type = LINE,
        enum_line_style line_style = SOLID,
        bool dynamic_color = false
    );

    inline void plot(
        QuantReal value1,
        QuantReal value2,
        Str buffer_name,
        Str color1 = "",
        Str color2 = "",
        Str fill_color = "",
        int thickness = 1,
        enum_plot_type type = ZONE,
        enum_line_style line_style = SOLID,
        bool dynamic_color = false
    );

    // - Plot related variables
    //bool                plots_enabled = false;
    bool                is_plot_headerized = false;
    Str                 current_plot_group = "main";
    int                 plot_count = 0;
    int                 plot_ix = 0;
    int                 value_count_ix = 0;

    PlotCollection   *   plot_data = nullptr;

};

template <bool PLOTTING_USED>
QuantStudyContext<PLOTTING_USED>::QuantStudyContext(HashTree & conf)
    :
    QuantStudyContextAbstract(conf)
{
}

template <bool PLOTTING_USED>
QuantStudyContext<PLOTTING_USED>::~QuantStudyContext()
{
    finalize_plot();
}

// // // - // // //

template <bool PLOTTING_USED>
bool QuantStudyContext<PLOTTING_USED>::verify_buffers()
{
    // If the whole study is considered primed - we've passed that stage
    // and can safely skip these checks.
    if (is_primed) {
        return true;
    }
    /*
        is_primed = true;

        // 141012/ORC
        for ( QuantPeriodization *per : the_jar.periodizations ) {
            for ( auto buf : per->buffer_heap.buffer_jar ) {

                //   *TODO*
                if ( buf.size < buf.minimum_to_be_primed ) {
                    per->buffer_heap.buffer_jar.is_primed = false;
                    is_primed = false;
                    // Continue setting all buffers needed to false, not only the
                    // first one. This for user-study buffer specific checks.
                }
            }
        }
    */
    return is_primed;
}

template <bool PLOTTING_USED>
void QuantStudyContext<PLOTTING_USED>::prepareRun()
{
    init_plotting();
}



template <> inline void QuantStudyContext<false>::close_plot_lap(QuantTime)
{
}

template <> inline void QuantStudyContext<true>::close_plot_lap(QuantTime ts)
{
    profiler.start(PLOTTING);
    if (is_plot_headerized == true) {
        //pxt::time_duration delta_ts ( ts - plot_data->buf_epoch );
        pxt::time_duration delta_ts = ts - plot_data->buf_epoch;
        //cerr << "pxt::time_duration delta_ts " << delta_ts << "\n";
        unsigned long long delta_ts_in_s = delta_ts.total_milliseconds() /
                                           plot_data->time_delta_ms_rel_resolution;
        //cerr << "pxt::time_duration delta_ts_in_s " << delta_ts_in_s << "\n";
        plot_data->timestamps << delta_ts_in_s << ",";
        assert(plot_ix == plot_count);
        ++value_count_ix;
        plot_ix = 0;
        profiler.end(PLOTTING);
        return;
        //} else {
        //    plot_buf << "], [";    // Close the header, pad with empty arr for last comma
    }
    else {
        cerr << "close_plot_lap - finalizing headers" << "\n";
        assert(plot_count <= MAX_PLOTS);
        is_plot_headerized = true;
        plot_ix = 0;
        profiler.end(PLOTTING);
        return;
    }
}

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
template <> inline void QuantStudyContext<false>::set_plot_group(Str)
{
}
//#pragma GCC diagnostic pop

template <> inline void QuantStudyContext<true>::set_plot_group(Str group)
{
    current_plot_group = group;
}

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
template <> inline void QuantStudyContext<false>::plot_ohlc(QuantReal,
        QuantReal, QuantReal, QuantReal, Str, Str, Str, Str, Str, bool)
{
}
//#pragma GCC diagnostic pop



//#include "QuantPeriodization.hh"    // Solely used for plot_ohlc atm. Not necessary by any means...

template <> inline void QuantStudyContext<true>::plot_ohlc(
    //QuantPeriodization &per,
    QuantReal o,
    QuantReal h,
    QuantReal l,
    QuantReal c,
    Str buffer_name,
    Str up_body_color,
    Str down_body_color,
    Str up_border_color,
    Str down_border_color,
    bool dynamic_color
)
{
    profiler.start(PLOTTING);
    if (is_plot_headerized) {       // *TODO* switch out a function ptr as soon as init is done..
        if (dynamic_color) {
            Str new_colors = up_body_color + down_body_color + up_border_color +
                             down_border_color;
            if (plot_data->last_plot_colors[plot_ix] != new_colors) {
                plot_data->last_plot_colors[plot_ix] = new_colors;
                plot_data->dynamics[plot_ix] << "[" << value_count_ix << ",\"" << up_body_color
                                             << "\",\"" << down_border_color << "\",\""
                                             << up_border_color << "\",\"" << down_border_color << "\"],";
            }
        }
        plot_data->series[plot_ix] << "[" << o << "," << h << "," << l << "," << c <<
                                   "],  ";
        plot_separator();
        profiler.end(PLOTTING);
        return;
    }
    else {
        cerr << "Builds plot header" << "\n";
        plot_data->headers[plot_ix]
                << enum_plot_type::OHLC << "," << "\"" << buffer_name << "\",\""
                << up_body_color << "\",\"" << down_body_color << "\",\""
                << up_border_color << "\",\"" << down_border_color << "\""
                << "";
        ++plot_count;
        ++plot_ix;
        profiler.end(PLOTTING);
    }
}

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
template <> inline void QuantStudyContext<false>::plot(QuantReal, QuantReal,
        Str, Str, Str, Str, int, enum_plot_type, enum_line_style, bool)
{
}
//#pragma GCC diagnostic pop

template <> inline void QuantStudyContext<true>::plot(
    QuantReal value1,
    QuantReal value2,
    Str buffer_name,
    Str color1,
    Str color2,
    Str fill_color,
    int thickness,
    enum_plot_type type,
    enum_line_style line_style,
    bool dynamic_color
)
{
    profiler.start(PLOTTING);
    // If we're not inited we sacrifice the very first value in order to
    // establish plot count first.
    if (is_plot_headerized) {       // *TODO* switch out a function ptr as soon as init is done..
        //cerr << "Plot is inited" << "\n";
        if (dynamic_color) {
            Str new_colors = color1 + color2 + fill_color;
            if (plot_data->last_plot_colors[plot_ix] != new_colors) {
                plot_data->dynamics[plot_ix] << "[" << value_count_ix << "\"" << color1 <<
                                             "\",\"" << color2 << "\",\"" << fill_color << "\"],";
                plot_data->last_plot_colors[plot_ix] = new_colors;
            }
        }
        plot_data->series[plot_ix] << "[" << value1 << "," << value2 << "],  ";
        plot_separator();
        profiler.end(PLOTTING);
        return;
    }
    else {
        cerr << "Builds plot header" << "\n";
        plot_data->headers[plot_ix] << type << "," << "\"" << buffer_name << "\",\"" <<
                                    color1 << "\",\"" << color2 << "\",\"" << fill_color << "\"," << thickness <<
                                    "," << line_style;
        ++plot_count;
        ++plot_ix;
        profiler.end(PLOTTING);
    }
}

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
template <> inline void QuantStudyContext<false>::plot(QuantReal, Str, Str,
        int, enum_plot_type, enum_line_style, bool)
{
}
//#pragma GCC diagnostic pop

template <> inline void QuantStudyContext<true>::plot(
    QuantReal value,
    Str buffer_name,
    Str color,
    int thickness,
    enum_plot_type type,
    enum_line_style line_style,
    bool dynamic_color
)
{
    profiler.start(PLOTTING);
    // If we're not inited we sacrifice the very first round of values in
    // order to establish plot count first.
    if (is_plot_headerized) {       // *TODO* switch out a function ptr as soon as init is done..
        if (dynamic_color && (plot_data->last_plot_colors[plot_ix] != color)) {
            plot_data->dynamics[plot_ix] << "[" << value_count_ix << ",\"" << color <<
                                         "\"],";
            plot_data->last_plot_colors[plot_ix] = color;
        }
        plot_data->series[plot_ix] << value << ", ";
        plot_separator();
        profiler.end(PLOTTING);
        return;
    }
    else {
        cerr << "Builds plot header" << "\n";
        plot_data->headers[plot_ix] << type << "," << "\"" << buffer_name << "\",\"" <<
                                    color << "\"," << thickness << "," << line_style;
        ++plot_count;
        ++plot_ix;
        profiler.end(PLOTTING);
    }
}


#endif

// // // - // // //

template <> void QuantStudyContext<false>::init_plotting()
{
}

template <> void QuantStudyContext<true>::init_plotting()
{
    cerr << "init_plotting()" << "\n";
    profiling_ts = clock();
    //plots_enabled = isBuffersOutputEnabled();
    //plot_buf = &std::cout;
    plot_data = new PlotCollection();
    plot_data->time_delta_ms_rel_resolution =
        1000; // seconds resolution   * 60; // minute resolution
}

template <> void QuantStudyContext<false>::finalize_plot()
{
}

template <> void QuantStudyContext<true>::finalize_plot()
{
    profiler.start(PLOTTING);
    constexpr double BUFFER_FORMAT_VERSION = 0.2;
    plot_stream
            << "{"   << "\n"
            << "\"version\": " << BUFFER_FORMAT_VERSION << ", "   << "\n";
    plot_stream
            << "\"run_stats\": {"    << "\n"
            << "  \"run_time\": " << ((clock() - profiling_ts) / (CLOCKS_PER_SEC / 1000))
            << "\n";
    plot_stream
            << "}, "   << "\n"
            << "\"summary\": {"    << "\n";
    // FOR EACH SUMMARY POINTS {
    plot_stream << "\"foo\": 47 " << "";
    // }
    plot_stream
            << "}, "   << "\n"
            << "\"markers\": [ ";
    // FOR EACH MARKERS {
    plot_stream << "[ 123123, \"tag1\", 12.671 ] ";
    // }
    plot_stream
            << "], "   << "\n"
            << "\"annotations\": [ ";
    // FOR EACH ANNOTATIONS {
    // *TODO* enum_annotation_type : LINE, ARROW, ELLIPSE, RECT, etc. etc. 141010
    plot_stream << "[ " << enum_plot_type::CIRCLE <<
                ", 123123, 11.14, 900, 0.04, \"grp2\" ] ";
    // }
    plot_stream
            << "], "   << "\n"
            << "\"timeline\": {" << "\n"
            << "  \"buf_epoch\": \"" << plot_data->buf_epoch << "\"," << "\n"
            << "  \"resolution\": " << plot_data->time_delta_ms_rel_resolution << "," <<
            "\n"
            << "  \"timestamps\": [ " << plot_data->timestamps.str() << "null ] \n";
    plot_stream
            << "}, "   << "\n"
            << "\"series\": [ "   << "\n";
    for (int i = 0; i < plot_count; ++i) {
        plot_stream
                << "{"
                << "\"conf\": [ " << (plot_data->headers[i]).str() << " ], "  << "\n"
                << "\"data\": [ " << (plot_data->series[i]).str() << "null ], "  << "\n"
                << "\"dynamics\": [ " << (plot_data->dynamics[i]).str() << "null ]" << "\n"
                << "}, "   << "\n";
    }
    plot_stream
            << "\n" << "null"   << "\n"
            << "]"   << "\n"
            << "}";
    profiler.end(PLOTTING);
}
