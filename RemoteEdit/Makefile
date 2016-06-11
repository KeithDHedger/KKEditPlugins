PACKAGENAME=$(shell /usr/bin/basename `pwd`)
PLUGNAME=$(shell echo ${PACKAGENAME}|/usr/bin/tr A-Z a-z)
MONAME=${PACKAGENAME}.mo
SETSID=setsid
ASKPASS=askpass
PLATFORM=gtk
GTKVERSION?=2
EXTRAMAKEFLAGS?=
GLOBAL?=0
DEVELOP?=0

PREFIX?=/usr

ifeq (3,$(GTKVERSION))
INSTALLSUFFIX=3
endif

ifeq (0,$(GLOBAL))
	LOCALEFOLDER=${HOME}/.KKEdit${INSTALLSUFFIX}/plugins-${PLATFORM}/locale
	INSTALLFOLDER=${HOME}/.KKEdit${INSTALLSUFFIX}/plugins-${PLATFORM}
else
	LOCALEFOLDER=$(PREFIX)/share/locale
	INSTALLFOLDER=$(PREFIX)/share/KKEdit${INSTALLSUFFIX}/plugins-${PLATFORM}
endif

ifeq (1,$(DEVELOP))
	DEVMODE=-D_DEVMODE_
endif

ifeq (3,$(GTKVERSION))
	USEGTK3=-D_USEGTK3_
endif


all:
	gcc ${EXTRAMAKEFLAGS} -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs ${PLATFORM}+-${GTKVERSION}.0 gmodule-2.0 glib-2.0 gtksourceview-${GTKVERSION}.0` -DLOCALEDIR=\"${LOCALEFOLDER}\" $(DEVMODE) $(USEGTK3) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

#	g++ ${EXTRAMAKEFLAGS} -fPIC -Wall -g -c ${ASKPASS}.cpp `pkg-config --cflags --libs ${PLATFORM}+-${GTKVERSION}.0 gtksourceview-${GTKVERSION}.0` -DLOCALEDIR=\"${LOCALEFOLDER}\" $(DEVMODE) $(USEGTK3) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o ${ASKPASS}
	gcc ${EXTRAMAKEFLAGS} -fPIC -Wall ${ASKPASS}.cpp `pkg-config --cflags --libs ${PLATFORM}+-${GTKVERSION}.0 gtksourceview-${GTKVERSION}.0` -DLOCALEDIR=\"${LOCALEFOLDER}\" $(DEVMODE) $(USEGTK3) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -o ${ASKPASS}

	g++ ${EXTRAMAKEFLAGS} -fPIC -Wall ${SETSID}.cpp -o ${SETSID}
#	strip ${ASKPASS} ${SETSID} 

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
