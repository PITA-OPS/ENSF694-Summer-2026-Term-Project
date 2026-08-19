#include "ServicePriorityQueue.h"
#include <utility>

/* REQUIRES: none.
   PROMISES: creates an empty service-request priority queue. */
ServicePriorityQueue::ServicePriorityQueue() : sequence(0) {}

/* REQUIRES: both requests have valid priority/order values.
   PROMISES: returns which request should be served first. */
bool ServicePriorityQueue::higherPriority(const ServiceRequest& a,
                                          const ServiceRequest& b) {
    if (a.priority != b.priority) return a.priority > b.priority;
    return a.arrivalOrder < b.arrivalOrder;
}

/* REQUIRES: index refers to an existing heap element.
   PROMISES: moves it upward until max-heap order is restored. */
void ServicePriorityQueue::bubbleUp(size_t index) {
    // I think that where it is
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (higherPriority(heap[parent], heap[index])) break;
        swap(heap[parent], heap[index]);
        index = parent;
    }
}

/* REQUIRES: index refers to an existing heap element.
   PROMISES: moves it downward until max-heap order is restored. */
void ServicePriorityQueue::bubbleDown(size_t index) {
    while (true) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t best = index;

        if (left < heap.size() && higherPriority(heap[left], heap[best]))
            best = left;
        if (right < heap.size() && higherPriority(heap[right], heap[best]))
            best = right;
        if (best == index) break;

        swap(heap[index], heap[best]);
        index = best;
    }
}

/* REQUIRES: request contains a valid priority.
   PROMISES: adds it and records its arrival order. */
void ServicePriorityQueue::enqueue(ServiceRequest request) {
    request.arrivalOrder = sequence++;
    heap.push_back(request);
    bubbleUp(heap.size() - 1);
}

/* REQUIRES: request can receive the removed item.
   PROMISES: removes the highest-priority request and reports success. */
bool ServicePriorityQueue::dequeue(ServiceRequest& request) {
    if (heap.empty()) return false;

    request = heap.front();
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty()) bubbleDown(0);
    return true;
}

/* REQUIRES: request can receive a copied item.
   PROMISES: copies the next request without removing it. */
bool ServicePriorityQueue::peek(ServiceRequest& request) const {
    if (heap.empty()) return false;
    request = heap.front();
    return true;
}

/* REQUIRES: the priority queue object exists.
   PROMISES: returns true exactly when it has no requests. */
bool ServicePriorityQueue::empty() const { return heap.empty(); }

/* REQUIRES: the priority queue object exists.
   PROMISES: returns the number of service requests. */
size_t ServicePriorityQueue::size() const { return heap.size(); }
