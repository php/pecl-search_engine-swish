--TEST--
SwishResults::getRemovedStopwords() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$results = $h->query("test");
var_dump($results->getRemovedStopwords(dirname(__FILE__)."/index.swish-e"));

$results = $h->query("test AND hello");
var_dump($results->getRemovedStopwords(dirname(__FILE__)."/index.swish-e"));

$results = $h->query("test or hello");
var_dump($results->getRemovedStopwords(dirname(__FILE__)."/index.swish-e"));
var_dump($results->getRemovedStopwords(dirname(__FILE__)."/index"));
var_dump($results->getRemovedStopwords(dirname(__FILE__)."/"));
var_dump($results->getRemovedStopwords(""));
var_dump($results->getRemovedStopwords());

echo "Done\n";
?>
--EXPECTF--	
array(0) {
}
array(0) {
}
array(0) {
}
bool(false)
bool(false)
bool(false)

Warning: SwishResults::getRemovedStopwords() expects exactly 1 parameter, 0 given in %s on line %d
NULL
Done
