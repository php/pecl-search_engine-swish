<?php

if (!extension_loaded("swish")) {
	die("skip swish extension is not available");
}

try {
	$h = new Swish(dirname(__FILE__)."/index.swish-e");
} catch (SwishException $e) {
	die("skip cannot open index (different library version?)");
}

?>
