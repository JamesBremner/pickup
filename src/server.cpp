#include <winsock2.h>
#include "wex.h"
#include "tcp.h"
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

    // get restaurants, drivers and orders for a zone from external source
    theZone.readDB();

    // stack orders
    theZone.orderStack();

    // optimize delivery routes
    theZone.delivery();

    // assign riders
    theZone.assignRiders();

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
                                       myClientSocket = &theTCP.clientSocket();
                                       theTCP.read(theTCP.clientSocket());
                                   });

    // handle message from client
    thefm.events()
        .tcpServerReadComplete([&]
                               {
                                   std::string smsg(theTCP.rcvbuf());
                                   if (smsg == "simu")
                                       simulate();
                                   else if (smsg == "calc")
                                       calculate();
                                   else
                                       std::cout << "Msg read: " << smsg << "\n";

                                   // setup for next message
                                   theTCP.read(theTCP.clientSocket());
                               });

    std::cout << "start server running\n";

    thefm.run();
}
