<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
	ob_start();

    if (isset($_COOKIE['count'])) {
        if (isset($_POST['decrement'])) {
            $count = $_COOKIE['count'] - 1;
        } else if (isset($_POST['increment'])) {
            $count = $_COOKIE['count'] + 1;
        } else if (isset($_POST['reset'])) {
            $count = 0;
        } else {
            $count = $_COOKIE['count'];
        }
    } else {
        $count = 0;
    }
    setcookie('count', $count, [
        'expires' => time() + 86400, // 1 day
        'path' => '/',
        'secure' => false, // Set to true if you're using HTTPS
        'httponly' => true, // Recommended to prevent JavaScript access to the cookie
        'samesite' => 'Lax' // Use 'None' if you need the cookie for cross-site requests
    ]);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width">
    <title>HTML Forms</title>
</head>
<body>
	<button onclick="window.location.href = '/cgi';">Home</button>

    <form method="POST">
		<p><?php echo $count; ?></p>
	    <button id="submit" type="submit" name="reset">
			Reset
		</button>
	    <button id="submit" type="submit" name="decrement">
			Decrement
		</button>
	    <button id="submit" type="submit" name="increment">
	        Increment
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
