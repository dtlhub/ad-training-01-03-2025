#!/usr/bin/env python3
import sys
import requests

from checklib import *
from bimba_lib import *
import docx2txt


class Checker(BaseChecker):
    vulns: int = 1
    timeout: int = 15
    uses_attack_data: bool = True

    def __init__(self, *args, **kwargs):
        super(Checker, self).__init__(*args, **kwargs)
        self.mch = CheckMachine(self)

    def action(self, action, *args, **kwargs):
        try:
            super(Checker, self).action(action, *args, **kwargs)
        except requests.exceptions.ConnectionError:
            self.cquit(Status.DOWN, "Connection error", "Got requests connection error")

    def check_auth(self):
        user = User.random()
        session = self.get_initialized_session()
        self.mch.register_user(session, user)
        self.mch.login(session, user)

    def check_upload(self):
        user = User.random()
        session = self.get_initialized_session()
        self.mch.register_user(session, user)
        self.mch.login(session, user)
        doc = Document.generate_doc()
        doc.uuid = self.mch.upload(session, document=doc)

    def check_download(self):
        user = User.random()
        session = self.get_initialized_session()
        self.mch.register_user(session, user)
        self.mch.login(session, user)
        doc = Document.generate_doc()
        self.mch.upload(session, document=doc)
        document_card = self.mch.search_document(session, doc.name)
        content = self.mch.download(session, document_card.uuid)
        if not content:
            self.cquit(Status.MUMBLE, "Download error", "Failed to download document")
        temp = docx2txt.process(f"/tmp/{document_card.uuid}.docx")
        text = " ".join([line.replace("\t", " ") for line in temp.split("\n") if line])
        expected_text = doc.text.replace("{{name}}", doc.data["name"]).replace(
            "{{age}}", doc.data["age"]
        )
        if expected_text != text:
            self.cquit(Status.MUMBLE, "Document error", "Document data is different")

    def check_search(self):
        pass

    def check(self):
        self.check_auth()
        self.check_upload()
        self.check_download()
        self.cquit(Status.OK)

    def put(self, flag_id: str, flag: str, vuln: str):
        user = User.random()
        session = self.get_initialized_session()
        self.mch.register_user(session, user)
        self.mch.login(session, user)
        doc = Document.generate_doc()
        doc.data = {"name": flag, "age": random.randint(18, 100)}
        self.mch.upload(session, doc)
        self.cquit(
            Status.OK,
            json.dumps({"username": user.username, "location": "documents"}),
            user.serialize(),
        )

    def get(self, flag_id: str, flag: str, vuln: str):
        user = User.deserialize(flag_id)
        session = get_initialized_session()
        self.mch.login(session, user, status=Status.CORRUPT)
        document_card = self.mch.search_document(session, "")
        content = self.mch.download(session, document_card.uuid)
        temp = docx2txt.process(f"/tmp/{document_card.uuid}.docx")
        text = " ".join([line.replace("\t", " ") for line in temp.split("\n") if line])
        self.assert_in(flag, text, "Document text is invalid", Status.CORRUPT)
        self.cquit(Status.OK)


if __name__ == "__main__":
    c = Checker(sys.argv[2])

    try:
        c.action(sys.argv[1], *sys.argv[3:])
    except c.get_check_finished_exception():
        cquit(Status(c.status), c.public, c.private)
