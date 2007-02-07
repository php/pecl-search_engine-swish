<?php

$h = new Swish("index.swish-e");

$s = $h->prepare();
$s->setSort('swishrank desc');

$results = $s->execute("for");

echo "There are ", $results->hits, " hits\n";
while ($r = $results->nextResult()) {
	printf("%s %d %s\n",
		$r->swishdocpath,
		$r->swishrank,
		$r->name
		);
};

echo "Done\n";

?>
