PLUGNAME=clipboardplugin
LIBPLUG=lib${PLUGNAME}.so
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
PACKAGENAME=`basename ${PWD}`
ROOTLOCALEDIR=/usr/share/locale
LOCALLOCALE=${HOME}/.KKEdit/plugins/locale
MONAME=ClipboardPlugin.mo

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o ${LIBPLUG} ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
	if [ ${USER} = "root" ];then \
		cp -rv "${LIBPLUG}" "${ROOTINSTALLDIR}";cd po;cp -rv . ${ROOTLOCALEDIR}; \
	else \
		cp -rv "${LIBPLUG}" "${USERINSTALLDIR}";cd po;cp -rv . ${LOCALLOCALE}; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${LIBPLUG}" ${ROOTLOCALEDIR}/*/LC_MESSAGES/${MONAME}; \
	else \
		rm -rv "${USERINSTALLDIR}/${LIBPLUG}" ${LOCALLOCALE}/*/LC_MESSAGES/${MONAME}; \
	fi

makepkg: clean
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
