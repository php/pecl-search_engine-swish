--TEST--
SwishResult::stem() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");
$rs = $h->query();
$rs = $h->query("test");

$r = $rs->nextResult();
var_dump($r->stem("developing"));
var_dump($r->stem("testing"));
var_dump($r->stem("tests"));
var_dump($r->stem("c"));
var_dump($r->stem("security"));
var_dump($r->stem("secure"));
var_dump($r->stem());

try {
	var_dump($r->stem(""));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

try {
	var_dump($r->stem(str_repeat("test", 100)));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

try {
	var_dump($r->stem("1234567890"));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--	
Warning: Swish::query() expects exactly 1 parameter, 0 given in %s on line %d
array(1) {
  [0]=>
  string(7) "develop"
}
array(1) {
  [0]=>
  string(4) "test"
}
array(1) {
  [0]=>
  string(4) "test"
}
array(1) {
  [0]=>
  string(1) "c"
}
array(1) {
  [0]=>
  string(5) "secur"
}
array(1) {
  [0]=>
  string(5) "secur"
}

Warning: SwishResult::stem() expects exactly 1 parameter, 0 given in %s on line %d
NULL
string(36) "The word was stemmed to empty string"
array(1) {
  [0]=>
  string(400) "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
}
array(1) {
  [0]=>
  string(10) "1234567890"
}
Done
