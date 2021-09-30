/*  Order stacking

(1) Collect orders per few minutes interval and sort all orders by their prep time 

(2) group orders by restaurant 

(3) Starting from the order that has the smallest remaining prep time,
 look for any orders in the same restaurant within the time window (let's say 3-5 mins),
 if exists group them as a stack. O(1).

 https://stackoverflow.com/q/69355474/16582

 */

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "cRunWatch.h"
#include "quadtree.h"
#include "config.h"
#include "cOrder.h"

pup::sConfig theConfig;
std::vector<pup::cRestaurant> theRestaurants;
std::vector<pup::cOrder> theOrders;
std::vector<pup::cStack> theStacks;
std::vector<pup::cRider> theRiders;
quad::cCell *theQuadTree;

/// Configure simulation parameters
void InitConfig()
{
    theConfig.OrdersPerHour = 20000;  // incoming order per hour
    theConfig.GroupTimeMins = 5;      // order collection time
    theConfig.RestaurantCount = 5000; // number of restaurants
    theConfig.PickupWindowMins = 5;   // pickup window time
    theConfig.MaxPrepTimeMins = 15;   // maximum order preparation time
    theConfig.RestaurantMaxDimKm = 25; // maximum dimension of restaurant locations
    theConfig.CloseRiderDistanceKm = 0.33;

    theConfig.OrdersPerGroupTime =
        theConfig.GroupTimeMins * theConfig.OrdersPerHour / 60;
    theConfig.PickupWindowSecs = theConfig.PickupWindowMins * 60;
    theConfig.RiderCount = theConfig.RestaurantCount; // one rider per restaurant

    theRestaurants.resize(theConfig.RestaurantCount);

    std::cout
        << "Orders per hour                  " << theConfig.OrdersPerHour
        << "\nOrder collection time mins     " << theConfig.GroupTimeMins
        << "\nRestaurants                    " << theConfig.RestaurantCount
        << "\nPickup window mins             " << theConfig.PickupWindowMins
        << "\nMaximum order preparation mins " << theConfig.MaxPrepTimeMins
        << "\n";
}
namespace pup
{
    cRider::cRider()
    {
        myLocation.first = (rand() % theConfig.RestaurantMaxDimKm * 100) / 100.0;
        myLocation.second = (rand() % theConfig.RestaurantMaxDimKm * 100) / 100.0;
        myBusy = false;
    }
    cRestaurant::cRestaurant()
    {
        myLocation.first = (rand() % theConfig.RestaurantMaxDimKm * 100) / 100.0;
        myLocation.second = (rand() % theConfig.RestaurantMaxDimKm * 100) / 100.0;
    }
    cOrder::cOrder()
    {
        myTime = rand() % theConfig.MaxPrepTimeMins;
        myRest = rand() % theConfig.RestaurantCount;
        myWaiting = true;
        myDelivery.first = (rand() % 250) / 100.0;
        myDelivery.second = (rand() % 250) / 100.0;
    }

}

/// Simulate orders generated in one collection time
void GenerateOrders()
{
    theOrders.clear();
    for (int o; o < theConfig.OrdersPerGroupTime; o++)
    {
        theOrders.push_back(pup::cOrder());
    }
}

void LocateRiders()
{
    theRiders.resize(theConfig.RestaurantCount);

    float dim2 = theConfig.RestaurantMaxDimKm / 2.0;
    theQuadTree = new quad::cCell(quad::cPoint(dim2, dim2), dim2);

    int index = 0;
    for (auto &r : theRiders)
    {
        theQuadTree->insert(quad::cPoint(
            r.myLocation.first,
            r.myLocation.second,
            index++ ));
    }
}

/// sort order by time
void SortByTime()
{
    std::sort(
        theOrders.begin(), theOrders.end(),
        [](pup::cOrder &a, pup::cOrder &b)
        {
            return (a.myTime < b.myTime);
        });
}

/// sort order by resteraunt, preserving time order
void SortByRest()
{
    std::stable_sort(
        theOrders.begin(), theOrders.end(),
        [](const pup::cOrder &a, const pup::cOrder &b)
        {
            return (a.myRest < b.myRest);
        });
}

/// find resteraunt with earlier ready order
int FindRestFirstNextPickup()
{
    int nextRest = -1;
    int nextPickup = INT32_MAX;
    for (int rest = 0; rest < theConfig.RestaurantCount; rest++)
    {
        for (auto &order : theOrders)
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

/// stack orders for a resteraunt
pup::cStack
PickupOrders(int rest)
{
    pup::cStack S;
    int orderIndex;
    for (orderIndex = 0; orderIndex < theOrders.size(); orderIndex++)
    {
        if (
            theOrders[orderIndex].myRest == rest &&
            theOrders[orderIndex].myWaiting)
            break;
    }
    if (orderIndex == theOrders.size())
        return S; // all orders picked up

    for (; orderIndex < theOrders.size(); orderIndex++)
    {
        if (theOrders[orderIndex].myTime > theConfig.PickupWindowSecs)
            break;
        if (theOrders[orderIndex].myRest != rest)
            break;
        S.myOrder.push_back(theOrders[orderIndex]);
        theOrders[orderIndex].myWaiting = false;
    }

    // std::cout << "Picked up " << v.size() << " orders "
    //     << *v.begin() << " to " << v.back() << "\n";
    // for (auto &o : v)
    //     std::cout << o << ",";
    // std::cout << "\n";

    return S;
}

// stack all orders
int orderStack()
{
    raven::set::cRunWatch aWatcher("stacking orders");

    int stackCount = 0;

    // sort
    SortByTime();
    SortByRest();

    // loop until all orders picked up
    while (1)
    {
        // resteraunt with earliest ready order
        int nextRest = FindRestFirstNextPickup();
        if (nextRest == -1)
            break; // all orders picked up

        // pickup some orders from resteraunt
        theStacks.push_back(PickupOrders(nextRest));

        stackCount++;
    }

    return stackCount;
}

/// time profile
main()
{
    std::cout << "Pickup\n";

    // Initialize
    InitConfig();
    GenerateOrders();
    LocateRiders();

    raven::set::cRunWatch::Start();

    int stackCount = orderStack();

    std::cout << stackCount << " order stacks created\n";

    for (auto &S : theStacks)
    {
        S.deliveryLocations();
        S.rider();
    }

    raven::set::cRunWatch::Report();
}