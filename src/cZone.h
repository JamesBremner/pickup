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
        std::vector<pup::cStack> myStacks;
        std::vector<cRestaurant> myRestaurants;
        cRiderPool * myRiders;

        cZone();

        // sort orders by restaurant and time
        void Sort();

        // stack all orders
        int orderStack();

        // arrange stack delivery locations into optimized order
        void delivery();

        // assign reiders to order stacks
        void assignRiders();

        std::string stackText( int stackIndex );
        
        int stackCount()
        {
            return myStacks.size();
        }

    private:
        void InitConfig();

        /// stack orders for a resteraunt
        cStack
        PickupOrders(int rest);

        /// find restaurant with earlier ready order
        int FindRestFirstNextPickup();
    };
}
extern pup::cZone theZone;