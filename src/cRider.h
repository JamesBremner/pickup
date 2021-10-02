namespace quad
{
    class cCell;
}
namespace pup
{
    /// A rider capable of picking up an order stack and delivering the orders to several points
    class cRider
    {
    public:
        std::pair<float, float> myLocation; // starting location
        bool myBusy;                        // true if assigned to an order stack

        /// Construct rider at random starting location
        cRider();
    };

    /// A pool of riders, available or busy delivering orders
    class cRiderPool
    {
    public:
        // CTOR
        cRiderPool();

        void simulate();

        // assign riders to pickup orders
        void assign();

        /** location of a rider
         * @param[in] index of rider
         * @return location, pair of floats ( x, y )
         * 
         * If index does not exist, returns ( -1e6,-1e6 )
         */
        std::pair<float, float> location( int index )
        {
            if( 0 > index || index >= myRiders.size() )
                return std::pair<float,float>( -1e6,-1e6 );
            return myRiders[ index ].myLocation;
        }

    private:
        std::vector<pup::cRider> myRiders; // the riders
        quad::cCell *myQuadTree;           // the riders starting locations in a quad tree

        void quadTreeBuild();
    };
}

extern pup::cRiderPool * theRiders;