<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);

	if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['file']) && $_FILES['file']['error'] == 0) {
	    $uploadDirectory = "upload/";

	    $targetFile = $uploadDirectory . basename($_FILES['file']['name']);

	    // Move the uploaded file to the target directory
	    if (move_uploaded_file($_FILES['file']['tmp_name'], $targetFile)) {
	        echo "File uploaded successfully.";
	    } else {
	        echo "Failed to upload the file.";
	    }
	} elseif ($_SERVER['REQUEST_METHOD'] === 'POST') {
	    echo "No file uploaded or there was an error uploading.";
	}

	// Handle DELETE request
    if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
        $deleteData = json_decode(file_get_contents("php://input"), true);
        $filename = $deleteData['filename'] ?? null;

        if ($filename) {
            $filePath = "upload/" . basename($filename);

            // Ensure the file exists and is within the expected directory
            if (realpath($filePath) === $filePath && file_exists($filePath)) {
                unlink($filePath);
                echo "File deleted successfully.";
            } else {
                http_response_code(404);
                echo "File not found.";
            }
        } else {
            http_response_code(400);
            echo "Filename not provided.";
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
                    <input type="file" id="file" name="file" class="border border-gray-300 rounded-md p-2" accept="image/*">
                </div>
            </div>
            <div class="flex flex-col gap-2 mt-4 justify-center items-center hidden">
                <img id="image_preview" class="rounded-md w-[200px]" alt="image_preview" src="">
                <button id="clear" type="button" class="bg-red-500 hover:bg-red-700 text-white py-2 px-4 rounded w-min">
                    Remove
                </button>
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
	        $uploadDirectory = "upload/";
	        $files = scandir($uploadDirectory);
	        foreach ($files as $file) {
	            if ($file != "." && $file != "..") {
	                echo '<div class="flex flex-row gap-4 justify-between items-center">';
	                echo '<span class="ml-2">' . $file . '</span>';
	                echo '<button type="button" data-filename="' . $file . '" class="deleteButton bg-red-500 hover:bg-red-700 text-white py-2 px-4 rounded w-min self-end">Delete</button>';
	                echo '</div>';
	            }
	        }
	      ?>
			</div>
    </div>

    <script>
        const fileInput = document.getElementById('file');
        const imagePreview = document.getElementById('image_preview');

        fileInput.addEventListener('change', (e) => {
            const file = e.target.files[0];
            const reader = new FileReader();

            reader.addEventListener('load', (e) => {
                imagePreview.src = e.target.result;
                imagePreview.parentElement.classList.remove('hidden');
            });

            reader.readAsDataURL(file);
        });

        document.getElementById('clear').addEventListener('click', (e) => {
            imagePreview.src = '';
            imagePreview.parentElement.classList.add('hidden');
            fileInput.value = '';
        });

		document.querySelectorAll('.deleteButton').forEach(button => {
		    button.addEventListener('click', function() {
		        const filename = this.getAttribute('data-filename');
		        console.log(filename);

		        fetch('/', {
		            method: 'DELETE',
		            body: JSON.stringify({ filename: filename }),
		            headers: {
		                'Content-Type': 'application/json'
		            }
		        }).then(response => {
		            return response.text();
		        }).then(data => {
		            alert(data);
		        }).catch(error => {
		            console.error('Error:', error);
		        });
		    });
		});

    </script>
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>