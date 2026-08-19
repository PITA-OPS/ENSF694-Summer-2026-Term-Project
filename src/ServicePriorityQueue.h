#ifndef SERVICE_PRIORITY_QUEUE_H
#define SERVICE_PRIORITY_QUEUE_H

#include <string>
#include <vector>

using namespace std;

struct ServiceRequest {
    int requestID = 0;
    string description;
    int priority = 1; // 3 = Emergency, 2 = Standard, 1 = Low
    long long arrivalOrder = 0;
};

class ServicePriorityQueue {
private:
    vector<ServiceRequest> heap;
    long long sequence;

    static bool higherPriority(const ServiceRequest& a, const ServiceRequest& b);
    void bubbleUp(size_t index);
    void bubbleDown(size_t index);

public:
    ServicePriorityQueue();

    void enqueue(ServiceRequest request);
    bool dequeue(ServiceRequest& request);
    bool peek(ServiceRequest& request) const;
    bool empty() const;
    size_t size() const;
};

#endif
