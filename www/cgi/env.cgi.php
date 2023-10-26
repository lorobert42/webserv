<?php
	ob_start();
?>
<html>
	<body>
		<h1>Environment Variables</h1>
		<?php
			foreach($_ENV as $key => $value) {
				echo "<p>$key = $value</p>";
			}
		?>
	</body>
</html>
<?php
	header("Content-Length: " . ob_get_length());
	$buffer = ob_get_contents();
	ob_end_clean();
	echo $buffer;
?>