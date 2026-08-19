#include "NavigationHistory.h"

/* REQUIRES: maximumDepth is greater than zero.
   PROMISES: creates an empty route-history stack with that limit. */
NavigationHistory::NavigationHistory(size_t maximumDepth)
    : maxDepth(maximumDepth) {}

/* REQUIRES: route is a completed route result.
   PROMISES: pushes it and drops the oldest route if the limit is reached. */
void NavigationHistory::push(const RouteResult& route) {
    if (stack.size() == maxDepth) {
        stack.erase(stack.begin());
    }
    stack.push_back(route);
}

/* REQUIRES: route can receive the removed item.
   PROMISES: pops the latest route and reports whether one existed. */
bool NavigationHistory::undo(RouteResult& route) {
    if (stack.empty()) return false;
    route = stack.back();
    stack.pop_back();
    return true;
}

/* REQUIRES: route can receive a copied item.
   PROMISES: copies the latest route without removing it. */
bool NavigationHistory::peek(RouteResult& route) const {
    if (stack.empty()) return false;
    route = stack.back();
    return true;
}

/* REQUIRES: the history object exists.
   PROMISES: returns true exactly when no routes are stored. */
bool NavigationHistory::empty() const { return stack.empty(); }
/* REQUIRES: the history object exists.
   PROMISES: returns the number of stored routes. */
size_t NavigationHistory::size() const { return stack.size(); }
