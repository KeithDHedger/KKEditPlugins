PLUGNAME=remoteedit
PLUG=lib${PLUGNAME}.so
SETSID=setsid
ASKPASS=askpass
ROOTINSTALLDIR=/usr/share/KKEdit/plugins
USERINSTALLDIR=${HOME}/.KKEdit/plugins
PACKAGENAME=RemoteEdit

all:
	g++ -fPIC -Wall -g -c ${PLUGNAME}.cpp `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0 glib-2.0 gtksourceview-2.0` -o ${PLUGNAME}.o
	g++ -shared -Wl,--version-script=exportmap,-soname,${PLUGNAME}.so -o lib${PLUGNAME}.so ${PLUGNAME}.o -lc
	g++ -fPIC askpass.cpp `pkg-config --cflags --libs gtk+-2.0` -o askpass
	g++ -fPIC -Wall setsid.cpp -o setsid
	strip ${ASKPASS} ${SETSID} 

clean: distclean

distclean:
	rm lib${PLUGNAME}.so||true
	rm ${PLUGNAME}.o||true
	rm askpass||true
	rm setsid||true

install:
	if [ ${USER} = "root" ];then \
		cp -rv ${ASKPASS} ${SETSID} ${PLUG} "${ROOTINSTALLDIR}"; \
	else \
		cp -rv ${ASKPASS} ${SETSID} ${PLUG} "${USERINSTALLDIR}"; \
	fi

uninstall:
	if [ ${USER} = "root" ];then \
		rm -rv "${ROOTINSTALLDIR}/${PLUG}" "${ROOTINSTALLDIR}/${ASKPASS}" "${ROOTINSTALLDIR}/${SETSID}"||true; \
	else \
		rm -rv "${USERINSTALLDIR}/${PLUG}" "${USERINSTALLDIR}/${ASKPASS}" "${USERINSTALLDIR}/${SETSID}"||true; \
	fi

makepkg: clean
	cd .. && tar -cvzf ${PACKAGENAME}.tar.gz ${PACKAGENAME}
