namespace pup {

    // pickup orders simulation parameters
struct sConfig
{
    // user values
    int OrdersPerHour;      // incoming order per hour
    int GroupTimeMins;      // order collection time
    int RestaurantCount;    // number of restaurants
    int ZoneDimKm;          // zone dimensions
    int PickupWindowMins;   // pickup window time
    int MaxPrepTimeMins;    // maximum order preparation time
    int RiderCount;         // number of delivery riders;
    float CloseRiderDistanceKm;    // maximum distance of rider to be allocated to order stack


    // calculated values
    int OrdersPerGroupTime;
    int PickupWindowSecs;

};
}
