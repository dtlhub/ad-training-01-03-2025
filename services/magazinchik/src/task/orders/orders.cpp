//
// Created by Андрей Шпак on 11.02.2025.
//
#include "orders.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <unordered_map>

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

    bool buy_order(char* username, int &balance, std::string &product_id) {
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

                if (product_id == order.name) {
                    if (balance >= order.price) {
                        balance -= order.price;
                        bought_file << username << ":" << order.name << "\n";
                        std::cout << "[INFO] Order " << order.name << " bought successfully!" << std::endl;
                        return true;
                    } else {
                        std::cerr << "[ERROR] Not enough balance!" << std::endl;
                        return false;
                    }
                }
            }
        }

        std::cerr << "[ERROR] Order not found!" << std::endl;
        return false;
    }



    std::vector<std::unordered_map<std::string, std::string> > file_to_vec() {
        std::vector<std::unordered_map<std::string, std::string> > orders;
        std::ifstream in(ORDERS_FILE);

        if (!in.is_open()) {
            std::cerr << "[ERROR] Couldn't open file for reading" << std::endl;
            return orders;
        }

        std::string line;
        while (std::getline(in, line)) {
            std::unordered_map<std::string, std::string> order;
            size_t pos = 0;
            std::vector<std::string> keys = {"name", "description", "author", "price"};
            int key_index = 0;

            while ((pos = line.find(':')) != std::string::npos && key_index < keys.size()) {
                order[keys[key_index++]] = line.substr(0, pos);
                line.erase(0, pos + 1);
            }

            if (key_index < keys.size()) {
                order[keys[key_index]] = line;
            }

            orders.push_back(order);
        }

        in.close();
        return orders;
    }

    bool has_bought_order(const std::string& username, const std::string& order_name) {
        std::ifstream bought_in(BOUGHT_ORDERS);
        if (!bought_in.is_open()) {
            std::cerr << "[INFO] Couldn't open bought orders file for reading" << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(bought_in, line)) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string buyer = line.substr(0, pos);
                std::string bought_order = line.substr(pos + 1);
                if (buyer == username && bought_order == order_name) {
                    return true;
                }
            }
        }
        return false;
    }

    std::vector<std::unordered_map<std::string, std::string> > my_orders(std::string& username) {
        std::vector<std::unordered_map<std::string, std::string> > orders;
        std::ifstream in(ORDERS_FILE);

        if (!in.is_open()) {
            std::cerr << "[ERROR] Couldn't open file for reading" << std::endl;
            return orders;
        }

        std::string line;
        while (std::getline(in, line)) {
            std::unordered_map<std::string, std::string> order;
            size_t pos = 0;
            std::vector<std::string> keys = {"name", "description", "author", "price"};
            int key_index = 0;

            while ((pos = line.find(':')) != std::string::npos && key_index < keys.size()) {
                order[keys[key_index++]] = line.substr(0, pos);
                line.erase(0, pos + 1);
            }

            if (key_index < keys.size()) {
                order[keys[key_index]] = line;
            }

            if (order["author"] == username || has_bought_order(username, order["name"])) {
                orders.push_back(order);
            }
        }

        in.close();
        return orders;
    }

}