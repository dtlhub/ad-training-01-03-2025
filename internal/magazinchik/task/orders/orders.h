#ifndef MAGAZINCHIK_ORDERS_H
#define MAGAZINCHIK_ORDERS_H

#include <string>
#include <vector>
#include <unordered_map>

namespace order {
    const std::string ORDERS_DIR = "/tmp/orders/";
    const std::string USER_ORDERS_DIR = "/tmp/user_orders/";

    struct Order {
        std::string name;
        std::string description;
        std::string author;
        int price;
    };

    bool add_order(const std::string& name, const std::string& description, const std::string& author, int price);
    bool buy_order(const std::string& username, int& balance, const std::string& product_id);
    std::vector<std::unordered_map<std::string, std::string>> file_to_vec();
    std::vector<std::unordered_map<std::string, std::string>> my_orders(const std::string& username);
    bool has_bought_order(const std::string& username, const std::string& order_name);
}

#endif // MAGAZINCHIK_ORDERS_H