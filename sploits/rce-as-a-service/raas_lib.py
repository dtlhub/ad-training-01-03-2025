import base64
from dataclasses import dataclass

import requests


@dataclass
class Launch:
    args: list[str]
    executable: str

    def to_json(self) -> dict:
        with open(self.executable, "rb") as f:
            content = base64.b64encode(f.read()).decode()
        return {
            "args": self.args,
            "wasm": content,
        }


@dataclass
class ExecutionResult:
    stdout: bytes
    stderr: bytes

    @classmethod
    def from_response(cls, response: requests.Response):
        data = response.json()
        stdout = base64.b64decode(data["stdout"])
        stderr = base64.b64decode(data["stderr"])
        return cls(stdout, stderr)


class RaasApi:
    PORT = 9091

    @property
    def url(self):
        return f"http://{self.host}:{self.PORT}"

    def __init__(self, host: str):
        self.host = host
        self.port = self.PORT

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
        return ExecutionResult.from_response(response)
