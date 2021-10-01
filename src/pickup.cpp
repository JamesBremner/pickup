/*  Order stacking

(1) Collect orders per few minutes interval and sort all orders by their prep time 

(2) group orders by restaurant 

(3) Starting from the order that has the smallest remaining prep time,
 look for any orders in the same restaurant within the time window (let's say 3-5 mins),
 if exists group them as a stack. O(1).

(4)  Optimize delivery route for every stack

(5)  Assign riders to order stacks

 https://stackoverflow.com/q/69355474/16582

 */


#include <stdlib.h>
#include <vector>
#include "cRunWatch.h"

#include "cZone.h"

pup::cZone theZone;

main()
{
    std::cout << "Pickup\n";

    raven::set::cRunWatch::Start();

    // stack orders
    theZone.orderStack();

    // optimize delivery routes
    theZone.delivery();

    // assign riders
    theZone.assignRiders();

    std::cout << theZone.stackCount() << " order stacks created\n";

    raven::set::cRunWatch::Report();
}