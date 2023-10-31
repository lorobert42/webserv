<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);

	if (isset($_COOKIE['click_count'])) {
	    if (isset($_POST['decrement'])) {
	        $count = $_COOKIE['click_count'] - 1;
	    } else if (isset($_POST['increment'])) {
	        $count = $_COOKIE['click_count'] + 1;
        } else if (isset($_POST['reset'])) {
			$count = 0;
		} else {
	        $count = $_COOKIE['click_count'];
	    }
	} else {
	    $count = 0;
	}
	setcookie('click_count', $count);

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
        <h1 class="text-2xl text-center mb-4 font-bold">Counter</h1>

        <form method="POST" class="flex flex-col justify-center items-center">
			<div><?php echo $count; ?></div>
            <div class="flex flex-row justify-center items-center gap-2">
                <button id="submit" type="submit" name="reset" class="mt-6 bg-red-500 hover:bg-red-700 text-white py-2 px-4 rounded w-min">
					Reset
				</button>
                <button id="submit" type="submit" name="decrement" class="mt-6 bg-blue-500 hover:bg-blue-700 text-white py-2 px-4 rounded w-min">
					Decrement
				</button>
                <button id="submit" type="submit" name="increment" class="mt-6 bg-blue-500 hover:bg-blue-700 text-white py-2 px-4 rounded w-min">
                    Increment
                </button>
            </div>
        </form>
    </div>
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>