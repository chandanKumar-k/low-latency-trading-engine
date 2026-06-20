#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Order {
private:
    string orderId, ticker, orderType;
    double price;
    int quantity;
public:
    Order(string id, string symbol, string type, double prc, int qty) 
        : orderId(id), ticker(symbol), orderType(type), price(prc), quantity(qty) {}

    string getType() const { return orderType; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    string getTicker() const { return ticker; }
    string getId() const { return orderId; }
};

bool compareBuyOrders(const Order& a, const Order& b) { return a.getPrice() > b.getPrice(); }
bool compareSellOrders(const Order& a, const Order& b) { return a.getPrice() < b.getPrice(); }

int main() {
    // Turn off stream buffering so communication with Python happens instantly
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    vector<Order> buyOrders;
    vector<Order> sellOrders;
    int orderCounter = 100;

    string command;
    // This loop stays alive continuously, keeping your vectors saved in memory!
    while (cin >> command) {
        if (command == "ADD") {
            string symbol, type;
            double price;
            int qty;
            cin >> symbol >> type >> price >> qty;

            orderCounter++;
            string id = "TXN" + to_string(orderCounter);
            Order incomingOrder(id, symbol, type, price, qty);

            if (type == "BUY") {
                buyOrders.push_back(incomingOrder);
                sort(buyOrders.begin(), buyOrders.end(), compareBuyOrders);
            } else {
                sellOrders.push_back(incomingOrder);
                sort(sellOrders.begin(), sellOrders.end(), compareSellOrders);
            }

            // Check for immediate matches using our exact sorted terminal logic!
            size_t i = 0;
            bool matchFound = false;
            while (i < buyOrders.size()) {
                bool currentMatched = false;
                for (size_t j = 0; j < sellOrders.size(); j++) {
                    if (buyOrders[i].getTicker() == sellOrders[j].getTicker() &&
                        buyOrders[i].getPrice() >= sellOrders[j].getPrice()) {
                        
                        // Stream out the match details directly to Python
                        cout << "MATCH_SUCCESS|" << buyOrders[i].getId() << "|" << sellOrders[j].getId() 
                             << "|" << buyOrders[i].getTicker() << "|" << sellOrders[j].getPrice() 
                             << "|" << buyOrders[i].getQuantity() << "\n" << flush;

                        buyOrders.erase(buyOrders.begin() + i);
                        sellOrders.erase(sellOrders.begin() + j);
                        currentMatched = true;
                        matchFound = true;
                        break;
                    }
                }
                if (!currentMatched) i++;
                else break; // Restart evaluation loop state
            }

            if (!matchFound) {
                cout << "NO_MATCH|Order placed safely in active storage queue registries.\n" << flush;
            }
        } 
        else if (command == "VIEW") {
            // Send the full active book state back to Python to display visually
            cout << "BOOK_START\n";
            for (const auto& o : buyOrders) {
                cout << "BUY|" << o.getId() << "|" << o.getTicker() << "|" << o.getPrice() << "|" << o.getQuantity() << "\n";
            }
            for (const auto& o : sellOrders) {
                cout << "SELL|" << o.getId() << "|" << o.getTicker() << "|" << o.getPrice() << "|" << o.getQuantity() << "\n";
            }
            cout << "BOOK_END\n" << flush;
        }
    }
    return 0;
}
