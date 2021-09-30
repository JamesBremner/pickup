namespace pup {

class cRestaurant
{
public:
    std::pair<float,float> myLocation;

    cRestaurant();
};

///  An order placed at a resteraunt to be delivered to a location
class cOrder
{
public:
    int myTime;         // time, after start of group, when order will be ready for pickup
    int myRest;         // index of restaurant where order was placed
    bool myWaiting;     // true if order has not yet been picked up

    // location order to be delivered to.  Km relative to restaurant location
    std::pair< float,float > myDelivery;

    /// Construct with random values
    cOrder();

    friend std::ostream &operator<<(std::ostream &os, cOrder o)
    {
        os << "rest " << o.myRest << " at " << o.myTime;
        return os;
    }
};

/// A stack of orders from one restaurant to be picked up together by one driver
class cStack
{
    public:
    std::vector< cOrder > myOrder;

    std::pair<float,float>
    restaurantLocation();

    std::vector<std::pair<float,float> >
    deliveryLocations();
};
}
