
#include <algorithm>
#include "cRunWatch.h"
#include "cZone.h"
namespace pup
{
    cRestaurant::cRestaurant()
    {
        myLocation.first = (rand() % theZone.myConfig.ZoneDimKm * 100) / 100.0;
        myLocation.second = (rand() % theZone.myConfig.ZoneDimKm * 100) / 100.0;
        //std::cout << " rest " << myLocation.first <<","<< myLocation.second << " ";
    }


    cZone::cZone()
    : myRiders( myConfig )
    {
        raven::set::cRunWatch::Start();

    }

    void cZone::populate()
    {
        std::cout << 
        "\nExternal source of orders not available\n"
        "Do you want to simulate a restaurant zone?\n"
        "Type:  pickup -sim\n\n";
        exit(1);
    }

    void cZone::simulate()
    {
        std::cout << "Simulating\n";

        InitConfig();

        myRestaurants.resize(myConfig.RestaurantCount);

        /// Simulate orders generated in one collection time
        myOrders.clear();
        for (int o; o < myConfig.OrdersPerGroupTime; o++)
        {
            myOrders.push_back(pup::cOrder());
        }

        myRiders.simulate();
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

        myConfig.OrdersPerGroupTime =
            myConfig.GroupTimeMins * myConfig.OrdersPerHour / 60;
        myConfig.PickupWindowSecs = myConfig.PickupWindowMins * 60;
        myConfig.RiderCount = myConfig.RestaurantCount; // one rider per restaurant

        std::cout
            << "Orders per hour                  " << myConfig.OrdersPerHour
            << "\nOrder collection time mins     " << myConfig.GroupTimeMins
            << "\nRestaurants                    " << myConfig.RestaurantCount
            << "\nPickup window mins             " << myConfig.PickupWindowMins
            << "\nMaximum order preparation mins " << myConfig.MaxPrepTimeMins
            << "\nMaximum distance of rider Km   " << myConfig.CloseRiderDistanceKm << "\n";
    }
    void cZone::Sort()
    {
        /// sort orders by time
        std::sort(
            myOrders.begin(), myOrders.end(),
            [](pup::cOrder &a, pup::cOrder &b)
            {
                return (a.myTime < b.myTime);
            });

        /// sort order by restaurant, preserving time order
        std::stable_sort(
            myOrders.begin(), myOrders.end(),
            [](const pup::cOrder &a, const pup::cOrder &b)
            {
                return (a.myRest < b.myRest);
            });
    }
    /// find resteraunt with earlier ready order
    int cZone::FindRestFirstNextPickup()
    {
        int nextRest = -1;
        int nextPickup = INT32_MAX;
        for (int rest = 0; rest < myConfig.RestaurantCount; rest++)
        {
            for (auto &order : myOrders)
            {
                if (order.myRest < rest)
                    continue;
                if (order.myRest != rest)
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
            int nextRest = FindRestFirstNextPickup();
            if (nextRest == -1)
                break; // all orders picked up

            // pickup some orders from resteraunt
            myStacks.push_back(PickupOrders(nextRest));

            stackCount++;
        }

        return stackCount;
    }

    pup::cStack
    cZone::PickupOrders(int rest)
    {
        pup::cStack S;
        int orderIndex;
        for (orderIndex = 0; orderIndex < myOrders.size(); orderIndex++)
        {
            if (
                myOrders[orderIndex].myRest == rest &&
                myOrders[orderIndex].myWaiting)
                break;
        }
        if (orderIndex == myOrders.size())
            return S; // all orders picked up

        for (; orderIndex < myOrders.size(); orderIndex++)
        {
            if (myOrders[orderIndex].myTime > myConfig.PickupWindowSecs)
                break;
            if (myOrders[orderIndex].myRest != rest)
                break;
            S.myOrder.push_back(myOrders[orderIndex]);
            myOrders[orderIndex].myWaiting = false;
        }

        // std::cout << "Picked up " << v.size() << " orders "
        //     << *v.begin() << " to " << v.back() << "\n";
        // for (auto &o : v)
        //     std::cout << o << ",";
        // std::cout << "\n";

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
        myRiders.assign();
    }

    void cZone::Report()
    {
        std::cout << stackCount() << " order stacks created\n";

        // detailed report of first 5 order stacks
        int count = 0;
        for ( auto& stack : myStacks )
        {
            std::cout << stack.text() << "\n";
            count++;
            if( count >= 5 )
                break;
        }

        // timing report
        raven::set::cRunWatch::Report();
    }

}
