/**
* Created:  2014-08-09
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

class GrapeWireBroker {
  public:

    getChannel(string unit_id, string topic, GWMsg msg)

}


GWPkg {
    int     unit_id;
    int     impl_id;
    int     machine_id;
    GWMsgId msg_id;
}


class GrapeWireChannel {
    GWMsgId     send( string topic, GWMsg msg )
    GWPkg       rcv( string topic )
    GWPkg       rcv( GWMsgId msg_id )

    GWMsgId     pub( string topic, GWMsg msg )
    GWPkg       sub( string topic )

}