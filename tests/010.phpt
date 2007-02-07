--TEST--
SwishSearch::resetLimit() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$search = $h->prepare("test");

$search->resetLimit();
$search->resetLimit(1);
$rs = $search->execute();
var_dump($rs->hits);

var_dump($search->setLimit("swishdocsize", 0, 5000));
$rs = $search->execute();
var_dump($rs->hits);

$search->resetLimit();
$rs = $search->execute();
var_dump($rs->hits);

echo "Done\n";
?>
--EXPECTF--	
Warning: SwishSearch::resetLimit() expects exactly 0 parameters, 1 given in %s on line %d
int(9)
NULL
int(5)
int(9)
Done
