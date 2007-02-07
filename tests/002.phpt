--TEST--
Swish::prepare() tests
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
		var_dump($s = $h->prepare($query));
		var_dump($r = $s->execute());
		var_dump($r->hits);
		var_dump($r = $s->execute());
		var_dump($r->hits);
		var_dump($r = $s->execute($query." OR php"));
		var_dump($r->hits);
	} catch (SwishException $e) {
		var_dump($e->getMessage());
	}
}

var_dump($s = $h->prepare());
var_dump($s = $h->prepare("a", "b"));

echo "Done\n";
?>
--EXPECTF--	
object(SwishSearch)#%d (0) {
}
string(25) "No search words specified"
object(SwishSearch)#%d (0) {
}
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
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(16)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(3) {
        [0]=>
        string(4) "test"
        [1]=>
        string(2) "or"
        [2]=>
        string(3) "php"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(16)
object(SwishSearch)#%d (0) {
}
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
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(16)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(3) {
        [0]=>
        string(4) "data"
        [1]=>
        string(2) "or"
        [2]=>
        string(3) "php"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(16)
object(SwishSearch)#%d (0) {
}
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
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(16)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(3) {
        [0]=>
        string(7) "paramet"
        [1]=>
        string(2) "or"
        [2]=>
        string(3) "php"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(16)
object(SwishSearch)#%d (0) {
}
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
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(16)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(3) {
        [0]=>
        string(4) "ouch"
        [1]=>
        string(2) "or"
        [2]=>
        string(3) "php"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(16)
object(SwishSearch)#%d (0) {
}
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
object(SwishResults)#%d (2) {
  ["hits"]=>
  int(16)
  ["indexes"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(%d) "%sindex.swish-e"
      ["parsed_words"]=>
      array(3) {
        [0]=>
        string(4) "blah"
        [1]=>
        string(2) "or"
        [2]=>
        string(3) "php"
      }
      ["removed_stop_words"]=>
      array(0) {
      }
    }
  }
}
int(16)
object(SwishSearch)#%d (0) {
}

Warning: Swish::prepare() expects at most 1 parameter, 2 given in %s on line %d
NULL
Done
