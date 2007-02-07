--TEST--
SwishResult properties
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$results = $h->query("test");

$result = $results->nextResult(); 
var_dump($result);
var_dump($result->test);
$a = array();
var_dump($result->$a);

echo "Done\n";
?>
--EXPECTF--	
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(1)
  ["swishrank"]=>
  int(1000)
  ["swishfilenum"]=>
  int(11)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.TESTING"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(6508)
  ["swishlastmodified"]=>
  int(1082042828)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Notice: Undefined property:  SwishResult::$Array in %s on line %d
NULL
Done
