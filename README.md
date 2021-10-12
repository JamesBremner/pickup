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

[GUI User's manual](https://github.com/JamesBremner/pickup/wiki/GUI-User-Manual)

### Performance

![image](https://user-images.githubusercontent.com/2046227/137009815-82a55c85-b0b8-4843-958d-982cc0d47f9d.png)

### Database

[Database Design Documentation](https://github.com/JamesBremner/pickup/wiki/Database-Design)

### Pickup

A console application to test and time profile the pickup code used by the server
 
[Pickup User's Manual](https://github.com/JamesBremner/pickup/wiki/Pickup-User's-Manual)
