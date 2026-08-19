#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include "Building.h"

using namespace std;

struct RouteResult {
    bool found = false;
    vector<string> path;
    double totalCost = 0.0;
    string source;
    string destination;
};

class Graph {
private:
    // is that right?
    struct Edge {
        int to;
        double weight;
    };

    struct Vertex {
        Building* building;
        vector<Edge> edges;
    };

    struct HeapNode {
        int vertex;
        double distance;
    };

    vector<Vertex> vertices;

    int indexOf(const string& buildingID) const;

    class MinHeap {
    private:
        vector<HeapNode> heap;
        void bubbleUp(size_t index);
        void bubbleDown(size_t index);
    public:
        bool empty() const;
        void push(const HeapNode& node);
        HeapNode pop();
    };

public:
    bool addBuilding(Building* building);
    bool addPath(const string& from, const string& to, double weight);
    bool loadFromFile(const string& filename,
                      vector<Building>& buildingStorage);

    RouteResult shortestPath(const string& source,
                             const string& destination) const;

    void printGraph() const;
    size_t vertexCount() const;
    size_t edgeCount() const;
};

#endif
