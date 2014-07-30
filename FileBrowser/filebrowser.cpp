/*
  filebrowser.cpp
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

#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "0.0.6"
#define NUM_COLUMNS 3
#define COLUMN_FILENAME 1
#define COLUMN_PATHNAME 2
#define COLUMN_PIXBUF 0

int	(*module_plug_function)(gpointer globaldata);

GtkWidget*		leftButton;
GtkTreeStore*	store;
GtkWidget*		treeview;
GtkWidget*		scrollbox;
GtkWidget*		leftBox;
bool			flag=false;
GtkWidget*		hideMenu;
bool			showing;
bool			colflag=false;
int				colsize=0;
bool			showInvisible;

GdkPixbuf* getPixBuf(char* name)
{
	GFile*				file=g_file_new_for_path(name);
	GFileInfo*			file_info=g_file_query_info(file,"standard::*",G_FILE_QUERY_INFO_NONE,NULL,NULL);
	GIcon*				icon=g_file_info_get_icon(file_info);
	GdkPixbuf*			pix=NULL;
	gchar*				path;
	gchar const* const*	names;
	GFile*				icon_file;
	char*				newname;

	if(G_IS_THEMED_ICON(icon))
		{
			names=g_themed_icon_get_names(G_THEMED_ICON(icon));
			pix=gtk_icon_theme_load_icon(gtk_icon_theme_get_default(),*names,16,(GtkIconLookupFlags)(GTK_ICON_LOOKUP_USE_BUILTIN|GTK_ICON_LOOKUP_FORCE_SVG|GTK_ICON_LOOKUP_GENERIC_FALLBACK|GTK_ICON_LOOKUP_FORCE_SIZE),NULL);
			if(pix==NULL)
				{
					asprintf(&newname,"gnome-mime-%s",*names);
					pix=gtk_icon_theme_load_icon(gtk_icon_theme_get_default(),(const gchar*)newname,16,(GtkIconLookupFlags)(GTK_ICON_LOOKUP_USE_BUILTIN|GTK_ICON_LOOKUP_FORCE_SVG|GTK_ICON_LOOKUP_GENERIC_FALLBACK|GTK_ICON_LOOKUP_FORCE_SIZE),NULL);
					debugFree(newname,"getPixBuf newname");
				}
		}
	else if(G_IS_FILE_ICON(icon))
		{
			icon_file=g_file_icon_get_file(G_FILE_ICON(icon));
			path=g_file_get_path(icon_file);
			pix=gdk_pixbuf_new_from_file_at_size(path,16,16,NULL);
			debugFree(path,"getPixBuf path");
			g_object_unref(G_OBJECT(icon_file));
		}
	g_object_unref(G_OBJECT(file));
	g_object_unref(G_OBJECT(file_info));

   return(pix);
}

void addFolderContents(const char* folder,GtkTreeIter* iter,bool root)
{
	GtkTreeIter child_iter;

	gtk_tree_store_append ((GtkTreeStore*)store,iter,NULL);
	gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,"/",COLUMN_PATHNAME,"/",COLUMN_PIXBUF,getPixBuf((char*)"/"),-1);

	gtk_tree_store_append(store,&child_iter,iter);
	gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,"",-1);
}

void addToIter(GtkTreeView* treeview,char* filename,GtkTreeIter* iter,char* folderpath)
{
	GtkTreeModel*		model;
	GtkTreeIter			childiter;
	GtkTreeIter			parentiter;
	char*				pathname;
	
	if(strcasecmp("/",folderpath)!=0)
		asprintf(&pathname,"%s/%s",folderpath,filename);
	else
		asprintf(&pathname,"/%s",filename);

	model=gtk_tree_view_get_model(treeview);
	gtk_tree_model_iter_parent(model,&parentiter,iter);
	childiter=*iter;

	gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,filename,COLUMN_PATHNAME,pathname,COLUMN_PIXBUF,getPixBuf(pathname),-1);
	if((gtk_tree_model_iter_next(model,&childiter)==false) && (g_file_test(pathname,G_FILE_TEST_IS_DIR)))
		gtk_tree_store_append((GtkTreeStore*)store,&childiter,iter);
}

bool addContents(GtkTreeView* treeview,GtkTreeIter* iter,char* name)
{
	FILE*			fp=NULL;
	char			line[1024];
	char*			command;
	GtkTreeModel*	model;
	GtkTreeIter		parentiter;

	model=gtk_tree_view_get_model(treeview);
	gtk_tree_model_iter_parent(model,&parentiter,iter);

	asprintf(&command,"ls -1 \"%s\"|sort",name);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					line[strlen(line)-1]=0;
					addToIter(treeview,line,iter,name);
					if(gtk_tree_model_iter_next(model,iter)==false)
						gtk_tree_store_append((GtkTreeStore*)store,iter,&parentiter);
				}
			pclose(fp);
		}
	return(gtk_tree_store_remove((GtkTreeStore*)store,iter));
}

void expandRow(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
{
	GtkTreeModel*		model;
	bool				gotchild;
	GtkTreeIter			childiter;
	char*				folder;
	GtkTreeIter*		cleariter;
	GtkTreeIter			childchild;

	model=gtk_tree_view_get_model(treeview);
	gotchild=gtk_tree_model_iter_children(model,&childiter,iter);
	gtk_tree_model_get(model,iter,COLUMN_PATHNAME,&folder,-1);
	cleariter=gtk_tree_iter_copy(&childiter);

	gtk_tree_model_iter_next(model,cleariter);
	if(gtk_tree_store_iter_is_valid((GtkTreeStore*)store,cleariter))
		while(gtk_tree_store_remove((GtkTreeStore*)store,cleariter));

	cleariter=gtk_tree_iter_copy(&childiter);
	gtk_tree_model_iter_children(model,&childchild,cleariter);
	if(gtk_tree_store_iter_is_valid((GtkTreeStore*)store,&childchild))
		gtk_tree_store_remove((GtkTreeStore*)store,&childchild);

	while(gotchild)
		{
			if(addContents(treeview,&childiter,(char*)folder))
				gotchild=gtk_tree_model_iter_next(model,&childiter);
			else
				gotchild=false;
		}
	flag=true;
}

void collapseRow(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
{
	char*	pathstring=gtk_tree_path_to_string(path);

	if(strcmp(pathstring,"0")==0)
		flag=false;
	debugFree(pathstring,"pathstring");
}

void onColWidthChange(GObject* gobject,GParamSpec* pspec,gpointer data)
{
	gint intval;
	g_object_get(data,"width",&intval,NULL);

	if(flag==true)
		{
			if(colflag==false)
				{
					colsize=intval+16;
					colflag=true;
					return;
				}
			colsize=colsize+intval;
			colflag=false;
			gtk_widget_set_size_request((GtkWidget*)leftBox,colsize,-1);
		}
	else
		gtk_widget_set_size_request((GtkWidget*)leftBox,100,-1);
}

const char* getApp(char* name)
{
	GFile*		file=g_file_new_for_path(name);
	GFileInfo*	file_info=g_file_query_info(file,"standard::*",G_FILE_QUERY_INFO_NONE,NULL,NULL);

    const char*	content_type=g_file_info_get_content_type(file_info);
    GAppInfo*	app_info=g_app_info_get_default_for_type(content_type,false);

	return(g_app_info_get_executable(app_info));
}

void onRowActivated(GtkTreeView* treeview, GtkTreePath* path,GtkTreeViewColumn* col,gpointer userdata)
{
	GtkTreeModel*	model;
	GtkTreeIter		iter;
	gchar*			name;
	char*			command;
	GdkModifierType	mask;
	const char*		app;

	gdk_window_get_pointer(NULL,NULL,NULL,&mask);

	model=gtk_tree_view_get_model(treeview);

	if(gtk_tree_model_get_iter(model,&iter,path))
		{
			gtk_tree_model_get(model,&iter,COLUMN_PATHNAME,&name,-1);
			app=getApp(name);
			asprintf(&command,"%s '%s'",app,name);
			
			if (GDK_CONTROL_MASK & mask )
				runCommand(command,NULL,false,TOOL_ASYNC,true,NULL);
			else
				runCommand(command,NULL,false,TOOL_ASYNC,false,NULL);
				
			debugFree(command,"comand from runcommand");
			debugFree(name,"name from runcommand");
		}
}

void touch(char* path)
{
	int	fd;

	fd=open(path,O_WRONLY|O_CREAT,DEFFILEMODE);
	if(fd!=-1)
		close(fd);
}

void showHideBrowser(plugData* pdata,bool startup)
{
	char*	filepath;

	asprintf(&filepath,"%s/filebrowser.rc",pdata->lPlugFolder);
	if(showing==true)
		{
			gtk_widget_show_all(scrollbox);
			if(pdata->leftShow==0 && startup==false)
				showSide(true);
			touch(filepath);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,"Hide Browser");
		}
	else
		{
			gtk_widget_hide(scrollbox);
			unlink(filepath);
			if(pdata->leftShow==1 && startup==false)
				hideSide(true);
			gtk_menu_item_set_label((GtkMenuItem*)hideMenu,"Show Browser");
		}
	debugFree(filepath,"filepath");
}

void toggleBrowser(GtkWidget* widget,gpointer data)
{
	showing=!showing;
	showHideBrowser((plugData*)data,false);
}

extern "C" int setSensitive(gpointer data)
{
	return(0);
}

extern "C" int switchTab(gpointer data)
{
	return(0);
}

void doStartUpCheck(plugData* pdata)
{
	char*	filepath;

	asprintf(&filepath,"%s/filebrowser.rc",pdata->lPlugFolder);
	if(g_file_test(filepath,G_FILE_TEST_EXISTS))
		showing=true;
	else
		showing=false;

	debugFree(filepath,"filepath");
}
                                                        
extern "C" int addToGui(gpointer data)
{
	plugData*			plugdata=(plugData*)data;
	GtkTreeViewColumn*	column;
	GtkTreeModel*		model=NULL;
	GtkCellRenderer*	renderer;
	GtkTreeIter			iter;
	GtkWidget*			menu;

	menu=gtk_menu_item_get_submenu((GtkMenuItem*)plugdata->mlist.menuView);
	hideMenu=gtk_menu_item_new_with_label("Hide Browser");
	gtk_signal_connect(GTK_OBJECT(hideMenu),"activate",G_CALLBACK(toggleBrowser),plugdata);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),hideMenu);
	gtk_widget_show_all(plugdata->mlist.menuView);

	store=gtk_tree_store_new(NUM_COLUMNS,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);
	addFolderContents("/",&iter,true);
	model=GTK_TREE_MODEL(store);
	treeview=gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_enable_tree_lines((GtkTreeView*)treeview,true);
	scrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrollbox,GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);

	gtk_container_add(GTK_CONTAINER(scrollbox),(GtkWidget*)treeview);
	gtk_container_add(GTK_CONTAINER(plugdata->leftUserBox),(GtkWidget*)scrollbox);

//pixbuf
	renderer=gtk_cell_renderer_pixbuf_new();
	column=gtk_tree_view_column_new_with_attributes("path",renderer,"pixbuf",COLUMN_PIXBUF,NULL);
	gtk_tree_view_column_set_resizable(column,true);
	gtk_tree_view_column_set_sizing(column,GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);
	gtk_tree_view_column_set_spacing(column,0);
	gtk_tree_view_column_set_expand(column,false);
	g_signal_connect((GtkWidget*)column,"notify::width",G_CALLBACK(onColWidthChange),column);

//colom file
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes(NULL,renderer,"text",COLUMN_FILENAME,NULL);
	gtk_tree_view_column_set_resizable(column,true);
	gtk_tree_view_column_set_sizing(column,GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);
	gtk_tree_view_column_set_expand(column,true);
	gtk_tree_view_column_set_spacing    (column,0);
	g_signal_connect((GtkWidget*)column,"notify::width",G_CALLBACK(onColWidthChange),column);

	gtk_tree_view_set_headers_visible((GtkTreeView*)treeview,false);

	g_signal_connect(treeview,"row-expanded",G_CALLBACK(expandRow),column);
	g_signal_connect(treeview,"row-collapsed",G_CALLBACK(collapseRow),column);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(onRowActivated),NULL);

	leftBox=(GtkWidget*)plugdata->leftUserBox;
	doStartUpCheck(plugdata);
	showHideBrowser(plugdata,true);
	showSide(true);
	return(0);
}

extern "C" int doAbout(gpointer data)
{
	plugData*		plugdata=(plugData*)data;
	char*			licencepath;
	const char		copyright[] ="Copyright \xc2\xa9 2014 K.D.Hedger";
	const char*		aboutboxstring="FileBrowser";
	char*			licence;
	GtkAboutDialog*	about;

	const char*	authors[]= {"K.D.Hedger <"MYEMAIL">\n",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};

	asprintf(&licencepath,"%s/docs/gpl-3.0.txt",plugdata->dataDir);

	g_file_get_contents(licencepath,&licence,NULL,NULL);
	about=(GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about,"FileBrowser");
	gtk_about_dialog_set_authors(about,authors);
	gtk_about_dialog_set_comments(about,aboutboxstring);
	gtk_about_dialog_set_copyright(about,copyright);
	gtk_about_dialog_set_version(about,VERSION);
	gtk_about_dialog_set_website(about,MYWEBSITE);
	gtk_about_dialog_set_logo_icon_name(about,"KKEditPlug");
	gtk_about_dialog_set_license(about,licence);

	gtk_dialog_run(GTK_DIALOG(about));
	gtk_widget_destroy((GtkWidget*)about);
	debugFree(licence,"licence");
	debugFree(licencepath,"licencepath");
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(scrollbox);
			gtk_widget_destroy(hideMenu);
			hideSide(true);
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
		}
	return(0);
}
