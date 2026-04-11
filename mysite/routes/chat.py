import datetime
from flask import Blueprint, render_template, request, jsonify, current_app
from flask_login import login_required, current_user

from models import get_db_connection

chat_bp = Blueprint('chat', __name__)


def _db():
    return get_db_connection(current_app.config['DB_PATH'])


@chat_bp.route('/')
@login_required
def index():
    return render_template('index.html')


@chat_bp.route('/send_web', methods=['POST'])
@login_required
def send_web():
    msg = request.form.get('message', '').strip()
    if msg:
        with _db() as conn:
            conn.execute(
                "INSERT INTO messages (user_id, sender, content, timestamp) VALUES (?, ?, ?, ?)",
                (current_user.id, 'WEB', msg, datetime.datetime.now().isoformat())
            )
            conn.commit()
    return jsonify(status="success")


@chat_bp.route('/messages')
@login_required
def get_messages():
    with _db() as conn:
        c = conn.cursor()
        c.execute(
            "SELECT sender, content, timestamp FROM messages WHERE user_id = ? ORDER BY id DESC LIMIT 40",
            (current_user.id,)
        )
        msgs = [{"sender": r[0], "content": r[1], "timestamp": r[2]} for r in c.fetchall()]
    return jsonify(msgs[::-1])
