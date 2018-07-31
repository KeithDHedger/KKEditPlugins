/*
 *
 * ©K. D. Hedger. Tue 31 Jul 12:07:36 BST 2018 keithdhedger@gmail.com

 * This file (example-plugin.cpp) is part of KKEditPlugins.

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
#include <string.h>
#include <libintl.h>
#include <locale.h>

#include "../common.h"
#ifdef _DEVMODE_
#include "../../KKEdit/gtk-kkedit/KKEdit/src/kkedit-plugins.h"
#else
#include <kkedit-plugins.h>
#endif

#define VERSION "0.3.0"
#define TEXTDOMAIN "example-plugin"

int	(*module_plug_function)(gpointer globaldata);

GtkWidget*	examplemenu;
GtkWidget*	leftButton;
GtkWidget*	rightButton;
GtkWidget*	topLabel;
GtkWidget*	bottomLabel;
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

//example of how to run an external command and send the output to the tool output window in KKEdit
//command should be somthing like "ls /"
//plugdata is the standard data passed to functions in the plugin.
void runCommandAndOut(const char* command,plugData* plugdata)
{
	FILE*		fp=NULL;
	char		line[1024];
	GtkTextIter	iter;

	showToolOutput(true);
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

//run when module loaded, not really a lot of use.
extern "C" const gchar* g_module_check_init(GModule *module)
{
	perror("doin init");
	currentdomain=strdup(textdomain(NULL));
	return(NULL);
}

//run when module unloaded, not really a lot of use.
extern "C" void g_module_unload(GModule *module)
{
	perror("doin cleanup");
	return;
}

//callback for the two buttons added by the demo
void clickButton(GtkWidget* widget,gpointer data)
{
	runCommandAndOut(gtk_widget_get_name(widget),(plugData*)data);
}

//open the plug help in the doc viewer or browser from the menu item added by the demo
void openPlugHelp(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;

	asprintf(plugdata->thePage,"https://sites.google.com/site/kkeditlinuxtexteditor/kkedit-plugins");
	showDocView(USEURI,(char*)gettext("KKEdit Plugin Help"),(char*)gettext("KKEdit Plugin Help"));
}

//main function called after KKEdit has built it's main GUI to add extra GUI items.
extern "C" int addToGui(gpointer data)
{
	GtkWidget*		menu;
	plugData*		plugdata=(plugData*)data;

	setTextDomain(true,plugdata);
	printf("Adding to GUI from example-plugin\n");

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuHelp);
	examplemenu=createNewStockMenuItem(GTK_STOCK_NEW,GTK_STOCK_NEW);
	gtk_menu_item_set_label((GtkMenuItem*)examplemenu,gettext("Plugin Help"));
	g_signal_connect(G_OBJECT(examplemenu),"activate",G_CALLBACK(openPlugHelp),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),examplemenu);
	gtk_widget_show(examplemenu);

	leftButton=gtk_button_new_with_label(gettext("left side button\nat top"));
	gtk_box_pack_start(GTK_BOX(plugdata->leftUserBox),leftButton,false,false,0);
	gtk_widget_set_name(leftButton,gettext("echo Left Button Clicked"));
	g_signal_connect(G_OBJECT(leftButton),"clicked",G_CALLBACK(clickButton),plugdata);

	showSide(true);
	gtk_widget_show(leftButton);

	rightButton=gtk_button_new_with_label(gettext("right side button\nat bottom"));
	gtk_box_pack_end(GTK_BOX(plugdata->rightUserBox),rightButton,false,false,0);
	gtk_widget_set_name(rightButton,gettext("echo Right Button Clicked"));
	g_signal_connect(G_OBJECT(rightButton),"clicked",G_CALLBACK(clickButton),plugdata);

	showSide(false);
	gtk_widget_show(rightButton);

	topLabel=gtk_label_new(gettext("Top user vbox demo label\nSelect 'Edit->Plugin Prefs' to disable this plugin from the dialog box.\nUnselect 'kkedit-example-plug and click 'Apply'"));
	gtk_box_pack_end(GTK_BOX(plugdata->topUserBox),topLabel,true,true,0);
	gtk_widget_show_all(plugdata->topUserBox);

	bottomLabel=gtk_label_new(gettext("Bottom user vbox demo label"));
	gtk_box_pack_end(GTK_BOX(plugdata->bottomUserBox),bottomLabel,true,true,0);
	gtk_widget_show_all(plugdata->bottomUserBox);


	setTextDomain(false,plugdata);
	printf("Done adding GUI from example-plugin\n");
	return(0);
}

//run when buffer contents have changed etc ( as in when the save button is enabled/dimmed.
extern "C" int setSensitive(gpointer data)
{
	printf("set sensitive\n");
	return(0);
}

//run after openining a file
extern "C" int openFile(gpointer data)
{
	printf("open file \n");
	return(0);
}

//guess
extern "C" int saveFile(gpointer data)
{
	printf("save file\n");
	return(0);
}

//go on guess I dare ya
extern "C" int newFile(gpointer data)
{
	printf("new file\n");
	return(0);
}

//no wrong this is run just before a file is closed
extern "C" int closeFile(gpointer data)
{
	printf("close file\n");
	return(0);
}

//run when a new tab is created, runs before openfile/newfile.
extern "C" int newTab(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	printf("doing new tab\n");

	if(plugdata->page->filePath!=NULL)
		printf("filePath = %s\n",plugdata->page->filePath);

	if(plugdata->page->fileName!=NULL)
		printf("fileName = %s\n",plugdata->page->fileName);

	if(plugdata->page->dirName!=NULL)
		printf("dirName = %s\n",plugdata->page->dirName);
		
	printf("finished new tab\n");
	return(0);
}

//run when tabs switched either manually or ecase of opening a new tab.
extern "C" int switchTab(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	printf("switching tab\n");
	printf("new tab is %i\n",plugdata->currentTab);
	return(0);
}

//run from the 'Preferences' button in the pluginin prefs.
extern "C" int plugPrefs(gpointer data)
{
	printf("doing plugPrefs from example-plugin ...\n");
	return(0);
}

//run from the 'About' button in the pluginin prefs.
extern "C" int doAbout(gpointer data)
{
printf("about from example plug\n");
	plugData*	plugdata=(plugData*)data;
	char*		licencepath;
	const char	copyright[] ="Copyright ©2014 K.D.Hedger";
	char*		licence;
	GtkAboutDialog*	about;

	setTextDomain(true,plugdata);

	const char*	aboutboxstring=gettext("KKEdit Project Plugin - An example plugin");

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,gettext("KKEdit Example Plugin"));
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
	setTextDomain(false,plugdata);
	return(0);
}

//return 0 if its safe to unload module non zero to mark the plugin as disabled but don't unload it.
extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(examplemenu);
			gtk_widget_show_all(plugdata->mlist.menuBar);
			gtk_widget_destroy(leftButton);
			gtk_widget_destroy(rightButton);
			gtk_widget_destroy(topLabel);
			gtk_widget_destroy(bottomLabel);
			hideSide(true);
			hideSide(false);
			gtk_widget_hide(plugdata->rightUserBox);
		}
	else
		{
//when calling a 'standard' function like 'addToGui' from within the plugin itself get the actual symbol as below
//as you may have a number of plugins enbled with a similar function.
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
			gtk_widget_show_all(plugdata->mlist.menuBar);
		}
	printf("doing can enable from example-plug...\n");
	return(0);

}
