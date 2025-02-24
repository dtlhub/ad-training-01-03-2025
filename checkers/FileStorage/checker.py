#!/usr/bin/env python3

import sys
import requests
import json
from checklib import *
import random
import string
import traceback
from bs4 import BeautifulSoup

from checklib.checker import CheckFinished

PORT = 5000

class CheckMachine:
    @property
    def url(self):
        return f'http://{self.c.host}:{self.port}'

    def __init__(self, checker: BaseChecker):
        self.c = checker
        self.port = PORT

    def register(self, session: requests.Session, username: str, password: str):
        url = f'{self.url}/register'
        data = {
            "username": username,
            "password": password,
        }
        response = session.post(url, data=data)

        self.c.assert_eq(response.status_code, 200, "Failed to register user")

        self.login(session, username, password, Status.MUMBLE)

    def login(self, session: requests.Session, username: str, password: str, status: Status):
        url = f'{self.url}/login'
        data = {
            "username": username,
            "password": password,
        }
        response = session.post(url, data=data)

        self.c.assert_eq(response.status_code, 200, "Failed to login", status)

    def put_file(self, session: requests.Session, data: str):
        url = f'{self.url}/upload'
        response = session.post(url, files={"file": ("flag.txt", data)})
        self.c.assert_eq(response.status_code, 302, "Failed to put file")

    def get_file(self, session: requests.Session, status: Status) -> str:
        url = f'{self.url}/my_files'
        response = session.get(url)
        soup = BeautifulSoup(response.text, "html.parser")
        files = soup.select('ul.file-list a')
        found_file = next((file["href"] for file in files if "flag.txt" in file.text), None)
        if found_file:
            result = s.get(f"{self.url}{found_file}").text
        else:
            result = False
        self.c.assert_eq(response.status_code, 200, "Failed to get file", status)
        return result


def rnd_integer(min_value: int, max_value: int) -> int:
    return random.randint(min_value, max_value)

def generate_flag(length=31):
    chars = string.ascii_uppercase + string.digits
    return ''.join(random.choices(chars, k=length))+'='

class Checker(BaseChecker):
    vulns: int = 2
    timeout: int = 5
    uses_attack_data: bool = True

    def __init__(self, *args, **kwargs):
        super(Checker, self).__init__(*args, **kwargs)
        self.username = "static_user"
        self.password = "static_pass"
        self.mch = CheckMachine(self)

    def cquit(self, status, public='', private=''):
        if status == Status.OK:
            print(public)
            sys.exit(101)
        else:
            print(private, file=sys.stderr)
            sys.exit(104)

    def action(self, action, *args, **kwargs):
        try:
            super(Checker, self).action(action, *args, **kwargs)
        except requests.exceptions.ConnectionError as e:
            self.cquit(Status.DOWN, 'Connection error', f'Got requests connection error: {str(e)}')
        except Exception as e:
            error_traceback = traceback.format_exc()
            self.cquit(Status.DOWN, 'Unexpected error', f'Unexpected error: {str(e)}\nTraceback:\n{error_traceback}')

    def check(self):
        try:
            session = get_initialized_session()
            username, password = rnd_username(), rnd_password()

            order_name = rnd_string(10)
            order_description = generate_flag()
            order_price = rnd_integer(100, 1000)

            self.mch.register(session, username, password)
            self.mch.login(session, username, password, Status.MUMBLE)
            self.mch.create_order(session, order_name, order_description, order_price)

            orders = self.mch.get_order(session, Status.MUMBLE)
            self.assert_in(order_name, orders, "Order not found in the list")
            self.assert_in(order_description, orders, "Order description not found in the list")

            self.cquit(Status.OK)
        except CheckFinished:
            raise
        except Exception as e:
            error_traceback = traceback.format_exc()
            self.cquit(Status.DOWN, 'Unexpected error', f'Unexpected error in check: {str(e)}\nTraceback:\n{error_traceback}')

    def info(self):
        info = {
            "actions": [
                {"name": "check", "usage": "checker.py check <ip>"},
                {"name": "put", "usage": "checker.py put <ip> <flag_id> <flag> <vuln>"},
                {"name": "get", "usage": "checker.py get <ip> <login:password:order_name> <flag> <vuln>"}
            ],
            "vulns": self.vulns,
            "vuln_details": [{"1": "bof"}, {"2": "change_passwd"}],
            "timeout": 5,
            "attack_data": False,
        }
        print(json.dumps(info, indent=4))
        sys.exit(101)

    def put(self, flag_id: str, flag: str, vuln: str):
        session = get_initialized_session()

        order_name = rnd_string(10)
        order_description = flag
        order_price = rnd_integer(101, 500)

        if vuln == "1":
            order_name += "_1"
        elif vuln == "2":
            order_name += "_2"

        try:
            self.mch.register(session, self.username, self.password)
        except AssertionError:
            pass

        self.mch.login(session, self.username, self.password, Status.MUMBLE)
        self.mch.create_order(session, order_name, order_description, order_price)

        self.cquit(Status.OK, order_name, order_name)

    def get(self, flag_id: str, flag: str, vuln: str):
        session = get_initialized_session()

        self.mch.login(session, self.username, self.password, Status.CORRUPT)
        orders = self.mch.get_order(session, Status.CORRUPT)

        self.assert_in(flag_id, orders, "Order not found in the list", Status.CORRUPT)
        self.assert_in(flag, orders, "Flag not found in the order description", Status.CORRUPT)

        self.cquit(Status.OK)

if __name__ == '__main__':
    c = Checker(sys.argv[2])
    try:
        if sys.argv[1] == "info":
            c.info()
        else:
            c.action(sys.argv[1], *sys.argv[3:])
    except c.get_check_finished_exception():
        cquit(Status(c.status), c.public, c.private)