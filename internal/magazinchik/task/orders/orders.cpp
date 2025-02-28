#include "orders.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <random>

#include "../user/user.h"

namespace fs = std::filesystem;

namespace order {
    bool add_order(const std::string& name, const std::string& description, const std::string& author, int price) {
        std::string user_orders_dir = USER_ORDERS_DIR + author;
        fs::create_directories(user_orders_dir);

        fs::create_directories(ORDERS_DIR);

        std::ofstream user_out(user_orders_dir + "/" + name);
        if (user_out.is_open()) {
            user_out << description << "\n" << author << "\n" << price << "\n";
        } else {
            std::cerr << "[ERROR] Couldn't create order file in user directory: " << name << std::endl;
            return false;
        }

        std::ofstream order_out(ORDERS_DIR + "/" + name);
        if (order_out.is_open()) {
            order_out << description << "\n" << author << "\n" << price << "\n";
            return true;
        } else {
            std::cerr << "[ERROR] Couldn't create order file in general orders directory: " << name << std::endl;
            return false;
        }
    }

    bool buy_order(const std::string& username, int& balance, const std::string& product_id) {
        std::ifstream in(ORDERS_DIR + product_id);
        if (!in.is_open()) {
            std::cerr << "[ERROR] Order not found: " << product_id << std::endl;
            return false;
        }

        std::string description, author;
        int price;
        std::getline(in, description);
        std::getline(in, author);
        in >> price;
        in.close();

        if (balance < price) {
            std::cerr << "[ERROR] Not enough balance!" << std::endl;
            return false;
        }
        balance -= price;

        user::User u = user::find_user_by_username(username.data());

        std::string user_file_path = user::USERS_DIR + username;
        std::ofstream user_file(user_file_path, std::ios::trunc);
        if (user_file.is_open()) {
            std::string passwd;
            passwd.assign(u.password);
            user_file << passwd << ":" << balance;
            user_file.close();
        } else {
            std::cerr << "[ERROR] Couldn't update user file: " << username << std::endl;
            return false;
        }

        fs::create_directories(USER_ORDERS_DIR + username);
        std::ofstream out(USER_ORDERS_DIR + username + "/" + product_id);
        if (out.is_open()) {
            out << description << "\n" << author << "\n" << price << "\n";
            out.close();
            return true;
        } else {
            std::cerr << "[ERROR] Couldn't save user order: " << product_id << std::endl;
            return false;
        }
    }

    std::vector<std::unordered_map<std::string, std::string>> file_to_vec() {
        std::vector<std::unordered_map<std::string, std::string>> orders;
        std::vector<std::filesystem::directory_entry> entries;

        for (const auto& entry : fs::directory_iterator(ORDERS_DIR)) {
            entries.push_back(entry);
        }

        std::shuffle(entries.begin(), entries.end(), std::random_device());

        int count = 0;
        for (const auto& entry : entries) {
            if (count >= 50) break;

            std::ifstream in(entry.path());
            if (in.is_open()) {
                std::unordered_map<std::string, std::string> order;
                std::string description, author, price;
                std::getline(in, description);
                std::getline(in, author);
                std::getline(in, price);

                order["name"] = entry.path().filename();
                order["description"] = description;
                order["author"] = author;
                order["price"] = price;

                orders.push_back(order);
                count++;
            }
        }
        return orders;
    }


    std::vector<std::unordered_map<std::string, std::string>> my_orders(const std::string& username) {
        std::vector<std::unordered_map<std::string, std::string>> orders;
        std::unordered_set<std::string> unique_orders;

        auto load_orders_from_directory = [&](const std::string& directory_path, bool check_author = false) {
            if (!fs::exists(directory_path)) {
                return;
            }

            for (const auto& entry : fs::directory_iterator(directory_path)) {
                std::ifstream in(entry.path());
                if (in.is_open()) {
                    std::unordered_map<std::string, std::string> order;
                    std::string description, author, price;
                    std::getline(in, description);
                    std::getline(in, author);
                    std::getline(in, price);

                    if (check_author && author != username) {
                        continue;
                    }

                    order["name"] = entry.path().filename();
                    order["description"] = description;
                    order["author"] = author;
                    order["price"] = price;

                    std::string unique_key = order["name"] + "|" + order["description"] + "|" + order["author"] + "|" + order["price"];

                    if (unique_orders.find(unique_key) == unique_orders.end()) {
                        unique_orders.insert(unique_key);
                        orders.push_back(order);
                    }
                }
            }
        };

        std::string user_orders_path = USER_ORDERS_DIR + username;
        load_orders_from_directory(user_orders_path);

        return orders;
    }

    bool has_bought_order(const std::string& username, const std::string& order_name) {
        std::string order_path = USER_ORDERS_DIR + username + "/" + order_name;
        return fs::exists(order_path);
    }
}