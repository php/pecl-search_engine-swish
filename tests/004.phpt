--TEST--
SwishResults and SwishResult basic tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$results = $h->query("test");

while ($results && $result = $results->nextResult()) {
	var_dump($result);
	var_dump($result->test);
}


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
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(2)
  ["swishrank"]=>
  int(899)
  ["swishfilenum"]=>
  int(12)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.TESTING2"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(4922)
  ["swishlastmodified"]=>
  int(1048578318)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(3)
  ["swishrank"]=>
  int(610)
  ["swishfilenum"]=>
  int(10)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.SUBMITTING_PATCH"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(4557)
  ["swishlastmodified"]=>
  int(1072136752)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(4)
  ["swishrank"]=>
  int(604)
  ["swishfilenum"]=>
  int(7)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.QNX"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(2112)
  ["swishlastmodified"]=>
  int(923518833)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(5)
  ["swishrank"]=>
  int(530)
  ["swishfilenum"]=>
  int(15)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.WIN32-BUILD-SYSTEM"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(6290)
  ["swishlastmodified"]=>
  int(1072147878)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(6)
  ["swishrank"]=>
  int(308)
  ["swishfilenum"]=>
  int(3)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.EXT_SKEL"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(7126)
  ["swishlastmodified"]=>
  int(1056902837)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(7)
  ["swishrank"]=>
  int(248)
  ["swishfilenum"]=>
  int(1)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.CVS-RULES"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(5183)
  ["swishlastmodified"]=>
  int(1151000661)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(8)
  ["swishrank"]=>
  int(161)
  ["swishfilenum"]=>
  int(16)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.Zeus"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(4289)
  ["swishlastmodified"]=>
  int(1076280587)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
object(SwishResult)#%d (8) {
  ["swishreccount"]=>
  int(9)
  ["swishrank"]=>
  int(161)
  ["swishfilenum"]=>
  int(8)
  ["swishdbfile"]=>
  string(%d) "%sindex.swish-e"
  ["swishdocpath"]=>
  string(%d) "%sREADME.SELF-CONTAINED-EXTENSIONS"
  ["swishtitle"]=>
  NULL
  ["swishdocsize"]=>
  int(4768)
  ["swishlastmodified"]=>
  int(1035408917)
}

Notice: Undefined property:  SwishResult::$test in %s on line %d
NULL
Done
