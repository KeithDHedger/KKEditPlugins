/*
 *
 * ©K. D. Hedger. Tue  6 Oct 12:23:01 BST 2015 kdhedger68713@gmail.com

 * This file (openuri.cpp) is part of KKEditPlugins.

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


#include <stdlib.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <string.h>

#include <libintl.h>
#include <locale.h>

#include "../common.h"
#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://kkedit.darktech.org"
#define VERSION "0.3.0"
#define TEXTDOMAIN "OpenUri"

GtkWidget*	menuPlug;
char*		currentdomain=NULL;

int	(*module_plug_function)(gpointer globaldata);

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

extern "C" const gchar* g_module_check_init(GModule *module)
{
	currentdomain=strdup(textdomain(NULL));
	return(NULL);
}

extern "C" void g_module_unload(GModule *module)
{
	return;
}

void theCallBack(GtkWidget* widget,gpointer data)
{
	char*		command=NULL;
	GtkTextIter	start;
	GtkTextIter	end;
	char*		text;
	pageStruct*	page=NULL;

	page=getPageStructPtr(-1);
	if(page==NULL)
		return;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			asprintf(&command,"xdg-open %s &",text);
			system(command);
			free(command);
			free(text);
		}
}

extern "C" int addToContext(gpointer data)
{
	GtkWidget*	menuitem;
	plugData*	plugdata=(plugData*)data;

	setTextDomain(true,plugdata);
	menuitem=createNewImageMenuItem(GTK_STOCK_OPEN,gettext("Open Selection"));
	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->contextPopUpMenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(theCallBack),(void*)plugdata);

	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	submenu=NULL;

	plugData*	plugdata=(plugData*)data;

	setTextDomain(true,plugdata);
	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuNav);
	if(submenu!=NULL)
		{
			menuPlug=createNewImageMenuItem(GTK_STOCK_OPEN,gettext("Open Selection"));
			g_signal_connect(G_OBJECT(menuPlug),"activate",G_CALLBACK(theCallBack),plugdata);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuPlug);
		}
	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	char*			licence;
	GtkAboutDialog*	about;
	char*			translators;

	setTextDomain(true,plugdata);

	const char*		aboutboxstring=gettext("Open Uri from selection");
	const char*	authors[]= {"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,gettext("\nMore by the same author\n"),"Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Open Uri"));
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,copyright);
	gtk_about_dialog_set_version(about,VERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
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
			gtk_widget_destroy(menuPlug);
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
