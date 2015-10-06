/*
 *
 * ©K. D. Hedger. Tue  6 Oct 12:23:18 BST 2015 kdhedger68713@gmail.com

 * This file (clipboardplugin.cpp) is part of KKEditPlugins.

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

#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://kkedit.darktech.org"
#define VERSION "0.0.3"
#define MAXCLIPS 10
#define MAXCLIPMENULEN 43
#define CLIPENDLEN 20
#define CLIPENDLENSTR "20"
#define TEXTDOMAIN "ClipboardPlugin"

struct clips
{
	GtkWidget*	menuItem;
	char*		text;
};

GtkWidget*		menuPlug;
GtkClipboard*	mainClipboard;
int				currentClip=-1;
clips			clip[MAXCLIPS];
plugData*		plugdata;
gulong			clipid;
char*			currentdomain=NULL;
bool			manual=false;

int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	currentdomain=strdup(textdomain(NULL));
	return(NULL);
}

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

extern "C" const gchar* g_module_unload(GModule *module)
{
	for(int j=0;j<MAXCLIPS;j++)
		{
			if(clip[j].text!=NULL)
				free(clip[j].text);
		}
	g_signal_handler_disconnect(mainClipboard,clipid);
	return(NULL);
}

void setCurrentClip(void)
{
	currentClip++;
	if(currentClip>=MAXCLIPS)
		currentClip=0;
}

void clipChanged(GtkClipboard* clipboard,gpointer user_data)
{
	char*	texthold;
	char*	label;


	if (manual==true)
		{
			manual=false;
			return;
		}

	if (gtk_clipboard_wait_is_text_available(mainClipboard)==true)
		{
			setCurrentClip();
			if(clip[currentClip].text != NULL)
				free(clip[currentClip].text);
			clip[currentClip].text=gtk_clipboard_wait_for_text(clipboard);

			texthold=g_strescape(clip[currentClip].text,NULL);
			texthold=g_strstrip(texthold);

			if(strlen(clip[currentClip].text)>MAXCLIPMENULEN)
				{
					asprintf(&label,"%." CLIPENDLENSTR "s...%s",texthold,(char*)&texthold[strlen(texthold)-CLIPENDLEN]);
					gtk_menu_item_set_label((GtkMenuItem*)clip[currentClip].menuItem,label);
					free(label);
				}
			else
				gtk_menu_item_set_label((GtkMenuItem*)clip[currentClip].menuItem,texthold);

			free(texthold);
			gtk_widget_show_all(menuPlug);
		}
}

void theCallBack(GtkWidget* widget,gpointer data)
{
	int	clipnum=(int)(long)data;

	if(clip[clipnum].text!=NULL)
		{
			manual=true;
			gtk_clipboard_set_text(mainClipboard,clip[clipnum].text,-1);
		}
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menu;
	char*		command;

	plugdata=(plugData*)data;
	currentClip=-1;
	setTextDomain(true,plugdata);

	menuPlug=gtk_menu_item_new_with_label(gettext("C_lipboard"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuPlug,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuPlug),menu);

	for(int j=0;j<MAXCLIPS;j++)
		{
			asprintf(&command,gettext("Clip Number. %i"),j+1);
			clip[j].menuItem=(GtkWidget*)gtk_menu_item_new_with_label(command);
			free(command);
			clip[j].text=NULL;
			g_signal_connect(GTK_OBJECT(clip[j].menuItem),"activate",G_CALLBACK(theCallBack),(void*)(long)j);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),clip[j].menuItem);
		}

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuPlug);					

	mainClipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	clipid=g_signal_connect(G_OBJECT(mainClipboard),"owner-change",G_CALLBACK(clipChanged),plugdata);
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

	const char*		aboutboxstring=gettext("Clipboard Plugin - Add's multiple clips");
	const char*	authors[]= {"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,gettext("\nMore by the same author\n"),"Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));
	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"ClipboardPlugin");
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
