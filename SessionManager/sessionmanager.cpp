/*
  sessionmanager.cpp
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
#define	MAXSESSIONS 4

char*		prefsPath;
GtkWidget*	menuPlug;
GtkWidget*	saveSessionMenu=NULL;
GtkWidget*	restoreSessionMenu=NULL;
GtkWidget*	holdWidget=NULL;
GtkWidget*	menusm;
int	(*module_plug_function)(gpointer globaldata);
extern void saveSession(GtkWidget* widget,gpointer data);
extern void restoreSession(GtkWidget* widget,gpointer data);

char*		sessionNames[MAXSESSIONS]={NULL,};

GtkWidget* findMenu(GtkWidget* parent, const gchar* name)
{
	const gchar* mname=NULL;

	if ( (GTK_IS_MENU_ITEM(parent)) && !(GTK_IS_SEPARATOR_MENU_ITEM(parent)) )
		{
			mname=gtk_menu_item_get_label((GtkMenuItem *)parent);
			if(mname!=NULL)
				{
					if(strcmp(name,mname)==0)
						{
							holdWidget=parent;
						}
				}
		}

	if (GTK_IS_CONTAINER(parent))
		{
			GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
			while ((children = g_list_next(children)) != NULL)
				{
					GtkWidget* widget = findMenu((GtkWidget*)children->data, name);

					if (widget != NULL)
						{
							return widget;
						}
				}
			g_list_free(children);
		}

	return NULL;
}

extern "C" const gchar* g_module_check_init(GModule *module)
{
	for(int j=0;j<MAXSESSIONS;j++)
		asprintf(&sessionNames[j],"Session %i",j);

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

void saveSessionNum(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;

	printf("save %s\n",gtk_widget_get_name(widget));
}

void restoreSessionNum(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;

	printf("restore %s\n",gtk_widget_get_name(widget));
}

extern "C" int addToGui(gpointer data)
{

	GtkWidget*	menuitem;
	char*		sessionname;
	GtkWidget*	menu;

	plugData*	plugdata=(plugData*)data;

	holdWidget=NULL;
	findMenu(gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuFile),"Save Session");
	if(holdWidget!=NULL)
		{
			saveSessionMenu=holdWidget;
			g_signal_handlers_disconnect_by_func(saveSessionMenu,(void*)saveSession,NULL);
			menu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(saveSessionMenu),menu);

			for(int j=0;j<MAXSESSIONS;j++)
				{
					asprintf(&sessionname,"Save %s",sessionNames[j]);
					menuitem=gtk_menu_item_new_with_label(sessionname);
					gtk_widget_set_name(menuitem,sessionNames[j]);
					free(sessionname);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(saveSessionNum),plugdata);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
				}
				gtk_widget_show_all(saveSessionMenu);
	}

	holdWidget=NULL;
	findMenu(gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuFile),"Restore Session");
	if(holdWidget!=NULL)
		{
			restoreSessionMenu=holdWidget;
			g_signal_handlers_disconnect_by_func(restoreSessionMenu,(void*)restoreSession,NULL);
			menu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(restoreSessionMenu),menu);

			for(int j=0;j<MAXSESSIONS;j++)
				{
					asprintf(&sessionname,"Restore %s",sessionNames[j]);
					menuitem=gtk_menu_item_new_with_label(sessionname);
					gtk_widget_set_name(menuitem,sessionNames[j]);
					free(sessionname);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(restoreSessionNum),plugdata);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
				}
				gtk_widget_show_all(restoreSessionMenu);
	}

	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	prefs;
	GtkWidget*	vbox;
	int			response;
	char*		command;
	plugData*	plugdata=(plugData*)data;

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("SessionManager",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	prefs=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)prefs,prefsPath);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Prefs Path"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,prefs,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY);
		{
			asprintf(&command,"echo %s>%s/sessionmanager.rc",gtk_entry_get_text((GtkEntry*)prefs),plugdata->lPlugFolder);
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
	const char*		aboutboxstring="SessionManager";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"SessionManager");
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