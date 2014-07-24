/*
  plugs/kkedit-example-plug.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <string.h>

#include "kkedit-plugins.h"
#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "1.0.1"

int	(*module_plug_function)(gpointer globaldata);

GtkWidget*	examplemenu;
GtkWidget*	leftButton;
GtkWidget*	rightButton;
GtkWidget*	topLabel;
GtkWidget*	bottomLabel;

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
	return(NULL);
}

//run when module unloaded, not really a lot of use.
extern "C" const gchar* g_module_unload(GModule *module)
{
	perror("doin cleanup");
	return(NULL);
}

//callback for the two buttons added by the demo
void clickButton(GtkWidget* widget,gpointer data)
{
	runCommandAndOut(gtk_widget_get_name(widget),(plugData*)data);
}

//open the plug help in the doc viewer or browser from the menu item added by the demo
void openPlugHelp(GtkWidget* widget,gpointer data)
{
	plugData*	pdata=(plugData*)data;
	asprintf(pdata->thePage,"file://%s/example-plugin/plughelp.html",(char*)pdata->gPlugFolder);
	showDocView(USEURI,(char*)"KKEdit Plugin Help",(char*)"KKEdit Plugin Help");
}

//main function called after KKEdit has built it's main GUI to add extra GUI items.
extern "C" int addToGui(gpointer data)
{
	GtkWidget*		menu;
	plugData*		plugdata=(plugData*)data;

	printf("Adding to GUI from example-plugin\n");

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuHelp);
	examplemenu=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,NULL);
	gtk_menu_item_set_label((GtkMenuItem*)examplemenu,"Plugin Help");
	gtk_signal_connect(GTK_OBJECT(examplemenu),"activate",G_CALLBACK(openPlugHelp),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),examplemenu);

	leftButton=gtk_button_new_with_label("left side button\nat top");
	gtk_box_pack_start(GTK_BOX(plugdata->leftUserBox),leftButton,false,false,0);
	gtk_widget_set_name(leftButton,"echo Left Button Clicked");
	gtk_signal_connect(GTK_OBJECT(leftButton),"clicked",G_CALLBACK(clickButton),plugdata);

	gtk_widget_show(leftButton);
	//setVisiblity(true,true);
	gtk_widget_show_all(plugdata->leftUserBox);
	leftVisibleRef(true);

	rightButton=gtk_button_new_with_label("right side button\nat bottom");
	gtk_box_pack_end(GTK_BOX(plugdata->rightUserBox),rightButton,false,false,0);
	//whattoop="echo Right Button Clicked";
	gtk_widget_set_name(rightButton,"echo Right Button Clicked");
	gtk_signal_connect(GTK_OBJECT(rightButton),"clicked",G_CALLBACK(clickButton),plugdata);
	gtk_widget_show_all(plugdata->rightUserBox);

	topLabel=gtk_label_new("Top user vbox demo label\nSelect 'Edit->Plugin Prefs' to disable this plugin from the dialog box.\nUnselect 'kkedit-example-plug and click 'Apply'");
	gtk_box_pack_end(GTK_BOX(plugdata->topUserBox),topLabel,true,true,0);
	gtk_widget_show_all(plugdata->topUserBox);

	bottomLabel=gtk_label_new("Bottom user vbox demo label");
	gtk_box_pack_end(GTK_BOX(plugdata->bottomUserBox),bottomLabel,true,true,0);
	gtk_widget_show_all(plugdata->bottomUserBox);


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
	const char	copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*	aboutboxstring="KKEdit Project Plugin";
	char*		licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"KKEdit Example Plugin");
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
			//gtk_widget_hide(plugdata->leftUserBox);
			setVisiblity(false,true);
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
