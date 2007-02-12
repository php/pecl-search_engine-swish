--TEST--
swish constants
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

var_dump(Swish::META_TYPE_UNDEF);
var_dump(Swish::META_TYPE_STRING);
var_dump(Swish::META_TYPE_ULONG);
var_dump(Swish::META_TYPE_DATE);

var_dump(Swish::IN_FILE_BIT);
var_dump(Swish::IN_TITLE_BIT);
var_dump(Swish::IN_HEAD_BIT);
var_dump(Swish::IN_BODY_BIT);
var_dump(Swish::IN_COMMENTS_BIT);
var_dump(Swish::IN_HEADER_BIT);
var_dump(Swish::IN_EMPHASIZED_BIT);
var_dump(Swish::IN_META_BIT);

var_dump(Swish::IN_FILE);
var_dump(Swish::IN_TITLE);
var_dump(Swish::IN_HEAD);
var_dump(Swish::IN_BODY);
var_dump(Swish::IN_COMMENTS);
var_dump(Swish::IN_HEADER);
var_dump(Swish::IN_EMPHASIZED);
var_dump(Swish::IN_META);

echo "Done\n";
?>
--EXPECTF--	
int(0)
int(4)
int(8)
int(16)
int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(1)
int(2)
int(4)
int(8)
int(16)
int(32)
int(64)
int(128)
Done
