//
// Created by Андрей Шпак on 11.02.2025.
//

#ifndef MAGAZINCHIK_ORDERS_H
#define MAGAZINCHIK_ORDERS_H

#define ORDERS_FILE "/tmp/orders.txt"
#define BOUGHT_ORDERS "/tmp/bought.txt" // <username>:<order_name>

namespace order{
    struct Order{
        char name[32];
        char description[512]; // inset flag here
        char author[32];
        int price=200;
    };

    bool add_order(char* name, char* description, char* author, int price = 200);
    bool buy_order(char username[32] , int balance);
}

#endif //MAGAZINCHIK_ORDERS_H
