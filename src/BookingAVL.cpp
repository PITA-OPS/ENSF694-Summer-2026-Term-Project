#include "BookingAVL.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

/* REQUIRES: both DateTime values that contain comparable fields.
   PROMISES: returns true when this value occurs before other. */
bool DateTime::operator<(const DateTime& other) const { return key() < other.key(); }
/* REQUIRES: ^
   PROMISES: true when both date/time values are equal. */
bool DateTime::operator==(const DateTime& other) const { return key() == other.key(); }
/* REQUIRES: ^
   PROMISES: returns true when this value is not later than other. */
bool DateTime::operator<=(const DateTime& other) const { return key() <= other.key(); }

/* REQUIRES: datetime fields that contain numeric values.
   PROMISES: converts the fields to one sortable numeric key. */
long long DateTime::key() const {
    long long value = year;
    value = value * 100 + month;
    value = value * 100 + day;
    value = value * 100 + hour;
    value = value * 100 + minute;
    return value;
}

/* REQUIRES: the DateTime object.
   PROMISES: formats as YYYY-MM-DD HH:MM. */
string DateTime::toString() const {
    ostringstream out;
    out << setfill('0')
        << setw(4) << year << "-"
        << setw(2) << month << "-"
        << setw(2) << day << " "
        << setw(2) << hour << ":"
        << setw(2) << minute;
    return out.str();
}

/* REQUIRES: date is YYYY-MM-DD and time is HH:MM.
   PROMISES: DateTime from strings. */
DateTime DateTime::parse(const string& date, const string& time) {
    DateTime dt;
    char dash1, dash2, colon;
    istringstream ds(date);
    istringstream ts(time);
    ds >> dt.year >> dash1 >> dt.month >> dash2 >> dt.day;
    ts >> dt.hour >> colon >> dt.minute;
    return dt;
}

/* REQUIRES: nothing
   PROMISES: creation of an empty booking tree. */
BookingAVL::BookingAVL() : root(nullptr), itemCount(0) {}
/* REQUIRES: the AVL tree was constructed normally.
   PROMISES: releases of all nodes */
BookingAVL::~BookingAVL() { destroy(root); }

/* REQUIRES: valid times and IDs for booking.
   PROMISES: orders bookings by start time, then booking ID. */
bool BookingAVL::lessBooking(const Booking& a, const Booking& b) {
    if (a.start.key() != b.start.key()) return a.start.key() < b.start.key();
    return a.bookingID < b.bookingID;
}

/* REQUIRES: node is nullptr or a valid AVL node.
   PROMISES: zero for nullptr or the node's height. */
int BookingAVL::height(Node* node) { return node ? node->height : 0; }

/* REQUIRES: node is nullptr or a valid AVL node.
   PROMISES: returns left height minus right height. */
