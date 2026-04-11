from flask import Blueprint, render_template, request, redirect, url_for, flash, current_app
from flask_login import login_user, logout_user, login_required
from werkzeug.security import generate_password_hash, check_password_hash
import sqlite3

from models import User, get_db_connection

auth_bp = Blueprint('auth', __name__)


def _db():
    return get_db_connection(current_app.config['DB_PATH'])


@auth_bp.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        username = request.form.get('username', '').strip().lower()
        password = request.form.get('password', '')
        phone    = ''.join(filter(str.isdigit, request.form.get('phone', '')))

        if not username or not password or not phone:
            flash('All fields are required.')
            return render_template('signup.html')

        if len(phone) != 10:
            flash('Please enter a valid 10-digit US phone number.')
            return render_template('signup.html')

        hashed_pw = generate_password_hash(password)
        try:
            with _db() as conn:
                conn.execute(
                    "INSERT INTO users (username, password, phone) VALUES (?, ?, ?)",
                    (username, hashed_pw, phone)
                )
                conn.commit()
            return redirect(url_for('auth.login'))
        except sqlite3.IntegrityError:
            flash('Username or phone number already in use.')

    return render_template('signup.html')


@auth_bp.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form.get('username', '').strip().lower()
        password = request.form.get('password', '')

        with _db() as conn:
            user = conn.execute(
                "SELECT * FROM users WHERE username = ?", (username,)
            ).fetchone()

        if user and check_password_hash(user['password'], password):
            login_user(User(user['id'], user['username'], user['phone'], user['is_admin']))
            return redirect(url_for('chat.index'))
        flash('Invalid username or password.')

    return render_template('login.html')


@auth_bp.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('auth.login'))
