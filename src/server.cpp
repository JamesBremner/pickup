#include <winsock2.h>
#include "raven_sqlite.h"
#include "cRunWatch.h"
#include "cZone.h"
#include "await.h"
#include "cTCP.h"

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

    raven::await::cAwait waiter;

    // construct TCP server
    raven::set::cTCP theTCP;

void readHandler()
{
    if (!theTCP.isConnected())
    {
        // wait for another client
        theTCP.server("", "5000");
        return;
    }
    std::string smsg = theTCP.readMsg();
    if (smsg == "simu")
        simulate();
    else if (smsg == "calc")
        calculate();
    else
        std::cout << "Msg read: " << smsg << "\n";

    // setup for next message
    waiter(
        [&]
        { theTCP.read(); },
        readHandler);
}
main()
{


    theTCP.server("", "5000");

    waiter(
        [&]
        { theTCP.acceptClient(); },
        [&]
        {
            std::cout << "client connected" << std::endl;

            // wait for first message
            waiter(
                [&]
                { theTCP.read(); },
                readHandler);
        });

    waiter.run();
}
