<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>User Documents</title>
    <style>
        body {
            background: linear-gradient(to right, #fbc2eb, #a18cd1);
            font-family: 'Arial', sans-serif;
            padding: 50px;
            color: #fff;
            text-align: center;
        }
        .document-list, .upload-form {
            max-width: 600px;
            margin: 0 auto;
            background: rgba(255, 255, 255, 0.2);
            padding: 20px;
            border-radius: 12px;
            box-shadow: 0 0 10px rgba(255, 182, 193, 0.7);
            margin-bottom: 30px;
        }
        h2 {
            color: #ffe4e1;
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.2);
        }
        .document-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: #ffe4e1;
            margin: 10px 0;
            padding: 10px;
            border-radius: 8px;
            box-shadow: 2px 2px 4px rgba(0, 0, 0, 0.1);
        }
        button, input[type="submit"] {
            background-color: #ff69b4;
            border: none;
            border-radius: 8px;
            color: white;
            padding: 8px 12px;
            font-size: 14px;
            cursor: pointer;
            box-shadow: 2px 2px 4px rgba(255, 20, 147, 0.5);
        }
        button:hover, input[type="submit"]:hover {
            background-color: #ff1493;
        }
        input[type="text"], input[type="file"], textarea {
            width: 90%;
            padding: 10px;
            margin: 10px 0;
            border: none;
            border-radius: 8px;
            background: #fff;
            display: block;
            margin: 10px auto;
            box-shadow: 2px 2px 4px rgba(0, 0, 0, 0.1);
        }
        .header {
            background: rgba(255, 255, 255, 0.2);
            padding: 15px;
            border-radius: 12px;
            margin-bottom: 30px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .user-info {
            display: flex;
            align-items: center;
            gap: 20px;
        }
        .search-container {
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .search-box {
            padding: 8px;
            border-radius: 8px;
            border: none;
            width: 200px;
        }
        .search-btn {
            background-color: #ff69b4;
            border: none;
            border-radius: 8px;
            color: white;
            padding: 8px 15px;
            cursor: pointer;
            box-shadow: 2px 2px 4px rgba(255, 20, 147, 0.5);
        }
        .search-btn:hover {
            background-color: #ff1493;
        }
        .logout-btn {
            background-color: #ff1493;
        }
    </style>
</head>
<body>
    <div class="header">
        <div class="user-info">
            <span>Welcome, ${username!"Guest"}!</span>
            <button class="logout-btn" onclick="logout()">Logout</button>
        </div>
        <div class="search-container">
            <input type="text" 
                   class="search-box" 
                   id="searchInput" 
                   placeholder="Search documents...">
            <button class="search-btn" id="search" onclick="window.location.href=`/document/search?name=`+document.getElementById('searchInput').value.toLowerCase()">Search</button>
        </div>
    </div>

<div class="document-list">
    <h2>Your Documents</h2>
    <#-- Iterate over each document and display it -->
    <#if documents?? && documents?size gt 0>
        <#list documents as document>
            <div class="document-item">
                <span>${document.name}</span>
                <button onclick="downloadDocument('${document.uuid}')">Download</button>
            </div>
        </#list>
    <#else>
        <p>No documents found</p>
    </#if>
</div>

<div class="upload-form">
    <h2>Add New Document</h2>
    <input type="text" id="doc-name" placeholder="Document Name">
    <textarea id="doc-object" placeholder="Document Object (JSON format)"></textarea>
    <input type="file" id="doc-file">
    <input type="submit" value="Submit" onclick="uploadDocument()">
</div>

<script>

    function downloadDocument(uid) {
        const url = `/document/download/`+uid;
        fetch(url)
            .then(response => response.blob())
            .then(blob => {
                const url = window.URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.style.display = 'none';
                a.href = url;
                a.download = document.docx; // or the appropriate file extension based on your app
                document.body.appendChild(a);
                a.click();
                window.URL.revokeObjectURL(url);
            })
            .catch(error => {
                alert('An error occurred while downloading the document: ' + error.message);
            });
    }

    function uploadDocument() {
        const name = document.getElementById('doc-name').value;
        const object = document.getElementById('doc-object').value;
        const fileInput =document.getElementById('doc-file');
        
        if (fileInput.files.length === 0) {
            alert('Please select a file to upload.');
            return;
        }
        
        const formData = new FormData();
        formData.append('name', name);
        formData.append('object', object);
        formData.append('file', fileInput.files[0]);
        
        fetch('/document/upload', {
            method: 'POST',
            body: formData
        })
        .then(response => response.json())
        .then(data => {
            if (data.message) {
                alert('Document uploaded successfully!');
                location.reload();  // Optionally reload the page to reflect new document
            } else {
                alert(data.error);
            }
        })
        .catch(error => {
            alert('An error occurred while uploading the document: ' + error.message);
        });
    }

</script>

</body>
</html>
