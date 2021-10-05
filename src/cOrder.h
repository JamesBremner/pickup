
namespace raven
{
    namespace sqlite
    {
        class cDB;
    }
}

namespace pup
{

    class cZone;

    class cRestaurant
    {
    public:
        std::pair<float, float> myLocation;

        cRestaurant(float x, float y);

        /** Construct restaurant at random location
         * @param[in] dim maximum location value
         * 
         * The restaurant will be placed somewhere in the rectangle
         * top left 0, 0  bottom right dim, dim
         */
        cRestaurant(int dim);
    };

    class cRestaurantHolder
    {

    public:
        std::vector<cRestaurant> myRestaurant;

        void simulate(const sConfig &config);

        void write(raven::sqlite::cDB &db);
        void read(raven::sqlite::cDB &db);

        int index(cRestaurant *prest);
        cRestaurant *pointer(int id);

        std::vector<cRestaurant>::iterator
        begin()
        {
            return myRestaurant.begin();
        }
        std::vector<cRestaurant>::iterator
        end()
        {
            return myRestaurant.end();
        }
    };

    ///  An order placed at a restaurant to be delivered to a location
    class cOrder
    {
    public:
        int myTime;          // time, after start of group, when order will be ready for pickup
        cRestaurant *myRest; // restaurant where order was placed
        bool myWaiting;      // true if order has not yet been picked up

        // location order to be delivered to.  Km relative to restaurant location
        std::pair<float, float> myDelivery;

        cOrder(
            int ready, cRestaurant *rest,
            float x, float y);

        /// Construct with random values, used for simulation
        cOrder(cZone *zone);

        friend std::ostream &operator<<(std::ostream &os, cOrder o)
        {
            os << "rest " << o.myRest << " at " << o.myTime;
            return os;
        }
    };

    class cOrderHolder
    {
    public:
        std::vector<cOrder> myOrder;
        void simulate(cZone *zone);
        void write(raven::sqlite::cDB &db, cZone *zone);
        void read(raven::sqlite::cDB &db, cZone *zone);

        std::vector<cOrder>::iterator
        begin()
        {
            return myOrder.begin();
        }
        std::vector<cOrder>::iterator
        end()
        {
            return myOrder.end();
        }
    };

    /// A stack of orders from one restaurant to be picked up together by one driver
    class cStack
    {
    public:
        std::vector<cOrder> myOrder; /// the orders
        int myRider;                 /// index of assigned rider
        cRestaurant *restaurant();   /// pointer to restaurant where orders placed

        /// delivery locations in optimized order
        std::vector<std::pair<float, float>>
        deliveryLocations();

        int orderCount() const
        {
            return myOrder.size();
        }

        // detailed report
        std::string text(cZone *zone);
    };

}
