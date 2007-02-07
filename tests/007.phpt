--TEST--
Swish handle properties test
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

var_dump($h);
var_dump($h->indexes);
var_dump($h->test);
$a = array();
var_dump($h->$a);

echo "Done\n";
?>
--EXPECTF--	
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

Notice: Undefined property:  Swish::$test in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Notice: Undefined property:  Swish::$Array in %s on line %d
NULL
Done
