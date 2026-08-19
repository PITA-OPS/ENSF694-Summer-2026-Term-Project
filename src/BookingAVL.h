#ifndef BOOKING_AVL_H
#define BOOKING_AVL_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct DateTime {
    //Favour please check
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;

    bool operator<(const DateTime& other) const;
    bool operator==(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;

    long long key() const;
    string toString() const;

    static DateTime parse(const string& date, const string& time);
};

struct Booking {
    int bookingID = 0;
    string roomID;
    string eventName;
    DateTime start;
    DateTime end;
};

class BookingAVL {
private:
    // this one too
    struct Node {
        Booking booking;
        Node* left;
        Node* right;
        int height;
        /* REQUIRES: b contains the booking stored by this node.
           PROMISES: creates one AVL node with no children and height 1. */
        explicit Node(const Booking& b)
            : booking(b), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;
    size_t itemCount;

    static bool lessBooking(const Booking& a, const Booking& b);
    static int height(Node* node);
    static int balance(Node* node);
    static void updateHeight(Node* node);
    static Node* rotateRight(Node* y);
    static Node* rotateLeft(Node* x);
    static Node* minNode(Node* node);

    Node* insert(Node* node, const Booking& booking, bool& inserted);
    Node* remove(Node* node, const Booking& target, bool& removed);
    Node* rebalance(Node* node);

    void rangeQuery(Node* node, const DateTime& from, const DateTime& to,
                    vector<Booking>& result) const;
    void dayQuery(Node* node, int year, int month, int day,
                  vector<Booking>& result) const;
    void nextQuery(Node* node, const DateTime& now, const Booking*& best) const;
    void printTree(Node* node, const string& prefix, bool isLeft,
                   ostream& out) const;
    void destroy(Node* node);
    bool verifyBalanced(Node* node, int& computedHeight) const;

public:
    BookingAVL();
    ~BookingAVL();

    BookingAVL(const BookingAVL&) = delete;
    BookingAVL& operator=(const BookingAVL&) = delete;

    bool insert(const Booking& booking);
    bool removeByID(int bookingID);
    bool findByID(int bookingID, Booking& result) const;

    vector<Booking> rangeQuery(const DateTime& from, const DateTime& to) const;
    vector<Booking> eventsOnDay(int year, int month, int day) const;
    bool nextUpcoming(const DateTime& now, Booking& result) const;

    size_t size() const;
    bool isBalanced() const;
    void printTree(ostream& out = cout) const;
};

#endif
