//#include <iostream>
#include "HashTable.h"

/* REQUIRES: capacity is greater than zero.
   PROMISES: creates an empty hash table and requested bucket count. */
BuildingHashTable::BuildingHashTable(size_t capacity)
    : buckets(capacity, nullptr), itemCount(0) {}

/* REQUIRES: the hash table was constructed normally.
   PROMISES: releases all dyn alloc table nodes. */
BuildingHashTable::~BuildingHashTable() {
    for (Node* head : buckets) {
        while (head != nullptr) {
            Node* doomed = head;
            head = head->next;
            delete doomed;
        }
    }
}

/* REQUIRES: valid key and buckets .
   PROMISES: returns bucket index fkey. */
size_t BuildingHashTable::hash(const string& key) const {
    unsigned long long h = 5381;
    for (unsigned char c : key) {
        h = ((h << 5) + h) + c;
    }
    return static_cast<size_t>(h % buckets.size());
}

/* REQUIRES: value points to a proper building.
   PROMISES: inserts/updates the key and reports whether newly added. */
bool BuildingHashTable::insert(const string& key, Building* value) {
    size_t index = hash(key);
    for (Node* p = buckets[index]; p != nullptr; p = p->next) {
        if (p->key == key) {
            p->value = value;
            return false;
        }
    }
    buckets[index] = new Node(key, value, buckets[index]);
    ++itemCount;
    return true;
}

/* REQUIRES: key to building to search for.
   PROMISES: returns the building pointer, or nullptr if not. */
Building* BuildingHashTable::find(const string& key) const {
    size_t index = hash(key);
    for (Node* p = buckets[index]; p != nullptr; p = p->next) {
        if (p->key == key) return p->value;
    }
    return nullptr;
}

/* REQUIRES: key is the building key to remove
   PROMISES: removes the matching entry and reports whether it existed. */
bool BuildingHashTable::remove(const string& key) {
    size_t index = hash(key);
    Node* p = buckets[index];
    Node* prev = nullptr;

    while (p != nullptr) {
        if (p->key == key) {
            if (prev == nullptr) buckets[index] = p->next;
            else prev->next = p->next;
            delete p;
            --itemCount;
            return true;
        }
        prev = p;
        p = p->next;
    }
    return false;
}

/* REQUIRES: the hash table object
   PROMISES: returns number of stored entries; */
size_t BuildingHashTable::size() const { return itemCount; }
