--TEST--
SwishSearch::setLimit() more sophisticated tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$search = $h->prepare("test");
try {
	$search->setLimit("swishdocsize", 0, 5000);
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

$rs = $search->execute();
var_dump($rs->hits);

$search = $h->prepare("test");
try {
	$search->setLimit("swishdocsize", 4000, 10000);
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

$rs = $search->execute();
var_dump($rs->hits);

echo "Done\n";
?>
--EXPECTF--	
int(5)
int(8)
Done
