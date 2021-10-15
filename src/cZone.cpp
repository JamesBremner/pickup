
#include <algorithm>
#include <filesystem>
#include "cRunWatch.h"
#include "sqlite3.h"
#include "raven_sqlite.h"
#include "cZone.h"
namespace pup
{
    cRestaurant::cRestaurant(float x, float y)
    {
        myLocation.first = x;
        myLocation.second = y;
    }
    cRestaurant::cRestaurant(int dim)
    {
        myLocation.first = (rand() % (dim * 100)) / 100.0;
        myLocation.second = (rand() % (dim * 100)) / 100.0;
        //std::cout << " rest " << myLocation.first <<","<< myLocation.second << " ";
    }

    cZone::cZone()
        : myRiders(myConfig)
    {
        raven::set::cRunWatch::Start();
        raven::set::cRunWatch::Clear();
    }

    void cZone::readDB()
    {
        raven::set::cRunWatch aWatcher("\tRead DB");
        raven::sqlite::cDB db("C:/ProgramData/RavensPoint/Pickup/pickup.dat");
        if (!db.getHandle())
        {
            std::cout << "\nExternal source of orders not available\n"
                         "Do you want to simulate a restaurant zone?\n"
                         "Type:  pickup -sim\n\n";
            exit(1);
        }
        myConfig.read(db);
        myRestaurants.read(db);
        myOrders.read(db, this);
        myRiders.read(db);
    }

    void cZone::readConfig()
    {
        // set default config values
        // in case problem reading DB
        InitConfig();

        raven::set::cRunWatch aWatcher("\tRead DB");
        raven::sqlite::cDB db("C:/ProgramData/RavensPoint/Pickup/pickup.dat");
        if (!db.getHandle())
            return;
        myConfig.read(db);
    }

    void cZone::simulate()
    {
        std::cout << "Simulating\n";

        readConfig();

        myRestaurants.simulate(myConfig);

        myOrders.simulate(this);

        myRiders.simulate();
    }

    void cZone::writeDB()
    {
        raven::set::cRunWatch aWatcher("\tWrite DB");
        std::filesystem::create_directories("C:/ProgramData/RavensPoint/Pickup");
        raven::sqlite::cDB db("C:/ProgramData/RavensPoint/Pickup/pickup.dat");
        myRestaurants.write(db);
        myOrders.write(db, this);
        myRiders.write(db);
        myConfig.write(db);
    }
    void cZone::stacksWriteDB()
    {
        raven::set::cRunWatch aWatcher("\tWrite stacks");
        std::filesystem::create_directories("C:/ProgramData/RavensPoint/Pickup");
        raven::sqlite::cDB db("C:/ProgramData/RavensPoint/Pickup/pickup.dat");
        myStacks.write(db,this);
    }

    void cZone::InitConfig()
    {
        myConfig.OrdersPerHour = 20000;  // incoming order per hour
        myConfig.GroupTimeMins = 5;      // order collection time
        myConfig.RestaurantCount = 5000; // number of restaurants
        myConfig.PickupWindowMins = 5;   // pickup window time
        myConfig.MaxPrepTimeMins = 15;   // maximum order preparation time
        myConfig.ZoneDimKm = 25;         // zone dimension
        myConfig.CloseRiderDistanceKm = 10;

        myConfig.RiderCount = myConfig.RestaurantCount; // one rider per restaurant

        myConfig.calculate();
    }
    void cZone::Sort()
    {
        /// sort orders by time
        std::sort(
            myOrders.myOrder.begin(), myOrders.myOrder.end(),
            [](pup::cOrder &a, pup::cOrder &b)
            {
                return (a.myTime < b.myTime);
            });

        /// sort order by restaurant, preserving time order
        std::stable_sort(
            myOrders.myOrder.begin(), myOrders.myOrder.end(),
            [](const pup::cOrder &a, const pup::cOrder &b)
            {
                return (a.myRest < b.myRest);
            });
    }
    /// find resteraunt with earlier ready order
    cRestaurant *cZone::FindRestFirstNextPickup()
    {
        cRestaurant *nextRest = 0;
        int nextPickup = INT32_MAX;
        for (auto &R : myRestaurants)
        {
            for (auto &order : myOrders)
            {
                if (order.myRest != &R)
                    continue;
                if (!order.myWaiting)
                    continue;
                if (order.myTime < nextPickup)
                {
                    nextRest = order.myRest;
                    nextPickup = order.myTime;
                    if (nextPickup == 0)
                        break;
                }
                else
                {
                    continue;
                }
            }
            if (nextPickup == 0)
                break;
        }
        return nextRest;
    }
    int cZone::orderStack()
    {
        raven::set::cRunWatch aWatcher("\tStack orders");

        myStacks.clear();

        int stackCount = 0;

        // sort
        Sort();

        // loop until all orders picked up
        while (1)
        {
            // resteraunt with earliest ready order
            auto nextRest = FindRestFirstNextPickup();
            if (!nextRest)
                break; // all orders picked up

            // pickup some orders from resteraunt
            myStacks.add(PickupOrders(nextRest));

            stackCount++;
        }

        return stackCount;
    }

    pup::cStack
    cZone::PickupOrders(cRestaurant *rest)
    {
        pup::cStack S;
        int found = false;
        // loop over orders
        for (auto &O : myOrders)
        {
            // check for order to restaurant following required restaurant
            if (found && O.myRest != rest)
                break;

            // check for waiting order to required resteraunt
            if (O.myRest == rest && O.myWaiting)
            {
                found = true;
                // order ready too late to be picked up
                if (O.myTime > myConfig.PickupWindowSecs)
                    break;

                // add order to stack
                S.add(O);
                O.myWaiting = false;
            }
        }

        //std::cout << "Picked up " << S.text( this ) << "\n";

        return S;
    }

    void cZone::delivery()
    {
        for (auto &S : myStacks)
        {
            S.deliveryLocations();
        }
    }

    void cZone::assignRiders()
    {
        for (auto &S : myStacks)
        {
            myRiders.assign(S);
        }
    }

    void cZone::Report()
    {
        std::cout
            << "Zone dimension Km                " << myConfig.ZoneDimKm
            << "\nOrders per hour                  " << myConfig.OrdersPerHour
            << "\nOrder collection time mins     " << myConfig.GroupTimeMins
            << "\nRestaurants                    " << myConfig.RestaurantCount
            << "\nPickup window mins             " << myConfig.PickupWindowMins
            << "\nMaximum order preparation mins " << myConfig.MaxPrepTimeMins
            << "\nMaximum distance of rider Km   " << myConfig.CloseRiderDistanceKm << "\n";

        std::cout << stackCount() << " order stacks created\n";

        // // detailed report of first 5 order stacks
        // int count = 0;
        // for (auto &stack : myStacks)
        // {
        //     std::cout << stack.text(this) << "\n";
        //     count++;
        //     if (count >= 5)
        //         break;
        // }

        // timing report
        raven::set::cRunWatch::Report();
    }

    float cZone::distance(
        float x1, float y1,
        float x2, float y2,
        eDistMethod m)
    {
        switch (m)
        {
        case eDistMethod::manhatten:
            return std::fabs(x1 - x2) + std::fabs(y1 - y2);

        case eDistMethod::manhatten_haversine:
            // TODO Implement manhatten_haversine
            throw std::runtime_error("manhatten_haversine distance NYI");

        default:
            std::cout << m << "\n";
            throw std::runtime_error("Unrecognized distance calculation method");
        }
    }

}
