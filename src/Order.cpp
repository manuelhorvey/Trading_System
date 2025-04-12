#include "Order.h"
#include <iostream>

// Constructor to initialize the order with type, price, date, stop loss, and take profit
Order::Order(Type type, double price, const std::string& date, double stopLoss, double takeProfit)
    : orderType(type), entryPrice(price), orderDate(date), stopLoss(stopLoss), takeProfit(takeProfit), status(Status::PENDING) {}

// Getter for order type
Order::Type Order::getType() const {
    return orderType;
}

// Getter for entry price
double Order::getEntryPrice() const {
    return entryPrice;
}

// Getter for order date
const std::string& Order::getOrderDate() const {
    return orderDate;
}

// Getter for stop loss
double Order::getStopLoss() const {
    return stopLoss;
}

// Getter for take profit
double Order::getTakeProfit() const {
    return takeProfit;
}

// Getter for status
Order::Status Order::getStatus() const {
    return status;
}

// Method to execute the order (change status to EXECUTED)
void Order::execute() {
    if (status == Status::PENDING) {
        status = Status::EXECUTED;
        std::cout << "Order executed at price: " << entryPrice << std::endl;
    }
    else {
        std::cout << "Order cannot be executed because it is not pending." << std::endl;
    }
}

// Method to cancel the order (change status to CANCELLED)
void Order::cancel() {
    if (status == Status::PENDING) {
        status = Status::CANCELLED;
        std::cout << "Order cancelled." << std::endl;
    }
    else {
        std::cout << "Order cannot be cancelled because it is already executed or cancelled." << std::endl;
    }
}
