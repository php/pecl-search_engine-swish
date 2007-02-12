--TEST--
SwishSearch::setStructure() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$s = $h->prepare("test");
$s->setStructure(Swish::IN_FILE); //the default one
$rs = $s->execute("", "");
$rs = $s->execute();

var_dump($rs->hits);

$s = $h->prepare("test");
$s->setStructure(Swish::IN_TITLE); 
$rs = $s->execute();

var_dump($rs->hits);

$s->setStructure(Swish::IN_TITLE,1); 

echo "Done\n";
?>
--EXPECTF--	
Warning: SwishSearch::execute() expects at most 1 parameter, 2 given in %s on line %d
int(9)
int(0)

Warning: SwishSearch::setStructure() expects exactly 1 parameter, 2 given in %s on line %d
Done
