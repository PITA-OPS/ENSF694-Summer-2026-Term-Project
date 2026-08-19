#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include <vector>
#include "Building.h"

using namespace std;

class BuildingHashTable {
private:
    struct Node {
        string key;
        Building* value;
        Node* next;
        /* REQUIRES: v points to a valid Building and n is valid/null.
           PROMISES: creates one chained hash-table node. */
        Node(const string& k, Building* v, Node* n = nullptr)
            : key(k), value(v), next(n) {}
    };

    vector<Node*> buckets;
    size_t itemCount;

    size_t hash(const string& key) const;

public:
    explicit BuildingHashTable(size_t capacity = 31);
    ~BuildingHashTable();

    BuildingHashTable(const BuildingHashTable&) = delete;
    BuildingHashTable& operator=(const BuildingHashTable&) = delete;

    bool insert(const string& key, Building* value);
    Building* find(const string& key) const;
    bool remove(const string& key);
    size_t size() const;
};

#endif
