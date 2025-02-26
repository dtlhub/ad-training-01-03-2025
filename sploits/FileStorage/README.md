- 1-ая уязвимость (File Inclusion)
уязвимость заключается в отсутствии проверки имени файла при загрузке

```angular2html
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
```

хоть здесь и есть функция Markup(), но она не защищает от того, что злоумышленник может подняться на директорию выше и изменить любой из темплейтов, прокинув туда ssti

- 2-ая уязвимость (idoor)
уязвимость заключается в отсутствии проверки, действительно ли файл, который пытается скачать пользователь его

```angular2html
@app.route('/getfile', methods=["GET"])
def getfile():
	if 'user_id' in session:
		file_id = int(request.args.get("id"))
		file_path = db.get_filepath(file_id)
		if os.path.isfile(file_path):
			return send_file(file_path, as_attachment=True)
		else:
			return make_response(f"File not found.", 404)
	return redirect(url_for('login'))
```

любой пользователь, который автоизован. может скачать любой файл, зная его id

- 3-я уязвимость (logic)
При регистрации, вместо того, чтобы проверять есть ли пользователь, с таким именем, просто заносит его в бд, при этом использую REPLACE, которая даже при существовании юзера с таким-же именем, просто заменяет его на нового

```angular2html
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
```
