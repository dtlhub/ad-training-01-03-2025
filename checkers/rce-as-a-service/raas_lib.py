import requests

from checklib import BaseChecker, Status

from executable_checks import Launch, ExecutionResult


class RaasApi:
    PORT = 9091

    @property
    def url(self):
        return f"http://{self.c.host}:{self.PORT}"

    def __init__(self, checker: BaseChecker):
        self.c = checker
        self.port = self.PORT

    def login(
        self, session: requests.Session, username: str, password: str
    ) -> requests.Response:
        response = session.post(
            f"{self.url}/api/login",
            json={"username": username, "password": password},
        )
        self.check_return_code(response, 200, "Invalid status response on login")
        self.c.assert_in("username", response.cookies, "Cookie not set after login")
        return response

    def execute(
        self,
        session: requests.Session,
        launch: Launch,
        status: Status = Status.MUMBLE,
    ) -> ExecutionResult:
        response = session.post(
            f"{self.url}/api/execute",
            json=launch.to_json(),
        )

        if response.status_code != 200:
            self.c.cquit(
                status,
                "Unable to perform RCE",
                (
                    f"Failed to run executable {launch.executable}:\n"
                    f"Status code: {response.status_code}\n"
                    f"Response: {response.content}"
                ),
            )

        try:
            parsed = ExecutionResult.from_response(response)
        except Exception:
            self.c.cquit(
                status,
                "Unable to parse response",
                f"Response: {response.content}",
            )

        return parsed

    def check_return_code(
        self, response: requests.Response, expected_code: int, public_message: str
    ):
        if response.status_code != expected_code:
            self.c.cquit(
                Status.MUMBLE,
                public_message,
                f"Expected status code {expected_code}, got {response.status_code}: {response.content}",
            )
