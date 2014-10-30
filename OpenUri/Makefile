PACKAGENAME=$(shell /usr/bin/basename `pwd`)
PLUGNAME=$(shell echo ${PACKAGENAME}|/usr/bin/tr A-Z a-z)
MONAME=${PACKAGENAME}.mo

PREFIX=/usr
GLOBAL=0

ifeq (0,$(GLOBAL))
	LOCALEFOLDER=${HOME}/.KKEdit/plugins/locale
	INSTALLFOLDER=${HOME}/.KKEdit/plugins
else
	LOCALEFOLDER=$(PREFIX)/share/locale
	INSTALLFOLDER=$(PREFIX)/share/KKEdit/plugins
endif

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -DLOCALEDIR=\"${LOCALEFOLDER}\" -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
		mkdir -vp $(DESTDIR)${INSTALLFOLDER} $(DESTDIR)$(LOCALEFOLDER) || true
		cp -v lib${PLUGNAME}.so $(DESTDIR)$(INSTALLFOLDER)
		cd po;cp -rv . $(DESTDIR)$(LOCALEFOLDER)

uninstall:
	rm -rv $(DESTDIR)$(INSTALLFOLDER)/lib${PLUGNAME}.so $(DESTDIR)$(LOCALEFOLDER)/*/LC_MESSAGES/${MONAME}

makepkg: clean
	cd .. && tar -cvzhf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
