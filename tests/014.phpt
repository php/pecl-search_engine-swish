--TEST--
Swish::getMetaList() and Swish::getPropertyList() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

var_dump($h->getMetaList(dirname(__FILE__)."/index.swish-e"));
var_dump($h->getPropertyList(dirname(__FILE__)."/index.swish-e"));

var_dump($h->getMetaList(dirname(__FILE__)."/index.swish-e", 1));
var_dump($h->getPropertyList(dirname(__FILE__)."/index.swish-e", 1));

var_dump($h->getMetaList());
var_dump($h->getPropertyList());

echo "Done\n";
?>
--EXPECTF--	
array(1) {
  [0]=>
  array(3) {
    ["Name"]=>
    string(12) "swishdefault"
    ["Type"]=>
    int(0)
    ["ID"]=>
    int(1)
  }
}
array(8) {
  [0]=>
  array(3) {
    ["Name"]=>
    string(13) "swishreccount"
    ["Type"]=>
    int(8)
    ["ID"]=>
    int(2)
  }
  [1]=>
  array(3) {
    ["Name"]=>
    string(9) "swishrank"
    ["Type"]=>
    int(8)
    ["ID"]=>
    int(3)
  }
  [2]=>
  array(3) {
    ["Name"]=>
    string(12) "swishfilenum"
    ["Type"]=>
    int(8)
    ["ID"]=>
    int(4)
  }
  [3]=>
  array(3) {
    ["Name"]=>
    string(11) "swishdbfile"
    ["Type"]=>
    int(4)
    ["ID"]=>
    int(5)
  }
  [4]=>
  array(3) {
    ["Name"]=>
    string(12) "swishdocpath"
    ["Type"]=>
    int(4)
    ["ID"]=>
    int(6)
  }
  [5]=>
  array(3) {
    ["Name"]=>
    string(10) "swishtitle"
    ["Type"]=>
    int(4)
    ["ID"]=>
    int(7)
  }
  [6]=>
  array(3) {
    ["Name"]=>
    string(12) "swishdocsize"
    ["Type"]=>
    int(8)
    ["ID"]=>
    int(8)
  }
  [7]=>
  array(3) {
    ["Name"]=>
    string(17) "swishlastmodified"
    ["Type"]=>
    int(16)
    ["ID"]=>
    int(9)
  }
}

Warning: Swish::getMetaList() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: Swish::getPropertyList() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: Swish::getMetaList() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: Swish::getPropertyList() expects exactly 1 parameter, 0 given in %s on line %d
NULL
Done
