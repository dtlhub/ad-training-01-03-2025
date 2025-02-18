from abc import ABCMeta, abstractmethod
import base64
import random
import string
from dataclasses import dataclass

import requests
from checklib import rnd_string


def binaries_from_langs(base_name: str, langs: list[str]) -> list[str]:
    return [f"executables/wasm/{base_name}-{lang}.wasm" for lang in langs]


ALL_LANGUAGES = ["c"]


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


class ExecutableCheck(metaclass=ABCMeta):
    name: str

    @abstractmethod
    def get_launches(self) -> list[Launch]:
        pass

    @abstractmethod
    def check_response(self, results: list[ExecutionResult]):
        pass


class SimpleCheck(ExecutableCheck):
    name = "simple"

    def __init__(self):
        argc = random.randint(1, 5)
        encoding = random.choice(
            [
                lambda x: x,
                lambda x: x.encode().hex(),
                lambda x: base64.b64encode(x.encode()).decode().replace("=", "=="),
            ]
        )
        self.args = [encoding(rnd_string(random.randint(10, 100))) for _ in range(argc)]

    def get_launches(self) -> list[Launch]:
        binary = random.choice(binaries_from_langs(self.name, ALL_LANGUAGES))
        return [Launch(self.args, binary)]

    def check_response(self, results: list[ExecutionResult]):
        assert len(results) == 1
        assert " ".join(self.args) == results[0].stdout.decode()


class ReverserCheck(ExecutableCheck):
    name = "reverser"

    def __init__(self):
        self.string = rnd_string(random.randint(10, 100))

    def get_launches(self) -> list[Launch]:
        binary = random.choice(binaries_from_langs(self.name, ALL_LANGUAGES))
        return [Launch([base64.b64encode(self.string.encode()).decode()], binary)]

    def check_response(self, results: list[ExecutionResult]):
        assert len(results) == 1
        assert self.string[::-1] == results[0].stdout.decode()


def xor(a: bytes, b: bytes) -> bytes:
    return bytes(x ^ y for x, y in zip(a, b))


def create_writer(filename: str, content: bytes) -> tuple[Launch, bytes, bytes]:
    padding = bytes(random.randint(0, 255) for _ in range(random.randint(0, 20)))
    first = bytes(random.randint(0, 255) for _ in range(len(filename)))
    second = xor(filename.encode(), first) + padding
    if random.random() < 0.5:
        first, second = second, first

    binary = random.choice(binaries_from_langs("writer", ALL_LANGUAGES))
    return Launch([first.hex(), second.hex(), content.hex()], binary), first, second


def create_reader(filename: str) -> Launch:
    binary = random.choice(binaries_from_langs("reader", ALL_LANGUAGES))
    return Launch([filename], binary)


class FileSystemCheck(ExecutableCheck):
    name = "filesystem"

    def __init__(self):
        self.filename = (
            rnd_string(random.randint(10, 100))
            + "."
            + rnd_string(random.randint(2, 4), string.ascii_lowercase)
        )
        self.content = bytes(
            random.randint(0, 255) for _ in range(random.randint(10, 100))
        )
        self.writer, self.first, self.second = create_writer(
            self.filename, self.content
        )
        self.reader = create_reader(self.filename)

    def get_launches(self) -> list[Launch]:
        return [self.writer, self.reader]

    def check_response(self, results: list[ExecutionResult]):
        assert len(results) == 2
        writer, reader = results

        assert writer.stdout == xor(
            self.filename.encode(), self.first
        ), "writer stdout does not match"
        assert writer.stderr == xor(
            self.filename.encode(), self.second
        ), "writer stderr does not match"

        assert reader.stdout == self.content, "reader stdout does not match"
        assert reader.stderr == b"", (
            "reader stderr is not empty (got " + reader.stderr.decode() + ")"
        )
