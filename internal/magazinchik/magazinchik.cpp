#include "framework/router/router.h"
#include "framework/server.h"
#include "framework/template/template.h"
#include "framework/parser/parser.h"
#include "task/user/user.h"
#include "task/authorization/auth.h"
#include "task/orders/orders.h"

#include <sys/socket.h>
#include <iostream>
#include <string>
#include "string.h"

// ======= REGISTRATION

void handle_register_get(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> context = {
            {"title", "Magazinchik"},
            {"status" , ""},
    };

    std::string template_str = template_engine::load_template("templates/register.html");
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                           template_engine::render_template(template_str, context);
    send(client_socket, response.c_str(), response.length(), 0);
}

void handle_register_post(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    std::string username = params["username"];
    std::string password = params["password"];

    bool registration = user::add_user(username.data(), password.data());

    std::unordered_map<std::string, std::string> context = {
            {"title", "Magazinchik"},
            {"status" , "Registration Error"},
    };

    if(registration) {
        const char *token = "None";
        token = auth::login(username.data());

        std::string response = "HTTP/1.1 302 Found\r\n"
                               "Set-Cookie: session=" + std::string(token) + "; Path=/; HttpOnly\r\n"
                                                                             "Location: /\r\n"
                                                                             "Connection: close\r\n"
                                                                             "Content-Length: 0\r\n"
                                                                             "\r\n";
        send(client_socket, response.c_str(), response.length(), 0);
    } else {
        std::string template_str = template_engine::load_template("templates/register.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

// ======= LOGIN

void handle_login_get(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> context = {
            {"title", "Magazinchik"},
            {"username", ""},
            {"password", ""},
            {"balance", ""}
    };

    std::string template_str = template_engine::load_template("templates/login.html");
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                           template_engine::render_template(template_str, context);
    send(client_socket, response.c_str(), response.length(), 0);
}

void handle_login_post(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    std::string username = params["username"];
    std::string password = params["password"];

    password.erase(password.find_last_not_of("\r\n") + 1);

    const char *token = "None";
    if (user::checkUser(username.data(), password.data())) {
        token = auth::login(username.data());
    }

    std::string response = "HTTP/1.1 302 Found\r\n"
                           "Set-Cookie: session=" + std::string(token) + "; Path=/; HttpOnly\r\n"
                                                                         "Location: /\r\n"
                                                                         "Connection: close\r\n"
                                                                         "Content-Length: 0\r\n"
                                                                         "\r\n";
    send(client_socket, response.c_str(), response.length(), 0);
}

void handle_forgot_get(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> context = {
            {"title", "Magazinchik"},
            {"username", ""},
            {"password", ""},
            {"balance", ""}
    };

    std::string template_str = template_engine::load_template("templates/forgot.html");
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                           template_engine::render_template(template_str, context);
    send(client_socket, response.c_str(), response.length(), 0);
}

void handle_forgot_post(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    std::string username = params["username"];
    std::string password = params["password"];

    if(user::changePassword(username.data(), password.data())) {
        std::string response = "HTTP/1.1 302 Found\r\n"
                               "Path=/; HttpOnly\r\n"
                               "Location: /login\r\n"
                               "Connection: close\r\n"
                               "Content-Length: 0\r\n"
                               "\r\n";
        send(client_socket, response.c_str(), response.length(), 0);
    } else {
        std::string response = "HTTP/1.1 404 Found\r\n"
                               "Path=/; HttpOnly\r\n"
                               "Location: /register\r\n"
                               "Connection: close\r\n"
                               "Content-Length: 0\r\n"
                               "\r\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

// ======= MAIN PAGE

void handle_index_get(int client_socket, const parser::Request &req) {
    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        return;
    }

    std::string session = it->second;

    session.erase(session.find_last_not_of("\r\n") + 1);

    bool authorized = auth::is_authorized(session.c_str());

    if (authorized) {
        std::string username = auth::find_username_by_session(session);

        if (username.empty()) {
            std::cerr << "Username not found for session: " << session << std::endl;
            return;
        }

        user::User u = user::find_user_by_username(username.data());

        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", username},
                {"balance", std::to_string(u.balance)},
        };

        std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> list_context = {
                {"orders", order::file_to_vec()},
        };

        std::string template_str = template_engine::load_template("templates/authorized/index.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context, list_context);
        send(client_socket, response.c_str(), response.length(), 0);
    } else {
        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", ""},
                {"balance", ""},
        };
        std::string template_str = template_engine::load_template("templates/unauthorized/index.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

// ====== ORDERS

void handle_order_get(int client_socket, const parser::Request &req) {
    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        return;
    }

    std::string session = it->second;

    session.erase(session.find_last_not_of("\r\n") + 1);

    bool authorized = auth::is_authorized(session.c_str());

    if (authorized) {
        std::string username = auth::find_username_by_session(session);

        if (username.empty()) {
            std::cerr << "Username not found for session: " << session << std::endl;
            return;
        }

        user::User u = user::find_user_by_username(username.data());

        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", username},
                {"balance", std::to_string(u.balance)},
        };

        std::string template_str = template_engine::load_template("templates/authorized/order.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    } else {
        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
        };
        std::string template_str = template_engine::load_template("templates/login_required.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}
void handle_order_post(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    std::string name = params["name"];
    std::string description = params["description"];
    std::string price_str = params["price"];

    // Проверка на пустые параметры
    if (name.empty() || description.empty() || price_str.empty()) {
        std::cerr << "Missing required parameters!" << std::endl;
        return;
    }

    // Проверка на корректность цены
    int price = 0;
    try {
        price = std::stoi(price_str);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid price format!" << std::endl;
        return;
    }

    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        return;
    }

    std::string session = it->second;
    session.erase(session.find_last_not_of("\r\n") + 1);
    std::string username = auth::find_username_by_session(session);

    if (username.empty()) {
        std::cerr << "User not found for the given session!" << std::endl;
        return;
    }

    if (order::add_order(name, description, username, price)) {
        std::cout << "Order added successfully!" << std::endl;
    } else {
        std::cerr << "Failed to add order." << std::endl;
    }

    std::string response = "HTTP/1.1 302 Found\r\n"
                           "Path=/; HttpOnly\r\n"
                           "Location: /\r\n"
                           "Connection: close\r\n"
                           "Content-Length: 0\r\n"
                           "\r\n";
    send(client_socket, response.c_str(), response.length(), 0);
}


void handle_my_order_get(int client_socket, const parser::Request &req) {
    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        return;
    }

    std::string session = it->second;

    session.erase(session.find_last_not_of("\r\n") + 1);

    bool authorized = auth::is_authorized(session.c_str());

    if (authorized) {
        std::string username = auth::find_username_by_session(session);

        if (username.empty()) {
            std::cerr << "Username not found for session: " << session << std::endl;
            return;
        }

        user::User u = user::find_user_by_username(username.data());

        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", username},
                {"balance", std::to_string(u.balance)},
        };

        std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> list_context = {
                {"orders", order::my_orders(username)},
        };

        std::string template_str = template_engine::load_template("templates/authorized/my_order.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context, list_context);
        send(client_socket, response.c_str(), response.length(), 0);
    } else {
        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
        };
        std::string template_str = template_engine::load_template("templates/login_required.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

void handle_buy_get(int client_socket, const parser::Request &req) {
    size_t param_pos = req.query.find("product_id=");
    if (param_pos == std::string::npos) {
        std::cerr << "[ERROR] product_id not found in request!" << std::endl;
        std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nMissing product_id";
        send(client_socket, response.c_str(), response.length(), 0);
        return;
    }

    std::string product_id = req.query.substr(param_pos + 11);
    product_id.erase(product_id.find_last_not_of("\r\n ") + 1);

    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        std::unordered_map<std::string, std::string> context = {{"title", "Magazinchik"}};
        std::string template_str = template_engine::load_template("templates/login_required.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
        return;
    }

    std::string session = it->second;

    session.erase(session.find_last_not_of("\r\n") + 1);

    bool authorized = auth::is_authorized(session.c_str());
    if (!authorized) {
        std::cerr << "[ERROR] Unauthorized access attempt!" << std::endl;
        std::unordered_map<std::string, std::string> context = {{"title", "Magazinchik"}};
        std::string template_str = template_engine::load_template("templates/login_required.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
        return;
    }

    std::string username = auth::find_username_by_session(session);

    user::User user = user::find_user_by_username(username.data());
    if (strlen(user.username) == 0) {
        std::cerr << "[ERROR] User not found!" << std::endl;
        return;
    }

    if (order::buy_order(username.data(), user.balance, product_id)) {
        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", username},
                {"balance", std::to_string(user.balance)},
        };

        std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> list_context = {
                {"orders", order::my_orders(username)},
        };

        std::string template_str = template_engine::load_template("templates/authorized/my_order.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context, list_context);
        send(client_socket, response.c_str(), response.length(), 0);
    } else {
        std::unordered_map<std::string, std::string> context = {
                {"title", "Purchase Failed"},
                {"message", "Failed to buy order: " + product_id}
        };

        std::string template_str = template_engine::load_template("templates/fail.html");
        std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

int main() {
    // registration
    router::add_route("GET", "/register", handle_register_get);
    router::add_route("POST", "/register", handle_register_post);

    //login
    router::add_route("GET", "/login", handle_login_get);
    router::add_route("POST", "/login", handle_login_post);
    router::add_route("GET", "/forgot", handle_forgot_get);
    router::add_route("POST", "/forgot", handle_forgot_post);

    //order
    router::add_route("GET", "/order", handle_order_get);
    router::add_route("POST", "/order", handle_order_post);
    router::add_route("GET", "/my_order", handle_my_order_get);
    router::add_route("GET", "/buy", handle_buy_get);

    //main page
    router::add_route("GET", "/", handle_index_get);

    framework::run(8080,12);
    return 0;
}