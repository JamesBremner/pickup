# pickup

## design
 
  Order stacking and pickup

(1) Collect orders per few minutes interval and sort all orders by their prep time 

(2) group orders by restaurant 

(3) Starting from the order that has the smallest remaining prep time,
 look for any orders in the same restaurant within the time window (let's say 3-5 mins),
 if exists group them as a stack.
 
(4)  Optimize delivery route for every stack

(5)  Assign riders to order stacks

 https://stackoverflow.com/q/69355474/16582
 
 ## usage
 
 To process orders provided by an external source ( e.g. database or server )
 
 ```
>pickup
Pickup

External source of orders not available
Do you want to simulate a restaurant zone?
Type:  pickup -sim
```

To process orders in a simulated zone

```
>pickup -sim
Pickup
Simulating
Orders per hour                20000
Order collection time mins     5
Restaurants                    5000
Pickup window mins             5
Maximum order preparation mins 15
Maximum distance of rider Km   20

1407 order stacks created

Restaurant at 16 11
Rider # 285 at 16,11 delivers to ( 16.77,11.38 ) ( 16.41,12.66 )

Restaurant at 7 16
Rider # 264 at 7,16 delivers to ( 8.56,18.07 ) ( 7.46,18.23 )

Restaurant at 6 11
Rider # 51 at 7,11 delivers to ( 6.08,12.49 )

Restaurant at 16 24
Rider # 2115 at 16,18 delivers to ( 16.74,26.21 ) ( 17,25.21 )

Restaurant at 11 10
Rider # 993 at 11,10 delivers to ( 11.59,12.4 ) ( 13.17,10.93 )
raven::set::cRunWatch code timing profile
Calls           Mean (secs)     Total           Scope
       1        2.82665         2.82665         Stack orders
    1407        6.5226e-06      0.0091773       Optimize stack delivery route
    1407        7.93461e-07     0.0011164       Allocate rider to stack
```
