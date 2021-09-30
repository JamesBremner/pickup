#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <cRunWatch.h>
#include "cPathFinder.h"
#include "cOrder.h"
namespace pup
{
    std::pair<float, float>
    cStack::restaurantLocation()
    {
        std::pair<float, float> location;
        if (myOrder.size())
            location = ::theRestaurants[myOrder[0].myRest].myLocation;
        return location;
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
        auto path = finder.tsp( );

        // assemble delivery locations in optimal visit order
        std::vector<std::pair<float, float>> vOpt;
        for( int idx = 1; idx < path.size()-1; idx++ )
            vOpt.push_back( vl[ path[idx] ] );

        return vOpt;
    }
}