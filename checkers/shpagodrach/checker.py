#!/usr/bin/env python3
import sys
import random
import string
import os
argv = [c for c in sys.argv]
os.environ['PWNLIB_NOTERM'] = '1'
from pwn import *
from checklib import *
from threading import Lock

context.log_console = sys.stderr
context.log_level = 'error'

PORT = 7272

serial_lock = Lock()

def generate_str(length=15):
    letters = string.ascii_letters + string.digits
    return ''.join(random.choice(letters) for _ in range(length))

class PwnCheckMachine:
    def __init__(self, host):
        self.host = host
        self.port = PORT

    def connect(self):
        return remote(self.host, self.port, timeout=7)

    def create_gladiator(self, io, name, password, comment):
        io.sendlineafter(b"> ", b"1")
        io.sendlineafter(b"Enter name (login): ", name.encode())
        io.sendlineafter(b"Enter password: ", password.encode())
        io.sendlineafter(b"Enter comment: ", comment.encode())
        output = io.recvline(timeout=7)
        if b"Gladiator created!" not in output:
            raise Exception("Failed to create gladiator")

    def login_gladiator(self, io, name, password):
        io.sendlineafter(b"> ", b"2")
        io.sendlineafter(b"Enter name (login): ", name.encode())
        io.sendlineafter(b"Enter password: ", password.encode())
        output = io.recvuntil(b"\n", timeout=7)
        if b"Successfully hired gladiator" not in output:
            raise Exception("Failed to login as gladiator: " + output.decode(errors='ignore'))

    def update_gladiator(self, io, new_password):
        io.sendlineafter(b"> ", b"4")
        io.sendlineafter(b"Enter index: ", b"2")
        io.sendlineafter(b"Enter new value: ", new_password.encode())
        if b"Error" in io.recvline(timeout=7).strip():
            raise Exception("Failed to update gladiator data")


    def check_fight(self, io, comment):
        io.sendlineafter(b"> ", b"6")
        fight = io.recvuntil(b'You lost!\n', timeout=5)
        if (b'You lost!\n' not in fight[-10:]):
            raise Exception("Fight result is invalid")
        other_comment = io.recvline(timeout=7).strip()
        if other_comment != comment.encode():
            raise Exception("Comment mismatch or HUGE LUCKY")

    def view_gladiator(self, io):
        io.sendlineafter(b"> ", b"5")
        data = io.recvuntil(b"---", timeout=7)
        return data

    def delete_gladiator(self, io, name, password):
        io.sendlineafter(b"> ", b"3")
        io.sendlineafter(b"Enter name (login): ", name.encode())
        io.sendlineafter(b"Enter password: ", password.encode())
        output = io.recvline(timeout=7)
        if b"has been deleted." not in output:
            raise Exception("Failed to delete gladiator")

    def exit_service(self, io):
        io.sendlineafter(b"> ", b"7")

class Checker(BaseChecker):
    vulns = 1
    timeout = 15
    uses_attack_data = False

    def __init__(self, host, *args, **kwargs):
        super().__init__(host, *args, **kwargs)
        self.mch = PwnCheckMachine(host)

    def check(self):
        with serial_lock:
            try:
                io = self.mch.connect()
            except Exception:
                self.cquit(Status.DOWN, "Connection error")
            name = generate_str()
            password = generate_str()
            new_password = generate_str()
            comment = generate_str(20)
            try:
                self.mch.create_gladiator(io, name, password, comment)
                self.mch.login_gladiator(io, name, password)
                self.mch.check_fight(io, comment)
                data = self.mch.view_gladiator(io)
                self.mch.update_gladiator(io, new_password)
                self.mch.delete_gladiator(io, name, new_password)
                self.mch.exit_service(io)
                io.close()
            except Exception as e:
                io.close()
                self.cquit(Status.MUMBLE, str(e))
            if name.encode() not in data or comment.encode() not in data:
                self.cquit(Status.MUMBLE, "Gladiator data not found in view")
            self.cquit(Status.OK)

    def put(self, flag_id, flag, vuln="2"):
        with serial_lock:
            try:
                io = self.mch.connect()
            except Exception:
                self.cquit(Status.DOWN, "Connection error")

            name = generate_str()
            password = generate_str()
            try:
                self.mch.create_gladiator(io, name, password, flag)
                self.mch.exit_service(io)
                io.close()
            except Exception as e:
                io.close()
                self.cquit(Status.MUMBLE, str(e))

            flag_id = f"{name}:{password}"
            self.cquit(Status.OK, flag_id)

    def get(self, flag_id, flag, exploit=None, vuln="1"):
        with serial_lock:
            flag_id = flag_id.split()[0]

            try:
                io = self.mch.connect()
            except Exception:
                self.cquit(Status.DOWN, "Connection error")

            try:
                name, password = flag_id.split(":", 1)
            except Exception:
                io.close()
                self.cquit(Status.CORRUPT, f"Invalid flag_id format: {flag_id}")

            try:
                self.mch.login_gladiator(io, name, password)
                data = self.mch.view_gladiator(io)
                io.close()
            except Exception as e:
                io.close()
                self.cquit(Status.MUMBLE, str(e))

            if flag.encode() not in data:
                self.cquit(Status.CORRUPT, "Flag not found")

            self.cquit(Status.OK)

if __name__ == '__main__':
    #print("DEBUG: sys.argv =", sys.argv, file=sys.stderr)
    c = Checker(argv[2])
    try:
        c.action(argv[1], *argv[3:])
    except c.get_check_finished_exception():
        cquit(Status(c.status), c.public, c.private)
