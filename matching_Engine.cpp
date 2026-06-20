#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Required for std::sort

using namespace std;

// MODULE 1: THE INDIVIDUAL ORDER BLUEPRINT

class Order {
private:
    string orderId;
    string ticker;     
    string orderType;  
    double price;
    int quantity;

public:
    Order(string id, string symbol, string type, double prc, int qty) {
        orderId = id;
        ticker = symbol;
        orderType = type;
        price = prc;
        quantity = qty;
    }

    // Getters
    string getType() const { return orderType; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    string getTicker() const { return ticker; }
    string getId() const { return orderId; }

    void displayLog() const {
        cout << "[" << orderType << "] ID: " << orderId 
             << " | Stock: " << ticker << " | Price: Rs. " << price 
             << " | Shares: " << quantity << endl;
    }
};

// CUSTOM SORTING COMPARATORS (The Speed Keys)
// Buyers want the HIGHEST price first (Descending order)
bool compareBuyOrders(const Order& a, const Order& b) {
    return a.getPrice() > b.getPrice();
}

// Sellers want the LOWEST price first (Ascending order)
bool compareSellOrders(const Order& a, const Order& b) {
    return a.getPrice() < b.getPrice();
}

// MODULE 2: THE OPTIMIZED ORDER BOOK
class OrderBook {
private:
    vector<Order> buyOrders;
    vector<Order> sellOrders;

public:
    // Using C++ Reference (&) to pass memory instantly without copying
    void receiveOrder(const Order& newOrder) {
        if (newOrder.getType() == "BUY") {
            buyOrders.push_back(newOrder);
            // Re-sort the buy book so the highest bid is at index 0
            sort(buyOrders.begin(), buyOrders.end(), compareBuyOrders);
        } else {
            sellOrders.push_back(newOrder);
            // Re-sort the sell book so the lowest ask is at index 0
            sort(sellOrders.begin(), sellOrders.end(), compareSellOrders);
        }
        cout << ">>> System Log: Logged ";
        newOrder.displayLog();
    }

    // HIGH PERFORMANCE MATCHING
    void processMatching() {
        // If either book is empty, no transaction can happen
        if (buyOrders.empty() || sellOrders.empty()) {
            return;
        }

        // Because our vectors are sorted, the best match candidates are ALWAYS at index 0!
        // This removes the slow nested loops.
        size_t i = 0;
        while (i < buyOrders.size()) {
            bool matched = false;
            for (size_t j = 0; j < sellOrders.size(); j++) {
                if (buyOrders[i].getTicker() == sellOrders[j].getTicker() &&
                    buyOrders[i].getPrice() >= sellOrders[j].getPrice()) {
                    
                    cout << "\n=============================================" << endl;
                    cout << "   MATCH FOUND! EXECUTING LIVE TRANSACTION   " << endl;
                    cout << "=============================================" << endl;
                    cout << "Buyer ID: " << buyOrders[i].getId() << " <---> Seller ID: " << sellOrders[j].getId() << endl;
                    cout << "Stock: " << buyOrders[i].getTicker() << endl;
                    cout << "Execution Price: Rs. " << sellOrders[j].getPrice() << endl;
                    cout << "Shares Transacted: " << buyOrders[i].getQuantity() << endl;
                    cout << "=============================================\n" << endl;

                    // Erase matched elements from memory logs
                    buyOrders.erase(buyOrders.begin() + i);
                    sellOrders.erase(sellOrders.begin() + j);
                    matched = true;
                    break; // Break the inner loop to re-evaluate from the top
                }
            }
            if (!matched) {
                i++; // Move to next buy order if top one didn't match anything
            }
        }
    }

    void displayActiveBook() const {
        cout << "\n--- CURRENT ACTIVE ORDER BOOK STATUS ---" << endl;
        cout << "[BUY LIMITS]:" << endl;
        if (buyOrders.empty()) cout << "  (No active buy orders)" << endl;
        for (const auto& o : buyOrders) { cout << "  "; o.displayLog(); }

        cout << "[SELL LIMITS]:" << endl;
        if (sellOrders.empty()) cout << "  (No active sell orders)" << endl;
        for (const auto& o : sellOrders) { cout << "  "; o.displayLog(); }
        cout << "----------------------------------------\n" << endl;
    }
};

// MODULE 3: USER-INTERACTIVE INTERFACE
// ==========================================
int main() {
    cout << "====================================================" << endl;
    cout << "   PESU LOW-LATENCY TRADING ENGINE v3.0 - READY    " << endl;
    cout << "====================================================\n" << endl;

    OrderBook centralExchange;
    int choice;
    int orderCounter = 100;

    while (true) {
        cout << "1. Insert New Trade Order" << endl;
        cout << "2. View Active Order Book Standing" << endl;
        cout << "3. Trigger Core Matching Engine Execution" << endl;
        cout << "4. Shut Down Exchange Hub System" << endl;
        cout << "Select Operation Option (1-4): ";
        cin >> choice;

        if (choice == 1) {
            string symbol, type;
            double prc;
            int qty;

            cout << "Enter Ticker Symbol (e.g., INFY, RELIANCE): ";
            cin >> symbol;
            cout << "Enter Transaction Type (BUY / SELL): ";
            cin >> type;
            cout << "Enter Target Limit Price (Rs.): ";
            cin >> prc;
            cout << "Enter Share Volume Quantity: ";
            cin >> qty;

            orderCounter++;
            string id = "TXN" + to_string(orderCounter);
            
            Order userOrder(id, symbol, type, prc, qty);
            centralExchange.receiveOrder(userOrder);
            cout << "Order successfully pushed into internal memory banks.\n" << endl;

        } else if (choice == 2) {
            centralExchange.displayActiveBook();
        } else if (choice == 3) {
            centralExchange.processMatching();
        } else if (choice == 4) {
            cout << "Shutting down trading cores cleanly. Core offline." << endl;
            break;
        } else {
            cout << "Invalid selection parameter. Try again.\n" << endl;
        }
    }

    return 0;
}
