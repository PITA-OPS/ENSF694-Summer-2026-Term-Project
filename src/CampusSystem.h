#ifndef CAMPUS_SYSTEM_H
#define CAMPUS_SYSTEM_H

#include <vector>
#include <string>
#include "Building.h"
#include "HashTable.h"
#include "Graph.h"
#include "NavigationHistory.h"
#include "BookingAVL.h"
#include "ServicePriorityQueue.h"
#include "RequestQueue.h"

using namespace std;

class CampusSystem {
private:
    vector<Building> buildings;
    BuildingHashTable lookup;
    Graph graph;
    NavigationHistory history;
    BookingAVL bookings;
    ServicePriorityQueue serviceQueue;
    RequestQueue requestQueue;

public:
    CampusSystem();

    bool loadMap(const string& filename);
    bool loadBookings(const string& filename);

    bool addBuilding(const Building& building);
    Building* findBuilding(const string& id);
    bool removeBuildingFromLookup(const string& id);

    RouteResult navigate(const string& source, const string& destination);
    bool undoNavigation(RouteResult& undoneRoute);

    bool addBooking(const Booking& booking);
    bool removeBooking(int bookingID);
    vector<Booking> bookingsInRange(const DateTime& from, const DateTime& to) const;
    vector<Booking> bookingsOnDay(int year, int month, int day) const;
    bool nextBooking(const DateTime& now, Booking& booking) const;

    void submitServiceRequest(ServiceRequest request);
    bool serveNextRequest(ServiceRequest& request);

    void enqueueIncoming(const IncomingRequest& request);
    bool processIncoming(IncomingRequest& request);

    const Graph& getGraph() const;
    const BookingAVL& getBookings() const;
    size_t buildingCount() const;

    void runDemo();
};

#endif
