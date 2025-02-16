import requests

from checklib import BaseChecker

from executable_checks import Launch, ExecutionResult


class RaasApi:
    PORT = 9091

    @property
    def url(self):
        return f"http://{self.c.host}:{self.PORT}"

    def __init__(self, checker: BaseChecker):
        self.c = checker
        self.port = self.PORT

    def login_checked(
        self, session: requests.Session, username: str, password: str
    ) -> requests.Response:
        response = self.login(session, username, password)
        self.c.assert_in("username", session.cookies, "Cookie not set after login")

        err = f"Invalid status response on login: {response.status_code}"
        self.c.assert_eq(response.status_code, 200, err)
        self.c.assert_in("username", response.cookies, "Cookie not set after login")

        return response

    def login(self, session: requests.Session, username: str, password: str):
        return session.post(
            f"{self.url}/api/login",
            json={"username": username, "password": password},
        )

    def execute(self, session: requests.Session, launch: Launch) -> ExecutionResult:
        response = session.post(
            f"{self.url}/api/execute",
            json=launch.to_json(),
        )
        print(f"Response: {response.content}")
        self.c.assert_eq(
            response.status_code, 200, "Invalid status response on execute"
        )
        return ExecutionResult.from_response(response)
