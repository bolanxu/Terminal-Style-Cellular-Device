#!/usr/bin/env python3
"""
Run this once to create your first admin account:
    python create_admin.py
"""
import os
import sqlite3
from getpass import getpass
from werkzeug.security import generate_password_hash

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DB_PATH  = os.path.join(BASE_DIR, 'chat.db')

# Make sure tables exist before we try to insert
from db import init_db
init_db(DB_PATH)

print("=== Create Admin Account ===")
username = input("Username: ").strip().lower()
password = getpass("Password: ")
phone_raw = input("Phone (10-digit, optional): ").strip()
phone = ''.join(filter(str.isdigit, phone_raw)) or None

if not username or not password:
    print("Username and password are required.")
    raise SystemExit(1)

try:
    with sqlite3.connect(DB_PATH) as conn:
        conn.execute(
            "INSERT INTO users (username, password, phone, is_admin) VALUES (?, ?, ?, 1)",
            (username, generate_password_hash(password), phone)
        )
        conn.commit()
    print(f"\nAdmin '{username}' created. Log in at /login then go to /admin")
except sqlite3.IntegrityError:
    # User already exists — just promote them
    with sqlite3.connect(DB_PATH) as conn:
        conn.execute("UPDATE users SET is_admin=1 WHERE username=?", (username,))
        conn.commit()
    print(f"\nUser '{username}' already exists — promoted to admin.")
