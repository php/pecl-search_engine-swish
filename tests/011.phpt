--TEST--
SwishResults::getParsedWords() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$results = $h->query("test");
var_dump($results->getParsedWords(dirname(__FILE__)."/index.swish-e"));

$results = $h->query("test AND hello");
var_dump($results->getParsedWords(dirname(__FILE__)."/index.swish-e"));

$results = $h->query("test or hello");
var_dump($results->getParsedWords(dirname(__FILE__)."/index.swish-e"));

$results = $h->query("test or hello");
var_dump($results->getParsedWords(dirname(__FILE__)."/test"));
var_dump($results->getParsedWords(dirname(__FILE__)."/"));

$results = $h->query("test or hello a b c d e f g");
var_dump($results->getParsedWords(dirname(__FILE__)."/index.swish-e"));

echo "Done\n";
?>
--EXPECTF--	
array(1) {
  [0]=>
  string(4) "test"
}
array(3) {
  [0]=>
  string(4) "test"
  [1]=>
  string(3) "and"
  [2]=>
  string(5) "hello"
}
array(3) {
  [0]=>
  string(4) "test"
  [1]=>
  string(2) "or"
  [2]=>
  string(5) "hello"
}
bool(false)
bool(false)
array(10) {
  [0]=>
  string(4) "test"
  [1]=>
  string(2) "or"
  [2]=>
  string(5) "hello"
  [3]=>
  string(1) "a"
  [4]=>
  string(1) "b"
  [5]=>
  string(1) "c"
  [6]=>
  string(1) "d"
  [7]=>
  string(1) "e"
  [8]=>
  string(1) "f"
  [9]=>
  string(1) "g"
}
Done
