#include <iostream>
#include <limits>
#include <string>
#include "CampusSystem.h"

using namespace std;

/* REQUIRES: route contains a route result.
   PROMISES: prints the route/cost or a not-found message. */
static void printRoute(const RouteResult& route) {
    if (!route.found) {
        cout << "No route found. Check the building IDs.\n";
        return;
    }

    cout << "Route: ";
    for (size_t i = 0; i < route.path.size(); ++i) {
        cout << route.path[i];
        if (i + 1 < route.path.size()) cout << " -> ";
    }
    cout << "\nTotal travel time: " << route.totalCost << " minutes\n";
}

/* REQUIRES: b contains booking information.
   PROMISES: prints one booking in a readable format. */
static void printBooking(const Booking& b) {
    cout << "#" << b.bookingID << " | " << b.roomID
              << " | " << b.start.toString()
              << " | " << b.eventName << "\n";
}

/* REQUIRES: prompt describes the requested integer.
   PROMISES: keeps prompting until the user enters an integer. */
static int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) return value;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid number. Try again.\n";
    }
}

/* REQUIRES: standard output is available.
   PROMISES: displays the numbered main menu. */
static void showMenu() {
    cout <<
        "\n========== CAMPUS SYSTEM ==========\n"
        "1. Find shortest route\n"
        "2. Undo last navigation\n"
        "3. Find building/resource\n"
        "4. Add booking\n"
        "5. Remove booking\n"
        "6. Query bookings by time range\n"
        "7. Submit service request\n"
        "8. Serve highest-priority request\n"
        "9. Enqueue incoming request\n"
        "10. Process next incoming request\n"
        "11. Show campus graph\n"
        "12. Show AVL booking tree\n"
        "13. Run required demo scenarios\n"
        "0. Exit\n"
        "===================================\n";
}

/* REQUIRES: run from the project root or supply valid file paths.
   PROMISES: loads data, runs the menu loop, and returns an exit status. */
int main(int argc, char* argv[]) {
    string mapFile = "data/campus_map.txt";
    string bookingFile = "data/bookings.txt";

    if (argc >= 2) mapFile = argv[1];
    if (argc >= 3) bookingFile = argv[2];

    CampusSystem campus;

    if (!campus.loadMap(mapFile)) {
        cerr << "Error: could not load map file: " << mapFile << "\n";
        return 1;
    }

    if (!campus.loadBookings(bookingFile)) {
        cerr << "Warning: could not load booking file: " << bookingFile << "\n";
    }

    cout << "Loaded " << campus.getGraph().vertexCount()
              << " campus nodes and " << campus.getGraph().edgeCount()
              << " pathways.\n";
    cout << "Loaded " << campus.getBookings().size() << " bookings.\n";

    while (true) {
        showMenu();
        int choice = readInt("Select an option: ");

        if (choice == 0) {
            cout << "Goodbye.\n";
            break;
        }

        if (choice == 1) {
            string source, destination;
            cout << "Source building ID: ";
            cin >> source;
            cout << "Destination building ID: ";
            cin >> destination;
            printRoute(campus.navigate(source, destination));
        } else if (choice == 2) {
            RouteResult undone;
            if (campus.undoNavigation(undone)) {
                cout << "Undid route " << undone.source
                          << " -> " << undone.destination << "\n";
                cout << "Returned to previous origin: "
                          << undone.source << "\n";
            } else {
                cout << "Navigation history is empty.\n";
            }
        } else if (choice == 3) {
            string id;
            cout << "Building ID: ";
            cin >> id;
            Building* building = campus.findBuilding(id);
            if (building) {
                cout << "Found: " << building->getID()
                          << " - " << building->getName() << "\n";
            } else {
                cout << "Building/resource not found.\n";
            }
        } else if (choice == 4) {
            Booking b;
            string date, startTime, endTime;
            b.bookingID = readInt("Booking ID: ");
            cout << "Room ID: ";
            cin >> b.roomID;
            cout << "Date (YYYY-MM-DD): ";
            cin >> date;
            cout << "Start time (HH:MM): ";
            cin >> startTime;
            cout << "End time (HH:MM): ";
            cin >> endTime;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Event name: ";
            getline(cin, b.eventName);

            b.start = DateTime::parse(date, startTime);
            b.end = DateTime::parse(date, endTime);

            cout << (campus.addBooking(b) ? "Booking added.\n"
                                               : "Booking not added.\n");
        } else if (choice == 5) {
            int id = readInt("Booking ID to remove: ");
            cout << (campus.removeBooking(id) ? "Booking removed.\n"
                                                   : "Booking not found.\n");
        } else if (choice == 6) {
            string fromDate, fromTime, toDate, toTime;
            cout << "From date (YYYY-MM-DD): ";
            cin >> fromDate;
            cout << "From time (HH:MM): ";
            cin >> fromTime;
            cout << "To date (YYYY-MM-DD): ";
            cin >> toDate;
            cout << "To time (HH:MM): ";
            cin >> toTime;

            auto results = campus.bookingsInRange(
                DateTime::parse(fromDate, fromTime),
                DateTime::parse(toDate, toTime));

            if (results.empty()) cout << "No bookings in that range.\n";
            for (const auto& b : results) printBooking(b);
        } else if (choice == 7) {
            ServiceRequest r;
            r.requestID = readInt("Request ID: ");
            r.priority = readInt("Priority (3=Emergency, 2=Standard, 1=Low): ");
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Description: ";
            getline(cin, r.description);
            campus.submitServiceRequest(r);
            cout << "Service request added.\n";
        } else if (choice == 8) {
            ServiceRequest r;
            if (campus.serveNextRequest(r)) {
                cout << "Serving #" << r.requestID
                          << " priority=" << r.priority
                          << " " << r.description << "\n";
            } else {
                cout << "Service queue is empty.\n";
            }
        } else if (choice == 9) {
            IncomingRequest r;
            r.requestID = readInt("Incoming request ID: ");
            cout << "Type: ";
            cin >> r.type;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Payload: ";
            getline(cin, r.payload);
            campus.enqueueIncoming(r);
            cout << "Incoming request enqueued.\n";
        } else if (choice == 10) {
            IncomingRequest r;
            if (campus.processIncoming(r)) {
                cout << "Processed #" << r.requestID
                          << " " << r.type << " " << r.payload << "\n";
            } else {
                cout << "Incoming request queue is empty.\n";
            }
        } else if (choice == 11) {
            campus.getGraph().printGraph();
        } else if (choice == 12) {
            campus.getBookings().printTree();
            cout << "Balanced: "
                      << (campus.getBookings().isBalanced() ? "YES" : "NO")
                      << "\n";
        } else if (choice == 13) {
            campus.runDemo();
        } else {
            cout << "Unknown option.\n";
        }
    }

    return 0;
}
