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
    """
    Message sent from the website chat box.
    Stored as sender='WEB'. The ESP8266 picks it up via /get_for_arduino.
    No SignalWire call here — outbound SMS only happens when the ESP8266
    posts back via /post_from_arduino.
    """
    msg = request.form.get('message', '').strip()
    if not msg:
        return jsonify(status='error'), 400

    with _db() as conn:
        conn.execute(
            "INSERT INTO messages (user_id, sender, content, timestamp) VALUES (?, 'WEB', ?, ?)",
            (current_user.id, msg, datetime.datetime.now().isoformat())
        )
        conn.commit()

    return jsonify(status='success')


@chat_bp.route('/messages')
@login_required
def get_messages():
    """
    Returns the last 40 messages for the current user.
    Resolves contact names for inbound SMS messages.
    """
    with _db() as conn:
        rows = conn.execute(
            '''SELECT sender, content, timestamp, from_number
               FROM messages
               WHERE user_id = ? AND sender != 'SMS_QUEUE'
               ORDER BY id DESC LIMIT 40''',
            (current_user.id,)
        ).fetchall()

        # Build contact map for display names
        contacts = {}
        for row in conn.execute('SELECT phone, name FROM contacts'):
            if row['name']:
                contacts[row['phone']] = row['name']

    msgs = []
    for r in reversed(rows):
        from_num = r['from_number']
        contact_name = contacts.get(from_num) if from_num else None
        msgs.append({
            'sender':       r['sender'],
            'content':      r['content'],
            'timestamp':    r['timestamp'],
            'from_number':  from_num,
            'contact_name': contact_name,
        })
    return jsonify(msgs)
