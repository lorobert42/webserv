<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
    $UPLOAD_DIR = "upload/";

    ob_start();

    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    	$upload_message = "No file uploaded...";

        // File upload handling
        if (isset($_FILES['file']) && $_FILES['file']['error'] == 0) {
            $targetFile = $UPLOAD_DIR . basename($_FILES['file']['name']);

            // Move the uploaded file to the target directory
            if (move_uploaded_file($_FILES['file']['tmp_name'], $targetFile)) {
                $upload_message = "File successfully uploaded!";
            } else {
                $upload_message = "There was an error uploading the file, please try again!";
            }
        }
    }
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width">
    <title>File Uploaded</title>
</head>
<body>
	<h1><?php echo $upload_message; ?></h1>
	<button onclick="window.location.href = '/cgi';">Home</button>
</body>
</html>
<?php
	http_response_code(201);
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>
