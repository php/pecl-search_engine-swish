--TEST--
SwishResults::nextResult() and SwishResults::seekResult() basic tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$results = $h->query("test");

var_dump($results->seekResult(0));
var_dump($results->seekResult(2));
try {
	var_dump($results->seekResult(1000));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
echo "Done\n";
?>
--EXPECTF--	
int(0)
int(2)
string(15) "No more results"
Done
