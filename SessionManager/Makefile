PLUGNAME=sessionmanager
LIBPLUG=lib${PLUGNAME}.so
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
PACKAGENAME=`basename ${PWD}`

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o ${LIBPLUG} ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
	if [ ${USER} = "root" ];then \
		cp -rv "${LIBPLUG}" "${ROOTINSTALLDIR}"; \
	else \
		cp -rv "${LIBPLUG}" "${USERINSTALLDIR}"; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${LIBPLUG}"; \
	else \
		rm -rv "${USERINSTALLDIR}/${LIBPLUG}"; \
	fi

makepkg: clean
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
