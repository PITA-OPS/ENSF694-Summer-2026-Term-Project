#include "CampusSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

/* REQUIRES: none.
   PROMISES: creates an empty system and initializes all data structures. */
CampusSystem::CampusSystem()
    : buildings(), lookup(53), graph(), history(50),
      bookings(), serviceQueue(), requestQueue(8) {
    buildings.reserve(64);
}

/* REQUIRES: building has a unique non-empty ID.
   PROMISES: stores it in the lookup table and graph. */
bool CampusSystem::addBuilding(const Building& building) {
    if (lookup.find(building.getID()) != nullptr) return false;
    buildings.push_back(building);
    Building* ptr = &buildings.back();
    lookup.insert(ptr->getID(), ptr);
    graph.addBuilding(ptr);
    return true;
}

/* REQUIRES: filename names a readable campus map file.
   PROMISES: loads map data and reports whether the file opened. */
bool CampusSystem::loadMap(const string& filename) {
    ifstream in(filename);
    if (!in) return false;

    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        string from, to;
        double weight;
        if (!(iss >> from >> to >> weight)) continue;

        if (!lookup.find(from)) addBuilding(Building(from, from));
        if (!lookup.find(to)) addBuilding(Building(to, to));
        graph.addPath(from, to, weight);
    }
    return true;
}

/* REQUIRES: filename names a readable booking file.
   PROMISES: loads bookings and reports whether the file opened. */
bool CampusSystem::loadBookings(const string& filename) {
    ifstream in(filename);
    if (!in) return false;

    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        Booking b;
        string date, startTime, endTime;
        if (!(iss >> b.bookingID >> b.roomID >> date >> startTime >> endTime))
            continue;

        getline(iss >> ws, b.eventName);
        b.start = DateTime::parse(date, startTime);
        b.end = DateTime::parse(date, endTime);
        bookings.insert(b);
    }
    return true;
}

/* REQUIRES: id is the building ID to search for.
   PROMISES: returns its ptr, or nullptr if missing. */
Building* CampusSystem::findBuilding(const string& id) {
    return lookup.find(id);
}

/* REQUIRES: id is the lookup key to remove.
   PROMISES: removes it and reports whether it existed. */
bool CampusSystem::removeBuildingFromLookup(const string& id) {
    return lookup.remove(id);
}

/* REQUIRES: source/destination are building IDs.
   PROMISES: finds a route and records successful navigation. */
RouteResult CampusSystem::navigate(const string& source,
                                   const string& destination) {
    RouteResult route = graph.shortestPath(source, destination);
    if (route.found) history.push(route);
    return route;
}

/* REQUIRES: undoneRoute can receive the removed route.
   PROMISES: removes the latest route and reports success. */
bool CampusSystem::undoNavigation(RouteResult& undoneRoute) {
    return history.undo(undoneRoute);
}

/* REQUIRES: booking contains a valid ID and times.
   PROMISES: adds it to the AVL index and reports success. */
bool CampusSystem::addBooking(const Booking& booking) {
    return bookings.insert(booking);
}

/* REQUIRES: bookingID is the booking to remove.
   PROMISES: removes it and reports whether it existed. */
bool CampusSystem::removeBooking(int bookingID) {
    return bookings.removeByID(bookingID);
}

/* REQUIRES: from is not after to.
   PROMISES: returns bookings starting in the inclusive range. */
vector<Booking> CampusSystem::bookingsInRange(const DateTime& from,
                                                   const DateTime& to) const {
    return bookings.rangeQuery(from, to);
}

/* REQUIRES: year/month/day identify a date.
   PROMISES: returns bookings starting on that day. */
vector<Booking> CampusSystem::bookingsOnDay(int year, int month, int day) const {
    return bookings.eventsOnDay(year, month, day);
}

/* REQUIRES: now is the reference date/time.
   PROMISES: copies the next booking to the output parameter. */
bool CampusSystem::nextBooking(const DateTime& now, Booking& booking) const {
    return bookings.nextUpcoming(now, booking);
}

/* REQUIRES: request contains valid service data.
   PROMISES: adds it to the priority queue. */
void CampusSystem::submitServiceRequest(ServiceRequest request) {
    serviceQueue.enqueue(request);
}

/* REQUIRES: request can receive the selected item.
   PROMISES: removes the highest-priority request and reports success. */
bool CampusSystem::serveNextRequest(ServiceRequest& request) {
    return serviceQueue.dequeue(request);
}

