/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QuantFeed.hh"


#define COUNT 3

/*
 *
long long fakeData[COUNT] = {
    { 1410306056861, 135000, 135004 },
    { 1410306056872, 135003, 135007 },
    { 1410306059003, 135108, 135111 }
};

*/

class QuantSequentialData
{
  public:
    QuantSequentialData (QuantTime start_date, QuantTime end_date);
    ~QuantSequentialData ();
    bool poll ();

  private:
    QuantTime start_date, end_date;
    int pos;

};

bool QuantSequentialData::poll ()
{
    if ( pos >= COUNT )
        return false;

    //QuantTime raw_date = pxt::millisec(474747);
    QuantTime raw_date = 474747;

    if ( false )        // *TODO* place holder.. "if no data yet"
        return false;

    if ( raw_date > end_date )
        return false;

    return true;

}

QuantSequentialData::QuantSequentialData (QuantTime start_date, QuantTime end_date)
    :
        start_date(start_date),
        end_date(end_date),
        pos(0)
{

}

QuantSequentialData::~QuantSequentialData ()
{

}



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

QuantFeed::QuantFeed (
        QuantStudyContext &context,
        Str broker_id,
        Str symbol_id,
        //Mantissation &mantisator,
        QuantTime start_date
)   :
        ticks(5000),
        context(context),
        broker_id(broker_id),
        symbol_id(symbol_id),
        start_date(start_date),
        end_date(0)
{
    context.addFeed( this );

    // THIS is a continual operation - start_dateing with historical, and then
    // tailing the now continously

    if (start_date) {
        // Setup historical data reading phase first

    } else {
        // Start trailing actively right off the bat

    }

}

QuantFeed::QuantFeed (
        QuantStudyContext &context,
        Str broker_id,
        Str symbol_id,
        //Mantissation &mantisator,
        QuantTime start_date,
        QuantTime end_date
)   :
        ticks(5000),
        context(context),
        broker_id(broker_id),
        symbol_id(symbol_id),
        start_date(start_date),
        end_date(end_date)
{
    context.addFeed(this);

}


QuantFeed::~QuantFeed ()
{

}


// In event of asynchronous live data
void QuantFeed::handleData ()
{


}


bool QuantFeed::readNext ()
{


    // read the next value into buf, and return true, else if no more vals, false
    return false;

}




 *TODO* This should probably be in it's own class instead, this way we'll
// just have an 'interface' class IN the quantfeed class, everything implementation
// specific is in that, only readNext() and emit() is then public _and_ common.
int QuantFeed::loadNextSegment ()
{


    // load the next hourly segment from disk
    char *filename = make_a_fucking_str(data_root_dir, '/', symbol, '/', "DUKASCOPY", '/', time.year, '/', time.month - 1, '/', time.date, '/', time.hour, ".bi5")

    int size;
    char *buf = n47::dukascopy::read(filename, &size);

    return -2;
}




void QuantFeed::emit ()
{
    onPreTick.emit();
    onTick();
}

void QuantFeed::setLiveMode ()
{
    state = LIVE;

    // Do some other shit like setting up listeners etc. (should've been done earlier probably though) *TODO*

}