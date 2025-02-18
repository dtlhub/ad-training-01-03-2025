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
    if ((not (b'You lost!\n' in fight[-10:])) or ((b'You won!\n' in fight[-10:]))):
        log.error("Fight not working")
        return False
    if ((io.recv(20).decode() != comment)):
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
        log.error("Failed to login as gladiator")
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
        print("DOWN", flush=True)
        sys.exit(STATUS_DOWN)
    
    name = generate_str()
    password = generate_str()
    comment = generate_str(20)
    
    if not create_gladiator(io, name, password, comment):
        io.close()
        print("MUMBLE", flush=True)
        sys.exit(STATUS_MUMBLE)
    
    if not login_gladiator(io, name, password):
        io.close()
        print("MUMBLE", flush=True)
        sys.exit(STATUS_MUMBLE)
    if not check_fight(io, comment):
        io.close()
        print("MUMBLE", flush=True)
        sys.exit(STATUS_MUMBLE)

    
    data = view_gladiator(io)

    if not delete_gladiator(io, name, password):
        io.close()
        print("MUMBLE", flush=True)
        sys.exit(STATUS_MUMBLE)

    io.close()
    
    if name.encode() not in data or comment.encode() not in data:
        print("MUMBLE", flush=True)
        sys.exit(STATUS_MUMBLE)
    
    print("OK", flush=True)
    sys.exit(STATUS_UP)

def put(ip, flag):
    try:
        io = remote(ip, PORT, timeout=7)
    except Exception as e:
        print("DOWN", flush=True)
        sys.exit(STATUS_DOWN)
    
    name = generate_str()
    password = generate_str()
    
    if not create_gladiator(io, name, password, flag):
        io.close()
        print("MUMBLE", flush=True)
        sys.exit(STATUS_MUMBLE)
    
    exit_service(io)
    io.close()
    
    flag_id = f"{name}:{password}"
    print(flag_id, flush=True)
    sys.exit(STATUS_UP)

def get(ip, flag_id, flag):
    try:
        io = remote(ip, PORT, timeout=7)
    except Exception as e:
        print("DOWN", flush=True)
        sys.exit(STATUS_DOWN)
    
    try:
        name, password = flag_id.split(":")
    except Exception as e:
        io.close()
        print("CORRUPT", flush=True)
        sys.exit(STATUS_CORRUPT)
    
    if not login_gladiator(io, name, password):
        io.close()
        print("MUMBLE", flush=True)
        sys.exit(STATUS_MUMBLE)
    
    data = view_gladiator(io)
    io.close()
    
    if flag.encode() not in data:
        print("CORRUPT", flush=True)
        sys.exit(STATUS_CORRUPT)
    
    print("OK", flush=True)
    sys.exit(STATUS_UP)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: checker.py <action> <ip> [flag_id] [flag]")
        sys.exit(1)
    
    action = sys.argv[1]
    ip = sys.argv[2]
    
    if action == "check":
        check(ip)
    elif action == "put":
        if len(sys.argv) != 4:
            print("Usage: checker.py put <ip> <flag_id> <flag>")
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
