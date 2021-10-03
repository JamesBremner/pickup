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
        pup::sConfig myConfig;
        std::vector<cOrder> myOrders;
        std::vector<cStack> myStacks;
        std::vector<cRestaurant> myRestaurants;
        cRiderPool myRiders;

        cZone();

        // populate zone with restaurants, drivers and orders from an external source
        void populate();

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
         * Fo discussion https://github.com/JamesBremner/pickup/issues/2
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
            int x1, int y1,
            int x2, int y2,
            eDistMethod m = eDistMethod::manhatten);

    private:
        void InitConfig();

        /// stack orders for a resteraunt
        cStack
        PickupOrders(cRestaurant *rest);

        /// find restaurant with earlier ready order
        cRestaurant *FindRestFirstNextPickup();
    };
}
