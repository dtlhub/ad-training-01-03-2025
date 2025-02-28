<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login and Registration</title>
    <style>
        body {
            background: linear-gradient(to right, #fbc2eb, #a18cd1);
            font-family: 'Arial', sans-serif;
            color: #ffffff;
            text-align: center;
            padding: 50px;
        }
        .container {
            max-width: 400px;
            margin: 0 auto;
            background: rgba(255, 255, 255, 0.2);
            padding: 20px;
            border-radius: 12px;
            box-shadow: 0 0 10px rgba(255, 182, 193, 0.7);
        }
        h2 {
            margin-bottom: 20px;
            color: #ffe4e1;
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.2);
        }
        input[type="text"], input[type="password"] {
            width: 90%;
            padding: 10px;
            margin: 10px 0;
            border: none;
            border-radius: 8px;
            background: #ffe4e1;
            box-shadow: 2px 2px 4px rgba(0, 0, 0, 0.1);
        }
        button {
            width: 95%;
            padding: 10px;
            margin-top: 10px;
            background-color: #ff69b4;
            border: none;
            border-radius: 8px;
            color: white;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            box-shadow: 2px 2px 4px rgba(255, 20, 147, 0.5);
        }
        button:hover {
            background-color: #ff1493;
        }
    </style>
</head>
<body>

<div class="container">
    <h2>Login</h2>
    <input type="text" id="username" placeholder="Username">
    <input type="password" id="password" placeholder="Password">
    <button onclick="login()">Login</button>
	<button onclick="register()">Register</button>
</div>



<script>
    function login() {
        const username = document.getElementById('username').value;
        const password = document.getElementById('password').value;

        fetch('/auth/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ username, password })
        })
        .then(response => response.json())
        .then(data => {
            if (data.accessToken) {
                // Set the cookie and redirect
                document.cookie = `jwt=`+data.accessToken+`; path=/; samesite=strict`;
                window.location.href = '/document/';
            } else {
                throw new Error('Login failed: No access token received');
            }
        })
        .catch(error => {
            alert('An error occurred: ' + error.message);
        });
    }

    function register() {
        const username = document.getElementById('username').value;
        const password = document.getElementById('password').value;

        fetch('/auth/register', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ username, password })
        })
        .then(response => response.json())
        .then(data => {
            if (data.message) {
                alert('Registration successful! Please log in.');
            } else {
                alert(data.error);
            }
        })
        .catch(error => {
            alert('An error occurred: ' + error.message);
        });
		}
</script>

</body>
</html>
