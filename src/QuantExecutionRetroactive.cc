#ifdef INTERFACE
/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantBase.hh"
#include "QuantBuffersBase.hh"
#include "QuantBuffersIntertwinedHeap.hh"
#include "QuantBuffersIntertwinedBufferAbstract.hh"
#include "QuantFeedAbstract.hh"
#include "QuantPeriodizationAbstract.hh"
#include "QuantStudyContextBase.hh"
#include "QuantExecutionContext.hh"


/*
 *
 *
 * PSEUDO CODE FROM THubV8....

feed::poll() -> Record
    rec-count = SQB_IndexFile_mmap_buf[REC_COUNT_OFFSET]

    ref last_rec = records[last]

    if record-pos == rec-count
        last_rec.time = 0
        return ref last_rec

    read_tick(last_rec, SQB_payload_mmap_buf)
    return ref last_rec



# catch up loop
# while record-pos < rec-count
#    read-record()

# LIVE loop - do the regular closest-value looping here too? Maybe so!
# In that case rec.time = timetype::max() is special case of "awaiting values"


if TPL_CONST_USE_SIGNALS {
    #include <csignal>

    sigset_t sig_mask;
    sigset_t orig_mask;
    timespec timeout;      // must be set to the smallest periodicity interval of all feeds. Or just go with 1 second (will NEVER (famous last words) use less then 1s periodizity. Too unsafe for workable strategies given network latency and murphys law.)
    pid_t pid;

    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, SIG X X X );
}

for ever {

    ts = QuantTime::max()  # we must reset each time since we can get out of order timestamps on different feeds b/c of network latencies

    // test all feeds for new data
    for (auto feed : feeds) {
        test_ts = feed.check_next() // Do not advance. feeds advance when they"ve been "emitted"
        if test_ts < ts
            ts = test_ts
            closest_feed = feed
    }

    if ts == max_time   # "no data available yet"
        if TPL_CONST_USE_SIGNALS
            sigtimedwait(&wait_sig_mask, ) some-confed-amount-of-time     # let the CPU rest a bit before we check for new data again... use as long time as we can afford strategy latency wise..
        else
            sleep for 100ms or something
    else
        tick = feed.consume();
        feed.emit();

}


*/




enum retro_mode { BACKTEST = 0, FULL };

class QuantExecutionRetroactive : public QuantExecutionContext
{
  public:
    QuantExecutionRetroactive(QuantTime start_date, QuantTime end_date,
                              bool buffers_output_enabled,
                              bool is_optimization);
    ~QuantExecutionRetroactive();

    void add(QuantStudyContextAbstract * study);
    void add(QuantFeedAbstract * feed);
    void add(QuantPeriodizationAbstract * periodization);

    int run(void);

  private:
    vector<QuantStudyContextAbstract *> studies;
    vector<QuantFeedAbstract *> feeds;
    vector<QuantPeriodizationAbstract *> periodizations;

    // Str                         symbol;
    retro_mode mode;
};

#endif

#include "QuantExecutionRetroactive.hh"
#include "QuantProfiling.hh"

QuantExecutionRetroactive::QuantExecutionRetroactive(
    QuantTime start_date, QuantTime end_date, bool buffers_output_enabled,
    bool is_optimization)
//: study ( study )
//, symbol ( symbol )
    : QuantExecutionContext{ start_date, end_date, buffers_output_enabled,
                             is_optimization },
      mode{ BACKTEST } {}

QuantExecutionRetroactive::~QuantExecutionRetroactive() {}

void QuantExecutionRetroactive::add(QuantStudyContextAbstract * study)
{
    studies.push_back(study);
}

void QuantExecutionRetroactive::add(QuantFeedAbstract * feed)
{
    // feed->set_date_range( start_date, end_date );
    feeds.push_back(feed);
}

void QuantExecutionRetroactive::add(QuantPeriodizationAbstract *
                                    periodization)
{
    // periodization->set_date_range( start_date, end_date );
    periodizations.push_back(periodization);
}

