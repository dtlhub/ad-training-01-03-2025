document.addEventListener('DOMContentLoaded', () => {
    const authSection = document.getElementById('auth-section');
    const executionSection = document.getElementById('execution-section');
    const loginForm = document.getElementById('login-form');
    const usernameInput = document.getElementById('username');
    const passwordInput = document.getElementById('password');
    const loginButton = document.getElementById('login-button');
    const authMessage = document.getElementById('auth-message');
    const userDisplay = document.getElementById('user-display');
    const logoutButton = document.getElementById('logout-button');
    const wasmFileInput = document.getElementById('wasm-file');
    const cliArgsInput = document.getElementById('cli-args');
    const executeButton = document.getElementById('execute-button');
    const stdoutElement = document.getElementById('stdout');
    const stderrElement = document.getElementById('stderr');

    checkAuthStatus();

    loginButton.addEventListener('click', handleLogin);
    logoutButton.addEventListener('click', handleLogout);
    executeButton.addEventListener('click', executeWasm);

    async function checkAuthStatus() {
        try {
            const response = await fetch('/api/execute', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    wasm: '',
                    args: []
                })
            });

            if (response.status === 200) {
                const usernameCookie = document.cookie
                    .split('; ')
                    .find(row => row.startsWith('username='));

                if (usernameCookie) {
                    const username = usernameCookie.split('=')[1];
                    showUserInterface(username);
                }
            } else {
                showLoginInterface();
            }
        } catch (error) {
            console.error('Error checking auth status:', error);
            showLoginInterface();
        }
    }

    function showLoginInterface() {
        authSection.classList.remove('hidden');
        executionSection.classList.add('hidden');
    }

    function showUserInterface(username) {
        authSection.classList.add('hidden');
        executionSection.classList.remove('hidden');
        userDisplay.textContent = username;
    }

    async function handleLogin() {
        const username = usernameInput.value.trim();
        const password = passwordInput.value.trim();

        if (!username || !password) {
            displayAuthMessage('Please enter both username and password.', 'error');
            return;
        }

        try {
            const response = await fetch('/api/login', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    username,
                    password
                })
            });

            if (response.ok) {
                showUserInterface(username);
                displayAuthMessage('', '');
            } else {
                const errorData = await response.json();
                displayAuthMessage(`Login failed: ${errorData.message || 'Unknown error'}`, 'error');
            }
        } catch (error) {
            console.error('Login error:', error);
            displayAuthMessage('Login failed. Please try again.', 'error');
        }
    }

    function handleLogout() {
        document.cookie = 'username=; Max-Age=0; path=/; SameSite=Strict';
        showLoginInterface();
    }

    function displayAuthMessage(message, type) {
        authMessage.textContent = message;
        authMessage.className = 'message';
        if (type) {
            authMessage.classList.add(type);
        }
    }

    function isPrintableText(str) {
        const nonPrintableCount = str.split('').filter(char => {
            const code = char.charCodeAt(0);
            return (code < 32 && code !== 10 && code !== 9) ||
                (code > 126 && code < 160);
        }).length;

        return (nonPrintableCount / str.length) < 0.05;
    }

    function decodeBase64(base64String) {
        try {
            const binaryString = atob(base64String);
            const bytes = new Uint8Array(binaryString.length);
            for (let i = 0; i < binaryString.length; i++) {
                bytes[i] = binaryString.charCodeAt(i);
            }
            return new TextDecoder().decode(bytes);
        } catch (error) {
            console.error('Error decoding base64:', error);
            return null;
        }
    }

    function formatOutput(base64String, outputElement) {
        if (!base64String) {
            outputElement.textContent = '(No output)';
            outputElement.classList.remove('warning');
            return;
        }

        const decodedText = decodeBase64(base64String);

        if (decodedText && isPrintableText(decodedText)) {
            outputElement.textContent = decodedText;
            outputElement.classList.remove('warning');
        } else {
            outputElement.innerHTML = '<div class="warning-banner">⚠️ Non-printable content detected. Showing raw base64:</div>' + base64String;
            outputElement.classList.add('warning');
        }
    }

    async function executeWasm() {
        const file = wasmFileInput.files[0];
        if (!file) {
            alert('Please select a WASM file');
            return;
        }

        const cliArgs = cliArgsInput.value.trim()
            ? cliArgsInput.value.split(',').map(arg => arg.trim())
            : [];

        stdoutElement.textContent = 'Running...';
        stderrElement.textContent = '';

        try {
            const base64Wasm = await readFileAsBase64(file);

            const response = await fetch('/api/execute', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    wasm: base64Wasm,
                    args: cliArgs
                })
            });

            if (response.ok) {
                const result = await response.json();

                formatOutput(result.stdout, stdoutElement);
                formatOutput(result.stderr, stderrElement);
            } else {
                const errorData = await response.json();
                stdoutElement.textContent = '(Execution failed)';
                stderrElement.textContent = errorData.message || 'Unknown error';
            }
        } catch (error) {
            console.error('Execution error:', error);
            stdoutElement.textContent = '(Execution failed)';
            stderrElement.textContent = error.message || 'Unknown error';
        }
    }

    function readFileAsBase64(file) {
        return new Promise((resolve, reject) => {
            const reader = new FileReader();
            reader.onload = () => {
                const base64String = reader.result.split(',')[1];
                resolve(base64String);
            };
            reader.onerror = error => reject(error);
            reader.readAsDataURL(file);
        });
    }
});