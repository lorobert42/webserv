<?php
	ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
	ob_start();

    if (isset($_POST['login'])) {
        setcookie('session', 'user', [
            'expires' => time() + 86400, // 1 day
            'path' => '/',
            'secure' => false, // Set to true if you're using HTTPS
            'httponly' => true, // Recommended to prevent JavaScript access to the cookie
            'samesite' => 'Lax' // Use 'None' if you need the cookie for cross-site requests
        ]);
        header("Refresh:0");
    }
    elseif (isset($_POST['logout'])) {
        setcookie('session', '', -1, '/');
        header("Refresh:0");
    }
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width">
    <title>HTML Session</title>
</head>
<body>
	<button onclick="window.location.href = '/';">Home</button>

    <?php if (!isset($_COOKIE['session'])) : ?>
        <p>Welcome Guest</p>
    <?php else : ?>
        <p>Welcome Admin</p>
    <?php endif; ?>

    <?php if (!isset($_COOKIE['session'])) : ?>
        <form method="POST">
            <button id="submit" type="submit" name="login">
                Login
            </button>
        </form>
    <?php else : ?>
        <form method="POST">
            <button id="submit" type="submit" name="logout">
                Logout
            </button>
        </form>
    <?php endif; ?>
</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>