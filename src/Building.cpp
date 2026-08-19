#include "Building.h"

/* REQUIRES: none
   PROMISES: creates an empty room with zero capacity. */
Room::Room() : roomID(""), capacity(0), roomType("") {}

/* REQUIRES: cap is >0 and id/type describe the room.
   PROMISES: creates a room with the given ID, capacity, and type. */
Room::Room(const string& id, int cap, const string& type)
    : roomID(id), capacity(cap), roomType(type) {}

/* REQUIRES: room object exists.
   PROMISES: returns the room ID w/o changing the room. */
const string& Room::getID() const { return roomID; }

/* REQUIRES: room object exists.
   PROMISES: returns room capacity w/o changing the room. */
int Room::getCapacity() const { return capacity; }

/* REQUIRES: the Room object exists.
   PROMISES: returns the room type w/o changing the room. */
const string& Room::getType() const { return roomType; }

/* REQUIRES: nothng.
   PROMISES: creates an empty building at coordinates (0, 0). */
Building::Building() : buildingID(""), name(""), x(0.0), y(0.0) {}

/* REQUIRES: id/name describe the building.
   PROMISES: creates a building with the id, name, and coordinates. */
Building::Building(const string& id, const string& buildingName,
                   double xCoord, double yCoord)
    : buildingID(id), name(buildingName), x(xCoord), y(yCoord) {}

/* REQUIRES: the Building object exists.
   PROMISES: returns the building ID without changing the building. */
const string& Building::getID() const { return buildingID; }

/* REQUIRES: the Building object exists.
   PROMISES: returns the building name without changing the building. */
const string& Building::getName() const { return name; }

/* REQUIRES: the Building object exists.
   PROMISES: returns the building x-coordinate. */
double Building::getX() const { return x; }

/* REQUIRES: the Building object exists.
   PROMISES: returns the building y-coordinate. */
double Building::getY() const { return y; }

/* REQUIRES: room has a valid room ID.
   PROMISES: adds the room only if its ID is not already present. */
void Building::addRoom(const Room& room) {
    if (findRoom(room.getID()) == nullptr) {
        rooms.push_back(room);
    }
}

/* REQUIRES: roomID identifies the room to remove.
   PROMISES: remove the matching room and reports whether it was found. */
bool Building::removeRoom(const string& roomID) {
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        if (it->getID() == roomID) {
            rooms.erase(it);
            return true;
        }
    }
    return false;
}

/* REQUIRES: roomID is the room ID to search for.
   PROMISES: returns the matching room ptr, or nullptr if not found. */
Room* Building::findRoom(const string& roomID) {
    for (auto& room : rooms) {
        if (room.getID() == roomID) return &room;
    }
    return nullptr;
}

/* REQUIRES: roomID is the room ID to search for.
   PROMISES: returns the matching room ptr, or nullptr if not found */
const Room* Building::findRoom(const string& roomID) const {
    for (const auto& room : rooms) {
        if (room.getID() == roomID) return &room;
    }
    return nullptr;
}

/* REQUIRES: the Building object exists.
   PROMISES: returns the room collection without changing the building. */
const vector<Room>& Building::getRooms() const { return rooms; }
