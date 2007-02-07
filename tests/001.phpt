--TEST--
Swish::__construct() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$indexes = array(
	__FILE__,
	__FILE__."t",
	dirname(__FILE__)."/index.swish-e",
	"",
	"nonexistent_file"
	);

$tmp_file = __FILE__.".prop";
touch($tmp_file); 

foreach($indexes as $file) {
	try {
		$h = new Swish($file);
		var_dump($h);
	} catch (SwishException $e) {
		var_dump($e->getMessage());
	}
}

unlink($tmp_file);

try {
	$h = new Swish();
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

try {
	$h = new Swish("a", "b");
} catch (SwishException $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--	
string(%d) "File "%s001.php" has an unknown format."
string(%d) "Couldn't open the property file "%s001.phpt.prop": No such file or directory"
object(Swish)#%d (1) {
  ["indexes"]=>
  array(1) {
    [0]=>
    array(2) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["headers"]=>
      array(24) {
        ["Name"]=>
        string(0) ""
        ["Saved as"]=>
        string(13) "index.swish-e"
        ["Total Words"]=>
        int(1888)
        ["Total Files"]=>
        int(16)
        ["Removed Files"]=>
        int(0)
        ["Total Word Pos"]=>
        int(17662)
        ["Removed Word Pos"]=>
        int(0)
        ["Indexed on"]=>
        string(%d) "%s"
        ["Description"]=>
        string(0) ""
        ["Pointer"]=>
        string(0) ""
        ["Maintained by"]=>
        string(0) ""
        ["MinWordLimit"]=>
        int(1)
        ["MaxWordLimit"]=>
        int(40)
        ["WordCharacters"]=>
        string(101) "0123456789abcdefghijklmnopqrstuvwxyz%s"
        ["BeginCharacters"]=>
        string(101) "0123456789abcdefghijklmnopqrstuvwxyz%s"
        ["EndCharacters"]=>
        string(101) "0123456789abcdefghijklmnopqrstuvwxyz%s"
        ["IgnoreFirstChar"]=>
        string(0) ""
        ["IgnoreLastChar"]=>
        string(0) ""
        ["StopWords"]=>
        array(0) {
        }
        ["BuzzWords"]=>
        array(0) {
        }
        ["Stemming Applied"]=>
        bool(true)
        ["Soundex Applied"]=>
        bool(false)
        ["Fuzzy Mode"]=>
        string(%d) "%s"
        ["IgnoreTotalWordCountWhenRanking"]=>
        bool(false)
      }
    }
  }
}
string(22) "No index file supplied"
string(75) "Could not open the index file 'nonexistent_file': No such file or directory"

Warning: Swish::__construct() expects exactly 1 parameter, 0 given in %s on line %d

Warning: Swish::__construct() expects exactly 1 parameter, 2 given in %s on line %d
Done
