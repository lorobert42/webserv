<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
    $UPLOAD_DIR = "upload/";
	ob_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width">
    <title>Uploaded files</title>
</head>
<body>
    <h1>Uploaded files</h1>
	<ul>
	<?php
        $files = scandir($UPLOAD_DIR);
        foreach ($files as $file) {
            if ($file != "." && $file != "..") {
            	echo '<li>' . $file . '</li>';
            }
        }
        if (count($files) == 2) {
            echo '<li>No files uploaded yet</li>';
        }
	?>
	</ul>
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>