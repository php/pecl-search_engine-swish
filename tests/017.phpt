--TEST--
SwishSearch::setPhraseDelimiter() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$s = $h->prepare("test_and_test");
$s->setPhraseDelimiter("_");
$rs = $s->execute();

var_dump($rs->getParsedWords(dirname(__FILE__)."/index.swish-e"));

$s = $h->prepare("test_and_test");
$rs = $s->execute();

var_dump($rs->getParsedWords(dirname(__FILE__)."/index.swish-e"));

$s = $h->prepare("test_and_test");
$s->setPhraseDelimiter();
$s->setPhraseDelimiter("");
$rs = $s->execute();

var_dump($rs->getParsedWords(dirname(__FILE__)."/index.swish-e"));

$s = $h->prepare("test_and_test");
$s->setPhraseDelimiter("t");
$rs = $s->execute();

var_dump($rs->getParsedWords(dirname(__FILE__)."/index.swish-e"));

echo "Done\n";
?>
--EXPECTF--	
array(5) {
  [0]=>
  string(4) "test"
  [1]=>
  string(1) "_"
  [2]=>
  string(3) "and"
  [3]=>
  string(1) "_"
  [4]=>
  string(4) "test"
}
array(3) {
  [0]=>
  string(4) "test"
  [1]=>
  string(3) "and"
  [2]=>
  string(4) "test"
}

Warning: SwishSearch::setPhraseDelimiter() expects exactly 1 parameter, 0 given in %s on line %d
array(3) {
  [0]=>
  string(4) "test"
  [1]=>
  string(3) "and"
  [2]=>
  string(4) "test"
}
array(7) {
  [0]=>
  string(1) "t"
  [1]=>
  string(1) "e"
  [2]=>
  string(1) "t"
  [3]=>
  string(3) "and"
  [4]=>
  string(1) "t"
  [5]=>
  string(1) "e"
  [6]=>
  string(1) "t"
}
Done
