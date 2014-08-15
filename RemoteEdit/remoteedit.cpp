/*
  remoteedit.cpp
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

#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "0.0.2"

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
bool		syncSave;
GList*		remoteSaves=NULL;

extern "C" const gchar* g_module_check_init(GModule *module)
{
	FILE*	fp;
	char	line[1024];
	char*	command;
	char*	filepath;

	asprintf(&command,"cat %s/.KKEdit/plugins/remotedata",getenv("HOME"));
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

	asprintf(&filepath,"%s/.KKEdit/plugins/remoteedit.sync",getenv("HOME"));
	if(g_file_test(filepath,G_FILE_TEST_EXISTS))
		syncSave=true;
	else
		syncSave=false;
	free(filepath);

	return(NULL);
}

void touch(char* path)
{
	int	fd;

	fd=open(path,O_WRONLY|O_CREAT,DEFFILEMODE);
	if(fd!=-1)
		close(fd);
}

void doSyncSave(plugData* pdata)
{
	char*	filepath;

	asprintf(&filepath,"%s/remoteedit.sync",pdata->lPlugFolder);
	if(syncSave==true)
		touch(filepath);
	else
		unlink(filepath);
	debugFree(filepath,"filepath");
}

void saveRemoteData(void)
{
	FILE*	fp;
	char*	command;
	asprintf(&command,"%s/.KKEdit/plugins/remotedata",getenv("HOME"));
	fp=fopen(command,"w");
	if(fp!=NULL)
		{
			fprintf(fp,"%s\n",dialogUser);
			fprintf(fp,"%s\n",dialogFile);
			fclose(fp);
		}
	free(command);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	if(pathToAskPass!=NULL)
		free(pathToAskPass);
	saveRemoteData();
	return(NULL);
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
	char*	command;
	int		exitstatus;
	char*	messagedata;

	if(strcasecmp(gtk_widget_get_name(widget),"openremote")==0)
		{
			if(pathToAskPass==NULL)
				asprintf(&command,"PS1=\"\" xterm -geometry 50x1 -e scp %s@%s %s",((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath,((remoteFiles*)data)->localFilePath);
			else	
				{
					asprintf(&command,"SSH_ASKPASS=%s %s scp %s@%s %s",pathToAskPass,pathToSetSid,((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath,((remoteFiles*)data)->localFilePath);

					exitstatus=system(command);
					free(command);
				}
			if(WEXITSTATUS(exitstatus)==0)
				{
					((remoteFiles*)data)->saved=false;
					openFile(((remoteFiles*)data)->localFilePath,0,true);
				}			
			else
				{
					((remoteFiles*)data)->saved=false;
					asprintf(&messagedata,"Can't open %s\nScp error %i",((remoteFiles*)data)->remoteFilePath,exitstatus);
					doMessage(messagedata,GTK_MESSAGE_ERROR);
				}
		}

	if(strcasecmp(gtk_widget_get_name(widget),"save")==0)
		{
			if(pathToAskPass==NULL)
				asprintf(&command,"PS1=\"\" xterm -geometry 50x1 -e scp %s %s@%s",((remoteFiles*)data)->localFilePath,((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath);
			else
				asprintf(&command,"SSH_ASKPASS=%s %s scp %s %s@%s",pathToAskPass,pathToSetSid,((remoteFiles*)data)->localFilePath,((remoteFiles*)data)->user,((remoteFiles*)data)->remoteFilePath);

			exitstatus=system(command);
			free(command);
			if(WEXITSTATUS(exitstatus)==0)
				((remoteFiles*)data)->saved=true;
			else
				{
					((remoteFiles*)data)->saved=false;
					asprintf(&messagedata,"Can't save %s\nScp error %i",((remoteFiles*)data)->remoteFilePath,exitstatus);
					doMessage(messagedata,GTK_MESSAGE_ERROR);
				}
		}
}

extern "C" int saveFile(gpointer data)
{
	GList*		tlist=NULL;
	plugData*	plugdata=(plugData*)data;

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

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("RemoteEdit",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	host=gtk_entry_new();
	user=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)host,dialogFile);
	gtk_entry_set_text((GtkEntry*)user,dialogUser);

	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Remote File"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,host,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("User Name"),true,true,4);
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
			remote->menuItem=gtk_image_menu_item_new_with_label(tempdata);
			free(tempdata);

			menu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(remote->menuItem),menu);

			menuitem=gtk_menu_item_new_with_label("Reload Remote File");
			gtk_widget_set_name(menuitem,"openremote");
			doRemote(menuitem,remote);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doRemote),remote);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);	

			remote->saveMenuItem=gtk_menu_item_new_with_label("Export To Remote File");
			gtk_widget_set_name(remote->saveMenuItem,"save");
			gtk_signal_connect(GTK_OBJECT(remote->saveMenuItem),"activate",G_CALLBACK(doRemote),remote);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),remote->saveMenuItem);	
	
			menuitem=gtk_menu_item_get_submenu(GTK_MENU_ITEM(menuMount));
			gtk_menu_shell_append(GTK_MENU_SHELL(menuitem),remote->menuItem);

			gtk_widget_show_all(menuMount);
			free(dialogUser);
			free(dialogFile);
			dialogFile=strdup(gtk_entry_get_text((GtkEntry*)host));
			dialogUser=strdup(gtk_entry_get_text((GtkEntry*)user));

			saveRemoteData();
			remoteSaves=g_list_prepend(remoteSaves,remote);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menuitem;
	GtkWidget*	menu;
	GtkWidget*	image;
	struct stat sb;

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
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuMount);					

	asprintf(&pathToAskPass,"%s/askpass",plugdata->lPlugFolder);
	stat(pathToAskPass,&sb);
	if(!S_ISREG(sb.st_mode))
		{
			free(pathToAskPass);
			asprintf(&pathToAskPass,"%s/askpass",plugdata->gPlugFolder);
			stat(pathToAskPass,&sb);
			if(!S_ISREG(sb.st_mode))
				{
					free(pathToAskPass);
					pathToAskPass=NULL;
				}
		}

	asprintf(&pathToSetSid,"%s/setsid",plugdata->lPlugFolder);
	stat(pathToSetSid,&sb);
	if(!S_ISREG(sb.st_mode))
		{
			free(pathToSetSid);
			asprintf(&pathToSetSid,"%s/setsid",plugdata->gPlugFolder);
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

	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*		aboutboxstring="A KKEdit plugin to import/export a file over SSH";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"Remote Edit");
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

extern "C" int plugPrefs(gpointer data)
{
	plugData*	plugdata=(plugData*)data;
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	showinvis;
	GtkWidget*	vbox;
	int			response;
	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("Remote Edit Plug In Prefs",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	showinvis=gtk_check_button_new_with_label("Save also exports file");
	gtk_toggle_button_set_active((GtkToggleButton*)showinvis,syncSave);
	gtk_box_pack_start((GtkBox*)vbox,showinvis,true,true,4);

	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_APPLY);
	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		{
			syncSave=gtk_toggle_button_get_active((GtkToggleButton*)showinvis);
			doSyncSave(plugdata);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
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
