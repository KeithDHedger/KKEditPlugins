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

#include "kkedit-plugins.h"
#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "0.0.1"

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

void newProject(GtkWidget* widget,gpointer data)
{
//	GtkWidget*	dialog;
//	GtkWidget*	dialogbox;
//	GtkWidget*	projects;
//	GtkWidget*	svn;
//	GtkWidget*	vbox;
//	int			response;
//	char*		command;
//	FILE*		fp;
//	char*		folder;
//
//	plugData*	plugdata=(plugData*)data;
//
//	vbox=gtk_vbox_new(false,0);
//
//	dialog=gtk_dialog_new_with_buttons("Project Type",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
//	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
//	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
//	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);
//
//	folder=strdup(plugdata->plugData->path);
//	asprintf(&command,"%s/bones",dirname(folder));
//	
//	
//	projects=gtk_entry_new();
//	svn=gtk_entry_new();
//
//	gtk_entry_set_text((GtkEntry*)projects,"/media/LinuxData/Development/Projects");
//	gtk_entry_set_text((GtkEntry*)svn,"/media/LinuxData/Development/SVN");
//	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Projects Folder"),true,true,4);
//	gtk_box_pack_start((GtkBox*)vbox,projects,true,true,4);
//	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Subversion Folder"),true,true,4);
//	gtk_box_pack_start((GtkBox*)vbox,svn,true,true,4);
//
//	gtk_widget_show_all(dialog);
//	response=gtk_dialog_run(GTK_DIALOG(dialog));
//	if(response==GTK_RESPONSE_APPLY);
//		{
//			asprintf(&command,"echo %s>%s/newproject.rc;echo %s>>%s/newproject.rc",gtk_entry_get_text((GtkEntry*)projects),plugdata->lPlugFolder,gtk_entry_get_text((GtkEntry*)svn),plugdata->lPlugFolder);
//			system(command);
//			free(command);
//		}
//	gtk_widget_destroy((GtkWidget*)dialog);
}

extern "C" int addMenus(gpointer data)
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

	folder=strdup(plugdata->plugData->path);
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
							sprintf(line,"New %.*s Project",(int)strlen(info)-5,info);
							menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
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
//new
///	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,NULL);
//	gtk_menu_item_set_label((GtkMenuItem*)menuitem,"New Project");
//	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(newProject),plugdata);
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuProjects);
	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuProjects);					

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

	gtk_entry_set_text((GtkEntry*)projects,"/media/LinuxData/Development/Projects");
	gtk_entry_set_text((GtkEntry*)svn,"/media/LinuxData/Development/SVN");
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Projects Folder"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,projects,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Subversion Folder"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,svn,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY);
		{
			asprintf(&command,"echo %s>%s/newproject.rc;echo %s>>%s/newproject.rc",gtk_entry_get_text((GtkEntry*)projects),plugdata->lPlugFolder,gtk_entry_get_text((GtkEntry*)svn),plugdata->lPlugFolder);
			system(command);
			free(command);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	printf("about from new project\n");
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*		aboutboxstring="KKEdit Project Plugin";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

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

	if(plugdata->plugData->unload==true)
		{
			gtk_widget_destroy(menuProjects);
			gtk_widget_show_all(plugdata->mlist.menuBar);	
		}
	else
		{
			if(g_module_symbol(plugdata->plugData->module,"addMenus",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	return(0);
}
