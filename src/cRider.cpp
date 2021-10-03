#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "cRunWatch.h"
#include "quadtree.h"

#include "cZone.h"

namespace pup
{
    cRider::cRider(const sConfig &config)
    {
        myLocation.first = (rand() % (config.ZoneDimKm * 100)) / 100.0;
        myLocation.second = (rand() % (config.ZoneDimKm * 100)) / 100.0;
        myBusy = false;
        //std::cout << " rider " << text();
    }

    std::string cRider::text()
    {
        std::stringstream ss;
        ss << myLocation.first << "," << myLocation.second << " ";
        return ss.str();
    }

    cRiderPool::cRiderPool(sConfig &config)
        : myConfig(config), myQuadTree(0)
    {
    }

    void cRiderPool::simulate()
    {
        // construct riders at random locations
        myRiders.clear();
        for (int k = 0; k < myConfig.RiderCount; k++)
            myRiders.push_back(myConfig);
        quadTreeBuild();
    }

    void cRiderPool::quadTreeBuild()
    {
        float dim2 = myConfig.ZoneDimKm / 2.0;

        if (myQuadTree)
            delete myQuadTree;
        myQuadTree = new quad::cCell(quad::cPoint(dim2, dim2), myConfig.ZoneDimKm);

        int index = 0;
        for (auto &r : myRiders)
        {
            myQuadTree->insert(quad::cPoint(
                r.myLocation.first,
                r.myLocation.second,
                index++));
        }
    }

    void cRiderPool::assign(cStack &S)
    {

        raven::set::cRunWatch aWatcher("Allocate rider to stack");

        // find riders acceptably close to restaurant
        auto rest = S.restaurant()->myLocation;
        quad::cCell close(
            quad::cPoint(rest.first, rest.second),
            myConfig.CloseRiderDistanceKm);
        auto riders = myQuadTree->find(close);

        // std::cout << "acceptable " << riders.size()
        //     <<" " << myConfig.CloseRiderDistanceKm << "\n";
        // for( auto& rrr : myRiders )
        //     std::cout << rrr.text() << " ";
        // std::cout << "\n";

        if (!riders.size())
        {
            // no acceptable rider for this stack, all too far away
            S.myRider = -1;
            return;
        }

        // find closest acceptable rider
        float d = 1.0e10;
        quad::cPoint *allocated = 0;
        for (auto rider : riders)
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

                // std::cout << "so far " <<  allocated->userData
                //     << " " << myRiders[allocated->userData].myBusy
                //     << "\n";
            }
        }
        if (!allocated)
        {
            // no acceptable rider for this stack, all busy
            S.myRider = -1;
            return;
        }
        S.myRider = allocated->userData;
        myRiders[allocated->userData].myBusy = true;
        //std::cout << "allocated rider " << allocated->userData << "\n";
    }
}
