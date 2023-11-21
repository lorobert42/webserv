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
    <title>Index CGI</title>
</head>
<body>
    <h1>Index CGI</h1>
	<ul>
		<li><a href="/form">Form</a></li>
		<li><a href="/list">Uploaded files</a></li>
		<li><a href="/counter">Counter</a></li>
		<li><a href="/session">Session</a></li>
		<li><a href="/python">Python</a></li>
		<li><a href="/loop">Loop</a></li>
	</ul>
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>