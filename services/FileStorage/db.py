import sqlite3
from hashlib import sha256

def init_db():
	conn = sqlite3.connect('./db.db')
	cursor = conn.cursor()
	cursor.execute('''
		CREATE TABLE IF NOT EXISTS users (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			username TEXT NOT NULL UNIQUE,
			email TEXT NOT NULL,
			hashed_password TEXT NOT NULL
		)
	''')
	cursor.execute('''
		CREATE TABLE IF NOT EXISTS files (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			username TEXT NOT NULL,
			file_path TEXT NOT NULL
		)
	''')
	conn.commit()
	conn.close()

def login(username, password):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	hashed_password = sha256(password.encode('utf-8')).hexdigest()
	cursor.execute('SELECT id FROM users WHERE username = ? AND hashed_password = ?', (username, hashed_password))
	result = cursor.fetchone()
	connection.close()
	if result:
		return result[0]
	else:
		return False

def register(username, email, password):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	hashed_password = sha256(password.encode('utf-8')).hexdigest()
	cursor.execute('REPLACE INTO users (username, email, hashed_password) VALUES (?, ?, ?)', (username, email, hashed_password))
	connection.commit()
	cursor.execute('SELECT id FROM users WHERE username = ?', (username,))
	result = cursor.fetchone()
	connection.close()
	return result[0]

def get_username(id):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	cursor.execute('SELECT username FROM users WHERE id = ?', (id,))
	result = cursor.fetchone()
	connection.close()
	if result:
		return result[0]
	else:
		return False

def get_files(username):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	cursor.execute('SELECT file_path, id FROM files WHERE username = ?', (username,))
	result = cursor.fetchall()
	connection.close()
	if result:
		return result
	else:
		return []

def add_file(username, file_path):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	cursor.execute('INSERT INTO files (username, file_path) VALUES (?, ?)', (username, file_path))
	connection.commit()
	return 0

def get_email(id):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	cursor.execute('SELECT email FROM users WHERE id = ?', (id,))
	result = cursor.fetchone()
	connection.close()
	if result:
		return result[0]
	else:
		return False

def get_filepath(id):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	cursor.execute('SELECT file_path FROM files WHERE id = ?', (id,))
	result = cursor.fetchone()
	connection.close()
	if result:
		return result[0]
	else:
		return False

def change(change_option, value, user_id):
	connection = sqlite3.connect('./db.db')
	cursor = connection.cursor()
	if change_option == "password":
		value = sha256(value.encode('utf-8')).hexdigest()
		change_option = "hashed_password"
	cursor.execute(f'UPDATE users SET {change_option} = ? WHERE id = ?', (value, user_id))
	connection.commit()
	return 0