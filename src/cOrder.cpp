#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <cRunWatch.h>
#include "cPathFinder.h"

#include "cZone.h"

namespace pup
{
    std::pair<float, float>
    cStack::restaurantLocation()
    {
        std::pair<float, float> location;
        if (myOrder.size() && theZone.myRestaurants.size())
            location = theZone.myRestaurants[myOrder[0].myRest].myLocation;
        return location;
    }

    cOrder::cOrder()
    {
        myTime = rand() % theZone.myConfig.MaxPrepTimeMins;
        myRest = rand() % theZone.myConfig.RestaurantCount;
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

                // manhatten distances
                float d =
                    std::fabs(vl[c1].first - vl[c2].first) + std::fabs(vl[c1].second - vl[c2].second);
                finder.addLink(
                    std::to_string(c2),
                    std::to_string(c1),
                    d);
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

    std::string cStack::text()
    {
        std::stringstream ss;

        auto rest = restaurantLocation();
        ss << "\nRestaurant at " << rest.first << " " << rest.second << "\n";
        if (myRider == -1)
        {
            ss << "No rider assigned\n";
            return ss.str();;
        }

        ss << "Rider # " << myRider
           << " at " << theZone.myRiders.location(myRider).first
           << "," << theZone.myRiders.location(myRider).second
           << " delivers to ";
        for (auto &o : myOrder)
        {
            ss
                << "( " << rest.first + o.myDelivery.first
                << "," << rest.second + o.myDelivery.second
                << " ) ";
        }

        return ss.str();
    }
}