#!/usr/bin/env python3

import sys
import traceback
from concurrent.futures import ThreadPoolExecutor

from checklib import (
    cquit,
    Status,
    BaseChecker,
    rnd_username as checklib_rnd_username,
    rnd_password,
    get_initialized_session,
)

from raas_lib import RaasApi
from executable_checks import (
    ExecutableCheck,
    SimpleCheck,
    ReverserCheck,
    FileSystemCheck,
    create_reader,
    random_filename,
    create_writer,
)


def rnd_username():
    return checklib_rnd_username().lower()


class Checker(BaseChecker):
    vulns: int = 1
    timeout: int = 15
    uses_attack_data: bool = True

    def __init__(self, *args, **kwargs):
        super(Checker, self).__init__(*args, **kwargs)
        self.raas = RaasApi(self)

    def check(self):
        self.check_auth()

        executable_checks = [
            SimpleCheck(),
            ReverserCheck(),
            FileSystemCheck(),
        ]
        for check in executable_checks:
            self.check_executable(check)
        self.cquit(Status.OK)

    def check_auth(self):
        s1 = get_initialized_session()
        u, p = rnd_username(), rnd_password()
        rsp1 = self.raas.login_checked(s1, u, p)
        self.assert_eq(rsp1.status_code, 200, "Invalid status response on first login")
        self.assert_in("username", rsp1.cookies, "Cookie not set after first login")

        s2 = get_initialized_session()
        rsp2 = self.raas.login(s2, u, p)
        self.assert_eq(rsp2.status_code, 200, "Invalid status response on second login")
        self.assert_in("username", rsp2.cookies, "Cookie not set after second login")

    def check_executable(self, check: ExecutableCheck):
        u, p = rnd_username(), rnd_password()

        results = []
        for launch in check.get_launches():
            s = get_initialized_session()
            self.raas.login_checked(s, u, p)
            result = self.raas.execute(s, launch)
            results.append(result)

        try:
            check.check_response(results)
        except Exception:
            self.cquit(
                Status.MUMBLE,
                f'"{check.name}" check failed',
                (
                    f'"{check.name}" check failed:\n'
                    f"Launches:\n{check.get_launches()}\n"
                    f"Results:\n{results}\n"
                    f"Traceback:\n{traceback.format_exc()}\n"
                ),
            )

    def put(self, flag_id: str, flag: str, vuln: str):
        s = get_initialized_session()
        u, p = rnd_username(), rnd_password()
        self.raas.login_checked(s, u, p)

        filename = random_filename()
        launch, _, _ = create_writer(filename, flag.encode())
        self.raas.execute(s, launch)

        self.cquit(Status.OK, u, f"{u}:{p}:{filename}")

    def get(self, flag_id: str, flag: str, vuln: str):
        s = get_initialized_session()
        u, p, filename = flag_id.split(":")
        self.raas.login_checked(s, u, p)

        launch = create_reader(filename)
        response = self.raas.execute(s, launch, Status.CORRUPT)
        self.assert_eq(
            response.stdout.strip().decode(),
            flag,
            "Invalid flag",
            Status.CORRUPT,
        )
        self.cquit(Status.OK)


if __name__ == "__main__":
    c = Checker(sys.argv[2])

    try:
        c.action(sys.argv[1], *sys.argv[3:])
    except c.get_check_finished_exception():
        cquit(Status(c.status), c.public, c.private)
