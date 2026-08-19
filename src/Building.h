#ifndef BUILDING_H
#define BUILDING_H

#include <string>
#include <vector>

using namespace std;

class Room {
private:
    string roomID;
    int capacity;
    string roomType;

public:
    Room();
    Room(const string& id, int cap, const string& type);

    const string& getID() const;
    int getCapacity() const;
    const string& getType() const;
};

class Building {
private:
    string buildingID;
    string name;
    double x;
    double y;
    vector<Room> rooms;

public:
    Building();
    Building(const string& id, const string& buildingName,
             double xCoord = 0.0, double yCoord = 0.0);

    const string& getID() const;
    const string& getName() const;
    double getX() const;
    double getY() const;

    void addRoom(const Room& room);
    bool removeRoom(const string& roomID);
    Room* findRoom(const string& roomID);
    const Room* findRoom(const string& roomID) const;
    const vector<Room>& getRooms() const;
};

#endif
