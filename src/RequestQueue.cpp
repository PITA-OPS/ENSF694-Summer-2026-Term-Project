#include "RequestQueue.h"

/* REQUIRES: initialCapacity is zero or greater.
   PROMISES: creates an empty circular queue with at least one slot. */
RequestQueue::RequestQueue(size_t initialCapacity)
    : data(initialCapacity == 0 ? 1 : initialCapacity),
      frontIndex(0), count(0) {}

/* REQUIRES: the queue storage is full.
   PROMISES: doubles capacity while preserving FIFO order. */
void RequestQueue::grow() {
    vector<IncomingRequest> larger(data.size() * 2);
    for (size_t i = 0; i < count; ++i) {
        larger[i] = data[(frontIndex + i) % data.size()];
    }
    data.swap(larger);
    frontIndex = 0;
}

/* REQUIRES: request contains incoming-request data.
   PROMISES: adds the requst at the rear of the queu. */
void RequestQueue::enqueue(const IncomingRequest& request) {
    if (count == data.size()) grow();
    size_t rear = (frontIndex + count) % data.size();
    data[rear] = request;
    ++count;
}

/* REQUIRES: request can receive the removed item.
   PROMISES: removes the oldest request and reports success. */
bool RequestQueue::dequeue(IncomingRequest& request) {
    if (count == 0) return false;
    request = data[frontIndex];
    frontIndex = (frontIndex + 1) % data.size();
    --count;
    return true;
}

/* REQUIRES: request can receive a copied item.
   PROMISES: copies the oldest request without removing it. */
bool RequestQueue::peek(IncomingRequest& request) const {
    if (count == 0) return false;
    request = data[frontIndex];
    return true;
}

/* REQUIRES: the queue object exists.
   PROMISES: returns true exactly when no requests are stored. */
bool RequestQueue::empty() const { return count == 0; }

/* REQUIRES: the queue object exists.
   PROMISES: returns the number of queued requests. */
size_t RequestQueue::size() const { return count; }
