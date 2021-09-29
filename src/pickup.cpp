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

struct sconfig
{
    // user values
    int OrdersPerHour;
    int GroupTimeMins;
    int ResterauntCount;
    int PickupWindowMins;
    int MaxPrepTimeMins;

    // calculated values
    int OrdersPerGroupTime;
    int PickupWindowSecs;

} theConfig;

/// Configure simulation parameters
void InitConfig()
{
    theConfig.OrdersPerHour = 20000;        // incoming order per hour
    theConfig.GroupTimeMins = 5;            // order collection time
    theConfig.ResterauntCount = 5000;       // number of restaurants
    theConfig.PickupWindowMins = 5;         // pickup window time
    theConfig.MaxPrepTimeMins = 15;         // maximum order preparation time

    theConfig.OrdersPerGroupTime =
        theConfig.GroupTimeMins * theConfig.OrdersPerHour / 60;
    theConfig.PickupWindowSecs = theConfig.PickupWindowMins * 60;

    std::cout 
        << "Orders per hour                  " << theConfig.OrdersPerHour
        << "\nOrder collection time mins     " << theConfig.GroupTimeMins
        << "\nRestaurants                    " << theConfig.ResterauntCount
        << "\nPickup window mins             " << theConfig.PickupWindowMins
        << "\nMaximum order preparation mins " << theConfig.MaxPrepTimeMins
        << "\n";
}

class cOrder
{
public:
    int myTime; // time, after start of group, when order will be ready for pickup
    int myRest;
    bool myWaiting;

    cOrder()
    {
        myTime = rand() % theConfig.MaxPrepTimeMins;
        myRest = rand() % theConfig.ResterauntCount;
        myWaiting = true;
    }

    friend std::ostream &operator<<(std::ostream &os, cOrder o)
    {
        os << "rest " << o.myRest << " at " << o.myTime;
        return os;
    }
};

std::vector<cOrder> theOrders;

/// Simulate orders generated in one collection time
void GenerateOrders()
{
    theOrders.clear();
    for (int o; o < theConfig.OrdersPerGroupTime; o++)
    {
        theOrders.push_back(cOrder());
    }
}

/// sort order by time
void SortByTime()
{
    std::sort(
        theOrders.begin(), theOrders.end(),
        [](cOrder &a, cOrder &b)
        {
            return (a.myTime < b.myTime);
        });
}

/// sort order by resteraunt, preserving time order
void SortByRest()
{
    std::stable_sort(
        theOrders.begin(), theOrders.end(),
        [](const cOrder &a, const cOrder &b)
        {
            return (a.myRest < b.myRest);
        });
}

/// find resteraunt with earlier ready order
int FindRestFirstNextPickup()
{
    int nextRest = -1;
    int nextPickup = INT32_MAX;
    for (int rest = 0; rest < theConfig.ResterauntCount; rest++)
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
std::vector<cOrder>
PickupOrders(int rest)
{
    std::vector<cOrder> v;
    int orderIndex;
    for (orderIndex = 0; orderIndex < theOrders.size(); orderIndex++)
    {
        if (
            theOrders[orderIndex].myRest == rest &&
            theOrders[orderIndex].myWaiting)
            break;
    }
    if (orderIndex == theOrders.size())
        return v; // all orders picked up

    for (; orderIndex < theOrders.size(); orderIndex++)
    {
        if (theOrders[orderIndex].myTime > theConfig.PickupWindowSecs)
            break;
        if (theOrders[orderIndex].myRest != rest)
            break;
        v.push_back(theOrders[orderIndex]);
        theOrders[orderIndex].myWaiting = false;
    }

    // std::cout << "Picked up " << v.size() << " orders "
    //     << *v.begin() << " to " << v.back() << "\n";
    // for (auto &o : v)
    //     std::cout << o << ",";
    // std::cout << "\n";

    return v;
}

// stack all orders
int orderStack()
{
    raven::set::cRunWatch aWatcher("stack");

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
        auto v = PickupOrders(nextRest);

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

    raven::set::cRunWatch::Start();

    int stackCount = orderStack();

    std::cout << stackCount << " order stacks created\n";

    raven::set::cRunWatch::Report();


}