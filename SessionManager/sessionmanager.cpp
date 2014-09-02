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
#define PLUGVERSION "0.0.1"
#define	MAXSESSIONS 8

char*		prefsPath;
GtkWidget*	menuPlug;
GtkWidget*	saveSessionMenu=NULL;
GtkWidget*	restoreSessionMenu=NULL;
GtkWidget*	holdWidget=NULL;
GtkWidget*	menusm;
int	(*module_plug_function)(gpointer globaldata);

extern void saveSession(GtkWidget* widget,gpointer data);
extern void restoreSession(GtkWidget* widget,gpointer data);
extern void closeAllTabs(GtkWidget* widget,gpointer data);
extern void toggleBookmark(GtkWidget* widget,GtkTextIter* titer);

extern GList*			newBookMarksList;

void rebuildMainMenu(GtkWidget* menu,plugData*	plugdata,GCallback* func);
void restoreSessionNum(GtkWidget* widget,gpointer data);

struct bookMarksNew
{
	pageStruct*			page;
	char*				label;
	GtkSourceMark*		mark;
	char*				markName;
	int					line;
};


char*		sessionNames[MAXSESSIONS]= {NULL,};

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
	char*	sessionfile;
	FILE*	fd=NULL;

	for(int j=0; j<MAXSESSIONS; j++)
		{
			asprintf(&sessionfile,"%s/.KKEdit/session-%i",getenv("HOME"),j);
			fd=fopen(sessionfile,"r");
			if(fd!=NULL)
				{
					fscanf(fd,"%a[^\n]s",&sessionNames[j]);
					fclose(fd);
				}
			else
				asprintf(&sessionNames[j],"Session %i",j);
		}

	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	for(int j=0; j<MAXSESSIONS; j++)
		free(sessionNames[j]);
	return(NULL);
}

char* getNewSessionName(int sessionnumber)
{
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	prefs;
	GtkWidget*	vbox;
	int			response;
	char*		command;

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("SessionManager",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	prefs=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)prefs,sessionNames[sessionnumber]);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Session Name"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,prefs,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY)
		asprintf(&command,"%s",gtk_entry_get_text((GtkEntry*)prefs));
	else
		command=NULL;

	gtk_widget_destroy((GtkWidget*)dialog);
	return(command);
}

void saveSessionPlug(char* name,plugData* plugdata,int snum)
{
	pageStruct*		page;
	FILE*			fd=NULL;
	char*			filename;
	GtkTextMark*	mark;
	GtkTextIter		iter;
	int				linenumber;
	GtkTextIter		markiter;
	GList*			ptr;

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	debugFree(filename,"saveSession filename");
	asprintf(&filename,"%s/.KKEdit/session-%i",getenv("HOME"),snum);
	fd=fopen(filename,"w");
	if (fd!=NULL)
		{
			fprintf(fd,"%s\n",name);
			for(int loop=0; loop<gtk_notebook_get_n_pages(plugdata->notebook); loop++)
				{
					page=getPageStructPtr(loop);
					mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
					linenumber=gtk_text_iter_get_line(&iter);
					fprintf(fd,"%i %s\n",linenumber,page->filePath);

					ptr=newBookMarksList;
					while(ptr!=NULL)
						{
							if(((bookMarksNew*)ptr->data)->page==page)
								{
									gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&markiter,(GtkTextMark*)((bookMarksNew*)ptr->data)->mark);
									fprintf(fd,"%i ",gtk_text_iter_get_line(&markiter));
									fprintf(fd,"%s\n",((bookMarksNew*)ptr->data)->label);
								}
							ptr=g_list_next(ptr);
						}
					fprintf(fd,"-1 endmarks\n");
				}

			fclose(fd);
			debugFree(filename,"saveSession filename");
		}
}

void saveSessionNum(GtkWidget* widget,gpointer data)
{
	char*		sname=NULL;
	int			snum=0;
	plugData*	plugdata=(plugData*)data;

	for(int j=0; j<MAXSESSIONS; j++)
		{
			if(strcmp(sessionNames[j],gtk_widget_get_name(widget))==0)
				snum=j;
		}

	sname=getNewSessionName(snum);
	if(sname!=NULL)
		{
			free(sessionNames[snum]);
			sessionNames[snum]=sname;
			saveSessionPlug(sname,plugdata,snum);
			rebuildMainMenu(saveSessionMenu,plugdata,(GCallback*)saveSessionNum);
			rebuildMainMenu(restoreSessionMenu,plugdata,(GCallback*)restoreSessionNum);
		}

}

