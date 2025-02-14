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

void handle_register_get(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> context = {
            {"title", "Magazinchik"},
            {"status" , ""},
    };

    std::string template_str = template_engine::load_template("templates/register.html");
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" +
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

        std::string response = "HTTP/1.1 302 Found\r\n"
                               "Path=/; HttpOnly\r\n"
                               "Location: /login\r\n"
                               "Content-Length: 0\r\n"
                               "\r\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }else{
        std::string template_str = template_engine::load_template("templates/register.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

void handle_login_get(int client_socket, const parser::Request &req) {

    std::unordered_map<std::string, std::string> context = {
            {"title", "Magazinchik"},
            {"username", ""},
            {"password", ""},
            {"balance", ""}
    };

    std::string template_str = template_engine::load_template("templates/login.html");
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" +
                           template_engine::render_template(template_str, context);
    send(client_socket, response.c_str(), response.length(), 0);
}

void handle_login_post(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    std::string username = params["username"];
    std::string password = params["password"];

    const char *token = "None";
    if(user::checkUser(username.data() , password.data())) {
        token = auth::login(username.data());
    }

    std::string response = "HTTP/1.1 302 Found\r\n"
                           "Set-Cookie: session=" + std::string(token) + "; Path=/; HttpOnly\r\n"
                                                                         "Location: /\r\n"
                                                                         "Content-Length: 0\r\n"
                                                                         "\r\n";
    send(client_socket, response.c_str(), response.length(), 0);
}



void handle_index_get(int client_socket, const parser::Request &req){
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        return;
    }

    std::string session = it->second;
    session[session.size()-1] = '\0';


    bool authorized = auth::is_authorized(session.c_str());

    if(authorized){
        session[session.size()-1] = '\n';
        std::string username = auth::find_username_by_session(session);

        user::User u = user::find_user_by_username(username.data());

        username.clear();
        username.assign(u.username);

        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", username},
                {"balance", std::to_string(u.balance)},
        };

        std::string template_str = template_engine::load_template("templates/authorized/index.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }else{
        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", ""} ,
                {"balance", ""},
        };
        std::string template_str = template_engine::load_template("templates/unauthorized/index.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}


void handle_order_get(int client_socket, const parser::Request &req){
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        return;
    }

    std::string session = it->second;
    session[session.size()-1] = '\0';

    bool authorized = auth::is_authorized(session.c_str());

    if(authorized) {
        session[session.size()-1] = '\n';
        std::string username = auth::find_username_by_session(session);

        user::User u = user::find_user_by_username(username.data());

        username.clear();
        username.assign(u.username);

        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
                {"username", username},
                {"balance", std::to_string(u.balance)},
        };


        std::string template_str = template_engine::load_template("templates/authorized/order.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" +
                               template_engine::render_template(template_str, context);
        send(client_socket, response.c_str(), response.length(), 0);
    }else{
        std::unordered_map<std::string, std::string> context = {
                {"title", "Magazinchik"},
        };
        std::string template_str = template_engine::load_template("templates/login_required.html");
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" +
                               template_engine::render_template(template_str,context);
        send(client_socket, response.c_str(), response.length(), 0);
    }
}


void handle_order_post(int client_socket, const parser::Request &req) {
    std::unordered_map<std::string, std::string> params = parser::parse_post_body(req.body);

    std::string name = params["name"];
    std::string description = params["description"];
    std::string price = params["price"];

    auto it = req.cookies.find("session");
    if (it == req.cookies.end()) {
        std::cerr << "Session cookie not found!" << std::endl;
        return;
    }

    std::string session = it->second;
    std::string username = auth::find_username_by_session(session);

    if (order::add_order(name.data(), description.data(), username.data(), stoi(price))) {
        std::cout << "Order added successfully!" << std::endl;
    } else {
        std::cerr << "Failed to add order." << std::endl;
    }

    std::string response = "HTTP/1.1 302 Found\r\n"
                           "Path=/; HttpOnly\r\n"
                           "Location: /\r\n"
                           "Content-Length: 0\r\n"
                           "\r\n";
    send(client_socket, response.c_str(), response.length(), 0);
}


int main() {
    // registration
    router::add_route("GET","/register" , handle_register_get);
    router::add_route("POST","/register" , handle_register_post);

    //login
    router::add_route("GET" , "/login" , handle_login_get);
    router::add_route("POST" , "/login" , handle_login_post);

    //order
    router::add_route("GET" , "/order" , handle_order_get);
    router::add_route("POST" , "/order" , handle_order_post);

    //main page
    router::add_route("GET" , "/" , handle_index_get);

    framework::run(8080);
    return 0;
}