/*


* PSEUDO CODE FOR SIGNALLED LIVE FEED FOLLOWING:



const_expr auto QUANTTIME_MAX = numeric_limits<QuantTime>::max()


seq_raw_buf::

// load_next() -> poll for available pre-fetch, then set it active
// load(path)  -> prefetch(path) -> load_next()
// prefetch(path)
// init_page(path) -> init a new page on path, and set it active


seq_raw_buf::load_next() ->
    while prefetch_bufs.size() == 0
        sleep 47

    munmap(mmap_buf)
    mmap_buf = prefetch_bufs.pop_front();




quff_buf::next_page() ->
    seq_raw_buf.set_path(next_page_path);
    seq_raw_buf.load();


    // *TODO* use n prefetch-steps. at first startup of quff_buf fetch up to n
    // on continous next_page:
    //  - un-map the current buf.
    //  - load() by pointing to the next pre-fetched, then
    //  - pre-fetch the next n'th page
    // pre-fetch is a feature of seq_raw_buf. But pre-fetching is managed
    // outside of it. A current buffer is thrown away when taking the next
    // (whether pre-fetched or loaded - used == disposed - no 'back caching')

    if prefetch_page_count > 0
        prefetch_ts = current_page_ts + (page_duration * prefetch_page_count)

        if suggested_work_range_end > prefetch_ts
            tmp_page_path = generate_page_path(prefetch_ts)
            seq_raw_buf.prefetch(tmp_page_path);


inline
quff_buf::read_record_count() -> size_t
    return seq_raw_buf[REC_COUNT_VAR_OFFSET]




feed::load_page() ->


feed::read_next() -> Record &

    rec & last_rec = record-buf[++record_buf_ix]

    if record-pos < rec-count
        read_quff_record(last_rec, quff_buf)    // "read_tick"
        return last_rec

    else
        if this-is-historic-quff-page // non-current / active quff-page
            load-next-fucking-page()
            return read_next()

        else
            // try re-reading the count-value, if it's been updated with new
data

            rec-count = quff_buf.read_record_count()

            if record-pos < rec-count
                return read_next()
            else
                --record_buf_ix         // re-use the record at next read since
this return is more of a "return message"
                rec.ts = QUANTTIME_MAX
                return rec




# catch up loop
# while record-pos < rec-count
#    read-record

# LIVE loop - do the regular closest-value looping here too? Maybe so!
# In that case rec.time = timetype::max() is special case of "awaiting values"


#include <csignal>

sigset_t sig_mask;
sigset_t orig_mask;
timespec timeout;      // must be set to the smallest periodicity interval of
all feeds. Or just go with 1 second (will NEVER (famous last words) use less
then 1s periodizity. Too unsafe for workable strategies given network latency
and murphys law.)
pid_t pid;

sigemptyset(&sig_mask);
sigaddset(&sig_mask, SIG X X X );

while (true) {

    ts = QUANTTIME_MAX  # we must reset each time since we can get out of order
timestamps on different feeds b/c of network latencies

    // test all feeds for new data
    for (auto feed : feeds) {
        test_ts = feed.check_next() // Do not advance. feeds advance when
they"ve been "emitted"
        if test_ts < ts
            ts = test_ts
            nearest_feed = feed
    }

    if ts == QUANTTIME_MAX   # "no data available yet"
        sigtimedwait(&wait_sig_mask, some-confed-amount-of-time)     # let the
CPU rest a bit before we check for new data again... use the longest time
allowed considering ghost-tick-periodicities

    else
        // tick = nearest_feed.consume();
        nearest_feed.emit();

}


*/

