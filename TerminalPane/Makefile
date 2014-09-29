PLUGNAME=terminalpane
FILES=lib${PLUGNAME}.so
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
PACKAGENAME=`basename ${PWD}`

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 gtksourceview-2.0 vte` -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc -lvte

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

makepkg: clean
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}