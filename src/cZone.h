#include <iostream>
#include <vector>
#include "config.h"
#include "cOrder.h"
#include "cRider.h"

namespace pup
{
    /**
 * "One zone could contain up to 5000 restaurants
 * and the sum of all orders within one zone
 * are up to 200k orders per hour."
 */

    class cZone
    {
    public:
        sConfig myConfig;
        cOrderHolder myOrders;
        vStack myStacks;
        cRestaurantHolder myRestaurants;
        cRiderPool myRiders;

        cZone();

        // populate zone with restaurants, drivers and orders from an external source
        void readDB();

        // simulate restaurants, drivers and orders for a zone
        void simulate();

        // sort orders by restaurant and time
        void Sort();

        // stack all orders
        int orderStack();

        // arrange stack delivery locations into optimized order
        void delivery();

        // assign riders to order stacks
        void assignRiders();

        // print report on standard output
        void Report();

        int stackCount()
        {
            return myStacks.size();
        }

        enum eDistMethod
        {
            manhatten,
            manhatten_haversine,
        };

        /** calculate distance between two points
         * @param[in] A a point
         * @param[in] B a point
         * @param[in] m calculation method, defaults to manhatten
         * @return the calculated distance
         * 
         * For discussion https://github.com/JamesBremner/pickup/issues/2
         * 
         */
        static float distance(
            const std::pair<float, float> &l1,
            const std::pair<float, float> &l2,
            eDistMethod m = eDistMethod::manhatten)
        {
            return distance(
                l1.first, l2.second,
                l1.second, l2.second,
                m);
        }
        static float distance(
            float x1, float y1,
            float x2, float y2,
            eDistMethod m = eDistMethod::manhatten);

        void writeDB();

        void stacksWriteDB();

    private:
        void InitConfig();

        /// stack orders for a resteraunt
        cStack
        PickupOrders(cRestaurant *rest);

        /// find restaurant with earlier ready order
        cRestaurant *FindRestFirstNextPickup();

        void configWrite(raven::sqlite::cDB &db);
        void configRead(raven::sqlite::cDB &db);
    };
}
