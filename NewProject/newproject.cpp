/*
 *
 * ©K. D. Hedger. Tue  6 Oct 12:24:24 BST 2015 kdhedger68713@gmail.com

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
#define TEXTDOMAIN "NewProject"

char*		SVNRepoPath;
char*		projectsPath;
args		mydata[]=
				{
					{"svnrepos",TYPESTRING,&SVNRepoPath},
					{"projects",TYPESTRING,&projectsPath},
					{NULL,0,NULL}
				};

GtkWidget*	menuProjects;
char*		currentdomain=NULL;

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

void runCommandAndOut(char* command,plugData* plugdata)
{
	FILE*		fp=NULL;
	char		line[1024];
	GtkTextIter	iter;

	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					gtk_text_buffer_insert_at_cursor(plugdata->toolOutBuffer,line,strlen(line));
					while(gtk_events_pending())
						gtk_main_iteration();
					gtk_text_buffer_get_end_iter(plugdata->toolOutBuffer,&iter);
					gtk_text_view_scroll_to_iter((GtkTextView*)plugdata->toolOutWindow,&iter,0,true,0,0);
				}
			pclose(fp);
		}
}

void openNewFiles(char* projectsPath,const char* projname)
{
	FILE*	fp=NULL;
	char	line[1024];
	char*	command;
	char*	filepath;

	asprintf(&command,"cat %s/%s/filestoopen",projectsPath,projname);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					line[strlen(line)-1]=0;
					asprintf(&filepath,"%s/%s/%s",projectsPath,projname,line);
					openFile((const gchar*)filepath,0,false);
					free(filepath);
				}
			pclose(fp);
		}

	free(command);
}

void newProject(GtkWidget* widget,gpointer data)
{
	const char*	name;
	char*		archive;
	plugData*	plugdata=(plugData*)data;

	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	project;
	GtkWidget*	createsvn;
	bool		makesvn=true;
	GtkWidget*	vbox;
	int			response;
	char*		command;
	const char*	projname;
	char*		appnamelower;
	int			ret;

	setTextDomain(true,plugdata);

	ret=system("which svn 2>&1 >/dev/null");
	ret=WEXITSTATUS(ret);
	if(ret!=0)
		makesvn=false;

	name=gtk_widget_get_name(widget);
	vbox=createNewBox(NEWVBOX,false,0);

	dialog=gtk_dialog_new_with_buttons("New Project",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	project=gtk_entry_new();
	createsvn=gtk_check_button_new_with_label(gettext("Creat SVN Repo"));
	gtk_toggle_button_set_active((GtkToggleButton*)createsvn,makesvn);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Project Name")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,project,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,createsvn,true,true,4);

	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_APPLY);
	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		{
			showToolOutput(true);

			asprintf(&command,"mkdir -vp %s;mkdir -vp %s",projectsPath,SVNRepoPath);
			runCommandAndOut(command,plugdata);
			free(command);

			asprintf(&archive,"mkdir %s/NewProject;cd %s/NewProject;tar -xvf %s/NewProject/bones/bones%s.tar.gz",plugdata->tmpFolder,plugdata->tmpFolder,plugdata->lPlugFolder,name);
			runCommandAndOut(archive,plugdata);
			makesvn=gtk_toggle_button_get_active((GtkToggleButton*)createsvn);
			free(archive);
			projname=gtk_entry_get_text((GtkEntry*)project);
			appnamelower=strdup(gtk_entry_get_text((GtkEntry*)project));
			for(unsigned int j=0;j<strlen(appnamelower);j++)
				appnamelower[j]=tolower(appnamelower[j]);

//filenames
			asprintf(&command,"cd %s/NewProject/bones%s;find -iname \"*<>APP<>*\" -type d -exec bash -c 'mv $0 ${0/<>APP<>/%s}' '{}' \\; 2>/dev/null",plugdata->tmpFolder,name,appnamelower);
			runCommandAndOut(command,plugdata);
			free(command);
			asprintf(&command,"cd %s/NewProject/bones%s;find -iname \"*<>APP<>*\" -type f -exec bash -c 'mv $0 ${0/<>APP<>/%s}' '{}'  \\; 2>/dev/null",plugdata->tmpFolder,name,appnamelower);
			runCommandAndOut(command,plugdata);
			free(command);
			asprintf(&command,"cd %s/NewProject/bones%s;find -iname \"*<>PROJ<>*\" -type d -exec bash -c 'mv $0 ${0/<>PROJ<>/%s}' '{}'  \\; 2>/dev/null",plugdata->tmpFolder,name,projname);
			runCommandAndOut(command,plugdata);
			free(command);
			asprintf(&command,"cd %s/NewProject/bones%s;find -iname \"*<>PROJ<>*\" -type f -exec bash -c 'mv $0 ${0/<>PROJ<>/%s}' '{}'  \\; 2>/dev/null",plugdata->tmpFolder,name,projname);
			runCommandAndOut(command,plugdata);
			free(command);

//in files
			asprintf(&command,"cd %s/NewProject/bones%s;find -type f|xargs grep -lI \"<>APP<>\"|xargs sed -i 's/<>APP<>/%s/g'",plugdata->tmpFolder,name,appnamelower);
			runCommandAndOut(command,plugdata);
			free(command);
			asprintf(&command,"cd %s/NewProject/bones%s;find -type f|xargs grep -lI \"<>PROJ<>\"|xargs sed -i 's/<>PROJ<>/%s/g'",plugdata->tmpFolder,name,projname);
			runCommandAndOut(command,plugdata);
			free(command);

			if(makesvn==true)
				{
					asprintf(&command,"cd %s/NewProject/bones%s;svnadmin create \"%s/%s\"",plugdata->tmpFolder,name,SVNRepoPath,projname);
					runCommandAndOut(command,plugdata);
					free(command);
					asprintf(&command,"cd %s/NewProject/bones%s;svn import . file://\"%s/%s\" -m \"Initial import\"",plugdata->tmpFolder,name,SVNRepoPath,projname);
					runCommandAndOut(command,plugdata);
					free(command);
					asprintf(&command,"cd %s;svn checkout  file://\"%s/%s\"",projectsPath,SVNRepoPath,projname);
					runCommandAndOut(command,plugdata);
					free(command);
				}
			else
				{
					asprintf(&command,"cp -r %s/NewProject/bones%s %s/%s",plugdata->tmpFolder,name,projectsPath,projname);
					runCommandAndOut(command,plugdata);
					free(command);
				}

			asprintf(&command,"rm -r %s/NewProject",plugdata->tmpFolder);
			system(command);
			free(command);
			free(appnamelower);
			openNewFiles(projectsPath,projname);
		}
	gtk_widget_hide(dialog);
	gtk_widget_destroy((GtkWidget*)dialog);

	setTextDomain(false,plugdata);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menuitem;
	GtkWidget*	menu;
	char*		command;
	FILE*		fp;
	FILE*		infofp;
	char*		folder;
	char		line[1024];
	char*		info;
	char*		infocommand;
	char		infoline[1024];

	plugData*	plugdata=(plugData*)data;

	setTextDomain(true,plugdata);

	menuProjects=gtk_menu_item_new_with_label(gettext("_Projects"));
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuProjects,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuProjects),menu);

	folder=strdup(plugdata->modData->path);
	asprintf(&command,"find %s/bones -iname \"*.info\"|sort",dirname(folder));
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(feof(fp)==0)
				{
					fgets(line,1024,fp);
					if(strlen(line)>0)
						{
							line[strlen(line)-1]=0;
							asprintf(&infocommand,"cat %s",line);
							infofp=popen(infocommand,"r");
							infoline[0]=0;
							fgets(infoline,1024,infofp);
							infoline[strlen(infoline)-1]=0;
							pclose(infofp);
							free(infocommand);
							info=basename(line);
							*(strstr(info,".info"))=0;
							sprintf(line,gettext("New %s Project"),info);
							menuitem=createNewStockMenuItem(GTK_STOCK_NEW,GTK_STOCK_NEW);
							gtk_widget_set_name(menuitem,info);
							gtk_widget_set_tooltip_text(menuitem,infoline);
							gtk_menu_item_set_label((GtkMenuItem*)menuitem,line);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(newProject),plugdata);
							gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
							line[0]=0;
					}
				}
		}
	pclose(fp);
	free(command);
	free(folder);

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuProjects);					

	asprintf(&projectsPath,"%s/Projects",getenv("HOME"));
	asprintf(&SVNRepoPath,"%s/SVN",getenv("HOME"));
	asprintf(&command,"%s/newproject.rc",plugdata->lPlugFolder);
	loadVarsFromFile(command,mydata);
	free(command);

	setTextDomain(false,plugdata);
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	projects;
	GtkWidget*	svn;
	GtkWidget*	vbox;
	int			response;
	plugData*	plugdata=(plugData*)data;
	char*		prefspath;

	setTextDomain(true,plugdata);
	vbox=createNewBox(NEWVBOX,false,0);

	dialog=gtk_dialog_new_with_buttons(gettext("KKEdit Project Plugin Prefs"),NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	projects=gtk_entry_new();
	svn=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)projects,projectsPath);
	gtk_entry_set_text((GtkEntry*)svn,SVNRepoPath);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Projects Folder")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,projects,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new(gettext("Subversion Folder")),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,svn,true,true,4);
	gtk_window_set_transient_for((GtkWindow*)dialog,(GtkWindow*)plugdata->prefsWindow);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		{
			asprintf(&prefspath,"%s/newproject.rc",plugdata->lPlugFolder);
			free(projectsPath);
			free(SVNRepoPath);
			projectsPath=strdup(gtk_entry_get_text((GtkEntry*)projects));
			SVNRepoPath=strdup(gtk_entry_get_text((GtkEntry*)svn));
			saveVarsToFile(prefspath,mydata);
			free(prefspath);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
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

	const char*		aboutboxstring=gettext("Project Plugin - Add's create barebones projects");

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);
	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>",gettext("French Translation"));

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("KKEdit Project Plugin"));
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
			gtk_widget_destroy(menuProjects);
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
