namespace pup {

    // pickup orders simulation parameters
struct sConfig
{
    // user values
    int OrdersPerHour;      // incoming order per hour
    int GroupTimeMins;      // order collection time
    int RestaurantCount;    // number of restaurants
    int RestaurantMaxDimKm; // maximum dimension of restaurant locations
    int PickupWindowMins;   // pickup window time
    int MaxPrepTimeMins;    // maximum order preparation time


    // calculated values
    int OrdersPerGroupTime;
    int PickupWindowSecs;

};
}
