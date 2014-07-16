PLUGNAME=newproject
FILES=lib${PLUGNAME}.so bones
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins

all:
	gcc -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	gcc -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true

install:
	if [ ${USER} = "root" ];then \
		mkdir -p "${ROOTINSTALLDIR}/newproject"; \
		cp -rv ${FILES} "${ROOTINSTALLDIR}/newproject"; \
	else \
		mkdir -p "${USERINSTALLDIR}/newproject"; \
		cp -rv ${FILES} "${USERINSTALLDIR}/newproject"; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/newproject"; \
	else \
		rm -rv "${USERINSTALLDIR}/newproject"; \
	fi
