#include <iostream>
#include <iomanip>
#include "tcp.h"
#include "propertygrid.h"
#include "raven_sqlite.h"
#include "config.h"

class cGUI
{
public:
    cGUI();
    void run();

private:
    wex::gui &myForm;
    wex::tabbed &tabs;
    wex::panel &pnlServer;
    wex::label &labelIP;
    wex::editbox &editIP;
    wex::label &labelPort;
    wex::editbox &editPort;
    wex::button &myConnectbn;
    wex::button &myCalcBn;
    wex::button &mySimBn;
    wex::label &myStatus;

    wex::panel &pnlZone;
    wex::propertyGrid &pg;
    wex::button &bnZone;

    wex::panel &pnRest;
    wex::label &lbRest;
    wex::editbox &edRest;

    wex::tcp &myTCP;

    void costructZonePG(wex::propertyGrid &pg);
    void constructResults();
    void status(const std::string &msg);
    void connect();
};

cGUI::cGUI()
    : myForm(wex::maker::make()),

      tabs(wex::maker::make<wex::tabbed>(myForm)),
      pnlServer(wex::maker::make<wex::panel>(tabs)),
      myConnectbn(wex::maker::make<wex::button>(pnlServer)),
      labelIP(wex::maker::make<wex::label>(pnlServer)),
      editIP(wex::maker::make<wex::editbox>(pnlServer)),
      labelPort(wex::maker::make<wex::label>(pnlServer)),
      editPort(wex::maker::make<wex::editbox>(pnlServer)),
      myCalcBn(wex::maker::make<wex::button>(pnlServer)),
      mySimBn(wex::maker::make<wex::button>(pnlServer)),
      myStatus(wex::maker::make<wex::label>(pnlServer)),

      pnlZone(wex::maker::make<wex::panel>(tabs)),
      pg(wex::maker::make<wex::propertyGrid>(pnlZone)),
      bnZone(wex::maker::make<wex::button>(pnlZone)),

      pnRest(wex::maker::make<wex::panel>(tabs)),
      lbRest(wex::maker::make<wex::label>(pnRest)),
      edRest(wex::maker::make<wex::editbox>(pnRest)),

      myTCP(wex::maker::make<wex::tcp>(myForm))
{
    myForm.move(50, 50, 400, 400);
    myForm.text("Pickup GUI");

    // construct tabbed panel
    tabs.move(0, 0, 400, 400);
    tabs.tabWidth(120);
    tabs.add("Server", pnlServer);
    tabs.add("Zone", pnlZone);
    tabs.add("Results", pnRest);

    myConnectbn.move({50, 50, 100, 30});
    myConnectbn.text("Connect");

    myStatus.move(50, 100, 300, 30);
    myStatus.text("Not connected");

    myConnectbn.events().click([this]
                               { connect(); });

    labelIP.move({5, 15, 45, 30});
    labelIP.text("Server IP");
    editIP.move({60, 10, 100, 20});
    editIP.text("127.0.0.1");

    labelPort.move({200, 15, 60, 30});
    labelPort.text("Server Port");
    editPort.move({260, 10, 80, 20});
    editPort.text("5000");

    myCalcBn.move(50, 150, 100, 30);
    myCalcBn.text("Calculate");
    myCalcBn.events().click([&]
                            { myTCP.send("calc"); });
    mySimBn.move(180, 150, 100, 30);
    mySimBn.text("Simulate");
    mySimBn.events().click([&]
                           { myTCP.send("simu"); });

    costructZonePG(pg);
    constructResults();

    myForm.events()
        .tcpServerAccept([&]
        {
            std::cout << "=> myForm.events().tcpServerAccept" << std::endl;
            status("Connected to server ");
        });

    myForm.events()
        .tcpRead([&]
                 {
                     if (!myTCP.isConnected())
                     {
                         status("Not connected");
                     }
                 });

    myForm.show();
    tabs.select(0);
}