int QuantExecutionRetroactive::run(void)
{
    cerr << "\n- - - - -\n";
    cerr << "QuantExecutionRetroactive::run\n\n";
    vector<QuantFeedAbstract *> study_feeds; // so that we can remove feeds as
    // they reach their last values -
    // might change the implementation
    // to check them every loop - will
    // only affect computations in the
    // very last minute of trading
    // data...  - 2014-09-11/ORC(20:51)
    for (auto study : studies) {
        study->prepareRun();
        study->init();
        for (auto per : study->the_jar.periodizations) {
            per->buffer_heap.allocate();
        }
        study_feeds.insert(study_feeds.end(), study->the_jar.feeds.begin(),
                           study->the_jar.feeds.end());
    }
    // Prime feeds by fetching next available tick in each of them
    for (auto f : study_feeds) {
        cerr << "calls readNext on each feed for priming\n";
        f->set_date_range(start_date, end_date);
        f->readNext(); // This will cause loads for each feed, unfortunately
        // waiting sequentially for each before calling fs for
        // the next..     - 2014-09-11/ORC(20:54)
        _Dn(f->ticks().time << " " << f->ticks().to_str());
    }
    cerr << "Feed count is " << study_feeds.size() << "\n";
    // if (state == HISTORICAL )     // There's ALWAYS historical data reading
    // at first, nothing else makes sense really (the pre-load may be short, but
    // will always be there) -  - 2014-09-11/ORC(20:56)
    //# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    //# # #
    //# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    //# # #
    // Historical / backtest event loop  - 2014-09-11/ORC(21:03)
    QuantFeedAbstract * nearest_feed = nullptr;
    QuantTime lowest_time;
    cerr << "\n- - - - -\n";
    cerr << "QuantExecutionRetroactive - Begins spool to start-time pre loop"
         << "\n";
// // // Pre loop - spool to start time point // // //
// spool forward til we're at the requested starting point in time -
// 2014-10-27/ORC
// replaces "if ( lowest_time >= start_date ) {"  around  "emit()" below.
    #ifdef IS_DEBUG
    int skipped_ticks_count = 0;
    profile_local();
    #endif
    while (true) {
        lowest_time = pxt::max_date_time;
        nearest_feed = nullptr;
        // Find the tick closest in time
        for (auto f : study_feeds) {
            // if ( ! f ) continue;
            // || lowest_time == pxt::not_a_date_time ) {
            if (f->ticks().time < lowest_time) {
                lowest_time = f->ticks().time;
                nearest_feed = f;
            }
        }
        if (lowest_time < start_date) { // Not yet at start time
            if (nearest_feed && nearest_feed->readNext() == true) {
                #ifdef IS_DEBUG
                ++skipped_ticks_count;
                #endif
                continue;
            }
            else {
                cerr << "\n\nQuantExecutionRetroactive - OUT OF TICK DATA "
                     "ALREADY IN PRE-LOOP!!"
                     << "\n\n";
                break;
            }
        }
        else {   // We've found the beginning tick
            break;
        }
    }
    #ifdef IS_DEBUG
    cerr << "QuantExecutionRetroactive - Found beginning tick. Skipped "
         << skipped_ticks_count << " ticks, took " << profile_local() << "\n";
    #endif
    cerr << "\n- - - - -\n";
    cerr << "QuantExecutionRetroactive - Begins main history loop"
         << "\n";
    profiler.start(GENERAL);
    // // // Main history tick loop // // //
    while (true) {
        lowest_time = pxt::max_date_time;
        nearest_feed = nullptr;
        // cerr << "\nBacktest Eventloop : Find closest tick in time\n";
        // Find the tick closest in time
        for (auto f : study_feeds) {
            // if ( ! f ) continue;
            if (f->ticks().time < lowest_time) {
                lowest_time = f->ticks().time;
                nearest_feed = f;
            }
        }
        if (lowest_time <= end_date) {
            // cerr << "Got it. emit it\n";
            nearest_feed->emit(); // Let it kick of its' chain of events
            nearest_feed->readNext();
            /*
            if ( nearest_feed->readNext() == true ) {         // More
            _historical_ data in the feed?
                continue;

            } else {
                cerr << "retval was false, no more data\n";

                / *
                int count = study_feeds.size();

                for ( int ix = 0; ix < count; ++ix ) {
                    if ( study_feeds[ix] == nearest_feed ) {
                        study_feeds[ix] = nullptr;
                        //study_feeds.erase ( ix );
                        break;
                    }
                }
                * /

                //if ( study_feeds.size() == 0 ) {
                //    break;  // Break out of history/backtest event-loop
                //}
            }
            */
        }
        else {
            cerr << "Past end date of backtesting date range - quits."
                 << "\n";
            cerr << (nearest_feed->ticks().time) << " vs "
                 << (nearest_feed->ticks().time) << " vs " << end_date << "\n";
            break;
        }
    }
    //# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    //# # #
    // Change "mode" to live _if_ not pure backtest that is
    if (mode == BACKTEST) {
        profiler.end(GENERAL);
        return 0;
    }
    cerr << "QuantExecutionRetroactive - Begins live tick listening loop"
         << "\n";
    for (auto f : feeds) {
        f->setLiveMode();
    }
    //# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    //# # #
    // Live event loop  - 2014-09-11/ORC(21:10)
    for (;;) {
        // run zmq-polls and such
        if (true) { // AKA "TODO"
            break;
        }
    }
    profiler.end(GENERAL);
    cerr << "QuantExecutionRetroactive - ends";
    return 0;
}
