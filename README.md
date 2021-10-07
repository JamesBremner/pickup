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
 
 ### Server
 
 The server application listens on port 5000 for a TCP client to connect.  
 
 If the client sends the message "simu" then a zone's restaurants, orders and riders
 will be simulated and stored in a database.
 
 If the client sends the message "calc" then a zone's restaurants, orders and riders will be read from a database,
the orders stacked, drivers assigned to pickup the order stacks
and the driver's delivery routes will be optimized.

### GUI

The GUI application provides a simple way to test the server application

- Enter the IP address and port number of the server.
- Click CONNECT button to connect to the server
- Click SIMULATE button to request a zone simulation
- Click CALCULATE button to request processing of the zone stored in the database

### Pickup

A console application to test and time profile the pickup code used by the server
 
 To simulate a zone's restaurants, orders and riders
 and store them in a database.
 
 ```
>pickup -sim
Pickup
Simulating
Zone dimension Km                25
Orders per hour                  20000
Order collection time mins     5
Restaurants                    5000
Pickup window mins             5
Maximum order preparation mins 15
Maximum distance of rider Km   10

raven::set::cRunWatch code timing profile
Calls           Mean (secs)     Total           Scope
       1        0.118815        0.118815                Write DB
```

To read a zone's restaurants, orders and riders from a database,
stack the orders, assign drivers to pickup the order stacks
and optimize the driver's delivery routes


```
>pickup
Pickup
5000 restaurants loaded
1666 orders loaded
5000 riders loaded

1407 order stacks created

raven::set::cRunWatch code timing profile
Calls           Mean (secs)     Total           Scope
       1        2.9227          2.9227          Stack orders
    1407        1.17992e-05     0.0166015       Allocate rider to stack
    1407        6.85053e-06     0.0096387       Optimize stack delivery route
       1        0.0066089       0.0066089       Read DB

```
