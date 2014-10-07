PACKAGENAME=$(shell /usr/bin/basename ${PWD})
PLUGNAME=$(shell echo ${PACKAGENAME}|/usr/bin/tr A-Z a-z)
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
ROOTLOCALEDIR=/usr/share/locale
LOCALLOCALE=${HOME}/.KKEdit/plugins/locale
MONAME=${PACKAGENAME}.mo

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
	if [ ${USER} = "root" ];then \
		cp -rv lib${PLUGNAME}.so "${ROOTINSTALLDIR}";cd po;cp -rv . ${ROOTLOCALEDIR}; \
	else \
		cp -rv lib${PLUGNAME}.so "${USERINSTALLDIR}";cd po;cp -rv . ${LOCALLOCALE}; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/lib${PLUGNAME}.so" ${ROOTLOCALEDIR}/*/LC_MESSAGES/${MONAME}; \
	else \
		rm -rv "${USERINSTALLDIR}/lib${PLUGNAME}.so" ${LOCALLOCALE}/*/LC_MESSAGES/${MONAME}; \
	fi

makepkg: clean
	cd .. && tar -cvhzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