void restoreSessionFromFile(char* filename)
{
	FILE*		fd=NULL;
	char		buffer[2048];
	int			intarg;
	char		strarg[2048];
	pageStruct*	page;
	GtkTextIter	markiter;
	int			currentline;
	int			currentpage=0;
	GtkTextMark*	mark;
	GtkTextIter	cursiter;

	fd=fopen(filename,"r");
	if (fd!=NULL)
		{
			fgets(buffer,2048,fd);
			closeAllTabs(NULL,NULL);
			while(fgets(buffer,2048,fd)!=NULL)
				{
					sscanf(buffer,"%i %[^\n]s",(int*)&currentline,(char*)&strarg);
					if(openFile(strarg,currentline,true)==true)
						{
							page=getPageStructPtr(currentpage);
							gtk_widget_show_all((GtkWidget*)page->view);
							intarg=999;
							fgets(buffer,2048,fd);
							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
							while(intarg!=-1)
								{
									buffer[0]=0;
									gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
									gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
									toggleBookmark(NULL,&markiter);
									fgets(buffer,2048,fd);
									sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
								}

							gtk_text_buffer_get_iter_at_line_offset((GtkTextBuffer*)page->buffer,&markiter,currentline,0);
							gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
							if(!gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&markiter,0,true,0,0.5))
								{
									mark=gtk_text_buffer_get_mark((GtkTextBuffer*)page->buffer,"insert");
									gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&cursiter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
									if(!gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&cursiter,0,true,0,0.5))
										gtk_text_view_scroll_to_mark((GtkTextView*)page->view,mark,0,true,0,0.5);
								}

							currentpage++;
						}
					else
						{
							intarg=999;
							fgets(buffer,2048,fd);
							sscanf(buffer,"%i",(int*)&intarg);
							while(intarg!=-1)
								{
									fgets(buffer,2048,fd);
									sscanf(buffer,"%i",(int*)&intarg);
								}

						}
				}
			fclose(fd);
			debugFree(filename,"restoreSession filename");
		}
}

void restoreSessionNum(GtkWidget* widget,gpointer data)
{
	char*		sessionfile;
	char*		sname=NULL;
	FILE*		fd=NULL;
	const char*	widgetname=NULL;

	widgetname=gtk_widget_get_name(widget);
	for(int j=0; j<MAXSESSIONS; j++)
		{
			asprintf(&sessionfile,"%s/.KKEdit/session-%i",getenv("HOME"),j);
			fd=fopen(sessionfile,"r");
			if(fd!=NULL)
				{
					fscanf(fd,"%a[^\n]s",&sname);
					if(strcmp(sname,widgetname)==0)
						{
							free(sname);
							fclose(fd);
							restoreSessionFromFile(sessionfile);
							return;
						}
					free(sname);
					fclose(fd);
				}
		}
}

void rebuildMainMenu(GtkWidget* menu,plugData*	plugdata,GCallback* func)
{
	GtkWidget*	menuitem;
	char*		sessionname;
	GtkWidget*	submenu;

	submenu=gtk_menu_item_get_submenu(GTK_MENU_ITEM(menu));
	gtk_widget_destroy(submenu);
	submenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu),submenu);

	for(int j=0; j<MAXSESSIONS; j++)
		{
			asprintf(&sessionname,"%s",sessionNames[j]);
			menuitem=gtk_menu_item_new_with_label(sessionname);
			gtk_widget_set_name(menuitem,sessionNames[j]);
			free(sessionname);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(func),plugdata);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuitem);
		}
	gtk_widget_show_all(menu);
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

			for(int j=0; j<MAXSESSIONS; j++)
				{
					asprintf(&sessionname,"%s",sessionNames[j]);
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

			for(int j=0; j<MAXSESSIONS; j++)
				{
					asprintf(&sessionname,"%s",sessionNames[j]);
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
	gtk_about_dialog_set_version(about,PLUGVERSION);
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
			return(1);
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	return(0);
}

