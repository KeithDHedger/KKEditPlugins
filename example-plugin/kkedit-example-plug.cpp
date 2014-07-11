/*
  plugs/kkedit-example-plug.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>

#include "kkedit-plugins.h"
#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "0.0.6"

GtkWidget*	examplemenu;
int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	perror("doin init");
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	perror("doin cleanup");
	return(NULL);
}

void openPlugHelp(GtkWidget* widget,gpointer data)
{
	plugData*	pdata=(plugData*)data;
	asprintf(pdata->thePage,"file://%s/example-plugin/plughelp.html",(char*)pdata->gPlugFolder);
	showDocView(USEURI,(char*)"KKEdit Plugin Help",(char*)"KKEdit Plugin Help");
}

extern "C" int addMenus(gpointer data)
{
	printf("adding  plug menus from example-plugin\n");
	GtkWidget*		menu;
	plugData*		plugdata=(plugData*)data;

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuHelp);
	examplemenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,NULL);
	gtk_menu_item_set_label((GtkMenuItem*)examplemenu,"Plugin Help");
	gtk_signal_connect(GTK_OBJECT(examplemenu),"activate",G_CALLBACK(openPlugHelp),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),examplemenu);

	printf("done adding  plug menus\n");
	return(0);
}

extern "C" int setSensitive(gpointer data)
{
	printf("set sensitive\n");
	return(0);
}

extern "C" int openFile(gpointer data)
{
	printf("open file \n");
	return(0);
}

extern "C" int saveFile(gpointer data)
{
	printf("save file\n");
	return(0);
}

extern "C" int newFile(gpointer data)
{
	printf("new file\n");
	return(0);
}

extern "C" int closeFile(gpointer data)
{
	printf("close file\n");
	return(0);
}

extern "C" int newTab(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	printf("doing new tab\n");

	if(plugdata->page->filePath!=NULL)
		printf("filePath = %s\n",plugdata->page->filePath);

	if(plugdata->page->fileName!=NULL)
		printf("fileName = %s\n",plugdata->page->fileName);

	if(plugdata->page->dirName!=NULL)
		printf("dirName = %s\n",plugdata->page->dirName);
		
	printf("finished new tab\n");
	return(0);
}

extern "C" int switchTab(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	printf("switching tab\n");
	printf("new tab is %i\n",plugdata->currentTab);
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
	printf("doing plugPrefs from example-plugin ...\n");
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*	plugdata=(plugData*)data;
	char*		licencepath;
	const char	copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*	aboutboxstring="KKEdit Project Plugin";
	char*		licence;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	g_file_get_contents(licencepath,&licence,NULL,NULL);

	gtk_show_about_dialog(NULL,"authors",authors,"comments",aboutboxstring,"copyright",copyright,"version",VERSION,"website",MYWEBSITE,"program-name","KKEdit Example Plugin","logo-icon-name","KKEdit","license",licence,NULL);

	free(licence);
	free(licencepath);
	return(0);
}

//return 0 if its safe to unload module
extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->plugData->unload==true)
		{
			gtk_widget_destroy(examplemenu);
			gtk_widget_show_all(plugdata->mlist.menuBar);	
		}
	else
		{
//when calling a 'standard' function like 'addMenus' from within the plugin itself get the actual symbol as below
			if(g_module_symbol(plugdata->plugData->module,"addMenus",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	printf("doing can enable from example-plug...\n");
	return(0);

}