int BookingAVL::balance(Node* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

/* REQUIRES: node is nullptr or a valid AVL node.
   PROMISES: recalculates its stored height when it exists. */
void BookingAVL::updateHeight(Node* node) {
    if (node) node->height = 1 + max(height(node->left), height(node->right));
}

/* REQUIRES: y has a valid left child.
   PROMISES: does right rotation and returns the new root. */
BookingAVL::Node* BookingAVL::rotateRight(Node* y) {
    Node* x = y->left;
    Node* t2 = x->right;
    x->right = y;
    y->left = t2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

/* REQUIRES: x has a valid right child.
   PROMISES: left rotation and returns the new root. */
BookingAVL::Node* BookingAVL::rotateLeft(Node* x) {
    Node* y = x->right;
    Node* t2 = y->left;
    y->left = x;
    x->right = t2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

/* REQUIRES: node is nullptr / valid subtree root.
   PROMISES: the most left node in subtree. */
BookingAVL::Node* BookingAVL::minNode(Node* node) {
    Node* current = node;
    while (current && current->left) current = current->left;
    return current;
}

/* REQUIRES: node is nullptr or a valid AVL subtree root.
   PROMISES: AVL gets balanced and returns the subtree root. */
BookingAVL::Node* BookingAVL::rebalance(Node* node) {
    //check this please
    if (!node) return node;

    updateHeight(node);
    int b = balance(node);

    if (b > 1) {
        if (balance(node->left) < 0)
            node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    if (b < -1) {
        if (balance(node->right) > 0)
            node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

/* REQUIRES: node is a valid subtree root or nullptr.
   PROMISES: recursively inserts booking and rebalances the subtree. */
BookingAVL::Node* BookingAVL::insert(Node* node, const Booking& booking,
                                     bool& inserted) {
    if (!node) {
        inserted = true;
        return new Node(booking);
    }

    if (lessBooking(booking, node->booking))
        node->left = insert(node->left, booking, inserted);
    else if (lessBooking(node->booking, booking))
        node->right = insert(node->right, booking, inserted);
    else
        return node;

    return rebalance(node);
}

/* REQUIRES: node is a valid subtree root or nullptr.
   PROMISES: recursively removes target and rebalances the subtree. */
BookingAVL::Node* BookingAVL::remove(Node* node, const Booking& target,
                                     bool& removed) {
    if (!node) return nullptr;

    if (lessBooking(target, node->booking)) {
        node->left = remove(node->left, target, removed);
    } else if (lessBooking(node->booking, target)) {
        node->right = remove(node->right, target, removed);
    } else {
        removed = true;

        if (!node->left || !node->right) {
            Node* child = node->left ? node->left : node->right;
            delete node;
            return child;
        }

        Node* successor = minNode(node->right);
        node->booking = successor->booking;
        bool ignored = false;
        node->right = remove(node->right, successor->booking, ignored);
    }

    return rebalance(node);
}

/* REQUIRES: booking has a valid ID and start time.
   PROMISES: adds if unique and reports if inserted. */
bool BookingAVL::insert(const Booking& booking) {
    bool inserted = false;
    root = insert(root, booking, inserted);
    if (inserted) ++itemCount;
    return inserted;
}

/* REQUIRES: bookingID is the ID to search for.
   PROMISES: copies the matching booking to result and reports success. */
bool BookingAVL::findByID(int bookingID, Booking& result) const {
    vector<Node*> stack;
    Node* current = root;

    while (current || !stack.empty()) {
        while (current) {
            stack.push_back(current);
            current = current->left;
        }
        current = stack.back();
        stack.pop_back();

        if (current->booking.bookingID == bookingID) {
            result = current->booking;
            return true;
        }
        current = current->right;
    }
    return false;
}

/* REQUIRES: bookingID is the ID to remove.
   PROMISES: removes that booking n reports whether existed. */
bool BookingAVL::removeByID(int bookingID) {
    Booking target;
    if (!findByID(bookingID, target)) return false;

    bool removed = false;
    root = remove(root, target, removed);
    if (removed) --itemCount;
    return removed;
}

/* REQUIRES: node is valid/null and from is not after to.
   PROMISES: adds matching bookings to result. */
void BookingAVL::rangeQuery(Node* node, const DateTime& from, const DateTime& to,
                            vector<Booking>& result) const {
    if (!node) return;

    if (!(node->booking.start < from))
        rangeQuery(node->left, from, to, result);

    if (!(node->booking.start < from) && node->booking.start <= to)
        result.push_back(node->booking);

    if (node->booking.start <= to)
        rangeQuery(node->right, from, to, result);
}

/* REQUIRES: 'from' is not to be after 'to'.
   PROMISES: returns bookings starting inside the inclusive range. */
vector<Booking> BookingAVL::rangeQuery(const DateTime& from,
                                            const DateTime& to) const {
    vector<Booking> result;
    rangeQuery(root, from, to, result);
    return result;
}

/* REQUIRES: node is valid/null and date fields identify a day.
   PROMISES: adds that day's bookings to result. */
void BookingAVL::dayQuery(Node* node, int year, int month, int day,
                          vector<Booking>& result) const {
    if (!node) return;
    dayQuery(node->left, year, month, day, result);
    const DateTime& dt = node->booking.start;
    if (dt.year == year && dt.month == month && dt.day == day)
        result.push_back(node->booking);
    dayQuery(node->right, year, month, day, result);
}

/* REQUIRES: year/month/day identify the requested date.
   PROMISES: returns all bookings starting on that date. */
vector<Booking> BookingAVL::eventsOnDay(int year, int month, int day) const {
    vector<Booking> result;
    dayQuery(root, year, month, day, result);
    return result;
}

/* REQUIRES: node is valid/null and now is the ref time.
   PROMISES: searches for the earliest booking at or after now. */
void BookingAVL::nextQuery(Node* node, const DateTime& now,
                           const Booking*& best) const {
    if (!node) return;

    if (!(node->booking.start < now)) {
        best = &node->booking;
        nextQuery(node->left, now, best);
    } else {
        nextQuery(node->right, now, best);
    }
}

/* REQUIRES: 'now' is the ref date/time.
   PROMISES: copies the next booking to result and gives success. */
bool BookingAVL::nextUpcoming(const DateTime& now, Booking& result) const {
    const Booking* best = nullptr;
    nextQuery(root, now, best);
    if (!best) return false;
    result = *best;
    return true;
}

/* REQUIRES: the AVL tree object exists.
   PROMISES: returns th number of bookngs. */
size_t BookingAVL::size() const { return itemCount; }

/* REQUIRES: node is valid/null and computedHeight is writable.
   PROMISES: checks AVL balance and returns the subtree height. */
bool BookingAVL::verifyBalanced(Node* node, int& computedHeight) const {
    if (!node) {
        computedHeight = 0;
        return true;
    }

    int lh = 0, rh = 0;
    if (!verifyBalanced(node->left, lh)) return false;
    if (!verifyBalanced(node->right, rh)) return false;

    computedHeight = 1 + max(lh, rh);
    return abs(lh - rh) <= 1 && node->height == computedHeight;
}

/* REQUIRES: the AVL tree object exists.
   PROMISES: returns true when every node satisfies AVL balance. */
bool BookingAVL::isBalanced() const {
    int h = 0;
    return verifyBalanced(root, h);
}

/* REQUIRES: node is valid/null and out is writable.
   PROMISES: print the subtree w branch indentation. */
void BookingAVL::printTree(Node* node, const string& prefix,
                           bool isLeft, ostream& out) const {
    if (!node) return;

    out << prefix << (isLeft ? "├── " : "└── ")
        << node->booking.bookingID << " "
        << node->booking.start.toString()
        << " [" << node->booking.roomID << "]\n";

    printTree(node->left, prefix + (isLeft ? "│   " : "    "), true, out);
    printTree(node->right, prefix + (isLeft ? "│   " : "    "), false, out);
}

/* REQUIRES: out is a writable output stream.
   PROMISES: prints the full AVL booking tree. */
void BookingAVL::printTree(ostream& out) const {
    if (!root) {
        out << "(empty)\n";
        return;
    }
    printTree(root, "", false, out);
}

/* REQUIRES: node is valid ornull.
   PROMISES: deletes every node in subtree. */
void BookingAVL::destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}
