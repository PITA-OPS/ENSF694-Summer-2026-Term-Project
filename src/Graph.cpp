#include "Graph.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <algorithm>

/* REQUIRES: buildingID is the ID to search for.
   PROMISES: returns its vertex index, or -1 if absent. */
int Graph::indexOf(const string& buildingID) const {
    for (size_t i = 0; i < vertices.size(); ++i) {
        if (vertices[i].building->getID() == buildingID) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

/* REQUIRES: index refers to an existing heap element.
   PROMISES: moves it upward until min-heap order is restored. */
void Graph::MinHeap::bubbleUp(size_t index) {
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap[parent].distance <= heap[index].distance) break;
        swap(heap[parent], heap[index]);
        index = parent;
    }
}

/* REQUIRES: index refers to an existing heap element.
   PROMISES: moves it downward until min-heap order is restored. */
void Graph::MinHeap::bubbleDown(size_t index) {
    while (true) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;

        if (left < heap.size() && heap[left].distance < heap[smallest].distance)
            smallest = left;
        if (right < heap.size() && heap[right].distance < heap[smallest].distance)
            smallest = right;
        if (smallest == index) break;

        swap(heap[index], heap[smallest]);
        index = smallest;
    }
}

/* REQUIRES: the heap object exists.
   PROMISES: returns true exactly when the heap is empty. */
bool Graph::MinHeap::empty() const { return heap.empty(); }

/* REQUIRES: node has a valid vertex index and distance.
   PROMISES: adds the node while preserving min-heap order. */
void Graph::MinHeap::push(const HeapNode& node) {
    heap.push_back(node);
    bubbleUp(heap.size() - 1);
}

/* REQUIRES: the heap is not empty.
   PROMISES: removes and returns the node with minimum distance. */
Graph::HeapNode Graph::MinHeap::pop() {
    HeapNode result = heap.front();
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty()) bubbleDown(0);
    return result;
}

/* REQUIRES: building points to a valid Building.
   PROMISES: adds a unique vertex and reports success. */
bool Graph::addBuilding(Building* building) {
    if (building == nullptr || indexOf(building->getID()) != -1) return false;
    vertices.push_back({building, {}});
    return true;
}

/* REQUIRES: both IDs exist and weight is non-negative.
   PROMISES: adds one undirected edge if it is not already present. */
bool Graph::addPath(const string& from, const string& to, double weight) {
    if (weight < 0) return false;

    int fromIndex = indexOf(from);
    int toIndex = indexOf(to);
    if (fromIndex == -1 || toIndex == -1) return false;

    for (const Edge& edge : vertices[fromIndex].edges) {
        if (edge.to == toIndex) return false;
    }

    vertices[fromIndex].edges.push_back({toIndex, weight});
    vertices[toIndex].edges.push_back({fromIndex, weight});
    return true;
}

/* REQUIRES: filename names a readable edge-list file.
   PROMISES: loads buildings/paths and reports whether the file opened. */
bool Graph::loadFromFile(const string& filename,
                         vector<Building>& buildingStorage) {
    ifstream in(filename);
    if (!in) return false;

    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        string from, to;
        double weight;

        if (!(iss >> from >> to >> weight)) continue;

        auto ensureBuilding = [&](const string& id) {
            if (indexOf(id) == -1) {
                buildingStorage.emplace_back(id, id);
                addBuilding(&buildingStorage.back());
            }
        };

        ensureBuilding(from);
        ensureBuilding(to);
        addPath(from, to, weight);
    }
    return true;
}

/* REQUIRES: source/destination are graph building IDs.
   PROMISES: returns the shortest route/cost, or found=false. */
RouteResult Graph::shortestPath(const string& source,
                                const string& destination) const {
    RouteResult result;
    result.source = source;
    result.destination = destination;

    int s = indexOf(source);
    int d = indexOf(destination);
    if (s == -1 || d == -1) return result;

    const double INF = numeric_limits<double>::infinity();
    vector<double> dist(vertices.size(), INF);
    vector<int> prev(vertices.size(), -1);
    vector<bool> visited(vertices.size(), false);

    MinHeap heap;
    dist[s] = 0.0;
    heap.push({s, 0.0});

    while (!heap.empty()) {
        HeapNode node = heap.pop();
        int u = node.vertex;

        if (visited[u]) continue;
        visited[u] = true;
        if (u == d) break;

        for (const Edge& edge : vertices[u].edges) {
            int v = edge.to;
            double alt = dist[u] + edge.weight;
            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;
                heap.push({v, alt});
            }
        }
    }

    if (dist[d] == INF) return result;

    result.found = true;
    result.totalCost = dist[d];

    for (int at = d; at != -1; at = prev[at]) {
        result.path.push_back(vertices[at].building->getID());
    }
    reverse(result.path.begin(), result.path.end());
    return result;
}

/* REQUIRES: the graph object exists.
   PROMISES: prints every vertex and its weighted neighbours. */
void Graph::printGraph() const {
    for (const Vertex& vertex : vertices) {
        cout << vertex.building->getID() << " -> ";
        for (size_t i = 0; i < vertex.edges.size(); ++i) {
            const Edge& edge = vertex.edges[i];
            cout << vertices[edge.to].building->getID()
                      << "(" << edge.weight << ")";
            if (i + 1 < vertex.edges.size()) cout << ", ";
        }
        cout << '\n';
    }
}

/* REQUIRES: the graph object exists.
   PROMISES: returns the number of vertices. */
size_t Graph::vertexCount() const { return vertices.size(); }

/* REQUIRES: the graph object exists.
   PROMISES: returns the number of undirected edges. */
size_t Graph::edgeCount() const {
    size_t total = 0;
    for (const Vertex& vertex : vertices) total += vertex.edges.size();
    return total / 2;
}
