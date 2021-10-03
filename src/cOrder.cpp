#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <cRunWatch.h>
#include "cPathFinder.h"

#include "cZone.h"

namespace pup
{
    cRestaurant *cStack::restaurant()
    {
        if (!myOrder.size())
            throw std::runtime_error("cStack::restaurantIndex stack has no orders");
        return myOrder[0].myRest;
    }

    cOrder::cOrder(cZone *zone)
    {
        myTime = rand() % zone->myConfig.MaxPrepTimeMins;
        myRest = &zone->myRestaurants[rand() % zone->myConfig.RestaurantCount];
        myWaiting = true;
        myDelivery.first = (rand() % 250) / 100.0;
        myDelivery.second = (rand() % 250) / 100.0;
    }

    std::vector<std::pair<float, float>>
    cStack::deliveryLocations()
    {
        raven::set::cRunWatch aWatcher("Optimize stack delivery route");
        std::vector<std::pair<float, float>> vl;

        // restaurant located in center of zone
        vl.push_back(std::pair<float, float>(0, 0));

        // delivery locations
        for (auto &d : myOrder)
            vl.push_back(d.myDelivery);

        raven::graph::cPathFinder finder;

        for (int c1 = 0; c1 < vl.size(); c1++)
        {
            for (int c2 = 0; c2 < vl.size(); c2++)
            {
                if (c1 == c2)
                    continue;

                finder.addLink(
                    std::to_string(c2),
                    std::to_string(c1),
                    cZone::distance(
                        vl[c1], vl[c2]));
            }
        }

        // apply travelling salesman problem algorithm
        auto path = finder.tsp();

        // assemble delivery locations in optimal visit order
        std::vector<std::pair<float, float>> vOpt;
        for (int idx = 1; idx < path.size() - 1; idx++)
            vOpt.push_back(vl[path[idx]]);

        return vOpt;
    }

    std::string cStack::text(cZone *zone)
    {
        std::stringstream ss;

        auto &restloc = restaurant()->myLocation;
        ss << "\nRestaurant at " << restloc.first << " " << restloc.second << "\n";
        if (myRider == -1)
        {
            ss << "No rider assigned\n";
            return ss.str();
            ;
        }

        ss << "Rider # " << myRider
           << " at " << zone->myRiders.location(myRider).first
           << "," << zone->myRiders.location(myRider).second
           << " delivers to ";
        for (auto &o : myOrder)
        {
            ss
                << "( " << restloc.first + o.myDelivery.first
                << "," << restloc.second + o.myDelivery.second
                << " ) ";
        }

        return ss.str();
    }
}