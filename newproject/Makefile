PLUGNAME=newproject
PACKAGENAME=$PLUGNAME
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
		mkdir -vp "${ROOTINSTALLDIR}/${PLUGNAME}"; \
		cp -rv ${FILES} "${ROOTINSTALLDIR}/${PLUGNAME}"; \
	else \
		mkdir -vp "${USERINSTALLDIR}/${PLUGNAME}"; \
		cp -rv ${FILES} "${USERINSTALLDIR}/${PLUGNAME}"; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${PLUGNAME}"; \
	else \
		rm -rv "${USERINSTALLDIR}/${PLUGNAME}"; \
	fi

makepkg:
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
	