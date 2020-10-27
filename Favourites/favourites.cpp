/*
 *
 * ©K. D. Hedger. Tue 31 Jul 12:08:41 BST 2018 keithdhedger@gmail.com

 * This file (newproject.cpp) is part of KKEditPlugins.

 * KKEditPlugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEditPlugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEditPlugins.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <libgen.h>
#include <string.h>
#include <ctype.h>

#include <libintl.h>
#include <locale.h>

#include "../common.h"
#ifdef _DEVMODE_
#include "../../KKEdit/gtk-kkedit/KKEdit/src/kkedit-plugins.h"
#else
#include <kkedit-plugins.h>
#endif

#define VERSION "0.3.0"
#define TEXTDOMAIN "FavouritesMenu"

GtkWidget	*favMenu;
GtkWidget	*favSubMenu;

char*		SVNRepoPath;
char*		projectsPath;
char		*currentdomain=NULL;

void setTextDomain(bool plugdomain,plugData* pdata)
{
	if(plugdomain==true)
		{
			//set domain to plug
			bindtextdomain(TEXTDOMAIN,LOCALEDIR);
			textdomain(TEXTDOMAIN);
			bind_textdomain_codeset(TEXTDOMAIN,"UTF-8");
		}
	else
		{
			//resetdomain
			bindtextdomain(currentdomain,pdata->locale);
			textdomain(currentdomain);
			bind_textdomain_codeset(currentdomain,"UTF-8");	
		}
}

int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	currentdomain=strdup(textdomain(NULL));
	return(NULL);
}

extern "C" void g_module_unload(GModule *module)
{
	return;
}

void openNewFile(GtkWidget *widg,gpointer filepath)
{
	openFile((const gchar*)gtk_widget_get_name(widg),0,false);
}

void addToFavourites(GtkWidget* widget,gpointer data)
{
	plugData	*plugdata=(plugData*)data;
	GtkWidget	*menuitem;
	char		*command;

	if(plugdata->page==NULL)
		return;
	setTextDomain(true,plugdata);

	menuitem=createNewStockMenuItem(GTK_STOCK_NEW,GTK_STOCK_NEW);
	gtk_widget_set_name(menuitem,plugdata->page->filePath);
	gtk_menu_item_set_label((GtkMenuItem*)menuitem,plugdata->page->fileName);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(openNewFile),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(favSubMenu),menuitem);
	gtk_widget_show_all(favMenu);

	asprintf(&command,"echo \"%s\" >> %s/favourites.rc",plugdata->page->filePath,plugdata->lPlugFolder);
	system(command);
	free(command);
	asprintf(&command,"sort -u %s/favourites.rc -o %s/favourites.rc &>/dev/null",plugdata->lPlugFolder,plugdata->lPlugFolder);
	system(command);
	free(command);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menuitem;
	char*		favfile;
	FILE*		fp=NULL;
	char		line[PATH_MAX];
	char		*slash=NULL;
	char*		retval=NULL;

	plugData*	plugdata=(plugData*)data;

	setTextDomain(true,plugdata);

	favMenu=gtk_menu_item_new_with_label(gettext("_Favourites"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)favMenu,true);
	favSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(favMenu),favSubMenu);

	menuitem=createNewStockMenuItem(GTK_STOCK_NEW,GTK_STOCK_NEW);
	gtk_widget_set_name(menuitem,"newfav");
	gtk_menu_item_set_label((GtkMenuItem*)menuitem,"Add Favourite");
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(addToFavourites),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(favSubMenu),menuitem);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(favSubMenu),menuitem);

	asprintf(&favfile,"%s/favourites.rc",plugdata->lPlugFolder);
	fp=fopen(favfile,"r");
	if(fp!=NULL)
		{
			do
				{
					line[0]=0;
					retval=fgets(line,PATH_MAX,fp);
					if(strlen(line)>0)
						{
							line[strlen(line)-1]=0;
							menuitem=createNewStockMenuItem(GTK_STOCK_NEW,GTK_STOCK_NEW);
							gtk_widget_set_name(menuitem,line);
							slash=strrchr(line,'/');
							gtk_menu_item_set_label((GtkMenuItem*)menuitem,++slash);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(openNewFile),NULL);
							gtk_menu_shell_append(GTK_MENU_SHELL(favSubMenu),menuitem);
						}
					}
			while(retval!=NULL);
 
			fclose(fp);
		}

	free(favfile);

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),favMenu);					
	gtk_widget_show_all(favMenu);
	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright ©2020 K.D.Hedger";
	char*			licence;
	GtkAboutDialog*	about;
	char*			translators;

	setTextDomain(true,plugdata);

	const char*		aboutboxstring=gettext("Favourites Menu");

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Favourites Menu Plugin"));
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,copyright);
	gtk_about_dialog_set_version(about,VERSION);
	gtk_about_dialog_set_website(about,KKEDITPAGE);
	gtk_about_dialog_set_website_label(about,"KKEdit Homepage");
	gtk_about_dialog_set_logo_icon_name(about,"KKEditPlug");
	gtk_about_dialog_set_license(about,licence);
	gtk_about_dialog_set_translator_credits(about,(const gchar*)translators);

	gtk_dialog_run(GTK_DIALOG(about));
	gtk_widget_destroy((GtkWidget*)about);
	free(licence);
	free(licencepath);
	free(translators);
	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(favMenu);
			gtk_widget_show_all(plugdata->mlist.menuBar);	
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	return(0);
}
