--TEST--
SwishSearch::setSort() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$search = $h->prepare("test");

$search->setSort("swishrank desc");
$rs = $search->execute();

while ($rs && $r = $rs->nextResult()) {
	var_dump($r->swishrank);
	var_dump($r->swishdocpath);
}

echo "=================\n";

$search->setSort("swishrank asc");
$rs = $search->execute();

while ($rs && $r = $rs->nextResult()) {
	var_dump($r->swishrank);
	var_dump($r->swishdocpath);
}

echo "=================\n";

$search->setSort("blah asc");

try {
	$rs = $search->execute();
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

$search->setSort();

echo "Done\n";
?>
--EXPECTF--	
int(1000)
string(%d) "%sREADME.TESTING"
int(899)
string(%d) "%sREADME.TESTING2"
int(610)
string(%d) "%sREADME.SUBMITTING_PATCH"
int(604)
string(%d) "%sREADME.QNX"
int(530)
string(%d) "%sREADME.WIN32-BUILD-SYSTEM"
int(308)
string(%d) "%sREADME.EXT_SKEL"
int(248)
string(%d) "%sREADME.CVS-RULES"
int(161)
string(%d) "%sREADME.Zeus"
int(161)
string(%d) "%sREADME.SELF-CONTAINED-EXTENSIONS"
=================
int(161)
string(%d) "%sREADME.Zeus"
int(161)
string(%d) "%sREADME.SELF-CONTAINED-EXTENSIONS"
int(248)
string(%d) "%sREADME.CVS-RULES"
int(308)
string(%d) "%sREADME.EXT_SKEL"
int(530)
string(%d) "%sREADME.WIN32-BUILD-SYSTEM"
int(604)
string(%d) "%sREADME.QNX"
int(610)
string(%d) "%sREADME.SUBMITTING_PATCH"
int(899)
string(%d) "%sREADME.TESTING2"
int(1000)
string(%d) "%sREADME.TESTING"
=================
string(%d) "Property 'blah' is not defined in index '%sindex.swish-e'"

Warning: SwishSearch::setSort() expects exactly 1 parameter, 0 given in %s on line %d
Done
