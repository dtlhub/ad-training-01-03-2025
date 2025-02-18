#!/usr/bin/env python3
import sys
import random
import string
from pwn import *

STATUS_UP = 101
STATUS_DOWN = 102
STATUS_MUMBLE = 103
STATUS_CORRUPT = 104

PORT = 7272

def exit_status(message, code):
    print(message, flush=True)
    sys.exit(code)

def exit_mumble(message="MUMBLE"):
    exit_status(message, STATUS_MUMBLE)

def exit_down(message="DOWN"):
    exit_status(message, STATUS_DOWN)

def exit_corrupt(message="CORRUPT"):
    exit_status(message, STATUS_CORRUPT)

def exit_ok(message="OK"):
    exit_status(message, STATUS_UP)

def generate_str(length=15):
    letters = string.ascii_letters + string.digits
    return ''.join(random.choice(letters) for _ in range(length))

def create_gladiator(io, name, password, comment):
    io.sendlineafter(b"> ", b"1")
    io.sendlineafter(b"Enter name (login): ", name.encode())
    io.sendlineafter(b"Enter password: ", password.encode())
    io.sendlineafter(b"Enter comment: ", comment.encode())
    output = io.recvline(timeout=3)
    if b"Gladiator created!" not in output:
        log.error("Failed to create gladiator")
        return False
    return True

def check_fight(io, comment):
    io.sendlineafter(b"> ", b"6")
    fight = io.recvuntil(b'You lost!\n', timeout=5)
    if (b'You lost!\n' not in fight[-10:]) or (b'You won!\n' in fight[-10:]):
        log.error("Fight not working")
        return False
    other_comment = io.recv(20).decode()
    if other_comment != comment:
        log.error("Another comment")
        return False
    return True

def login_gladiator(io, name, password):
    io.sendlineafter(b"> ", b"2")
    io.sendlineafter(b"Enter name (login): ", name.encode())
    io.sendlineafter(b"Enter password: ", password.encode())
    output = io.recvline(timeout=3)
    if b"Successfully hired gladiator" not in output:
        log.error("Failed to login as gladiator")
        return False
    return True

def delete_gladiator(io, name, password):
    io.sendlineafter(b'> ', b'3')
    io.sendlineafter(b"Enter name (login): ", name.encode())
    io.sendlineafter(b"Enter password: ", password.encode())
    output = io.recvline(timeout=3)
    if b"has been deleted." not in output:
        log.error("Failed to delete gladiator")
        return False
    return True

def view_gladiator(io):
    io.sendlineafter(b"> ", b"5")
    data = io.recvuntil(b"---", timeout=3)
    return data

def exit_service(io):
    io.sendlineafter(b"> ", b"7")

def check(ip):
    try:
        io = remote(ip, PORT, timeout=7)
    except Exception as e:
        exit_down("DOWN")
    
    name = generate_str()
    password = generate_str()
    comment = generate_str(20)
    
    if not create_gladiator(io, name, password, comment):
        io.close()
        exit_mumble("MUMBLE")
    
    if not login_gladiator(io, name, password):
        io.close()
        exit_mumble("MUMBLE")
    
    if not check_fight(io, comment):
        io.close()
        exit_mumble("MUMBLE")
    
    data = view_gladiator(io)
    
    if not delete_gladiator(io, name, password):
        io.close()
        exit_mumble("MUMBLE")
    
    io.close()
    
    if name.encode() not in data or comment.encode() not in data:
        exit_mumble("MUMBLE")
    
    exit_ok("OK")

def put(ip, flag):
    try:
        io = remote(ip, PORT, timeout=7)
    except Exception as e:
        exit_down("DOWN")
    
    name = generate_str()
    password = generate_str()
    
    if not create_gladiator(io, name, password, flag):
        io.close()
        exit_mumble("MUMBLE")
    
    exit_service(io)
    io.close()
    
    flag_id = f"{name}:{password}"
    print(flag_id, flush=True)
    sys.exit(STATUS_UP)

def get(ip, flag_id, flag):
    try:
        io = remote(ip, PORT, timeout=7)
    except Exception as e:
        exit_down("DOWN")
    
    try:
        name, password = flag_id.split(":")
    except Exception as e:
        io.close()
        exit_corrupt("CORRUPT")
    
    if not login_gladiator(io, name, password):
        io.close()
        exit_mumble("MUMBLE")
    
    data = view_gladiator(io)
    io.close()
    
    if flag.encode() not in data:
        exit_corrupt("CORRUPT")
    
    exit_ok("OK")

if __name__ == '__main__':
    try:
        if sys.argv[1] == "info":
            print("actions:")
            print("1) check --> checker.py check <ip>")
            print("2) put -->   checker.py put <ip> <flag>")
            print("3) get -->   checker.py get <ip> <login:password> <flag>")
            sys.exit(1)
        elif len(sys.argv) < 3:
            print("Usage: checker.py <action> <ip> [flag_id] [flag]")
            sys.exit(1)

        action = sys.argv[1]
        ip = sys.argv[2]
    
        if action == "check":
            check(ip)
        elif action == "put":
            if len(sys.argv) != 4:
                print("Usage: checker.py put <ip> <flag>")
                sys.exit(1)
            flag = sys.argv[3]
            put(ip, flag)
        elif action == "get":
            if len(sys.argv) != 5:
                print("Usage: checker.py get <ip> <login:password> <flag>")
                sys.exit(1)
            flag_id = sys.argv[3]
            flag = sys.argv[4]
            get(ip, flag_id, flag)
        else:
            print("Unknown action")
            sys.exit(1)
    except Exception as e:
        log.exception("Unexpected error")
        exit_mumble("MUMBLE")
