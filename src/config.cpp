#include <stdexcept>
#include "raven_sqlite.h"
#include "config.h"
namespace pup
{
    void cConfig::calculate()
    {
        OrdersPerGroupTime =
            GroupTimeMins * OrdersPerHour / 60;
        PickupWindowSecs = 
            PickupWindowMins * 60;
    }
    void cConfig::write(raven::sqlite::cDB &db)
    {
        if (!db.getHandle())
            throw std::runtime_error("DB not open");
        db.Query(
            "CREATE TABLE IF NOT EXISTS config "
            " ( ZoneDimKm, CloseRiderDistanceKm, "
            " OrdersPerHour, GroupTimeMins, RestaurantCount, "
            " PickupWindowMins, MaxPrepTimeMins, RiderCount );");
        db.Query("DELETE FROM config;");
        db.Query(
            "INSERT INTO config VALUES "
            " ( %d, %f, %d, %d, %d, %d, %d, %d );",
            ZoneDimKm, CloseRiderDistanceKm,
            OrdersPerHour, GroupTimeMins, RestaurantCount,
            PickupWindowMins, MaxPrepTimeMins, RiderCount);
        if (db.myError)
            throw std::runtime_error(
                "Zone Save: " + std::string(db.myError));
    }
    void cConfig::read(raven::sqlite::cDB &db)
    {
        if (!db.getHandle())
            throw std::runtime_error("DB not open");
        db.Query(
            "SELECT * FROM config;");
        if (db.myColCount != 8)
            throw std::runtime_error(
                "Error reading config from db");
        ZoneDimKm = atof(db.myResultA[0].c_str());
        CloseRiderDistanceKm = atof(db.myResultA[1].c_str());
        OrdersPerHour = atof(db.myResultA[2].c_str());
        GroupTimeMins = atof(db.myResultA[3].c_str());
        RestaurantCount = atof(db.myResultA[4].c_str());
        PickupWindowMins = atof(db.myResultA[5].c_str());
        MaxPrepTimeMins = atof(db.myResultA[6].c_str());
        RiderCount = atof(db.myResultA[7].c_str());
        
        calculate();
    }
}
