<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
    $UPLOAD_DIR = "upload/";

	// Handle POST request
	if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['file']) && $_FILES['file']['error'] == 0) {

	    $targetFile = $UPLOAD_DIR . basename($_FILES['file']['name']);

	    // Move the uploaded file to the target directory
	    if (move_uploaded_file($_FILES['file']['tmp_name'], $targetFile)) {
	        $upload_message = "File uploaded successfully.";
	        header("Location: /"); // Add this line to reload the page after successful upload
	    } else {
	        $upload_message = "There was an error uploading the file, please try again!";
	    }
	}
	elseif ($_SERVER['REQUEST_METHOD'] === 'POST') {
		$upload_message = "No file uploaded or there was an error uploading.";
	}

	// Handle DELETE request
    if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
		$filename = $_SERVER['HTTP_X_FILENAME'];
		$targetFile = $UPLOAD_DIR . $filename;

		if (file_exists($targetFile)) {
			unlink($targetFile);
			echo "File deleted successfully.";
		} else {
			echo "File does not exist.";
		}

    }

	ob_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width">
    <title>HTML Forms</title>
    <script src="https://cdn.tailwindcss.com"></script>
</head>
<body class="bg-gray-50 flex flex-col pt-10 p-4">
    <div class="mx-auto p-6 bg-white shadow-md rounded-lg w-full max-w-[600px] flex flex-col">
        <h1 class="text-2xl text-center mb-4 font-bold">Upload a file</h1>

        <form method="POST" class="flex flex-col" enctype="multipart/form-data" action="/">
            <div class="flex flex-col gap-4">
                <div class="flex flex-col">
                    <label for="file" class="text-sm text-gray-500 ml-2">File</label>
                    <input type="file" id="file" name="file" class="border border-gray-300 rounded-md p-2" required>
                    <?php if (isset($upload_message)) { ?>
						<span class="text-sm text-gray-500 ml-2 mt-1"><?php echo $upload_message; ?></span>
					<?php } ?>
                </div>
            </div>
            <button id="submit" type="submit" class="mt-8 bg-blue-500 hover:bg-blue-700 text-white py-2 px-4 rounded w-min self-end">
                Upload
            </button>
        </form>
    </div>

    <div class="mx-auto p-6 bg-white shadow-md rounded-lg w-full max-w-[600px] flex flex-col mt-4">
	<h1 class="text-2xl text-center mb-4 font-bold">Uploaded files</h1>
		<div class="flex flex-col gap-6 mt-4">
		<?php
	        $files = scandir($UPLOAD_DIR);
	        foreach ($files as $file) {
	            if ($file != "." && $file != "..") {
	                echo '<div class="flex flex-row gap-4 justify-between items-center">';
	                echo '<span>' . $file . '</span>';
	                echo '<div class="flex flex-row gap-2 justify-between items-center">';
	                    echo '<a href="' . $UPLOAD_DIR . $file . '" download="' . $file . '" class="bg-blue-500 hover:bg-blue-700 text-white py-2 px-4 rounded w-min">Download</a>';
	                    echo '<button type="button" class="bg-red-500 hover:bg-red-700 text-white py-2 px-4 rounded w-min" onclick="deleteFile(\'' . $file . '\')">Delete</button>';
                    echo '</div>';
	                echo '</div>';
	            }
	        }
	        if (count($files) == 2) {
	            echo '<span class="text-center">There are no files uploaded yet...</span>';
	        }
		?>
		</div>
    </div>

    <script>
        const fileInput = document.getElementById('file');

        const form = document.querySelector('form');
        form.addEventListener('submit', function(e) {
            e.preventDefault();

            const formData = new FormData(form);

            fetch('/', {
                method: 'POST',
                body: formData
            })
            .then(response => {
				if (response.ok) {
					fileInput.value = '';
					window.location.reload();
				}
			})
        });

		function deleteFile(filename) {
			if (window.confirm("Are you sure you want to delete " + filename + "?")) {
				fetch('/', {
                    method: 'DELETE',
                    headers: {
                        'Content-Type': 'application/json',
                        'X-Filename': filename,
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
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>