namespace quad
{
    class cCell;
}
namespace pup {
/// A rider capable of picking up an order stack and delivering the orders to several points
class cRider
{
    public:
    std::pair<float,float> myLocation;  // starting location
    bool myBusy;                        // true if assigned to an order stack

    /// Construct rider at random starting location
    cRider();
};

/// A pool of riders, available or busy delivering orders
class cRiderPool
{
public:
    std::vector<pup::cRider> myRiders;      // the riders
    quad::cCell * myQuadTree;               // the riders starting locations in a quad tree

    // CTOR
    cRiderPool();

    // assign riders to pickup orders
    void assign();
};
}