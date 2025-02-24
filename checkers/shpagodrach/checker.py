#!/usr/bin/env python3
import sys
import random
import string
import os
argv = [c for c in sys.argv]

# Сокращённый отладочный вывод
def dbg(msg):
    print(f"DBG:{msg}", file=sys.stderr)

os.environ['PWNLIB_NOTERM'] = '1'
from pwn import *
from checklib import *
from threading import Lock

context.log_console = sys.stderr
context.log_level = 'error'

PORT = 7272
serial_lock = Lock()

def generate_str(length=15):
    dbg(f"gen_str:{length}")
    return ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(length))

class PwnCheckMachine:
    def __init__(self, host):
        dbg(f"init:{host}")
        self.host = host
        self.port = PORT

    def connect(self):
        dbg(f"connect:{self.host}:{self.port}")
        return remote(self.host, self.port, timeout=7)

    def create_gladiator(self, io, name, password, comment):
        dbg(f"create:{name}")
        dbg(io.recv(2048))
        #io.sendlineafter(b"> ", b"1")
        io.sendline(b"1")
        dbg("send 1")
        io.sendlineafter(b"Enter name (login): ", name.encode())
        dbg("send name")
        io.sendlineafter(b"Enter password: ", password.encode())
        dbg("send pass")
        io.sendlineafter(b"Enter comment: ", comment.encode())
        dbg("send comm")
        if b"Gladiator created!" not in io.recvline(timeout=3):
            raise Exception("Failed to create gladiator")

    def login_gladiator(self, io, name, password):
        dbg(f"login:{name}")
        io.sendlineafter(b"> ", b"2")
        io.sendlineafter(b"Enter name (login): ", name.encode())
        io.sendlineafter(b"Enter password: ", password.encode())
        out = io.recvuntil(b"\n", timeout=3)
        if b"Successfully hired gladiator" not in out:
            raise Exception("Failed to login: " + out.decode(errors='ignore'))

    def check_fight(self, io, comment):
        dbg(f"fight")
        io.sendlineafter(b"> ", b"6")
        f = io.recvuntil(b'You lost!\n', timeout=5)
        if (b'You lost!\n' not in f[-10:]):
            raise Exception("Fight result invalid")
        cmt = io.recvline(timeout=3).strip()
        if cmt != comment.encode():
            raise Exception("Comment mismatch")

    def view_gladiator(self, io):
        dbg(f"view")
        io.sendlineafter(b"> ", b"5")
        return io.recvuntil(b"---", timeout=3)

    def delete_gladiator(self, io, name, password):
        dbg(f"del:{name}")
        io.sendlineafter(b"> ", b"3")
        io.sendlineafter(b"Enter name (login): ", name.encode())
        io.sendlineafter(b"Enter password: ", password.encode())
        if b"has been deleted." not in io.recvline(timeout=3):
            raise Exception("Delete failed")

    def exit_service(self, io):
        dbg("exit")
        io.sendlineafter(b"> ", b"7")

class Checker(BaseChecker):
    vulns = 1
    timeout = 5
    uses_attack_data = False

    def __init__(self, host, *args, **kwargs):
        dbg(f"checker_init:{host}")
        super().__init__(host, *args, **kwargs)
        self.mch = PwnCheckMachine(host)

    def check(self):
        dbg("check")
        with serial_lock:
            try:
                io = self.mch.connect()
            except Exception:
                self.cquit(Status.DOWN, "Conn err")
            n = generate_str()
            p = generate_str()
            cmt = generate_str(20)
            try:
                self.mch.create_gladiator(io, n, p, cmt)
                self.mch.login_gladiator(io, n, p)
                self.mch.check_fight(io, cmt)
                data = self.mch.view_gladiator(io)
                self.mch.delete_gladiator(io, n, p)
                self.mch.exit_service(io)
                io.close()
            except Exception as e:
                io.close()
                self.cquit(Status.MUMBLE, f"MUMBLE:{e}")
            if n.encode() not in data or cmt.encode() not in data:
                self.cquit(Status.MUMBLE, "No data")
            self.cquit(Status.OK)

    def put(self, flag_id, flag, vuln="2"):
        dbg("put")
        with serial_lock:
            try:
                io = self.mch.connect()
            except Exception:
                self.cquit(Status.DOWN, "Conn err")
            n = generate_str()
            p = generate_str()
            try:
                self.mch.create_gladiator(io, n, p, flag)
                self.mch.exit_service(io)
                io.close()
            except Exception as e:
                io.close()
                self.cquit(Status.MUMBLE, f"MUMBLE:{e}")
            self.cquit(Status.OK, f"{n}:{p}")

    def get(self, flag_id, flag, exploit=None, vuln="1"):
        dbg("get")
        with serial_lock:
            fid = flag_id.split()[0]
            try:
                io = self.mch.connect()
            except Exception:
                self.cquit(Status.DOWN, "Conn err")
            try:
                n, p = fid.split(":", 1)
            except:
                io.close()
                self.cquit(Status.CORRUPT, f"Bad fid:{fid}")
            try:
                self.mch.login_gladiator(io, n, p)
                d = self.mch.view_gladiator(io)
                io.close()
            except Exception as e:
                io.close()
                self.cquit(Status.MUMBLE, f"MUMBLE:{e}")
            if flag.encode() not in d:
                self.cquit(Status.CORRUPT, "No flag")
            self.cquit(Status.OK)

if __name__ == '__main__':
    dbg("main")
    c = Checker(argv[2])
    try:
        c.action(argv[1], *argv[3:])
    except c.get_check_finished_exception():
        cquit(Status(c.status), c.public, c.private)
