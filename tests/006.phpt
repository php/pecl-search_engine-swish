--TEST--
SwishSearch::setLimit() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$search = $h->prepare("test");
try {
	var_dump($search->setLimit("swishdocsize", 0, 10000));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($search->setLimit("swishdocsize", -1, -1000000));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($search->setLimit("swishrank", -1, -1000000));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($search->setLimit("swishdocsiz", 0, 10000));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
var_dump($search->setLimit());

echo "Done\n";
?>
--EXPECTF--	
NULL
string(42) "Property 'swishdocsize' is already limited"
NULL
NULL

Warning: SwishSearch::setLimit() expects exactly 3 parameters, 0 given in %s on line %d
NULL
Done
