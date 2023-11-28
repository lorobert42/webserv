<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);

	ob_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width">
    <title>Form to upload a file</title>
</head>
<body>
	<button onclick="window.location.href = '/cgi';">Home</button>

	<h1>Form to upload a file</h1>
	<form method="POST" enctype="multipart/form-data" action="/upload">
        <label for="file">File</label>
        <input type="file" id="file" name="file" required>
        <button id="submit" type="submit">
            Upload
        </button>
    </form>
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>
