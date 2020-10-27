/*
 *
 * ©K. D. Hedger. Mon 28 Mar 16:34:26 BST 2016 kdhedger68713@gmail.com

 * This file (common.h) is part of KKEditPlugins.

 * Projects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * Projects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Projects.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _COMMON_
#define _COMMON_

#ifdef _USEGTK3_

#define GTK_STOCK_APPLY gettext("Apply")
#define GTK_STOCK_CANCEL gettext("Cancel")
#define GTK_STOCK_NEW gettext("New")
#define GTK_STOCK_HELP gettext("Help")
#define GTK_STOCK_OPEN gettext("Open")
#define GTK_STOCK_CONNECT gettext("Connect")
#define GTK_STOCK_OK gettext("OK")

#define GDK_C GDK_KEY_C
#define GDK_V GDK_KEY_V

#endif

#define MYEMAIL "keithdhedger@gmail.com"
#define MYWEBSITE "http://keithdhedger.github.io/"
#define GLOBALWEBSITE "http://keithdhedger.github.io"
#define ASPELLPAGE GLOBALWEBSITE "/pages/aspellgui/aspelgui.html"
#define MANPAGEPAGE GLOBALWEBSITE "/pages/manpageeditor/help.html"
#define CLIPVIEW GLOBALWEBSITE "/pages/clipviewer/clip.html"
#define THEMEMANAGER GLOBALWEBSITE "/pages/apps.html#themeed"
#define COMPMANAGER GLOBALWEBSITE "/pages/apps.html#xfcecomp"
#define KKEDITPAGE "http://keithdhedger.github.io/KKEdit/"
#define KKEDITPAGEFR "http://keithdhedger.github.io/KKEdit/help.fr.html"
#define KKTERMINALPAGE GLOBALWEBSITE "/pages/kkterminal/kkterminal.html"

#define COPYRITE "Copyright © 2013-2020 K.D.Hedger"

enum {NEWVBOX=0,NEWHBOX};

const char	*moreapps[]={
								"\nMore by the same author\n",
								"KKEdit\nhttp://keithdhedger.github.io/KKEdit/\n",
								"KKTerminal\nhttp://keithdhedger.github.io/pages/kkterminal/kkterminal.html\n",
								"KKFileManager\nhttp://github.com/KeithDHedger/KKFileManager\n",
								"XDecorations\nhttp://keithdhedger.github.io/pages/xdecorations/xdecorations.html\n",
								"Xfce-Theme-Manager\nhttp://keithdhedger.github.io/pages/apps.html#themeed\n",
								"Xfce4-Composite-Editor\nhttp://keithdhedger.github.io/pages/apps.html#xfcecomp\n",
								"Manpage Editor\nhttp://keithdhedger.github.io/pages/manpageeditor/manpageeditor.html\n",
								"GtkSu\nhttp://keithdhedger.github.io/pages/gtksu/gtksu.html\n",
								"ASpell GUI\nhttp://keithdhedger.github.io/pages/aspellgui/aspelgui.html\n",
								"Clipboard Viewer\nhttp://keithdhedger.github.io/pages/clipviewer/clip.html",
								"\nDevelopment versions can be found here:\nhttp://github.com/KeithDHedger"
							};

const char	*authors[]={"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,moreapps[0],moreapps[1],moreapps[2],moreapps[3],moreapps[4],moreapps[5],moreapps[6],moreapps[7],moreapps[8],moreapps[9],NULL};

GtkWidget* createNewBox(int orient,bool homog,int spacing)
{
	GtkWidget	*retwidg=NULL;

#ifdef _USEGTK3_
	if(orient==NEWVBOX)
		retwidg=gtk_box_new(GTK_ORIENTATION_VERTICAL,spacing);
	else
		retwidg=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,spacing);
	gtk_box_set_homogeneous((GtkBox*)retwidg,homog);
#else
	if(orient==NEWVBOX)
		retwidg=gtk_vbox_new(homog,spacing);
	else
		retwidg=gtk_hbox_new(homog,spacing);
#endif

	return(retwidg);
}

GtkWidget* createNewStockMenuItem(const char* stock,const char* label)
{
	GtkWidget*	item;

#ifdef _USEGTK3_
	item=gtk_menu_item_new_with_mnemonic(label);
#else
	item=gtk_image_menu_item_new_from_stock(stock,NULL);
#endif

	return(item);
}

GtkWidget* createNewImageMenuItem(const char* stock,const char* label)
{
	GtkWidget	*item;

#ifdef _USEGTK3_
	item=gtk_menu_item_new_with_label(label);
#else
	GtkWidget	*image;
	item=gtk_image_menu_item_new_with_label(label);
	image=gtk_image_new_from_stock(stock,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)item,image);
#endif

	return(item);
}

#endif
