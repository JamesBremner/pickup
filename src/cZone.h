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

        // assign reiders to order stacks
        void assignRiders();

        void Report();

        int stackCount()
        {
            return myStacks.size();
        }

    private:
        void InitConfig();

        /// stack orders for a resteraunt
        cStack
        PickupOrders(cRestaurant * rest);

        /// find restaurant with earlier ready order
        cRestaurant * FindRestFirstNextPickup();
    };
}
