import sqlite3
import datetime
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Initialize the database with the "read" column added to track whether a message has been retrieved
def init_db():
    with sqlite3.connect('chat.db') as conn:
        conn.execute('''
            CREATE TABLE IF NOT EXISTS messages (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                sender TEXT,
                content TEXT,
                timestamp TEXT,
                read INTEGER DEFAULT 0  -- "read" column to track if the message was fetched
            )
        ''')

init_db()

@app.errorhandler(400)
def bad_request(error):
    return jsonify(error="Bad Request", message=str(error)), 400

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/send_web', methods=['POST'])
def send_web():
    """Web sends a message to the database."""
    msg = request.form.get('message')
    if msg:
        with sqlite3.connect('chat.db') as conn:
            conn.execute("INSERT INTO messages (sender, content, timestamp) VALUES (?, ?, ?)",
                         ('WEB', msg, datetime.datetime.now().isoformat()))
    return jsonify(status="success")

@app.route('/get_for_arduino', methods=['GET'])
def get_for_arduino():
    """Arduino pulls the latest unread message sent by the WEB and marks it as read."""
    with sqlite3.connect('chat.db') as conn:
        c = conn.cursor()

        # Retrieve the most recent unread 'WEB' message
        c.execute("SELECT content, id FROM messages WHERE sender='WEB' AND read=0 ORDER BY id DESC LIMIT 1")
        row = c.fetchone()

        if row:
            # Get the message content
            message_content = row[0]

            # Mark the message as read
            c.execute("UPDATE messages SET read=1 WHERE id=?", (row[1],))
            conn.commit()

            return message_content
        else:
            return "NO_MSG"

@app.route('/post_from_arduino', methods=['POST'])
def post_from_arduino():
    """Arduino pushes a message to the WEB."""
    msg = request.data.decode('utf-8')
    if msg:
        with sqlite3.connect('chat.db') as conn:
            conn.execute("INSERT INTO messages (sender, content, timestamp) VALUES (?, ?, ?)",
                         ('MODEM', msg, datetime.datetime.now().isoformat()))
    return "OK"

@app.route('/messages', methods=['GET'])
def get_messages():
    """Get the last 20 messages from the database."""
    with sqlite3.connect('chat.db') as conn:
        c = conn.cursor()
        c.execute("SELECT sender, content FROM messages ORDER BY id DESC LIMIT 20")
        msgs = [{"sender": r[0], "content": r[1]} for r in c.fetchall()]
    return jsonify(msgs[::-1])

if __name__ == '__main__':
    app.run(debug=True)
