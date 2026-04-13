import datetime
from functools import wraps
from flask import Blueprint, render_template, request, redirect, url_for, flash, jsonify, current_app
from flask_login import login_required, current_user
from werkzeug.security import generate_password_hash
import sqlite3

from models import get_db_connection

admin_bp = Blueprint('admin', __name__, template_folder='../templates/admin')


def _db():
    return get_db_connection(current_app.config['DB_PATH'])


def admin_required(f):
    @wraps(f)
    @login_required
    def decorated(*args, **kwargs):
        if not current_user.is_admin:
            flash('Admin access required.')
            return redirect(url_for('chat.index'))
        return f(*args, **kwargs)
    return decorated


# ── Dashboard ─────────────────────────────────────────────────────────────────

@admin_bp.route('/')
@admin_required
def dashboard():
    with _db() as conn:
        stats = {
            'total_users':       conn.execute("SELECT COUNT(*) FROM users").fetchone()[0],
            'total_messages':    conn.execute("SELECT COUNT(*) FROM messages").fetchone()[0],
            'web_messages':      conn.execute("SELECT COUNT(*) FROM messages WHERE sender='WEB'").fetchone()[0],
            'modem_messages':    conn.execute("SELECT COUNT(*) FROM messages WHERE sender='MODEM'").fetchone()[0],
            'unread':            conn.execute("SELECT COUNT(*) FROM messages WHERE read=0").fetchone()[0],
            'pending_contacts':  conn.execute("SELECT COUNT(*) FROM contacts WHERE name IS NULL").fetchone()[0],
        }

        # Pending (nameless) contacts with message preview
        pending_contacts = conn.execute('''
            SELECT
                c.phone,
                COUNT(m.id)    AS msg_count,
                MAX(m.timestamp) AS last_ts,
                MAX(m.content) AS last_msg
            FROM contacts c
            LEFT JOIN messages m ON m.from_number = c.phone
            WHERE c.name IS NULL
            GROUP BY c.phone
            ORDER BY last_ts DESC
        ''').fetchall()

        recent = conn.execute('''
            SELECT m.id, m.sender, m.content, m.timestamp, m.read, u.username
            FROM messages m LEFT JOIN users u ON m.user_id = u.id
            ORDER BY m.id DESC LIMIT 10
        ''').fetchall()

    return render_template('admin/dashboard.html',
                           stats=stats,
                           pending_contacts=pending_contacts,
                           recent=recent)


# ── User Management ───────────────────────────────────────────────────────────

@admin_bp.route('/users')
@admin_required
def users():
    with _db() as conn:
        all_users = conn.execute('''
            SELECT u.id, u.username, u.phone, u.is_admin,
                   COUNT(m.id) as msg_count
            FROM users u
            LEFT JOIN messages m ON m.user_id = u.id
            GROUP BY u.id
            ORDER BY u.id
        ''').fetchall()
    return render_template('admin/users.html', users=all_users)


@admin_bp.route('/users/create', methods=['GET', 'POST'])
@admin_required
def create_user():
    if request.method == 'POST':
        username = request.form.get('username', '').strip().lower()
        password = request.form.get('password', '')
        phone    = request.form.get('phone', '').strip()
        is_admin = 1 if request.form.get('is_admin') else 0

        if not username or not password:
            flash('Username and password are required.')
            return render_template('admin/create_user.html')

        try:
            with _db() as conn:
                conn.execute(
                    "INSERT INTO users (username, password, phone, is_admin) VALUES (?, ?, ?, ?)",
                    (username, generate_password_hash(password), phone or None, is_admin)
                )
                conn.commit()
            flash(f'User "{username}" created.')
            return redirect(url_for('admin.users'))
        except sqlite3.IntegrityError:
            flash('Username or phone already in use.')

    return render_template('admin/create_user.html')


