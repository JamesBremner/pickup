#include <iostream>
#include "tcp.h"

class cGUI
{
public:
    cGUI();
    void run();

private:
    wex::gui &myForm;
    wex::label &labelIP;
    wex::editbox &editIP;
    wex::label &labelPort;
    wex::editbox &editPort;
    wex::button &myConnectbn;
    wex::button &myCalcBn;
    wex::button &mySimBn;
    wex::label &myStatus;
    wex::tcp &myTCP;
    SOCKET *myClientSocket;

    void status(const std::string &msg);
    void connect();
};

cGUI::cGUI()
    : myForm(wex::maker::make()),

      myConnectbn(wex::maker::make<wex::button>(myForm)),
      labelIP(wex::maker::make<wex::label>(myForm)),
      editIP(wex::maker::make<wex::editbox>(myForm)),
      labelPort(wex::maker::make<wex::label>(myForm)),
      editPort(wex::maker::make<wex::editbox>(myForm)),
      myCalcBn(wex::maker::make<wex::button>(myForm)),
      mySimBn(wex::maker::make<wex::button>(myForm)),
      myStatus(wex::maker::make<wex::label>(myForm)),

      myTCP(wex::maker::make<wex::tcp>(myForm))
{
    myForm.move(50, 50, 400, 300);
    myForm.text("Pickup GUI");

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
    myForm.show();
}
void cGUI::connect()
{
    try
    {
        myTCP.client(
            editIP.text(),
            editPort.text() );
        status("Connected to server ");
        myTCP.read();
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
    myForm.run();
}
int main()
{

    cGUI gui;
    gui.run();
    return 0;
}