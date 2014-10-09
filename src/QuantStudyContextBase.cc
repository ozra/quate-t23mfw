#include "QuantStudyContextBase.hh"
#ifdef INTERFACE
/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantStudyContextAbstract.hh"


#define plot_stream std::cout

template <bool PLOTTING_USED>
class QuantStudyContext : public QuantStudyContextAbstract {
  public:
    QuantStudyContext ( HashTree &conf );
    virtual ~QuantStudyContext ();

    void prepareRun ();

    // - - Plotting functions
    void init_plotting ();
    void finalize_plot ();
    inline void close_plot_lap ();
    void plot_separator ();

    inline void set_plot_group ( Str group );

    inline void plot_ohlc (
        QuantPeriodization &per,
        Str buffer_name,
        Str up_body_color = "",
        Str down_body_color = "",
        Str up_border_color = "",
        Str down_border_color = "",
        bool dynamic_color = false
    );

    inline void plot (
        QuantReal value,
        Str buffer_name,
        Str color = "",
        int thickness = 1,
        enum_plot_type type = LINE,
        enum_line_style line_style = SOLID,
        bool dynamic_color = false
    );

    inline void plot (
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

    //QuantReal           plot_batch[MAX_PLOTS];    // This is a generous amount - we'll most likely never fucking ever reach this high a count of plots in one study (that would be the new definition of insane!)
    //Str                 *last_plot_colors[];
    Str                 last_plot_colors[MAX_PLOTS];

    std::stringstream   plot_buf;

};

template <bool PLOTTING_USED>
QuantStudyContext<PLOTTING_USED>::QuantStudyContext ( HashTree &conf )
:
    QuantStudyContextAbstract ( conf )
{}

template <bool PLOTTING_USED>
QuantStudyContext<PLOTTING_USED>::~QuantStudyContext () {
    finalize_plot();
}

template <bool PLOTTING_USED>
void QuantStudyContext<PLOTTING_USED>::prepareRun () {
    init_plotting();
}


// // // - -
// // // - -

template <bool PLOTTING_USED>
void QuantStudyContext<PLOTTING_USED>::plot_separator () {
    if ( ++plot_ix < plot_count ) {     // Stack up values
        plot_buf << ",";
    } else {                            // Finsh of a block and start a new one
        plot_buf << "], [";
        plot_ix = 0;
    }
}


#endif

template <>
void QuantStudyContext<false>::init_plotting () {
}

template <>
void QuantStudyContext<true>::init_plotting () {
    cerr << "init_plotting()" << "\n";

    //plots_enabled = isBuffersOutputEnabled();
    //plot_buf = &std::cout;

    constexpr double BUFFER_FORMAT_VERSION = 0.2;

    plot_buf
        << "{  "
        << "\"version\": " << BUFFER_FORMAT_VERSION << ", "
        << "\"summary\": { \"foo\": 47 } " << ", "
        //<< "\"markers\": { \"mk1\": [ 123123, ] } " << ", ";
        << "\"markers\": { [ 123123, \"tag1\", 12.671 ] } " << ", "
        << "\"annotations\": [ CIRCLE, 123123, 11.14, 900, 0.04, \"grp2\" ] " << ", "
        << "\"series\": { "
        << "  \"s1\": { \"conf\": {}, \"data\": [] } } " << ", "
        << "  \"s2\": { \"conf\": {}, \"data\": [] } } "
        << "}  "
        << "}";

}

template <>
void QuantStudyContext<false>::close_plot_lap () {
}

template <>
void QuantStudyContext<true>::close_plot_lap () {
    if ( is_plot_headerized == false ) {
        cerr << "close_plot_lap - not inited done yet" << "\n";

        plot_buf << "], [";    // Close the header, pad with empty arr for last comma

        //last_plot_colors = new Str[plot_count];
        is_plot_headerized = true;

    //} else {
    //    plot_buf << "], [";    // Close the header, pad with empty arr for last comma
    } else {
        assert( plot_ix == plot_count );
        assert( plot_count <= MAX_PLOTS );
        plot_ix = 0;
    }

}

template <>
void QuantStudyContext<false>::finalize_plot () {
}

template <>
void QuantStudyContext<true>::finalize_plot () {
    plot_buf << "] ]";

    plot_stream << plot_buf.str();

    cerr << plot_buf.str();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <>
void QuantStudyContext<false>::set_plot_group ( Str group ) {
}
#pragma GCC diagnostic pop

template <>
void QuantStudyContext<true>::set_plot_group ( Str group ) {
    current_plot_group = group;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <>
void QuantStudyContext<false>::plot_ohlc ( QuantPeriodization &a, Str b, Str c, Str d, Str e, Str f, bool g ) {
}
#pragma GCC diagnostic pop

//#include "QuantPeriodization.hh"    // Solely used for plot_ohlc atm. Not necessary by any means...

template <>
void QuantStudyContext<true>::plot_ohlc (
    QuantPeriodization &per,
    Str buffer_name,
    Str up_body_color,
    Str down_body_color,
    Str up_border_color,
    Str down_border_color,
    bool dynamic_color
) {
    if ( is_plot_headerized ) {     // *TODO* switch out a function ptr as soon as init is done..
        //cerr << "Plot is inited" << "\n";

        //plot_batch[plot_ix] = value;
        bool color_changed = false;

        if ( dynamic_color ) {
            Str new_colors = up_body_color + down_body_color + up_border_color + down_border_color;
            if ( ( color_changed = ( last_plot_colors[plot_ix] != new_colors ) ) == true ) {
                last_plot_colors[plot_ix] = new_colors;
            }
        }

        if ( color_changed ) {
            plot_buf << " [";
        }

        // *TODO*
        //plot_buf << " [" << per.open[0] << ",\"" << per.high[0] << ",\""
        //        << per.low[0]  << ",\"" << per.close[0] << "] ";

        if ( color_changed ) {
            plot_buf << ", [\"" << up_body_color << "\",\"" << down_border_color << "\",\""
                << up_border_color << "\",\"" << down_border_color << "\"] ] ";
        }

        plot_separator();

    } else {
        cerr << "Builds plot header" << "\n";
        plot_buf
            << enum_plot_type::OHLC << "," << "\"" << buffer_name << "\",\""
            << up_body_color << "\",\"" << down_body_color << "\",\""
            << up_border_color << "\",\"" << down_border_color << "\","
            << "], [";
        ++plot_count;

    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <>
void QuantStudyContext<false>::plot ( QuantReal a, QuantReal b, Str c, Str d, Str e, Str f, int g, enum_plot_type h, enum_line_style i, bool j ) {
}
#pragma GCC diagnostic pop

template <>
void QuantStudyContext<true>::plot (
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
) {
    // If we're not inited we sacrifice the very first value in order to
    // establish plot count first.
    if ( is_plot_headerized ) {     // *TODO* switch out a function ptr as soon as init is done..
        //cerr << "Plot is inited" << "\n";

        bool color_changed = false;

        if ( dynamic_color ) {
            Str new_colors = color1 + color2 + fill_color;
            if ( ( color_changed = ( last_plot_colors[plot_ix] != new_colors ) ) == true ) {
                last_plot_colors[plot_ix] = new_colors;
            }
        }

        if ( color_changed ) {
            plot_buf << " [";
        }

        plot_buf << " [" << value1 << "," << value2 << "]";

        if ( color_changed ) {
            plot_buf << ", [\"" << color1 << "\",\"" << color2 << "\",\"" << fill_color << "\"]";
        }

        plot_separator();

    } else {
        cerr << "Builds plot header" << "\n";
        plot_buf << type << "," << "\"" << buffer_name << "\",\"" << color1 << "\",\"" << color2 << "\",\"" << fill_color << "\"," << thickness << "," << line_style << "], [";
        ++plot_count;
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <>
void QuantStudyContext<false>::plot ( QuantReal a, Str b, Str c, int d, enum_plot_type e, enum_line_style f, bool g ) {
}
#pragma GCC diagnostic pop

template <>
void QuantStudyContext<true>::plot (
    QuantReal value,
    Str buffer_name,
    Str color,
    int thickness,
    enum_plot_type type,
    enum_line_style line_style,
    bool dynamic_color
) {

    // *TODO*
    // * C++ is faster than JS
    // * Plotting is only done when values are needed outside (most commonly from JS)
    // * Optimizing for speed is _not_ necessary in this code path
    // * -> It is therefor better to format the JSON properly in C++, before
    //      passing to outer data space (nominally; JS usage)
    //
    //      plot v                  // line, [bar, histogram], circle, diamond, cross, plus, dash, square
    //      zone v1, v2
    //      ohlc o, h, l, c
    //      ohlcm o, h, l, c, m
    //

    // If we're not inited we sacrifice the very first value in order to
    // establish plot count first.
    if ( is_plot_headerized ) {     // *TODO* switch out a function ptr as soon as init is done..
        //cerr << "Plot is inited" << "\n";

        //plot_batch[plot_ix] = value;
        if ( dynamic_color && ( last_plot_colors[plot_ix] != color ) ) {
            plot_buf << " [" << value << ",\"" << color << "\"] ";
            last_plot_colors[plot_ix] = color;
        } else {
            plot_buf << value;
        }

        if ( ++plot_ix < plot_count ) {     // Stack up values
            plot_buf << ",";
        } else {                            // Finsh of a block and start a new one
            plot_buf << "], [";
        }

    } else {
        cerr << "Builds plot header" << "\n";
        plot_buf << type << "," << "\"" << buffer_name << "\",\"" << color << "\"," << thickness << "," << line_style << "], [";
        ++plot_count;
    }
}
