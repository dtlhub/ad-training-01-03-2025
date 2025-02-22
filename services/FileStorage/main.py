#!/usr/bin/python3
from flask import Flask, make_response, request, render_template, redirect, session, url_for, send_file
import db, os
from markupsafe import Markup

app = Flask(__name__)

app.secret_key = b'qk&bfgyru76258xzvzx!i#$'
app.config['MAX_CONTENT_LENGTH'] = 1 * 1000 * 1000
app.config['DEBUG'] = True

db.init_db()


@app.route('/', methods=["GET"])
def index():
	if 'user_id' in session:
		return redirect(url_for('my_files'))
	return redirect(url_for('login'))

@app.route('/getfile<int:file_id>', methods=["GET"])
def getfile(file_id):
	if 'user_id' in session:
		file_path = db.get_filepath(file_id)
		if os.path.isfile(file_path):
			return send_file(file_path, as_attachment=True)
		else:
			return make_response(f"File not found.", 404)
	return redirect(url_for('login'))

@app.route('/my_files', methods=["GET"])
def my_files():
	if 'user_id' in session:
		return render_template("my_files.html", files=db.get_files(db.get_username(session["user_id"])))
	return redirect(url_for('login'))

@app.route('/profile', methods=["GET"])
def profile():
	if 'user_id' in session:
		return render_template("profile.html", username=db.get_username(session["user_id"]), email=db.get_email(session["user_id"]))
	return redirect(url_for('login'))

@app.route('/upload', methods=["POST", "GET"])
def upload():
	if 'user_id' in session:
		if request.method == "POST":
			file = request.files.get('file')
			if file:
				if not os.path.exists(os.path.join("uploads", db.get_username(session["user_id"]))):
					os.makedirs(os.path.join("uploads", db.get_username(session["user_id"])))
				db.add_file(db.get_username(session["user_id"]), os.path.join("uploads", db.get_username(session["user_id"]), Markup(file.filename)))
				file.save(os.path.join("uploads", db.get_username(session["user_id"]), Markup(file.filename)))
			return redirect(url_for('my_files'))
		return render_template("upload.html")
	return redirect(url_for('login'))

@app.route("/login", methods=["POST", "GET"])
def login():
	if request.method == "POST":
		username = Markup(request.form.get("username"))
		password = Markup(request.form.get("password"))
		if user_id := db.login(username, password):
			session['user_id'] = user_id
			return redirect(url_for('index'))

	return render_template("login.html")

@app.route("/register", methods=["POST", "GET"])
def register():
	if request.method == "POST":
		username = Markup(request.form.get("username"))
		email = Markup(request.form.get("email"))
		password = Markup(request.form.get("password"))
		if user_id := db.register(username, email, password):
			session['user_id'] = user_id
			return redirect(url_for('index'))

	return render_template("register.html")

@app.route('/change', methods=["POST"])
def change():
	if 'user_id' in session:
		change_option = Markup(request.form.get("change-option"))
		value = Markup(request.form.get("new-value"))
		db.change(change_option, value, session["user_id"])
		return redirect(url_for('profile'))
	return redirect(url_for('login'))

if __name__ == "__main__":
	app.run(port=5000, host='0.0.0.0')