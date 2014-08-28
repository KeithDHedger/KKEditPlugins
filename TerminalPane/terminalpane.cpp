/*
  terminalpane.cpp
*/

#include <stdio.h>
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

#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>

#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "https://sites.google.com/site/kkeditlinuxtexteditor"
#define VERSION "0.0.2"

int	(*module_plug_function)(gpointer globaldata);

GtkWidget*	terminal=NULL;
GtkWidget*	hideMenu=NULL;
bool		showing;
GtkWidget*	swindow;
char*		foreColour=strdup("#000000");
char*		backColour=strdup("#ffffff");
int			childPid;

void touch(char* path)
{
	int	fd;

	fd=open(path,O_WRONLY|O_CREAT,DEFFILEMODE);
	if(fd!=-1)
		close(fd);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	gtk_widget_hide(swindow);
	gtk_widget_destroy(swindow);
	return(NULL);
}

void doStartUpCheck(plugData* pdata)
{
	char*	filepath;

	asprintf(&filepath,"%s/terminal.rc",pdata->lPlugFolder);
	if(g_file_test(filepath,G_FILE_TEST_EXISTS))
		showing=true;
	else
		showing=false;

	debugFree(filepath,"filepath");
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

void showHideTerminal(plugData* pdata,bool startup)
{
	char*	filepath;

	asprintf(&filepath,"%s/terminal.rc",pdata->lPlugFolder);
	if(showing==true)
		{
//			if(pdata->leftShow==0 && startup==false)
			gtk_widget_show_all(pdata->bottomUserBox);
			touch(filepath);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,"Hide Terminal");
		}
	else
		{
			unlink(filepath);
//			if(pdata->leftShow==1 && startup==false)
			gtk_widget_hide(pdata->bottomUserBox);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,"Show Terminal");
		}
	debugFree(filepath,"filepath");
}

void toggleTerminal(GtkWidget* widget,gpointer data)
{
	showing=!showing;
	showHideTerminal((plugData*)data,false);
}

gboolean doButton(GtkWidget* widget,gpointer data)
{
	gtk_widget_set_can_focus(terminal,true);
	gtk_widget_grab_focus(terminal);

	return(false);
}

gboolean on_key_press (GtkWidget *terminal, GdkEventKey *event)
{
	if (event->state == (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
		{
			switch (event->keyval)
				{
				case GDK_C:
					vte_terminal_copy_clipboard (VTE_TERMINAL (terminal));
					return true;
				case GDK_V:
					vte_terminal_paste_clipboard (VTE_TERMINAL (terminal));
					return true;
				}
		}
	return false;
}

args mydata[]={
					{"forecol",2,&foreColour},
					{"backcol",2,&backColour},
					{NULL,0,NULL}
				  };

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	menu;
	plugData*	plugdata=(plugData*)data;
	GdkColor	colour;
	char*		startterm[2]={0,0};
	char*		filename;

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuView);
	hideMenu=gtk_menu_item_new_with_label("Hide Terminal");
	gtk_signal_connect(GTK_OBJECT(hideMenu),"activate",G_CALLBACK(toggleTerminal),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),hideMenu);
	gtk_widget_show_all(plugdata->mlist.menuView);

	terminal=vte_terminal_new();
	vte_terminal_set_default_colors((VteTerminal*)terminal);

	asprintf(&filename,"%s/terminalpane.rc",plugdata->lPlugFolder);
	loadVarsFromFile(filename,mydata);
	debugFree(filename,"readConfig filename");

	gdk_color_parse((const gchar*)foreColour,&colour);
	vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkColor*)&colour);

	gdk_color_parse((const gchar*)backColour,&colour);
	vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkColor*)&colour);

	swindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(swindow), terminal);
	gtk_container_add(GTK_CONTAINER(plugdata->bottomUserBox),swindow);

	g_signal_connect(terminal,"key-press-event",G_CALLBACK(on_key_press),NULL);
	g_signal_connect(terminal,"button-press-event",G_CALLBACK(doButton),NULL);
	vte_terminal_set_emulation((VteTerminal *)terminal,"xterm");

	startterm[0]=vte_get_user_shell();
	vte_terminal_fork_command_full((VteTerminal *)terminal,VTE_PTY_DEFAULT,NULL,startterm,NULL,(GSpawnFlags)(G_SPAWN_DEFAULT|G_SPAWN_LEAVE_DESCRIPTORS_OPEN),NULL,NULL,&childPid,NULL);

	doStartUpCheck(plugdata);
	showHideTerminal(plugdata,true);

	return(0);
}

extern "C" int plugPrefs(gpointer data)
{
#if 1
	GtkWidget*	dialog;
	GtkWidget*	dialogbox;
	GtkWidget*	fcolour;
	GtkWidget*	bcolour;
	GtkWidget*	vbox;
	int			response;
	char*		command;
	GdkColor	colour;

	plugData*	plugdata=(plugData*)data;

	vbox=gtk_vbox_new(false,0);

	dialog=gtk_dialog_new_with_buttons("TerminalPane",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);

	fcolour=gtk_entry_new();
	bcolour=gtk_entry_new();

	gtk_entry_set_text((GtkEntry*)fcolour,foreColour);
	gtk_entry_set_text((GtkEntry*)bcolour,backColour);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Foreground Colour"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,fcolour,true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Background Colour"),true,true,4);
	gtk_box_pack_start((GtkBox*)vbox,bcolour,true,true,4);

	gtk_widget_show_all(dialog);
	response=gtk_dialog_run(GTK_DIALOG(dialog));
	if(response==GTK_RESPONSE_APPLY);
		{
			asprintf(&command,"echo \"%s\">%s/terminalpane.rc;echo \"%s\">>%s/terminalpane.rc",gtk_entry_get_text((GtkEntry*)fcolour),plugdata->lPlugFolder,gtk_entry_get_text((GtkEntry*)bcolour),plugdata->lPlugFolder);
			system(command);
			free(command);
			vte_terminal_set_default_colors((VteTerminal*)terminal);
			gdk_color_parse((const gchar*)foreColour,&colour);
			vte_terminal_set_color_foreground((VteTerminal*)terminal,(const GdkColor*)&colour);
			gdk_color_parse((const gchar*)backColour,&colour);
			vte_terminal_set_color_background((VteTerminal*)terminal,(const GdkColor*)&colour);
			gtk_widget_show_all(terminal);
		}
	gtk_widget_destroy((GtkWidget*)dialog);
#endif
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*		aboutboxstring="A Simple Terminal Pane For KKEdit";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"TerminalPane");
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
			//gtk_widget_hide(plugdata->bottomUserBox);
			//gtk_widget_destroy(swindow);
			//gtk_widget_destroy(hideMenu);
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
