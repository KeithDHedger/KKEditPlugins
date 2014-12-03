PACKAGENAME=$(shell /usr/bin/basename `pwd`)
PLUGNAME=$(shell echo ${PACKAGENAME}|/usr/bin/tr A-Z a-z)
MONAME=${PACKAGENAME}.mo
SETSID=setsid
ASKPASS=askpass
PLATFORM=gtk

PREFIX=/usr
GLOBAL=0

ifeq (0,$(GLOBAL))
	LOCALEFOLDER=${HOME}/.KKEdit/plugins-${PLATFORM}/locale
	INSTALLFOLDER=${HOME}/.KKEdit/plugins-${PLATFORM}
else
	LOCALEFOLDER=$(PREFIX)/share/locale
	INSTALLFOLDER=$(PREFIX)/share/KKEdit/plugins-${PLATFORM}
endif

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -DLOCALEDIR=\"${LOCALEFOLDER}\" -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc
	g++ -fPIC ${ASKPASS}.cpp `pkg-config --cflags --libs gtk+-2.0` -DLOCALEDIR=\"${LOCALEDIR}\" -o ${ASKPASS}
	g++ -fPIC -Wall ${SETSID}.cpp -o ${SETSID}
	strip ${ASKPASS} ${SETSID} 

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true
	rm askpass||true
	rm setsid||true

install:
		mkdir -vp $(DESTDIR)${INSTALLFOLDER}/$(PACKAGENAME) $(DESTDIR)$(LOCALEFOLDER) || true
		cp -v lib${PLUGNAME}.so ${ASKPASS} ${SETSID} $(DESTDIR)$(INSTALLFOLDER)/$(PACKAGENAME)
		cd po;cp -rv . $(DESTDIR)$(LOCALEFOLDER)

uninstall:
	rm -rv $(DESTDIR)$(INSTALLFOLDER)/$(PACKAGENAME) $(DESTDIR)$(LOCALEFOLDER)/*/LC_MESSAGES/${MONAME}

makepkg: clean
	cd .. && tar -cvzhf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
