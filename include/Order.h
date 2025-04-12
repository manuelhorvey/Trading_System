#ifndef ORDER_H
#define ORDER_H

#include <string>

class Order {
public:
    enum class Type {
        BUY,
        SELL
    };

    enum class Status {
        PENDING,
        EXECUTED,
        CANCELLED
    };

    // Constructor to initialize an order
    Order(Type type, double price, const std::string& date, double stopLoss, double takeProfit);

    // Getters
    Type getType() const;
    double getEntryPrice() const;
    const std::string& getOrderDate() const;
    double getStopLoss() const;
    double getTakeProfit() const;
    Status getStatus() const;

    // Methods to execute or cancel the order
    void execute();
    void cancel();

private:
    Type orderType;   // Type of order (BUY or SELL)
    double entryPrice; // Entry price at which the order is placed
    std::string orderDate; // The date of the order
    double stopLoss;  // Stop loss for the order
    double takeProfit; // Take profit for the order
    Status status;    // Status of the order (PENDING, EXECUTED, CANCELLED)
};

#endif // ORDER_H
