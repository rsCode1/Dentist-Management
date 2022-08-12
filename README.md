# Dentist-Management
A project that simulates the work-flow of a Dentist and customers. using semaphores in C.

## Goal of Project
Simulate the work-flow between a dentist ,and his client with the help of semaphores and linked lists.

## Exercise constraints
* In the dentist clinic there are 3 dentists ,3 chairs and a waiting room.
* In the waiting room there is a couch that fits 4 clients ,the rest of the waiting clients are forced to stand.
* The clinic cant hold more than N clients,no client shall enter if the clinic is full.
* Inside the clinic , a client can sit on the couch,stand,or be in treatment.
* When the dentist is available the client which sat on the couch for the longest time is receiveing treatment.
* Once a seat in the couch is available,another client must sit .
* When the treatment of a client is over ,the dentist can receive payment,but only for one client at a time.

### Demo
![demo](https://github.com/rsCode1/Dentist-Management/blob/638ffc04f380fa641410ffabf58e71b75a39c82e/demo.png)


