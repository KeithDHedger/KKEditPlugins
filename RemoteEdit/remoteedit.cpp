/*
  remoteedit.cpp
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
#define VERSION "0.0.1"

GtkWidget*	menuMount;
char*		mountPoint=NULL;
void doTabMenu(GtkWidget *widget,gpointer data);
int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	return(NULL);
}

void unMountSSHFS(GtkWidget* widget,gpointer data)
{
//	plugData*	plugdata=(plugData*)data;
	char*		command;

	asprintf(&command,"fusermount -u %s 2>&1 >/dev/null",(char*)data);
	system(command);
	free(command);
	gtk_widget_destroy(widget);
	gtk_widget_show_all(menuMount);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	unMountSSHFS(NULL,NULL);
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

void mountSSHFS(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	host;
	GtkWidget*	user;
	GtkWidget*	passwd;
	GtkWidget*	vbox;
	int			response;
	char*		command;
	char*		remotedirname;
	char*		remotefilename;
	GtkWidget*	menuitem;
	GtkWidget*	menu;
	GtkWidget*	image;

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("RemoteEdit",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	host=gtk_entry_new();
	user=gtk_entry_new();
	passwd=gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry*)passwd,false);

	gtk_entry_set_text((GtkEntry*)host,"192.168.1.66:/etc/fstab");
	gtk_entry_set_text((GtkEntry*)user,"keithhedger");
	gtk_entry_set_text((GtkEntry*)passwd,"hogandnana");

	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Remote File"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,host,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("User Name"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,user,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Password"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,passwd,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	
	if(response==GTK_RESPONSE_APPLY)
		{
			command=strdup(gtk_entry_get_text((GtkEntry*)host));
			remotedirname=strdup(dirname(command));
			free(command);

			if(mountPoint!=NULL)
				free(mountPoint);
			asprintf(&mountPoint,"%s/%s",plugdata->tmpFolder,remotedirname);

			asprintf(&command,"mkdir -vp %s",mountPoint);
			system(command);
			free(command);

			command=strdup(gtk_entry_get_text((GtkEntry*)host));
			remotefilename=strdup(basename(command));
			free(command);

			asprintf(&command,"echo %s|sshfs -o password_stdin %s@%s %s",gtk_entry_get_text((GtkEntry*)passwd),gtk_entry_get_text((GtkEntry*)user),remotedirname,mountPoint);
			system(command);
			free(command);

			asprintf(&command,"%s/%s",mountPoint,remotefilename);
			openFile((const gchar*)command,0,true);

			asprintf(&command,"Un-Mount %s",remotedirname);
			menuitem=gtk_image_menu_item_new_with_label(command);
			image=gtk_image_new_from_stock(GTK_STOCK_DISCONNECT,GTK_ICON_SIZE_MENU);
			gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(unMountSSHFS),mountPoint);
			menu=gtk_menu_item_get_submenu(GTK_MENU_ITEM(menuMount));
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
			gtk_widget_show_all(menuMount);

			free(command);
			free(remotefilename);
			free(remotedirname);

		}
	gtk_widget_destroy((GtkWidget*)dialog);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menuitem;
	GtkWidget*	menu;
	GtkWidget*	image;
	char*		command;
	plugData*	plugdata=(plugData*)data;

	menuMount=gtk_menu_item_new_with_label("_Remote Edit");
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuMount,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuMount),menu);

	menuitem=gtk_image_menu_item_new_with_label("Open Remote File");
	image=gtk_image_new_from_stock(GTK_STOCK_CONNECT,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(mountSSHFS),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

//	menuitem=gtk_image_menu_item_new_with_label("Un-Mount SSHFS");
//	image=gtk_image_new_from_stock(GTK_STOCK_DISCONNECT,GTK_ICON_SIZE_MENU);
//	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
//	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(unMountSSHFS),plugdata);
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuMount);					

//	if(mountPoint!=NULL)
//		free(mountPoint);
//	asprintf(&mountPoint,"%s/sshmount",plugdata->tmpFolder);
//
//	asprintf(&command,"mkdir -vp %s",mountPoint);
//	system(command);
//	free(command);
	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
/*
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	projects;
	GtkWidget*	svn;
	GtkWidget*	vbox;
	int			response;
	char*		command;
	plugData*	plugdata=(plugData*)data;

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("RemoteEdit",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
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
			asprintf(&command,"echo %s>%s/remoteedit.rc;echo %s>>%s/remoteedit.rc",gtk_entry_get_text((GtkEntry*)projects),plugdata->lPlugFolder,gtk_entry_get_text((GtkEntry*)svn),plugdata->lPlugFolder);
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
	const char*		aboutboxstring="RemoteEdit";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"RemoteEdit");
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
*/
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(menuMount);
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
