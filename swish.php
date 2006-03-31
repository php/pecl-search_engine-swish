<?php
if(!extension_loaded('swish')) {
	dl('swish.' . PHP_SHLIB_SUFFIX);
}

$low = 0;
$high = 80;

$h = new SwishHandle("index.swish-e");

$s = $h->prepare();
#$s->setSearchLimit('sale', $low * 100, $high * 100);
$s->setSort('swishrank desc');

$results = $s->execute("for");

echo "There are ", $results->getHits(), " hits\n";
while ($r = $results->nextResult()) {
	printf("%s %d %s\n",
		$r->property("swishdocpath"),
		$r->property("swishrank"),
		$r->property("name")
		);
};

echo "Done\n";

?>