void cGUI::constructResults()
{
    pnRest.fontName("courier");
    pnRest.fontHeight(18);
    lbRest.move({10, 10, 120, 30});
    lbRest.text("Restaurant #");
    edRest.move({150, 10, 100, 30});
    edRest.text("");
    edRest.events().change(edRest.id(), [this]
                           {
                               std::cout << edRest.text() << "\n";
                               int index = atoi(edRest.text().c_str());
                               if (0 >= index)
                                   return;
                               static std::string dbname("C:/ProgramData/RavensPoint/Pickup/pickup.dat");
                               raven::sqlite::cDB db(dbname.c_str());
                               if (!db.getHandle())
                                   return;

                               // get restaurant location
                               db.Query("SELECT * FROM restaurant WHERE rowid = %d",
                                        index);
                               if (db.myResultA.size() != 2)
                                   return;
                               float restX = atof(db.myResultA[0].c_str());
                               float restY = atof(db.myResultA[1].c_str());

                               // get stack of orders
                               db.Query(
                                   "SELECT rowid FROM stacks WHERE rst = %d;",
                                   index);
                               int stackIndex = atoi(db.myResultA[0].c_str());

                               // get rider
                               db.Query(
                                   "SELECT * FROM rider WHERE rowid = "
                                   " ( SELECT rider FROM stacks WHERE rowid = %d );",
                                   stackIndex);
                               float riderX = atof(db.myResultA[0].c_str());
                               float riderY = atof(db.myResultA[1].c_str());

                               // get deliveries
                               db.Query(
                                   "SELECT x, y FROM orderHolder WHERE rowid IN "
                                   " (SELECT orderIndex FROM route WHERE stack = %d );",
                                   stackIndex);

                               std::stringstream ss;
                               ss << std::setprecision(2) << std::fixed << "\n\n\n"
                                  << "\n  Restaurant at " << restX << ", " << restY
                                  << "\n  rider at      " << riderX << ", " << riderY
                                  << "\n  deliveries to ";
                               for (int k = 0; k < db.myResultA.size(); k += 2)
                                   ss
                                       << "\n                " << atof(db.myResultA[k].c_str()) << ", " << atof(db.myResultA[k + 1].c_str());

                               pnRest.text(ss.str());
                               pnRest.update();
                           });
}
void cGUI::costructZonePG(wex::propertyGrid &pg)
{
    static std::string dbname("C:/ProgramData/RavensPoint/Pickup/pickup.dat");

    pg.move({20, 50, 200, 200});
    pg.text("Zone Parameters");
    pg.string("Dimension (km)", "25");
    pg.string("Restaurants", "5000");
    pg.string("Orders / Hour", "20000");
    pg.string("Group Time (mins)", "15");
    pg.string("Max Prep Time (mins)", "15");
    pg.string("Riders", "5000");
    pg.string("Pickup Window (mins)", "5");
    pg.string("Rider Dist (km)", "5");

    bnZone.move({50, 300, 100, 30});
    bnZone.text("SAVE");
    bnZone.events().click([&]
                          {
                              raven::sqlite::cDB db(dbname.c_str());
                              pup::cConfig cfg;
                              cfg.ZoneDimKm = atof(pg.find("Dimension (km)")->value().c_str());
                              cfg.CloseRiderDistanceKm = atof(pg.find("Rider Dist (km)")->value().c_str());
                              cfg.GroupTimeMins = atof(pg.find("Group Time (mins)")->value().c_str());
                              cfg.MaxPrepTimeMins = atof(pg.find("Max Prep Time (mins)")->value().c_str());
                              cfg.OrdersPerHour = atof(pg.find("Orders / Hour")->value().c_str());
                              cfg.PickupWindowMins = atof(pg.find("Pickup Window (mins)")->value().c_str());
                              cfg.RiderCount = atof(pg.find("Riders")->value().c_str());
                              cfg.RestaurantCount = atof(pg.find("Restaurants")->value().c_str());
                              cfg.write(db);
                          });

    raven::sqlite::cDB db(dbname.c_str());
    if (!db.getHandle())
        return;

    db.Query("SELECT * FROM config;");
    if (db.myColCount != 8)
        return;

    pg.find("Dimension (km)")->value(db.myResultA[0]);
    pg.find("Rider Dist (km)")->value(db.myResultA[1]);
    pg.find("Orders / Hour")->value(db.myResultA[2]);
    pg.find("Group Time (mins)")->value(db.myResultA[3]);
    pg.find("Restaurants")->value(db.myResultA[4]);
    pg.find("Pickup Window (mins)")->value(db.myResultA[5]);
    pg.find("Max Prep Time (mins)")->value(db.myResultA[6]);
    pg.find("Riders")->value(db.myResultA[7]);

    db.Close();
}
void cGUI::connect()
{
    try
    {
        myTCP.client(
            editIP.text(),
            editPort.text());
        status("Waiting for server");
    }
    catch (std::runtime_error &e)
    {
        status(std::string("Cannot connect to server ") + e.what());
    }
}
void cGUI::status(const std::string &msg)
{
    myStatus.text(msg);
    myStatus.update();
}
void cGUI::run()
{
    myTCP.run();
    myForm.run();
}
int main()
{
    cGUI gui;
    gui.run();
    return 0;
}