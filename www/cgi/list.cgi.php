<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
    $UPLOAD_DIR = "upload/";
	ob_start();

	if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['filename'])) {
		$filename = urldecode($_POST['filename']);
		$targetFile = $UPLOAD_DIR . $filename;

		if (file_exists($targetFile)) {
			header('Content-Description: File Transfer');
			header('Content-Type: application/octet-stream');
			header('Content-Disposition: attachment; filename="' . $filename . '"');
			header('Content-Length: ' . filesize($targetFile));
			readfile($targetFile);
			exit;
		} else {
			echo "File does not exist.";
		}
	}

    if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
        $filename = urldecode($_SERVER['HTTP_X_FILENAME']);
        $targetFile = $UPLOAD_DIR . $filename;

        if (file_exists($targetFile)) {
            unlink($targetFile);
            echo "File deleted successfully.";
        } else {
            echo "File does not exist.";
        }
    }
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width">
    <title>Uploaded files</title>
</head>
<body>
	<button onclick="window.location.href = '/';">Home</button>

    <h1>Uploaded files</h1>
	<ul>
	<?php
        $files = scandir($UPLOAD_DIR);
        foreach ($files as $file) {
            if ($file != "." && $file != "..") {
            	echo '
            	<li>' . $file . '
            	    <form method="post" action="/list">
				        <input type="hidden" name="filename" value="' . $file . '">
				        <button type="submit">
				            Download
				        </button>
			        </form>
                    <button type="button" onclick="deleteFile(\'' . $file . '\')">
                        Delete
                    </button>
                </li>';
            }
        }
        if (count($files) == 2) {
            echo '<li>No files uploaded yet</li>';
        }
	?>
	</ul>
</body>
</html>
<script>
	function deleteFile(filename) {
		if (window.confirm("Are you sure you want to delete " + filename + "?")) {
			fetch('/list', {
                method: 'DELETE',
                headers: {
                    'Content-Type': 'application/json',
                    'X-Filename': encodeURIComponent(filename),
                },
            })
            .then(response => {
                if (response.ok) {
                    window.location.reload();
                }
            })
		}
	}
</script>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>