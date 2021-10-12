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
    void cRestaurantHolder::simulate(const cConfig &config)
    {
        myRestaurant.clear();
        for (int k = 0; k < config.RestaurantCount; k++)
            myRestaurant.push_back(
                cRestaurant(config.ZoneDimKm));
    }
    int cRestaurantHolder::index(cRestaurant *prest)
    {
        int id = 0;
        for (auto &r : myRestaurant)
        {
            if (&r == prest)
                break;
            id++;
        }
        return id;
    }
    cRestaurant *cRestaurantHolder::pointer(int id)
    {
        if (0 > id || id >= myRestaurant.size())
            throw std::runtime_error("cRestaurantHolder::pointer bad index");
        return &myRestaurant[id];
    }
    void cOrderHolder::simulate(cZone *zone)
    {
        myOrder.clear();
        for (int o = 0; o < zone->myConfig.OrdersPerGroupTime; o++)
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
        db.Query("BEGIN TRANSACTION;");
        db.Prepare("INSERT INTO restaurant VALUES ( ?, ? );");

        for (auto &rest : myRestaurant)
        {
            db.Bind(1, rest.myLocation.first);
            db.Bind(2, rest.myLocation.second);
            db.step();
            db.reset();
        }
        if (db.finalize())
            throw std::runtime_error("DB restaurant write error");
        db.Query("END TRANSACTION;");
    }
    void cRestaurantHolder::read(raven::sqlite::cDB &db)
    {
        myRestaurant.clear();

        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");
        db.Prepare("SELECT * FROM restaurant;");
        // loop over rows returned
        while (db.step() == SQLITE_ROW)
        {
            myRestaurant.push_back(
                cRestaurant(
                    (float)db.ColumnDouble(0),
                    (float)db.ColumnDouble(1)));
        }
        db.finalize();

        std::cout << myRestaurant.size() << " restaurants loaded\n";
    }
    void cOrderHolder::write(raven::sqlite::cDB &db, cZone *zone)
    {
        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");
        if (db.Query(
                "CREATE TABLE IF NOT EXISTS orderholder ( rdy, rst, x, y );"))
            throw std::runtime_error("DB cannot create orderholder");
        db.Query("DELETE FROM orderholder;");
        db.Query("BEGIN TRANSACTION;");
        db.Prepare("INSERT INTO orderholder VALUES ( ?, ?, ?, ? );");

        for (auto &order : myOrder)
        {
            db.Bind(1, order.myTime);
            db.Bind(2, zone->myRestaurants.index(order.myRest));
            db.Bind(3, order.myDelivery.first);
            db.Bind(4, order.myDelivery.second);
            db.step();
            db.reset();
        }
        db.finalize();
        db.Query("END TRANSACTION;");

        std::cout << myOrder.size() << " orders stored to DB\n";
    }
    void vStack::write(raven::sqlite::cDB &db)
    {
        if (!db.getHandle())
            throw std::runtime_error("DB not open");
        if (db.Query(
                "CREATE TABLE IF NOT EXISTS stacks ( rider );"))
            throw std::runtime_error("DB cannot create stacks");
        if (db.Query(
                "CREATE TABLE IF NOT EXISTS route ( stack, orderIndex );"))
            throw std::runtime_error("DB cannot create route");
        db.Query("DELETE FROM stacks;");
        db.Query("BEGIN TRANSACTION;");
        db.Prepare("INSERT INTO stacks VALUES ( ? );");
        for (auto &stack : myStack)
        {
            db.Bind(1, stack.rider());
            db.step();
            db.reset();
        }
        db.finalize();
        db.Query("END TRANSACTION;");
        db.Query("BEGIN TRANSACTION;");
        db.Prepare("INSERT INTO route VALUES ( ?, ? );");
        int stackIndex = 0;
        for (auto &stack : myStack)
        {
            for (auto &order : stack)
            {
                db.Bind(1, stackIndex);
                db.Bind(2, order.myIndex);
                db.step();
                db.reset();
            }
            stackIndex++;
        }
        db.finalize();
        db.Query("END TRANSACTION;");
    }

    void cOrderHolder::read(raven::sqlite::cDB &db,
                            cZone *zone)
    {
        myOrder.clear();

        auto dbh = db.getHandle();
        if (!dbh)
            throw std::runtime_error("DB not open");

        db.Prepare("SELECT * FROM orderholder;");
        // loop over rows returned
        int index = 0;
        while (db.step() == SQLITE_ROW)
        {
            myOrder.push_back(
                cOrder(
                    db.ColumnInt(0),
                    zone->myRestaurants.pointer(db.ColumnInt(1)),
                    db.ColumnDouble(2),
                    db.ColumnDouble(3),
                    index++));
        }
        db.finalize();

        std::cout << myOrder.size() << " orders loaded\n";
    }

    cRestaurant *cStack::restaurant()
    {
        if (!myOrder.size())
            throw std::runtime_error("cStack::restaurant stack has no orders");
        return myOrder[0].myRest;
    }
    std::pair<float, float> cStack::restaurantLocation()
    {
        if (!myRestaurant)
            throw std::runtime_error("cStack::restaurantLocation no restaurant");
        return myRestaurant->myLocation;
    }

    cOrder::cOrder(
        int ready, cRestaurant *rest,
        float x, float y,
        int index)
    {
        myTime = ready;
        myRest = rest;
        myWaiting = true;
        myDelivery.first = x;
        myDelivery.second = y;
        myIndex = index;
    }
    cOrder::cOrder(cZone *zone)
    {
        myTime = rand() % zone->myConfig.MaxPrepTimeMins;
        myRest = &zone->myRestaurants.myRestaurant[rand() % zone->myConfig.RestaurantCount];
        myWaiting = true;
        myDelivery.first = (rand() % 250) / 100.0;
        myDelivery.second = (rand() % 250) / 100.0;
    }
    void cStack::add(const cOrder &order)
    {
        myOrder.push_back(order);
        myRestaurant = order.myRest;
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