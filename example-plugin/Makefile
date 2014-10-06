PLUGNAME=kkedit-example-plug
PACKAGENAME=${PLUGNAME}
FILES=lib${PLUGNAME}.so
USERINSTALLDIR=${HOME}/.KKEdit/plugins
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
FOLDER=example-plugin
ROOTLOCALEDIR=/usr/share/locale
LOCALLOCALE=${HOME}/.KKEdit/plugins/locale
MONAME=example-plugin.mo

all:
	gcc -DKKEDITHASH=\"${KKEDITHASH}\" -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
	if [ ${USER} = "root" ];then \
		cp -rv "${FILES}" "${ROOTINSTALLDIR}";cd po;cp -rv . ${ROOTLOCALEDIR}; \
	else \
		cp -rv "${FILES}" "${USERINSTALLDIR}";cd po;cp -rv . ${LOCALLOCALE}; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${FILES}" ${ROOTLOCALEDIR}/*/LC_MESSAGES/${MONAME}; \
	else \
		rm -rv "${USERINSTALLDIR}/${FILES}" ${LOCALLOCALE}/*/LC_MESSAGES/${MONAME}; \
	fi

makepkg:
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${FOLDER}
