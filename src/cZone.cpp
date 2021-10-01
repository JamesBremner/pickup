
#include <algorithm>
#include "cRunWatch.h"
#include "cZone.h"
namespace pup
{
    cRestaurant::cRestaurant()
    {
        myLocation.first = (rand() % theZone.myConfig.ZoneDimKm * 100) / 100.0;
        myLocation.second = (rand() % theZone.myConfig.ZoneDimKm * 100) / 100.0;
    }
    cOrder::cOrder()
    {
        myTime = rand() % theZone.myConfig.MaxPrepTimeMins;
        myRest = rand() % theZone.myConfig.RestaurantCount;
        myWaiting = true;
        myDelivery.first = (rand() % 250) / 100.0;
        myDelivery.second = (rand() % 250) / 100.0;
    }

    cZone::cZone()
    {
        InitConfig();

        /// Simulate orders generated in one collection time
        myOrders.clear();
        for (int o; o < myConfig.OrdersPerGroupTime; o++)
        {
            myOrders.push_back(pup::cOrder());
        }
    }
    void cZone::InitConfig()
    {
        myConfig.OrdersPerHour = 20000;  // incoming order per hour
        myConfig.GroupTimeMins = 5;      // order collection time
        myConfig.RestaurantCount = 5000; // number of restaurants
        myConfig.PickupWindowMins = 5;   // pickup window time
        myConfig.MaxPrepTimeMins = 15;   // maximum order preparation time
        myConfig.ZoneDimKm = 25;         // zone dimension
        myConfig.CloseRiderDistanceKm = 0.33;

        myConfig.OrdersPerGroupTime =
            myConfig.GroupTimeMins * myConfig.OrdersPerHour / 60;
        myConfig.PickupWindowSecs = myConfig.PickupWindowMins * 60;
        myConfig.RiderCount = myConfig.RestaurantCount; // one rider per restaurant

        myRestaurants.resize(myConfig.RestaurantCount);

        myRiders = new cRiderPool();

        std::cout
            << "Orders per hour                  " << myConfig.OrdersPerHour
            << "\nOrder collection time mins     " << myConfig.GroupTimeMins
            << "\nRestaurants                    " << myRestaurants.size()
            << "\nPickup window mins             " << myConfig.PickupWindowMins
            << "\nMaximum order preparation mins " << myConfig.MaxPrepTimeMins
            << "\n";
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
        raven::set::cRunWatch aWatcher("stacking orders");

        int stackCount = 0;

        // sort
        theZone.Sort();

        // loop until all orders picked up
        while (1)
        {
            // resteraunt with earliest ready order
            int nextRest = theZone.FindRestFirstNextPickup();
            if (nextRest == -1)
                break; // all orders picked up

            // pickup some orders from resteraunt
            theZone.myStacks.push_back(PickupOrders(nextRest));

            stackCount++;
        }

        return stackCount;
    }

    pup::cStack
    cZone::PickupOrders(int rest)
    {
        pup::cStack S;
        int orderIndex;
        for (orderIndex = 0; orderIndex < theZone.myOrders.size(); orderIndex++)
        {
            if (
                theZone.myOrders[orderIndex].myRest == rest &&
                theZone.myOrders[orderIndex].myWaiting)
                break;
        }
        if (orderIndex == theZone.myOrders.size())
            return S; // all orders picked up

        for (; orderIndex < theZone.myOrders.size(); orderIndex++)
        {
            if (theZone.myOrders[orderIndex].myTime > theZone.myConfig.PickupWindowSecs)
                break;
            if (theZone.myOrders[orderIndex].myRest != rest)
                break;
            S.myOrder.push_back(theZone.myOrders[orderIndex]);
            theZone.myOrders[orderIndex].myWaiting = false;
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
        myRiders->assign();
    }

}
