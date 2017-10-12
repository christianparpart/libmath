#! /bin/sh

if [ "$1" = "clean" ] || [ "$1" = "-clean" ] || [ "$1" = "--clean" ]; then
  rm -f Makefile aclocal.m4 config.cache config.guess config.h config.h.in config.log config.status config.sub configure install-sh libtool ltconfig ltmain.sh missing mkinstalldirs stamp-h stamp-h.in depcomp stamp-h1 *~
  find . \( -name '*.o' -o -name '*.lo' -o -name 'Makefile.in' -o -name 'Makefile' -o -name '.libs' -o -name '.deps' \) -exec rm -rf {} \; &> /dev/null
  rm -rf autom4te*cache
  exit 0
fi

export WANT_AUTOMAKE="1.7"
export WANT_AUTOCONF_2_5=1

ACINCLUDES="-I ."
[ -d /opt/surakware/share/aclocal ] && ACINCLUDES="${ACINCLUDES} -I /opt/surakware/share/aclocal"

aclocal ${ACINCLUDES} &&
autoheader &&
autoconf &&
libtoolize --automake &&
automake -ac || exit 1

echo "Don't forget the standard procudure:"
echo "example: ./configure --prefix=/usr && make && make install"
