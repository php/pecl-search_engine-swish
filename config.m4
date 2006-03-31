dnl $Id$
dnl vim:ts=2:sw=2:et:

PHP_ARG_WITH(swish, for swish support,
[  --with-swish=PATH      Include swish-e support])

if test "$PHP_SWISH" != "no"; then
  AC_MSG_CHECKING([for swish-e.h])
  if test -f $PHP_SWISH/include/swish-e.h ; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([didn't find $PHP_SWISH/include/swish-e.h])
  fi
  AC_MSG_CHECKING([for libswish-e.a])
  if test -f $PHP_SWISH/lib/libswish-e.a ; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([you need to install a swish-e distribution])
  fi
  PHP_ADD_INCLUDE($PHP_SWISH/include)
  PHP_ADD_LIBRARY_WITH_PATH(swish-e, $PHP_SWISH/lib, SWISH_SHARED_LIBADD)
  PHP_SUBST(SWISH_SHARED_LIBADD)
  PHP_NEW_EXTENSION(swish, swish.c, $ext_shared)
fi
