/*
 *
 * ©K. D. Hedger. Fri 29 Jan 11:51:00 GMT 2021 keithdhedger@gmail.com

 * This file (savehistory.cpp) is part of KKEditPlugins.

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
#include <sys/stat.h>
#include <sys/types.h>

#include <libintl.h>
#include <locale.h>
#include <time.h>

#include "../common.h"
#ifdef _DEVMODE_
#include "../../KKEdit/gtk-kkedit/KKEdit/src/kkedit-plugins.h"
#else
#include <kkedit-plugins.h>
#endif

#define VERSION "0.3.0"
#define TEXTDOMAIN "SaveHistory"

GtkWidget	*histMenu;
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

void deleteHistory(GtkWidget* widget,gpointer data)
{
	plugData	*plugdata=(plugData*)data;
	char		*command;

	asprintf(&command,"rm -r \"%s/SaveHistory/%s\"",plugdata->lPlugFolder,plugdata->page->fileName);
printf("%s",command);
	system(command);
	free(command);
}

extern "C" int addToGui(gpointer data)
{
	char		*historyfolder;
	plugData	*plugdata=(plugData*)data;

	asprintf(&historyfolder,"%s/SaveHistory",plugdata->lPlugFolder);
	mkdir(historyfolder,0700);
	free(historyfolder);

	setTextDomain(true,plugdata);

	histMenu=gtk_menu_item_new_with_label(gettext("_Delete Current File History"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)histMenu,true);

	gtk_widget_set_name(histMenu,"deletehist");
	g_signal_connect(G_OBJECT(histMenu),"activate",G_CALLBACK(deleteHistory),plugdata);

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),histMenu);					
	gtk_widget_show_all(histMenu);
	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int saveFile(gpointer data)
{
	char		*command;
	time_t		t=time(NULL);
	struct tm	tim=*localtime(&t);

	plugData	*plugdata=(plugData*)data;

	asprintf(&command,"mkdir -p \"%s/SaveHistory/%s\";cp '%s' '%s/SaveHistory/%s/%s-%i:%i-%02i:%02i:%02i'",
	plugdata->lPlugFolder,
	plugdata->page->fileName,
	plugdata->page->filePath,
	plugdata->lPlugFolder,
	plugdata->page->fileName,
	plugdata->page->fileName,
	tim.tm_year-100,tim.tm_yday,tim.tm_hour,tim.tm_min,tim.tm_sec);

	system(command);
	free(command);
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

	const char*		aboutboxstring=gettext("Save History");

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Save History Plugin"));
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
	plugData	*plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(histMenu);
			gtk_widget_show_all(plugdata->mlist.menuBar);	
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
		}
	return(0);
}
