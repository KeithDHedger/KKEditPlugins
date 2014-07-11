/*
  plugs/newproject.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>

#include "kkedit-plugins.h"

GtkWidget*		menuProjects;
GModule*		thisModule;
int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	thisModule=module;
	perror("doin ininit form newproject");
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	thisModule=NULL;
	printf("doin cleanup form newproject\n");
//	gtk_widget_destroy(menuProjects);
//	menuProjects
	printf("finished cleanup form newproject\n");
	return(NULL);
}

void newProject(GtkWidget* widget,gpointer data)
{
}

extern "C" int addMenus(gpointer data)
{
	printf("adding  plug menus\n");
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	plugData*		plugdata=(plugData*)data;

	menuProjects=gtk_menu_item_new_with_label("_Projects");
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuProjects,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuProjects),menu);

	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
	gtk_menu_item_set_label((GtkMenuItem*)menuitem,"New Project");
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newProject),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuProjects);

	printf("done adding  plug menus from newproject XXXX\n");
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
	printf("doing plugPrefs ...\n");
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	printf("doing about ...\n");
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->plugData->unload==true)
		{
			gtk_widget_destroy(menuProjects);
			gtk_widget_show_all(plugdata->mlist.menuBar);	
		}
	else
		{
			if(g_module_symbol(thisModule,"addMenus",(gpointer*)&module_plug_function))
				module_plug_function(data);
				printf("adding  plug menus from newproject\n");
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	printf("doing can enable from newproject...\n");
	return(0);

}
