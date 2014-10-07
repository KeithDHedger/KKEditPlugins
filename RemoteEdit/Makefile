PACKAGENAME=$(shell /usr/bin/basename ${PWD})
PLUGNAME=$(shell echo ${PACKAGENAME}|/usr/bin/tr A-Z a-z)
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
ROOTLOCALEDIR=/usr/share/locale
LOCALLOCALE=${HOME}/.KKEdit/plugins/locale
MONAME=${PACKAGENAME}.mo
SETSID=setsid
ASKPASS=askpass

all:
	g++ -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	g++ -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc
	g++ -fPIC ${ASKPASS}.cpp `pkg-config --cflags --libs gtk+-2.0` -o ${ASKPASS}
	g++ -fPIC -Wall ${SETSID}.cpp -o ${SETSID}
	strip ${ASKPASS} ${SETSID} 

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true
	rm askpass||true
	rm setsid||true

install:
	if [ ${USER} = "root" ];then \
		mkdir -vp "${ROOTINSTALLDIR}/${PACKAGENAME}" ||true; \
		cp -rv lib${PLUGNAME}.so ${ASKPASS} ${SETSID} ${ROOTINSTALLDIR}/${PACKAGENAME};cd po;cp -rv . ${ROOTLOCALEDIR}; \
	else \
		mkdir -vp "${USERINSTALLDIR}/${PACKAGENAME}" ||true; \
		cp -rv lib${PLUGNAME}.so ${ASKPASS} ${SETSID} ${USERINSTALLDIR}/${PACKAGENAME};cd po;cp -rv . ${LOCALLOCALE}; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${PACKAGENAME}" ${ROOTLOCALEDIR}/*/LC_MESSAGES/${MONAME}; \
	else \
		rm -rv "${USERINSTALLDIR}/${PACKAGENAME}" ${LOCALLOCALE}/*/LC_MESSAGES/${MONAME}; \
	fi

makepkg: clean
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
