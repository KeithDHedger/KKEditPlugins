/*
  clipboardplugin.cpp
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
struct clips
{
	char*				text;
};

int			maximumClips=-1;
char*		maxClips;
GtkWidget*	menuPlug;
GtkClipboard*	mainclipboard;
int				currentClip=0;
clips			clip[5];
bool		manual=false;

args		mydata[]=
				{
					{"maxclips",TYPEINT,&maximumClips},
					{NULL,0,NULL}
				};

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

void setCurrentClip(void)
{
	currentClip++;
	if(currentClip==maximumClips)
		currentClip=0;
}

void clipChanged(GtkClipboard* clipboard,gpointer user_data)
{
	if (manual==true)
		{
			manual=false;
			return;
		}
	else
		manual=true;

	if (gtk_clipboard_wait_is_text_available(mainclipboard)==true)
		{
			setCurrentClip();
			if(clip[currentClip].text != NULL)
				free(clip[currentClip].text);
			clip[currentClip].text=gtk_clipboard_wait_for_text(clipboard);
		}

	printf("%s\n",clip[currentClip].text);
}

void theCallBack(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;
	int			clipnum=atoi(gtk_widget_get_name(widget));
	printf("XXXXXXXXXX\n");
	printf("%s\n",gtk_widget_get_name(widget));

 gtk_text_buffer_insert_at_cursor((GtkTextBuffer*)plugdata->page->buffer,clip[clipnum-1].text,strlen(clip[clipnum-1].text));
//	gtk_text_buffer_paste_clipboard((GtkTextBuffer*)plugdata->page->buffer,
 //                                                        GtkClipboard *clipboard,
 //                                                        GtkTextIter *override_location,
 //                                                        gboolean default_editable);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menuitem;
	GtkWidget*	menu;
	char*		command;
	char		menuname[16];

	plugData*	plugdata=(plugData*)data;

	asprintf(&command,"%s/clipboardplugin.rc",plugdata->lPlugFolder);
	loadVarsFromFile(command,mydata);
	if(maximumClips==-1)
		maximumClips=5;
	free(command);

	menuPlug=gtk_menu_item_new_with_label("_Clipboard");
	gtk_menu_item_set_use_underline((GtkMenuItem*)menuPlug,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuPlug),menu);


	for(int j=1;j<=maximumClips;j++)
		{
			sprintf((char*)&menuname,"%i",j);
			asprintf(&command,"Clip No. %i",j);
			menuitem=gtk_menu_item_new_with_label(command);
			free(command);
			gtk_widget_set_name(menuitem,(char*)&menuname);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(theCallBack),plugdata);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
		}

	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->mlist.menuBar),menuPlug);					

	mainclipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	g_signal_connect(G_OBJECT(mainclipboard),"owner-change",G_CALLBACK(clipChanged),plugdata);
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
	char*		numclips[16];

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("ClipboardPlugin",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	prefs=gtk_entry_new();

	sprintf((char*)&numclips,"%i",maximumClips);
	gtk_entry_set_text((GtkEntry*)prefs,(char*)numclips);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Max Number of Clips"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,prefs,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY);
		{
			maximumClips=atoi(gtk_entry_get_text((GtkEntry*)prefs));
			asprintf(&command,"%s/clipboardplugin.rc",plugdata->lPlugFolder);
			saveVarsToFile(command,mydata);
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
	const char*		aboutboxstring="ClipboardPlugin";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

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
