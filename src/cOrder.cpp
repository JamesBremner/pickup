#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <cRunWatch.h>
#include "cPathFinder.h"
#include "sqlite3.h"
#include "raven_sqlite.h"

#include "cZone.h"

namespace pup
{
    void cRestaurantHolder::simulate(const sConfig &config)
    {
        myRestaurant.clear();
        for (int k = 0; k < config.RestaurantCount; k++)
            myRestaurant.push_back(
                cRestaurant(config.ZoneDimKm));
    }
    int cRestaurantHolder::index( cRestaurant* prest )
    {
        int id = 0;
        for( auto& r : myRestaurant ) {
            if( &r == prest )
                break;
            id++;
        }
        return id;
    }
    cRestaurant* cRestaurantHolder::pointer( int id )
    {
        if( 0 > id || id >= myRestaurant.size() )
            throw std::runtime_error("cRestaurantHolder::pointer bad index");
        return &myRestaurant[ id ];
    }
    void cOrderHolder::simulate(cZone *zone)
    {
        myOrder.clear();
        for (int o; o < zone->myConfig.OrdersPerGroupTime; o++)
        {
            myOrder.push_back(cOrder(zone));
        }
    }

    void cRestaurantHolder::write(raven::sqlite::cDB &db)
    {
        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");
        db.Query(
            "CREATE TABLE IF NOT EXISTS restaurant "
            " ( x, y );");
        db.Query("DELETE FROM restaurant;");
        sqlite3_stmt *stmt = 0;
        const char *tail = 0;
        int ret =
            sqlite3_prepare_v2(
                dbh,
                "INSERT INTO restaurant VALUES ( ?, ? );",
                -1,
                &stmt,
                &tail);
        for (auto &rest : myRestaurant)
        {
            ret = sqlite3_bind_double(stmt, 1, rest.myLocation.first);
            ret = sqlite3_bind_double(stmt, 2, rest.myLocation.second);
            ret = sqlite3_step(stmt);
            ret = sqlite3_reset(stmt);
        }
        if (sqlite3_finalize(stmt))
            throw std::runtime_error("DB restaurant write error");
    }
    void cRestaurantHolder::read(raven::sqlite::cDB &db)
    {
        myRestaurant.clear();

        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");

        sqlite3_stmt *stmt = 0;
        const char *tail = 0;
        int ret =
            sqlite3_prepare_v2(
                dbh,
                "SELECT * FROM restaurant;",
                -1,
                &stmt,
                &tail);
        // loop over rows returned
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            myRestaurant.push_back(
                cRestaurant(
                    sqlite3_column_double(stmt, 0),
                    sqlite3_column_double(stmt, 1)));
        }
        sqlite3_finalize(stmt);

        std::cout << myRestaurant.size() << " restaurants loaded\n";
    }
    void cOrderHolder::write(raven::sqlite::cDB &db, cZone* zone )
    {
        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");
        if (db.Query(
                "CREATE TABLE IF NOT EXISTS orderholder ( rdy, rst, x, y );"))
            throw std::runtime_error("DB cannot create orderholder");
        db.Query("DELETE FROM orderholder;");
        sqlite3_stmt *stmt = 0;
        const char *tail = 0;
        int ret =
            sqlite3_prepare_v2(
                dbh,
                "INSERT INTO orderholder VALUES ( ?, ?, ?, ? );",
                -1,
                &stmt,
                &tail);
        for (auto &order : myOrder)
        {

            ret = sqlite3_bind_int(stmt, 1, order.myTime);
            ret = sqlite3_bind_int(stmt, 2, zone->myRestaurants.index( order.myRest ));
            ret = sqlite3_bind_double(stmt, 3, order.myDelivery.first);
            ret = sqlite3_bind_double(stmt, 4, order.myDelivery.second);
            ret = sqlite3_step(stmt);
            ret = sqlite3_reset(stmt);
        }
        ret = sqlite3_finalize(stmt);
    }

    void cOrderHolder::read(raven::sqlite::cDB &db,
    cZone * zone )
    {
                myOrder.clear();

        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");

        sqlite3_stmt *stmt = 0;
        const char *tail = 0;
        int ret =
            sqlite3_prepare_v2(
                dbh,
                "SELECT * FROM orderholder;",
                -1,
                &stmt,
                &tail);
        // loop over rows returned
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            myOrder.push_back(
                cOrder(
                    sqlite3_column_int( stmt, 0),
                    zone->myRestaurants.pointer(sqlite3_column_int( stmt, 1)),
                    sqlite3_column_double(stmt, 2),
                    sqlite3_column_double(stmt, 3)));
        }
        sqlite3_finalize(stmt);

        std::cout << myOrder.size() << " orders loaded\n";
    }

    cRestaurant *cStack::restaurant()
    {
        if (!myOrder.size())
            throw std::runtime_error("cStack::restaurantIndex stack has no orders");
        return myOrder[0].myRest;
    }

    cOrder::cOrder(
        int ready, cRestaurant * rest,
     float x, float y)
    {
        myTime = ready;
        myRest = rest;
        myWaiting = true;
        myDelivery.first = x;
        myDelivery.second = y;
    }
    cOrder::cOrder(cZone *zone)
    {
        myTime = rand() % zone->myConfig.MaxPrepTimeMins;
        myRest = &zone->myRestaurants.myRestaurant[rand() % zone->myConfig.RestaurantCount];
        myWaiting = true;
        myDelivery.first = (rand() % 250) / 100.0;
        myDelivery.second = (rand() % 250) / 100.0;
    }

    std::vector<std::pair<float, float>>
    cStack::deliveryLocations()
    {
        raven::set::cRunWatch aWatcher("Optimize stack delivery route");

        std::vector<std::pair<float, float>> vl;

        if (!myOrder.size())
            return vl;

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