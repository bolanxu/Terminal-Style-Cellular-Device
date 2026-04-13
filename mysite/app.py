import sqlite3
import datetime
import os
from flask import Flask
from flask_login import LoginManager
from models import User, get_db_connection

app = Flask(__name__)
app.config['SECRET_KEY'] = 'your-secret-key-goes-here'

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DB_PATH = os.path.join(BASE_DIR, 'chat.db')
app.config['DB_PATH'] = DB_PATH

# ── SignalWire config — set these in your WSGI file via os.environ ────────────
# SW_SPACE_URL    = yourspace.signalwire.com
# SW_PROJECT_ID   = your-project-id
# SW_AUTH_TOKEN   = your-auth-token
# SW_FROM_NUMBER  = +1XXXXXXXXXX
for key in ('SW_SPACE_URL', 'SW_PROJECT_ID', 'SW_AUTH_TOKEN', 'SW_FROM_NUMBER'):
    app.config[key] = os.environ.get(key, '')

# --- LOGIN MANAGER ---
login_manager = LoginManager()
login_manager.login_view = 'auth.login'
login_manager.init_app(app)

@login_manager.user_loader
def load_user(user_id):
    with get_db_connection(DB_PATH) as conn:
        user = conn.execute("SELECT * FROM users WHERE id = ?", (user_id,)).fetchone()
        if user:
            return User(user['id'], user['username'], user['phone'], user['is_admin'])
    return None

# --- REGISTER BLUEPRINTS ---
from routes.auth import auth_bp
from routes.chat import chat_bp
from routes.arduino import arduino_bp
from routes.admin import admin_bp
from routes.signalwire import signalwire_bp

app.register_blueprint(auth_bp)
app.register_blueprint(chat_bp)
app.register_blueprint(arduino_bp)
app.register_blueprint(admin_bp, url_prefix='/admin')
app.register_blueprint(signalwire_bp)

# --- DB INIT ---
from db import init_db
init_db(DB_PATH)

if __name__ == '__main__':
    app.run(debug=True)
