#ifndef NAVIGATION_HISTORY_H
#define NAVIGATION_HISTORY_H

#include <vector>
#include "Graph.h"

using namespace std;

class NavigationHistory {
private:
    vector<RouteResult> stack;
    size_t maxDepth;

public:
    explicit NavigationHistory(size_t maximumDepth = 50);

    void push(const RouteResult& route);
    bool undo(RouteResult& route);
    bool peek(RouteResult& route) const;
    bool empty() const;
    size_t size() const;
};

#endif
