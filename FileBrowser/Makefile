PLUGNAME=filebrowser
PACKAGENAME=FileBrowser
FILES=lib${PLUGNAME}.so
USERINSTALLDIR=${HOME}/.KKEdit/plugins
ROOTINSTALLDIR=/usr/share/KKEdit/plugins

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
	if [ ${USER} = "root" ];then \
		cp -rv "${FILES}" "${ROOTINSTALLDIR}"; \
	else \
		cp -rv "${FILES}" "${USERINSTALLDIR}"; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${FILES}"; \
	else \
		rm -rv "${USERINSTALLDIR}/${FILES}"; \
	fi

makepkg:
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
	