--TEST--
SwishResult::getMetaList() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$rs = $h->query("test");

$r = $rs->nextResult();

var_dump($r->getMetaList("test"));
var_dump($r->getMetaList());

echo "Done\n";
?>
--EXPECTF--	
Warning: SwishResult::getMetaList() expects exactly 0 parameters, 1 given in %s on line %d
NULL
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
Done
