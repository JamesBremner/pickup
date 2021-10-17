#include <winsock2.h>
#include "wex.h"
#include "tcp.h"
#include "raven_sqlite.h"
#include "cRunWatch.h"
#include "cZone.h"

void simulate()
{
    pup::cZone theZone;

    // simulate restaurants, drivers and orders for a zone
    theZone.simulate();

    // write simulated zone to database
    theZone.writeDB();

    theZone.Report();
}
void calculate()
{
    pup::cZone theZone;

    {
        raven::set::cRunWatch aWatcher("\tZone calculation");

        // get restaurants, drivers and orders for a zone from external source
        theZone.readDB();

        // stack orders
        theZone.orderStack();

        // optimize delivery routes
        theZone.delivery();

        // assign riders
        theZone.assignRiders();

        // save results to database
        theZone.stacksWriteDB();
    }

    theZone.Report();
}
main()
{
    // hidden frame to receive windows messages
    wex::gui &thefm = wex::maker::make();

    // construct TCP server
    wex::tcp &theTCP = wex::maker::make<wex::tcp>(thefm);
    theTCP.server("5000");

    // handle new client connection
    thefm.events().tcpServerAccept([&]
                                   {
                                       std::cout << "Client connected\n";
                                       theTCP.read();
                                   });

    // handle message from client
    thefm.events()
        .tcpRead([&]
                 {
                     if( ! theTCP.isConnected() )
                    {
                        // wait for another client
                        theTCP.server("5000");
                        return;
                    }
                     std::string smsg(theTCP.rcvbuf());
                     if (smsg == "simu")
                         simulate();
                     else if (smsg == "calc")
                         calculate();
                     else
                         std::cout << "Msg read: " << smsg << "\n";

                     // setup for next message
                     theTCP.read();
                 });

    std::cout << "start server running\n";

    thefm.run();
}
