namespace pup
{

    class cZone;

    class cRestaurant
    {
    public:
        std::pair<float, float> myLocation;

        /** Construct restaurant at random location
         * @param[in] dim maximum location value
         * 
         * The restaurant will be placed somewhere in the rectangle
         * top left 0, 0  bottom right dim, dim
         */
        cRestaurant( int dim );
    };

    ///  An order placed at a restaurant to be delivered to a location
    class cOrder
    {
    public:
        int myTime;             // time, after start of group, when order will be ready for pickup
        cRestaurant * myRest;   // index of restaurant where order was placed
        bool myWaiting;         // true if order has not yet been picked up

        // location order to be delivered to.  Km relative to restaurant location
        std::pair<float, float> myDelivery;

        /// Construct with random values
        cOrder( cZone* zone );

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
        std::vector<cOrder> myOrder;
        int myRider; // index of assigned rider

        /// pointer to restaurant where order placed
        cRestaurant * restaurant();

        /// delivery locations in optimized order
        std::vector<std::pair<float, float>>
        deliveryLocations();

        // detailed report
        std::string text( cZone* zone);
    };

}
