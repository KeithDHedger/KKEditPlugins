PLUGNAME=remoteedit
FILES=lib${PLUGNAME}.so
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
PACKAGENAME=RemoteEdit

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc
	gcc -fPIC askpass.cpp `pkg-config --cflags --libs gtk+-2.0` -o askpass

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true
	rm askpass||true

install:
	if [ ${USER} = "root" ];then \
		cp -rv askpass "${FILES}" "${ROOTINSTALLDIR}"; \
	else \
		cp -rv askpass "${FILES}" "${USERINSTALLDIR}"; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${FILES}" "${ROOTINSTALLDIR}/askpass"; \
	else \
		rm -rv "${USERINSTALLDIR}/${FILES}" "${ROOTINSTALLDIR}/askpass; \
	fi

makepkg: clean
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
