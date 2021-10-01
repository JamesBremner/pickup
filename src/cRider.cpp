#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "cRunWatch.h"
#include "quadtree.h"
#include "cZone.h"
#include "cRider.h"

namespace pup
{
    cRider::cRider()
    {
        myLocation.first = (rand() % theZone.myConfig.ZoneDimKm * 100) / 100.0;
        myLocation.second = (rand() % theZone.myConfig.ZoneDimKm * 100) / 100.0;
        myBusy = false;
    }

    cRiderPool::cRiderPool()
    {
        // construct riders at random locations
        myRiders.resize(theZone.myConfig.RiderCount);

        float dim2 = theZone.myConfig.ZoneDimKm / 2.0;
        myQuadTree = new quad::cCell(quad::cPoint(dim2, dim2), dim2);

        int index = 0;
        for (auto &r : myRiders)
        {
            myQuadTree->insert(quad::cPoint(
                r.myLocation.first,
                r.myLocation.second,
                index++));
        }
    }

    void cRiderPool::assign()
    {
        for (auto &S : theZone.myStacks)
        {
            raven::set::cRunWatch aWatcher("Allocate rider to stack");

            // find riders acceptably close to restaurant
            auto rest = S.restaurantLocation();
            quad::cCell close(
                quad::cPoint(rest.first, rest.second),
                theZone.myConfig.CloseRiderDistanceKm);
            auto riders = myQuadTree->find(close);

            // find closest acceptable rider
            float d = 1.0e10;
            quad::cPoint *allocated;
            for (auto &rider : riders)
            {
                float manhatten =
                    fabs(rest.first - rider->x) + fabs(rest.second - rider->y);
                if (manhatten < d)
                {
                    // check that rider is not busy with other orders
                    if (myRiders[rider->userData].myBusy)
                        continue;
                    // TODO:  add any other constraint filter here

                    d = manhatten;
                    allocated = rider;
                }
            }
            myRiders[allocated->userData].myBusy = true;
        }
    }
}

