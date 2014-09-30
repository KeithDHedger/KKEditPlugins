/*
  openuri.cpp
*/

#include <stdlib.h>
//#include <glib.h>
#include <gtk/gtk.h>
#include <gmodule.h>

#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "0.0.1"

GtkWidget*	menuPlug;

int	(*module_plug_function)(gpointer globaldata);

extern "C" const gchar* g_module_check_init(GModule *module)
{
	return(NULL);
}

extern "C" const gchar* g_module_unload(GModule *module)
{
	return(NULL);
}

void theCallBack(GtkWidget* widget,gpointer data)
{
	char*		command=NULL;
	GtkTextIter	start;
	GtkTextIter	end;
	char*		text;
	pageStruct*	page=NULL;

	page=getPageStructPtr(-1);
	if(page==NULL)
		return;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			asprintf(&command,"xdg-open %s",text);
			system(command);
			free(command);
			free(text);
		}
}

extern "C" int addToContext(gpointer data)
{
	GtkWidget*	menuitem;
	GtkWidget*	image;
	plugData*	plugdata=(plugData*)data;

	menuitem=gtk_image_menu_item_new_with_label("Open Selection");
	image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(plugdata->contextPopUpMenu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(theCallBack),(void*)plugdata);

	return(0);
}

extern "C" int addToGui(gpointer data)
{
	GtkWidget*	submenu=NULL;
	GtkWidget*	image;

	plugData*	plugdata=(plugData*)data;

	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuNav);
	if(submenu!=NULL)
		{
			menuPlug=gtk_image_menu_item_new_with_label("Open Selection");
			image=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_MENU);
			gtk_image_menu_item_set_image((GtkImageMenuItem *)menuPlug,image);
			gtk_signal_connect(GTK_OBJECT(menuPlug),"activate",G_CALLBACK(theCallBack),plugdata);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuPlug);
		}
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*		aboutboxstring="Open Uri from selection";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"OpenUri");
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
