--TEST--
Swish::query() tests
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$h = new Swish(dirname(__FILE__)."/index.swish-e");

$a = array(
	'',
	'test',
	'data',
	'parameter',
	'ouch',
	'blah',
	);

foreach ($a as $query) {
	try {
		var_dump($r = $h->query($query));
		var_dump($r->hits);
		var_dump($r->indexes);
		$a = array();
		var_dump($r->$a);
	} catch (SwishException $e) {
		var_dump($e->getMessage());
	}
}

echo "Done\n";
?>
--EXPECTF--	
string(25) "No search words specified"
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(9)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(1) {
        [0]=>
        string(4) "test"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(9)
array(1) {
  [0]=>
  array(3) {
    ["name"]=>
    string(%d) "%sindex.swish-e"
    ["parsed_words"]=>
    array(1) {
      [0]=>
      string(4) "test"
    }
    ["removed_stop_words"]=>
    array(0) {
    }
  }
}

Notice: Array to string conversion in %s on line %d

Notice: Undefined property:  SwishResults::$Array in %s on line %d
NULL
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(5)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(1) {
        [0]=>
        string(4) "data"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(5)
array(1) {
  [0]=>
  array(3) {
    ["name"]=>
    string(%d) "%sindex.swish-e"
    ["parsed_words"]=>
    array(1) {
      [0]=>
      string(4) "data"
    }
    ["removed_stop_words"]=>
    array(0) {
    }
  }
}

Notice: Array to string conversion in %s on line %d

Notice: Undefined property:  SwishResults::$Array in %s on line %d
NULL
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(9)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(1) {
        [0]=>
        string(7) "paramet"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(9)
array(1) {
  [0]=>
  array(3) {
    ["name"]=>
    string(%d) "%sindex.swish-e"
    ["parsed_words"]=>
    array(1) {
      [0]=>
      string(7) "paramet"
    }
    ["removed_stop_words"]=>
    array(0) {
    }
  }
}

Notice: Array to string conversion in %s on line %d

Notice: Undefined property:  SwishResults::$Array in %s on line %d
NULL
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
      array(1) {
        [0]=>
        string(4) "ouch"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(0)
array(1) {
  [0]=>
  array(3) {
    ["name"]=>
    string(%d) "%sindex.swish-e"
    ["parsed_words"]=>
    array(1) {
      [0]=>
      string(4) "ouch"
    }
    ["removed_stop_words"]=>
    array(0) {
    }
  }
}

Notice: Array to string conversion in %s on line %d

Notice: Undefined property:  SwishResults::$Array in %s on line %d
NULL
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
      array(1) {
        [0]=>
        string(4) "blah"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(0)
array(1) {
  [0]=>
  array(3) {
    ["name"]=>
    string(%d) "%sindex.swish-e"
    ["parsed_words"]=>
    array(1) {
      [0]=>
      string(4) "blah"
    }
    ["removed_stop_words"]=>
    array(0) {
    }
  }
}

Notice: Array to string conversion in %s on line %d

Notice: Undefined property:  SwishResults::$Array in %s on line %d
NULL
Done
