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
        return f"http://{self.c.host}:{self.port}"

    def __init__(self, checker: BaseChecker):
        self.c = checker
        self.port = PORT

    def register(self, session: requests.Session, username: str, password: str, email: str, status: Status):
        url = f"{self.url}/register"
        data = {
            "username": username,
            "email": email,
            "password": password,
        }
        response = session.post(url, data=data)
        #print(f"[+] NEW USER REGISTERED: {username}:{password}")
        #print(f"[+] STATUS CODE: {response.status_code}")
        self.c.assert_eq(response.status_code, 200, "Failed to register user", status)

    def login(self, session: requests.Session, username: str, password: str, status: Status):
        url = f"{self.url}/login"
        data = {
            "username": username,
            "password": password
        }
        response = session.post(url, data=data)
        #print(f"[+] NEW USER AUTORISED: {username}:{password}")
        #print(f"[+] SESSION COOKIES:{session.cookies}")
        #print(f"[+] SESSION COOKIES:{response.text}")
        #print(f"[+] STATUS CODE: {response.status_code}")
        self.c.assert_eq(response.status_code, 200, "Failed to login", status)
    def logout(self, session: requests.Session, status: Status):
        url = f"{self.url}/logout"

        response = session.get(url)

        self.c.assert_eq(response.status_code, 200, "Failed to logout", status)
    def check_email_change(self, session: requests.Session, new_email: str, status: Status):
        url = f"{self.url}/change"

        data = {"change-option": "email", "new-value": new_email}

        response = session.post(url, data=data)

        self.c.assert_eq(response.status_code, 200, "Failed to change email(1)", status)

        response = session.get(f"{self.url}/profile")

        soup = BeautifulSoup(response.text, "html.parser")

        email = soup.find("div", class_="email").text

        self.c.assert_eq(email, new_email, "Failed to change email(2)", status)

    def change_password(self, session: requests.Session, new_password: str, status: Status):
        url = f"{self.url}/change"

        data = {"change-option": "password", "new-value": new_password}

        response = session.post(url, data=data)

        self.c.assert_eq(response.status_code, 200, "Failed to change email(1)", status)


    def put_file(self, session: requests.Session, data: str, status: Status):
        #print("[+] STARTING func put_file")
        #print(f"[+] SESSION COOKIE: {session.cookies}")
        #print(f"[+] FLAG: {data}")
        url = f"{self.url}/upload"
        response = session.post(url, files={"file": ("flag.txt", data)})
        #print(f"[+] RESPONSE CODE: {response.status_code}")
        self.c.assert_eq(response.status_code, 200, "Failed to put file")

    def get_file(self, session: requests.Session, status: Status) -> str:
        #print("[+] STARTING func get_file")
        #print(f"[+] SESSION COOKIES: {session.cookies}")
        url = f"{self.url}/my_files"
        response = session.get(url)
        soup = BeautifulSoup(response.text, "html.parser")
        files = soup.select("ul.file-list a")
        found_file = next(
            (file["href"] for file in files if "flag.txt" in file.text), None
        )
        if found_file:
            result = session.get(f"{self.url}{found_file}").text
        else:
            result = False
        #print(f"[+] files: {response.text}")
        #print(f"[+] RESULT: {result}")
        self.c.assert_eq(response.status_code, 200, "Failed to get file", status)
        return result


def rnd_integer(min_value: int, max_value: int) -> int:
    return random.randint(min_value, max_value)

def rnd_string(length=50):
    return ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(length))

def generate_flag(length=31):
    chars = string.ascii_uppercase + string.digits
    return "".join(random.choices(chars, k=length)) + "="


class Checker(BaseChecker):
    vulns: int = 1
    timeout: int = 5
    uses_attack_data: bool = True

    def __init__(self, *args, **kwargs):
        super(Checker, self).__init__(*args, **kwargs)
        self.mch = CheckMachine(self)

    def action(self, action, *args, **kwargs):
        try:
            super(Checker, self).action(action, *args, **kwargs)
        except requests.exceptions.ConnectionError as e:
            cquit(
                Status.DOWN,
                "Connection error",
                f"Got requests connection error: {str(e)}",
            )
        except Exception as e:
            error_traceback = traceback.format_exc()
            cquit(
                Status.DOWN,
                "Unexpected error",
                f"Unexpected error: {str(e)}\nTraceback:\n{error_traceback}",
            )


    def info(self):
        info = {
            "actions": [
                {"name": "check", "usage": "checker.py check <ip>"},
                {"name": "put", "usage": "checker.py put <ip> <flag_id> <flag> <vuln>"},
                {"name": "get", "usage": "checker.py get <ip> <login:password:order_name> <flag> <vuln>"}
            ],
            "vulns": self.vulns,
            "timeout": 5,
            "attack_data": True
        }
        print(json.dumps(info, indent=4))
        sys.exit(101)


    def check(self):
        try:
            session = get_initialized_session()
            username, password = rnd_username(), rnd_password()

            data = rnd_string()

            email = f"{username}@mail.ru"

            new_email = f"{rnd_string()}@mail.ru"
            new_password = rnd_password()

            self.mch.register(session, username, password, email, Status.MUMBLE)
            self.mch.put_file(session, data, Status.MUMBLE)
            self.mch.check_email_change(session, new_email, Status.MUMBLE)
            self.mch.change_password(session, new_password, Status.MUMBLE)
            self.mch.logout(session, Status.MUMBLE)
            self.mch.login(session, username, new_password, Status.MUMBLE)

            flag = self.mch.get_file(session, Status.MUMBLE)
            #print(f"[+] DATA: {data}, FLAG: {flag}")
            self.assert_in(data, flag, "no flag :(")

            cquit(Status.OK)
        except CheckFinished:
            raise
        except Exception as e:
            error_traceback = traceback.format_exc()
            cquit(
                Status.DOWN,
                "Unexpected error",
                f"Unexpected error in check: {str(e)}\nTraceback:\n{error_traceback}",
            )

    def put(self, flag_id: str, flag: str, vuln: str):
        #print("[+] stage 1")
        session = get_initialized_session()

        username, password = rnd_username(), rnd_password()

        email = f"{username}@mail.ru"

        #print("[+] stage 2")
        #print(f"[+] USERNAME: {username}, PASSWORD: {password}")
        #print(f"[+] FLAG: {flag}")

        self.mch.register(session, username, password, email, Status.CORRUPT)
        self.mch.put_file(session, flag, Status.CORRUPT)
        #print("[+] END OF func put()")
        cquit(Status.OK, username, f"{username}:{password}")

    def get(self, flag_id: str, flag: str, vuln: str):
        #print("[+] stage 3")
        #print("start get")
        session = get_initialized_session()

        username, password = flag_id.split(":")

        #print(f"[+] FLAG: {flag}")
        #print(f"[+] USERNAME: {username}, PASSWORD; {password}")
        self.mch.login(session, username, password, Status.CORRUPT)
        orders = self.mch.get_file(session, Status.CORRUPT)

        self.assert_in(
            flag, orders, "Flag not found in the order description", Status.CORRUPT
        )

        cquit(Status.OK)


if __name__ == "__main__":
    c = Checker(sys.argv[2])
    try:
        c.action(sys.argv[1], *sys.argv[3:])
    except c.get_check_finished_exception():
        cquit(Status(c.status), c.public, c.private)