/* REQUIRES: request contains incoming-request data.
   PROMISES: adds it to the FIFO processing queue. */
void CampusSystem::enqueueIncoming(const IncomingRequest& request) {
    requestQueue.enqueue(request);
}

/* REQUIRES: request can receive the oldest item.
   PROMISES: removes the oldest incoming request and reports success. */
bool CampusSystem::processIncoming(IncomingRequest& request) {
    return requestQueue.dequeue(request);
}

/* REQUIRES: the CampusSystem object exists.
   PROMISES: returns read-only access to the campus graph. */
const Graph& CampusSystem::getGraph() const { return graph; }
/* REQUIRES: the CampusSystem object exists.
   PROMISES: returns read-only access to the booking AVL tree. */
const BookingAVL& CampusSystem::getBookings() const { return bookings; }
/* REQUIRES: the CampusSystem object exists.
   PROMISES: returns the number of buildings in the lookup table. */
size_t CampusSystem::buildingCount() const { return lookup.size(); }

/* REQUIRES: route contains a route result.
   PROMISES: prints the route or a not-found message. */
static void printRoute(const RouteResult& route) {
    if (!route.found) {
        cout << "No route found.\n";
        return;
    }

    for (size_t i = 0; i < route.path.size(); ++i) {
        cout << route.path[i];
        if (i + 1 < route.path.size()) cout << " -> ";
    }
    cout << "\nTotal travel time: " << route.totalCost << " minutes\n";
}

/* REQUIRES: b contains booking information.
   PROMISES: prints one booking in a readable format. */
static void printBooking(const Booking& b) {
    cout << "#" << b.bookingID << " " << b.roomID
              << " | " << b.start.toString()
              << " - " << setfill('0') << setw(2) << b.end.hour
              << ":" << setw(2) << b.end.minute
              << " | " << b.eventName << "\n";
}

/* REQUIRES: map and booking data are loaded.
   PROMISES: runs and prints all required demo scenarios. */
void CampusSystem::runDemo() {
    cout << "\n========== DEMO SCENARIOS ==========\n";

    cout << "\n[1] Shortest path queries\n";
    printRoute(navigate("ICT", "RES"));
    printRoute(navigate("LIB", "GYM"));

    cout << "\n[2] Undo navigation\n";
    RouteResult undone;
    if (undoNavigation(undone)) {
        cout << "Undid route: " << undone.source
                  << " -> " << undone.destination << "\n";
        cout << "Previous origin restored to: " << undone.source << "\n";
    }

    cout << "\n[3] Booking range query\n";
    DateTime from = DateTime::parse("2026-08-17", "10:00");
    DateTime to = DateTime::parse("2026-08-17", "14:00");
    auto range = bookingsInRange(from, to);
    for (const auto& b : range) printBooking(b);
    cout << "Returned " << range.size() << " booking(s).\n";

    cout << "\n[4] Priority queue demo\n";
    submitServiceRequest({1, "Printer jam", 2, 0});
    submitServiceRequest({2, "Water leak", 3, 0});
    submitServiceRequest({3, "Replace whiteboard marker", 1, 0});
    submitServiceRequest({4, "Network outage", 3, 0});

    ServiceRequest service;
    while (serveNextRequest(service)) {
        cout << "Serving request #" << service.requestID
                  << " priority=" << service.priority
                  << " " << service.description << "\n";
    }

    cout << "\n[5] Fast lookup demo\n";
    Building* found = findBuilding("ICT");
    cout << "Lookup ICT: " << (found ? "FOUND" : "NOT FOUND") << "\n";
    found = findBuilding("DOES_NOT_EXIST");
    cout << "Lookup DOES_NOT_EXIST: "
              << (found ? "FOUND" : "NOT FOUND") << "\n";

    cout << "\n[6] FIFO incoming request pipeline\n";
    for (int i = 1; i <= 20; ++i) {
        enqueueIncoming({i, (i % 2 ? "NAV" : "SERVICE"),
                         "Request_" + to_string(i)});
    }

    IncomingRequest incoming;
    while (processIncoming(incoming)) {
        cout << "Processed #" << incoming.requestID
                  << " " << incoming.type
                  << " " << incoming.payload << "\n";
    }

    cout << "\n[Bonus] AVL booking index\n";
    cout << "Booking count: " << bookings.size() << "\n";
    cout << "AVL balanced: "
              << (bookings.isBalanced() ? "YES" : "NO") << "\n";

    cout << "\n====================================\n";
}
