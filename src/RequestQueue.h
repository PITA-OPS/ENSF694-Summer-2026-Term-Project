#ifndef REQUEST_QUEUE_H
#define REQUEST_QUEUE_H

#include <string>
#include <vector>

using namespace std;

struct IncomingRequest {
    int requestID = 0;
    string type;
    string payload;
};

class RequestQueue {
private:
    vector<IncomingRequest> data;
    size_t frontIndex;
    size_t count;

    void grow();

public:
    explicit RequestQueue(size_t initialCapacity = 8);

    void enqueue(const IncomingRequest& request);
    bool dequeue(IncomingRequest& request);
    bool peek(IncomingRequest& request) const;
    bool empty() const;
    size_t size() const;
};

#endif
