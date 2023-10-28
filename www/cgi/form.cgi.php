<?php
	ob_start();
?>
<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
	echo 'POST';
}
echo 'salut';
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
	                echo '<button type="button" class="bg-red-500 hover:bg-red-700 text-white py-2 px-4 rounded w-min self-center">Delete</button>';
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
    </script>
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>