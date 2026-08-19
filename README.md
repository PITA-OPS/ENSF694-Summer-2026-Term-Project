# ENSF 694 Summer 2026 Term Project

Campus Navigation and Event Management System implemented in C++.

## Features

- Weighted undirected campus graph
- Dijkstra shortest-path routing
- Navigation history with undo
- AVL-tree booking index
- Time-range and day booking queries
- Next-upcoming booking lookup
- Custom priority queue for campus service requests
- Custom hash table for building/resource lookup
- Custom FIFO circular queue for incoming requests
- Demo mode covering the required scenarios
- Bonus AVL balance demonstration

## Build

From the project root:

```bash (Terminal)
g++ -Wall src/*.cpp -o campus_app
```
    `or`
```bash (Terminal)
g++ -std=c++17 -Wall -Wextra -pedantic src/*.cpp -o campus_app
```

## Run

```bash (Terminal)
./campus_app
```

The program expects two files which are in the data subdirectory of the project. They are:

- `data/campus_map.txt`
- `data/bookings.txt`

You may override these paths (in main.cpp) to point to your own filepaths:

std::string mapFile = "data/campus_map.txt";
std::string bookingFile = "data/bookings.txt";

and run: 
./campus_app path/to/your/map.txt path/to/yout/bookings.txt


## Demo

Run:

./campus_app (in bash/Terminal)


Then choose option `13` from the main menu.

## Campus map format

Each non-comment line is:

SOURCE DESTINATION WEIGHT

Example:

ICT LIB 4

All paths are treated as undirected.


## Booking file format

Each non-comment line is:

ID ROOM_ID YYYY-MM-DD START_TIME END_TIME EVENT NAME...

Example:

1 ICT-121 2026-08-17 09:00 10:00 Algorithms Lecture


## AI disclosure

Generated the ⁠ `campus_map.txt` ⁠ and ⁠ `bookings.txt` ⁠ files as this is a redious non coding excersise - happy to discuss if needed but seemed out of scope of class
