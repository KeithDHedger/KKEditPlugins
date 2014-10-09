PACKAGENAME=$(shell /usr/bin/basename ${PWD})
PLUGNAME=$(shell echo ${PACKAGENAME}|/usr/bin/tr A-Z a-z)
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
LOCALEDIR=${HOME}/.KKEdit/plugins/locale
MONAME=${PACKAGENAME}.mo

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -DLOCALEDIR=\"${LOCALEDIR}\" -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
	if [ ${USER} = "root" ];then \
		mkdir -vp "${ROOTINSTALLDIR}/${PACKAGENAME}" ||true; \
		cp -rv lib${PLUGNAME}.so bones ${ROOTINSTALLDIR}/${PACKAGENAME};cd po;cp -rv . ${LOCALEDIR}; \
	else \
		mkdir -vp "${USERINSTALLDIR}/${PACKAGENAME}" ||true; \
		cp -rv lib${PLUGNAME}.so bones ${USERINSTALLDIR}/${PACKAGENAME};cd po;cp -rv . ${LOCALEDIR}; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${PACKAGENAME}" ${LOCALEDIR}/*/LC_MESSAGES/${MONAME}; \
	else \
		rm -rv "${USERINSTALLDIR}/${PACKAGENAME}" ${LOCALEDIR}/*/LC_MESSAGES/${MONAME}; \
	fi

makepkg:
	cd .. && tar -cvzhf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
	