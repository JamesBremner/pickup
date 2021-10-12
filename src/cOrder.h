
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

        void simulate(const cConfig &config);

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
        int myIndex;         // index of order in database

        // location order to be delivered to.  Km relative to restaurant location
        std::pair<float, float> myDelivery;

        cOrder(
            int ready, cRestaurant *rest,
            float x, float y,
            int index);

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
        cStack()
            : myRider(-1), myRestaurant(0)
        {
        }

        /// arrange delivery locations in optimized order
        std::vector<std::pair<float, float>>
        deliveryLocations();

        // add order to stack
        void add(const cOrder &order);

        int orderCount() const
        {
            return myOrder.size();
        }
        // pointer to restaurant
        cRestaurant *restaurant();

        // location of restaurant
        std::pair<float, float> restaurantLocation();

        void rider(int riderIndex)
        {
            myRider = riderIndex;
        }
        int rider() const
        {
            return myRider;
        }

        // detailed report
        std::string text(cZone *zone);

        std::vector<cOrder>::iterator begin()
        {
            return myOrder.begin();
        }
        std::vector<cOrder>::iterator end()
        {
            return myOrder.end();
        }

    private:
        std::vector<cOrder> myOrder; /// the orders
        int myRider;                 /// index of assigned rider
        cRestaurant *myRestaurant;   /// pointer to restaurant where orders placed
    };

    // container for stacks created by processing orders
    class vStack
    {
    public:
        /// add a stack
        void add(const cStack &s)
        {
            myStack.push_back(s);
        }

        // save stacks in database
        void write(raven::sqlite::cDB &db);

        int size() const
        {
            return (int)myStack.size();
        }
        void clear()
        {
            myStack.clear();
        }

        std::vector<cStack>::iterator begin()
        {
            return myStack.begin();
        }
        std::vector<cStack>::iterator end()
        {
            return myStack.end();
        }

    private:
        std::vector<cStack> myStack;
    };
}
