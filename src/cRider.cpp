#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "cRunWatch.h"
#include "sqlite3.h"
#include "raven_sqlite.h"
#include "quadtree.h"

#include "cZone.h"

namespace pup
{

    cRider::cRider(float x, float y)
    {
        myLocation.first = x;
        myLocation.second = y;
        myBusy = false;
    }
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
        if( myConfig.ZoneDimKm <= 0 )
            throw std::runtime_error("cRiderPool::quadTreeBuild bad zone dimension");

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

    void cRiderPool::write(raven::sqlite::cDB &db)
    {
        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");
        db.Query(
            "CREATE TABLE IF NOT EXISTS rider "
            " ( x, y );");
        db.Query("DELETE FROM rider;");
        db.Query("BEGIN TRANSACTION;");
        sqlite3_stmt *stmt = 0;
        const char *tail = 0;
        int ret =
            sqlite3_prepare_v2(
                dbh,
                "INSERT INTO rider VALUES ( ?, ? );",
                -1,
                &stmt,
                &tail);
        for (auto &r : myRiders)
        {
            ret = sqlite3_bind_double(stmt, 1, r.myLocation.first);
            ret = sqlite3_bind_double(stmt, 2, r.myLocation.second);
            ret = sqlite3_step(stmt);
            ret = sqlite3_reset(stmt);
        }
        if (sqlite3_finalize(stmt))
            throw std::runtime_error("DB rider write error");
        db.Query("END TRANSACTION;");
    }
    void cRiderPool::read(raven::sqlite::cDB &db)
    {
        myRiders.clear();

        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");

        sqlite3_stmt *stmt = 0;
        const char *tail = 0;
        int ret =
            sqlite3_prepare_v2(
                dbh,
                "SELECT * FROM rider;",
                -1,
                &stmt,
                &tail);
        // loop over rows returned
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            myRiders.push_back(
                cRider(
                    sqlite3_column_double(stmt, 0),
                    sqlite3_column_double(stmt, 1)));
        }
        sqlite3_finalize(stmt);

        std::cout << myRiders.size() << " riders loaded\n";

        quadTreeBuild();
    }

    void cRiderPool::assign(cStack &S)
    {
        raven::set::cRunWatch aWatcher("Allocate rider to stack");

        if (!S.orderCount())
        {
            // empty stack
            return;
        }

        // find riders acceptably close to restaurant
        auto rest = S.restaurantLocation();
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
            return;
        }

        // find closest acceptable rider
        float d = 1.0e10;
        quad::cPoint *allocated = 0;
        for (auto rider : riders)
        {
            float rd = cZone::distance(
                rest,
                std::make_pair(rider->x, rider->y));
            if (rd < d)
            {
                // check that rider is not busy with other orders
                if (myRiders[rider->userData].myBusy)
                    continue;
                // TODO:  add any other constraint filter here

                d = rd;
                allocated = rider;

                // std::cout << "so far " <<  allocated->userData
                //     << " " << myRiders[allocated->userData].myBusy
                //     << "\n";
            }
        }
        if (!allocated)
        {
            // no acceptable rider for this stack, all busy
            return;
        }
        S.rider( allocated->userData );
        myRiders[allocated->userData].myBusy = true;
        //std::cout << "allocated rider " << allocated->userData << "\n";
    }
}
