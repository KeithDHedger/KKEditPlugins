/*
 *
 * ©K. D. Hedger. Tue 31 Jul 12:39:57 BST 2018 keithdhedger@gmail.com

 * This file (remoteedit.cpp) is part of KKEditPlugins.

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
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <libgen.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libintl.h>
#include <locale.h>

#include "../common.h"
#ifdef _DEVMODE_
#include "../../KKEdit/gtk-kkedit/KKEdit/src/kkedit-plugins.h"
#else
#include <kkedit-plugins.h>
#endif

#define VERSION "0.3.1"
#define TEXTDOMAIN "RemoteEdit"
#define PLATFORM "gtk"

struct remoteFiles
{
	char*		fileName;
	char*		localFilePath;
	char*		remoteFilePath;
	char*		user;
	bool		saved;
	GtkWidget*	menuItem;
	GtkWidget*	saveMenuItem;
};

int	(*module_plug_function)(gpointer globaldata);

GtkWidget*	menuMount;
char*		dialogUser=strdup(getenv("USER"));
char*		dialogFile=strdup("");
char*		pathToAskPass=NULL;
char*		pathToSetSid=NULL;
bool		syncSave=false;
GList*		remoteSaves=NULL;
char*		currentdomain=NULL;
plugData*	globalPData=NULL;
char		*adminCommand=NULL;

args		mydata[]=
				{
					{"syncsave",TYPEBOOL,&syncSave},
					{"admincommand",TYPESTRING,&adminCommand},
					{NULL,0,NULL}
				};

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

void saveRemoteData(plugData *plugdata)
{
	FILE*	fp;
	char*	command;
	asprintf(&command,"%s/remotedata",plugdata->lPlugFolder);
	fp=fopen(command,"w");
	if(fp!=NULL)
		{
			fprintf(fp,"%s\n",dialogUser);
			fprintf(fp,"%s\n",dialogFile);
			fclose(fp);
		}
	free(command);
}

extern "C" void g_module_unload(GModule *module)
{
	if(pathToAskPass!=NULL)
		{
			free(pathToAskPass);
			pathToAskPass=NULL;
		}
	if(pathToSetSid!=NULL)
		{
			free(pathToSetSid);
			pathToSetSid=NULL;
		}
	return;
}

void doMessage(char* message,GtkMessageType type)
{
	GtkWidget*	dialog;

	dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,type,GTK_BUTTONS_CLOSE,"%s",message);
	gtk_dialog_run((GtkDialog*)dialog);
	gtk_widget_destroy(dialog);
}

void doRemote(GtkWidget* widget,gpointer data)
{
	char	*command;
	int		exitstatus;
	char	*messagedata;
	char	*scriptfile=NULL;

	setTextDomain(true,globalPData);
	if(strcasecmp(gtk_widget_get_name(widget),"openremote")==0)
		{
			if(strcmp(getenv("USER"),((remoteFiles*)data)->user)!=0)
				{
					asprintf(&scriptfile,"%s.scipt.sh",((remoteFiles*)data)->localFilePath);
					asprintf(&command,"echo -e \"#!/bin/sh\\nscp %s@%s %s\\nchown %s %s\\n\" > \"%s\"",((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath,((remoteFiles*)data)->localFilePath,getenv("USER"),((remoteFiles*)data)->localFilePath,scriptfile);
					system(command);
					free(command);
					chmod(scriptfile,S_IXUSR);
					asprintf(&command,"%s %s",adminCommand,scriptfile);
					free(scriptfile);
				}
			else
				{
					asprintf(&command,"scp %s@%s %s",((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath,((remoteFiles*)data)->localFilePath);
				}
			exitstatus=system(command);
			free(command);

			if(WEXITSTATUS(exitstatus)==0)
				{
					((remoteFiles*)data)->saved=false;
					openFile(((remoteFiles*)data)->localFilePath,0,true);
				}			
			else
				{
					((remoteFiles*)data)->saved=false;
					asprintf(&messagedata,gettext("Can't open %s\nscp error %i"),((remoteFiles*)data)->remoteFilePath,exitstatus);
					doMessage(messagedata,GTK_MESSAGE_ERROR);
				}
		}

	if(strcasecmp(gtk_widget_get_name(widget),"save")==0)
		{
			if(strcmp(getenv("USER"),((remoteFiles*)data)->user)!=0)
				asprintf(&command,"%s scp %s %s@%s",adminCommand,((remoteFiles*)data)->localFilePath,((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath);
			else
				asprintf(&command,"scp %s %s@%s",((remoteFiles*)data)->localFilePath,((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath);
			exitstatus=system(command);
			free(command);
			if(WEXITSTATUS(exitstatus)==0)
				((remoteFiles*)data)->saved=true;
			else
				{
					((remoteFiles*)data)->saved=false;
					asprintf(&messagedata,gettext("Can't save %s\nscp error %i"),((remoteFiles*)data)->remoteFilePath,exitstatus);
					doMessage(messagedata,GTK_MESSAGE_ERROR);
				}
		}
	setTextDomain(false,globalPData);
}

extern "C" int saveFile(gpointer data)
{
	GList*		tlist=NULL;
	plugData*	plugdata=(plugData*)data;

	if(syncSave==false)
		return(0);

	tlist=remoteSaves;

	while(tlist!=NULL)
		{
			if(strcmp(((remoteFiles*)tlist->data)->localFilePath,plugdata->page->filePath)==0)
				{
					doRemote(((remoteFiles*)tlist->data)->saveMenuItem,tlist->data);
					return(0);
				}
			tlist=g_list_next(tlist);
		}
	return(0);
}

void mountSSHFS(GtkWidget* widget,gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	GtkWidget*		dialog;
	GtkWidget*		dialogbox;
	GtkWidget*		host;
	GtkWidget*		user;
	GtkWidget*		vbox;
	int				response;
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	remoteFiles*	remote=NULL;
	char*			tempdata=NULL;

	vbox=createNewBox(NEWVBOX,false,0);

	setTextDomain(true,plugdata);
	dialog=gtk_dialog_new_with_buttons(gettext("Remote Edit"),NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	host=gtk_entry_new();
	user=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)host,dialogFile);
	gtk_entry_set_text((GtkEntry*)user,dialogUser);

	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Remote File")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,host,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("User Name")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,user,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	
	if(response==GTK_RESPONSE_APPLY)
		{
			tempdata=strdup(gtk_entry_get_text((GtkEntry*)host));
			remote=(remoteFiles*)malloc(sizeof(remoteFiles));
			remote->fileName=strdup(basename(tempdata));
			asprintf(&remote->localFilePath,"%s/%s",plugdata->tmpFolder,remote->fileName);
			free(tempdata);
			remote->remoteFilePath=strdup(gtk_entry_get_text((GtkEntry*)host));

			tempdata=strdup(gtk_entry_get_text((GtkEntry*)host));
			free(tempdata);

			remote->user=strdup(gtk_entry_get_text((GtkEntry*)user));
			remote->saved=true;
			asprintf(&tempdata,"%s@%s",remote->user,remote->remoteFilePath);
			remote->menuItem=gtk_menu_item_new_with_label(tempdata);
			free(tempdata);

			menu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(remote->menuItem),menu);

			menuitem=gtk_menu_item_new_with_label(gettext("Reload Remote File"));
			gtk_widget_set_name(menuitem,"openremote");
			doRemote(menuitem,remote);
			setTextDomain(true,plugdata);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doRemote),remote);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);	

			remote->saveMenuItem=gtk_menu_item_new_with_label(gettext("Export To Remote File"));
			gtk_widget_set_name(remote->saveMenuItem,"save");
			g_signal_connect(G_OBJECT(remote->saveMenuItem),"activate",G_CALLBACK(doRemote),remote);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),remote->saveMenuItem);	
	
			menuitem=gtk_menu_item_get_submenu(GTK_MENU_ITEM(menuMount));
			gtk_menu_shell_append(GTK_MENU_SHELL(menuitem),remote->menuItem);

			gtk_widget_show_all(menuMount);
			free(dialogUser);
			free(dialogFile);
			dialogFile=strdup(gtk_entry_get_text((GtkEntry*)host));
			dialogUser=strdup(gtk_entry_get_text((GtkEntry*)user));

			saveRemoteData(plugdata);
			remoteSaves=g_list_prepend(remoteSaves,remote);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
	setTextDomain(false,plugdata);
}

void getPrefs(plugData *plugdata)
{
	FILE*	fp;
	char	line[1024];
	char*	command;

	currentdomain=strdup(textdomain(NULL));
	asprintf(&command,"cat %s/remotedata",plugdata->lPlugFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			line[0]=0;
			fgets(line,1024,fp);
			if(strlen(line)>2)
				{
					line[strlen(line)-1]=0;
					free(dialogUser);
					dialogUser=strdup(line);
				}
			line[0]=0;
			fgets(line,1024,fp);
			if(strlen(line)>2)
				{
					line[strlen(line)-1]=0;
					free(dialogFile);
					dialogFile=strdup(line);
				}
			pclose(fp);
		}
	free(command);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menuitem;
	GtkWidget*	menu;
	struct stat sb;
	char		*prefspath;

	plugData*	plugdata=(plugData*)data;
	globalPData=plugdata;
	getPrefs(plugdata);

	setTextDomain(true,plugdata);
	menuMount=gtk_menu_item_new_with_label(gettext("_Remote Edit"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuMount,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuMount),menu);

	menuitem=createNewImageMenuItem(GTK_STOCK_CONNECT,gettext("Open Remote File"));
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(mountSSHFS),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuMount);					

	asprintf(&pathToAskPass,"%s/RemoteEdit/askpass",plugdata->lPlugFolder);
	stat(pathToAskPass,&sb);
	if(!S_ISREG(sb.st_mode))
		{
			free(pathToAskPass);
			asprintf(&pathToAskPass,"%s/RemoteEdit/askpass",plugdata->gPlugFolder);
			stat(pathToAskPass,&sb);
			if(!S_ISREG(sb.st_mode))
				{
					free(pathToAskPass);
					pathToAskPass=NULL;
				}
		}

	asprintf(&pathToSetSid,"%s/RemoteEdit/setsid",plugdata->lPlugFolder);
	stat(pathToSetSid,&sb);
	if(!S_ISREG(sb.st_mode))
		{
			free(pathToSetSid);
			asprintf(&pathToSetSid,"%s/RemoteEdit/setsid",plugdata->gPlugFolder);
			stat(pathToSetSid,&sb);
			if(!S_ISREG(sb.st_mode))
				{
					free(pathToSetSid);
					pathToSetSid=NULL;
					if(pathToAskPass!=NULL)
						free(pathToAskPass);
					pathToAskPass=NULL;
				}
		}

	asprintf(&adminCommand,"%s","gtksu -- ");
	syncSave=false;
	asprintf(&prefspath,"%s/remoteedit.rc",plugdata->lPlugFolder);
	loadVarsFromFile(prefspath,mydata);
	free(prefspath);

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

	const char*		aboutboxstring=gettext("A KKEdit plugin to import/export a file over SSH");

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("Remote Edit"));
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

extern "C" int plugPrefs(gpointer data)
{
	plugData	*plugdata=(plugData*)data;
	GtkWidget	*dialog;
	GtkWidget	*dialogbox;
	GtkWidget	*showinvis;
	GtkWidget	*vbox;
	int			response;
	GtkWidget	*admincom;
	char		*prefspath=NULL;

	vbox=createNewBox(NEWVBOX,false,0);

	setTextDomain(true,plugdata);
	dialog=gtk_dialog_new_with_buttons(gettext("Remote Edit Plug In Prefs"),NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	admincom=gtk_entry_new();
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Graphical Admin Command")),true,true,4);
	gtk_entry_set_text((GtkEntry*)admincom,adminCommand);
	gtk_box_pack_start((GtkBox*)vbox,admincom,true,true,4);

	showinvis=gtk_check_button_new_with_label(gettext("Save also exports file"));
	gtk_toggle_button_set_active((GtkToggleButton*)showinvis,syncSave);
	gtk_box_pack_start((GtkBox*)vbox,showinvis,true,true,4);

	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_APPLY);
	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		{
			syncSave=gtk_toggle_button_get_active((GtkToggleButton*)showinvis);
			debugFree(&adminCommand);
			asprintf(&adminCommand,"%s",gtk_entry_get_text((GtkEntry*)admincom));
			asprintf(&prefspath,"%s/remoteedit.rc",plugdata->lPlugFolder);
			
			saveVarsToFile(prefspath,mydata);
			debugFree(&prefspath);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(menuMount);
			gtk_widget_show_all(plugdata->mlist.menuBar);
			free(pathToAskPass);
			pathToAskPass=NULL;	
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	return(0);
}
