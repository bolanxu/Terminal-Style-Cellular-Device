import sqlite3
from flask_login import UserMixin


class User(UserMixin):
    def __init__(self, id, username, phone, is_admin=False):
        self.id = id
        self.username = username
        self.phone = phone
        self.is_admin = bool(is_admin)


def get_db_connection(db_path):
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    return conn
