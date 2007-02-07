--TEST--
SwishResults::nextResult() and SwishResults::seekResult() basic tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$results = $h->query("I_really_doubt_this_can_be_found");
var_dump($results);

try {
	var_dump($results->seekResult());
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($results->seekResult(-1));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($results->seekResult(0));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($results->seekResult(1000));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($results->nextResult(1000));
} catch (SwishException $e) {
	var_dump($e->getMessage());
}
try {
	var_dump($results->nextResult());
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--	
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(0)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(7) {
        [0]=>
        string(1) "i"
        [1]=>
        string(6) "realli"
        [2]=>
        string(5) "doubt"
        [3]=>
        string(3) "thi"
        [4]=>
        string(3) "can"
        [5]=>
        string(2) "be"
        [6]=>
        string(5) "found"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}

Warning: SwishResults::seekResult() expects exactly 1 parameter, 0 given in %s on line %d
NULL
string(33) "position cannot be less than zero"
string(15) "No more results"
string(15) "No more results"

Warning: SwishResults::nextResult() expects exactly 0 parameters, 1 given in %s on line %d
NULL
bool(false)
Done
