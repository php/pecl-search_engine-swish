dnl $Id$
dnl vim:ts=2:sw=2:et:

PHP_ARG_WITH(swish, for Swish-e support,
[  --with-swish=PATH      Include Swish-e support. PATH is path to swish-config])

if test "$PHP_SWISH" != "no"; then
  
  if test "$PHP_SWISH" = "yes"; then
    SWISH_CONFIG=`$php_shtool path swish_config`

    if test -z "$SWISH_CONFIG"; then 
        for i in /usr/local /usr; do
            if test -f $i/bin/swish-config; then
              PHP_SWISH=$i;
              SWISH_CONFIG=$i/bin/swish-config;
              break;
            fi
        done
    fi

  else

    for i in "$PHP_SWISH" "$PHP_SWISH/swish-config" "$PHP_SWISH/bin/swish-config" ; do
      if test -f "$i" && test -x "$i"; then
        SWISH_CONFIG="$i"
        break;
      fi
    done
  fi

  if test -z "$SWISH_CONFIG" || ! test -f "$SWISH_CONFIG" || ! test -x "$SWISH_CONFIG"; then 
    AC_MSG_ERROR([swish-config not found])
  fi

  SWISH_INC=`"$SWISH_CONFIG" --cflags`
  PHP_EVAL_INCLINE($SWISH_INC)

  SWISH_LIBS=`"$SWISH_CONFIG" --libs`
  PHP_EVAL_LIBLINE($SWISH_LIBS, SWISH_SHARED_LIBADD)

  old_CFLAGS=$CFLAGS
  CFLAGS="$SWISH_INC"
  AC_CACHE_CHECK(for stemmer constants, ac_cv_stemmer_constants,
    AC_TRY_COMPILE([
#include <stdio.h>
#include <swish-e.h>
    ],[
       int i = STEM_OK;
    ],[
       ac_cv_stemmer_constants=yes
    ],[
       ac_cv_stemmer_constants=no
    ])
  )
  CFLAGS=$old_CFLAGS

  if test "$ac_cv_stemmer_constants" = "yes"; then
    AC_DEFINE(HAVE_STEMMER_CONSTANTS, 1, [Whether stemmer constants are exported])
  fi

  PHP_SUBST(SWISH_SHARED_LIBADD)
  PHP_NEW_EXTENSION(swish, swish.c, $ext_shared)
fi
