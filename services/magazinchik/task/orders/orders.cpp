//
// Created by Андрей Шпак on 11.02.2025.
//
#include "orders.h"

#include <iostream>
#include <fstream>

namespace order{
    bool add_order(char* name, char* description, char* author, int price) {
        std::ofstream out(ORDERS_FILE, std::ios::app);
        if (!out.is_open()) {
            std::cerr << "[ERROR] Couldn't open file for writing" << std::endl;
            return false;
        }

        Order newOrder;

        strncpy(newOrder.name, name, sizeof(newOrder.name) - 1);
        newOrder.name[sizeof(newOrder.name) - 1] = '\0';

        strncpy(newOrder.description, description, sizeof(newOrder.description) - 1);
        newOrder.description[sizeof(newOrder.description) - 1] = '\0';

        strncpy(newOrder.author, author, sizeof(newOrder.author) - 1);
        newOrder.author[sizeof(newOrder.author) - 1] = '\0';

        newOrder.price = price;


        out << newOrder.name << ":"
            << newOrder.description << ":"
            << newOrder.author << ":"
            << newOrder.price << "\n";

        out.close();
        return true;
    }

    bool buy_order(char username[32], int balance) {
        std::ifstream orders_file(ORDERS_FILE);
        if (!orders_file.is_open()) {
            std::cerr << "[ERROR] Couldn't open orders file" << std::endl;
            return false;
        }

        std::ofstream bought_file(BOUGHT_ORDERS, std::ios::app);
        if (!bought_file.is_open()) {
            std::cerr << "[ERROR] Couldn't open bought orders file" << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(orders_file, line)) {
            Order order;

            if (sscanf(line.c_str(), "%31[^:]:%511[^:]:%31[^:]:%d",
                       order.name, order.description, order.author, &order.price) == 4) {

                if (balance >= order.price) {
                    bought_file << username << ":" << order.name << "\n";
                    std::cout << "[INFO] Order " << order.name << " bought successfully!" << std::endl;
                    return true;
                } else {
                    std::cerr << "[ERROR] Not enough balance!" << std::endl;
                    return false;
                }
            }
        }

        std::cerr << "[ERROR] Order not found!" << std::endl;
        return false;
    }

}