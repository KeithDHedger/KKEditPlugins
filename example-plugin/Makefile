PLUGNAME=kkedit-example-plug
KKEDITHASH="8fc2855319664347d10b00a865697d12"

all:
	gcc -DKKEDITHASH=\"${KKEDITHASH}\" -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:

uninstall:

check:
	echo "OK"