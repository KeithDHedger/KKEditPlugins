/*
  plugs/newproject.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <libgen.h>
#include <string.h>
#include <ctype.h>

#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "0.0.5"

char*		SVNRepoPath;
char*		projectsPath;

GtkWidget*	menuProjects;
int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	return(NULL);
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

	ret=system("which svn 2>&1 >/dev/null");
	ret=WEXITSTATUS(ret);
	if(ret!=0)
		makesvn=false;

	name=gtk_widget_get_name(widget);
	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("New Project",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	project=gtk_entry_new();
	createsvn=gtk_check_button_new_with_label("Creat SVN Repo");
	gtk_toggle_button_set_active((GtkToggleButton*)createsvn,makesvn);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Project Name"),true,true,4);
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

			asprintf(&archive,"mkdir %s/NewProject;cd %s/NewProject;tar -xvf %s/newproject/bones/bones%s.tar.gz",plugdata->tmpFolder,plugdata->tmpFolder,plugdata->lPlugFolder,name);
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
		}
	gtk_widget_destroy((GtkWidget*)dialog);
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

	menuProjects=gtk_menu_item_new_with_label("_Projects");
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
							sprintf(line,"New %s Project",info);
							menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
							gtk_widget_set_name(menuitem,info);
							gtk_widget_set_tooltip_text(menuitem,infoline);
							gtk_menu_item_set_label((GtkMenuItem*)menuitem,line);
							gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newProject),plugdata);
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
	asprintf(&command,"cat %s/newproject.rc",plugdata->lPlugFolder);
	fp=popen(command,"r");
		if(fp!=NULL)
			{
				fgets(line,1024,fp);
				line[strlen(line)-1]=0;
				if(strlen(line)>0)
					projectsPath=strdup(line);
				fgets(line,1024,fp);
				line[strlen(line)-1]=0;
				if(strlen(line)>0)
					SVNRepoPath=strdup(line);
				pclose(fp);
			}
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
	char*		command;
	plugData*	plugdata=(plugData*)data;

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("KKEdit Project Plugin Prefs",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	projects=gtk_entry_new();
	svn=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)projects,projectsPath);
	gtk_entry_set_text((GtkEntry*)svn,SVNRepoPath);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Projects Folder"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,projects,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Subversion Folder"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,svn,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY);
		{
			asprintf(&command,"echo %s>%s/newproject.rc;echo %s>>%s/newproject.rc",gtk_entry_get_text((GtkEntry*)projects),plugdata->lPlugFolder,gtk_entry_get_text((GtkEntry*)svn),plugdata->lPlugFolder);
			free(projectsPath);
			free(SVNRepoPath);
			projectsPath=strdup(gtk_entry_get_text((GtkEntry*)projects));
			SVNRepoPath=strdup(gtk_entry_get_text((GtkEntry*)svn));
			system(command);
			free(command);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*		aboutboxstring="KKEdit Project Plugin";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","KKEdit\nhttp://gtk-apps.org/content/show.php?content=158161\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"KKEdit Project Plugin");
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,copyright);
	gtk_about_dialog_set_version(about,VERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
	gtk_about_dialog_set_logo_icon_name(about,"KKEditPlug");
	gtk_about_dialog_set_license(about,licence);

	gtk_dialog_run(GTK_DIALOG(about));
	gtk_widget_destroy((GtkWidget*)about);
	free(licence);
	free(licencepath);
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
