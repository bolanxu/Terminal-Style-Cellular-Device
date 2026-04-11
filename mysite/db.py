import sqlite3


def init_db(db_path):
    with sqlite3.connect(db_path) as conn:
        # --- Users table ---
        conn.execute('''CREATE TABLE IF NOT EXISTS users (
            id       INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            phone    TEXT UNIQUE,
            is_admin INTEGER DEFAULT 0
        )''')

        # --- Messages table ---
        conn.execute('''CREATE TABLE IF NOT EXISTS messages (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            sender    TEXT,
            content   TEXT,
            timestamp TEXT,
            read      INTEGER DEFAULT 0,
            user_id   INTEGER
        )''')

        # --- Migrations: add columns if they didn't exist yet ---
        _ensure_column(conn, 'users',    'phone',    'TEXT UNIQUE')
        _ensure_column(conn, 'users',    'is_admin', 'INTEGER DEFAULT 0')
        _ensure_column(conn, 'messages', 'user_id',  'INTEGER')

        conn.commit()


def _ensure_column(conn, table, column, col_def):
    cursor = conn.execute(f"PRAGMA table_info({table})")
    cols = [row[1] for row in cursor.fetchall()]
    if column not in cols:
        conn.execute(f"ALTER TABLE {table} ADD COLUMN {column} {col_def}")
        conn.commit()