@admin_bp.route('/users/<int:user_id>/edit', methods=['GET', 'POST'])
@admin_required
def edit_user(user_id):
    with _db() as conn:
        user = conn.execute("SELECT * FROM users WHERE id = ?", (user_id,)).fetchone()
    if not user:
        flash('User not found.')
        return redirect(url_for('admin.users'))

    if request.method == 'POST':
        username = request.form.get('username', '').strip().lower()
        phone    = request.form.get('phone', '').strip()
        is_admin = 1 if request.form.get('is_admin') else 0
        new_pw   = request.form.get('password', '').strip()

        try:
            with _db() as conn:
                if new_pw:
                    conn.execute(
                        "UPDATE users SET username=?, phone=?, is_admin=?, password=? WHERE id=?",
                        (username, phone or None, is_admin, generate_password_hash(new_pw), user_id)
                    )
                else:
                    conn.execute(
                        "UPDATE users SET username=?, phone=?, is_admin=? WHERE id=?",
                        (username, phone or None, is_admin, user_id)
                    )
                conn.commit()
            flash('User updated.')
            return redirect(url_for('admin.users'))
        except sqlite3.IntegrityError:
            flash('Username or phone already in use.')

    return render_template('admin/edit_user.html', user=user)


@admin_bp.route('/users/<int:user_id>/delete', methods=['POST'])
@admin_required
def delete_user(user_id):
    if user_id == current_user.id:
        flash("You can't delete your own account.")
        return redirect(url_for('admin.users'))

    with _db() as conn:
        conn.execute("DELETE FROM messages WHERE user_id = ?", (user_id,))
        conn.execute("DELETE FROM users WHERE id = ?", (user_id,))
        conn.commit()
    flash('User and their messages deleted.')
    return redirect(url_for('admin.users'))


# ── Message Management ────────────────────────────────────────────────────────

@admin_bp.route('/messages')
@admin_required
def messages():
    page     = request.args.get('page', 1, type=int)
    per_page = 25
    offset   = (page - 1) * per_page
    username_filter = request.args.get('username', '').strip().lower()
    sender_filter   = request.args.get('sender', '').strip().upper()

    where_clauses, params = [], []
    if username_filter:
        where_clauses.append("u.username = ?")
        params.append(username_filter)
    if sender_filter:
        where_clauses.append("m.sender = ?")
        params.append(sender_filter)

    where_sql = ("WHERE " + " AND ".join(where_clauses)) if where_clauses else ""

    with _db() as conn:
        total = conn.execute(
            f"SELECT COUNT(*) FROM messages m LEFT JOIN users u ON m.user_id=u.id {where_sql}",
            params
        ).fetchone()[0]

        msgs = conn.execute(
            f'''SELECT m.id, m.sender, m.content, m.timestamp, m.read, u.username
                FROM messages m LEFT JOIN users u ON m.user_id = u.id
                {where_sql}
                ORDER BY m.id DESC LIMIT ? OFFSET ?''',
            params + [per_page, offset]
        ).fetchall()

        all_usernames = [r[0] for r in conn.execute("SELECT username FROM users ORDER BY username").fetchall()]

    total_pages = (total + per_page - 1) // per_page
    return render_template('admin/messages.html',
                           messages=msgs,
                           page=page,
                           total_pages=total_pages,
                           username_filter=username_filter,
                           sender_filter=sender_filter,
                           all_usernames=all_usernames)


@admin_bp.route('/messages/<int:msg_id>/delete', methods=['POST'])
@admin_required
def delete_message(msg_id):
    with _db() as conn:
        conn.execute("DELETE FROM messages WHERE id = ?", (msg_id,))
        conn.commit()
    return ('', 204)


@admin_bp.route('/messages/delete_all', methods=['POST'])
@admin_required
def delete_all_messages():
    user_id = request.form.get('user_id', type=int)
    with _db() as conn:
        if user_id:
            conn.execute("DELETE FROM messages WHERE user_id = ?", (user_id,))
        else:
            conn.execute("DELETE FROM messages")
        conn.commit()
    flash('Messages deleted.')
    return redirect(url_for('admin.messages'))
