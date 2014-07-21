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

#include <kkedit-plugins.h>

#define MYEMAIL "kdhedger68713@gmail.com"
#define MYWEBSITE "http://keithhedger.hostingsiteforfree.com/index.html"
#define VERSION "0.0.4"
#define NUM_COLUMNS 2
#define COLUMN_FILENAME 0
#define COLUMN_PATHNAME 1

int	(*module_plug_function)(gpointer globaldata);
GtkWidget*		leftButton;
GtkTreeStore*	store;
GtkWidget*		treeview;
char*			folderPath;
GtkWidget*		scrollbox;
GtkWidget*		leftBox;
bool			flag=false;

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

void addFolderContentsXX(char* folder,GtkTreeIter* iter,bool root)
{
	FILE*		fp=NULL;
	char		line[1024];
	char*		command;
	GtkTreeIter child_iter;
	char*		pathname;

	asprintf(&command,"ls -1 %s|sort",folder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					line[strlen(line)-1]=0;
					asprintf(&pathname,"%s%s",folder,line);
					gtk_tree_store_append ((GtkTreeStore*)store,iter,NULL);
					gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,line,COLUMN_PATHNAME,pathname,-1);
					if((g_file_test(pathname,G_FILE_TEST_IS_DIR)==true))
						{
							gtk_tree_store_append(store,&child_iter,iter);
							gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,line,-1);
						}
				}
			pclose(fp);
		}
}

void addFolderContents(char* folder,GtkTreeIter* iter,bool root)
{
	GtkTreeIter child_iter;

	gtk_tree_store_append ((GtkTreeStore*)store,iter,NULL);
	gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,"/",COLUMN_PATHNAME,"/",-1);

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

	gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,filename,COLUMN_PATHNAME,pathname,-1);
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

	asprintf(&command,"ls -1 %s|sort",name);
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
	free(pathstring);
}

void onColWidthChange(GObject* gobject,GParamSpec* pspec,gpointer data)
{
	gint intval;

	g_object_get(data,"width",&intval,NULL);
	if(flag==true)
		gtk_widget_set_size_request((GtkWidget*)leftBox,intval+16,-1);
	else
		gtk_widget_set_size_request((GtkWidget*)leftBox,100,-1);
}

void onRowActivated (GtkTreeView        *treeview,
                       GtkTreePath        *path,
                       GtkTreeViewColumn  *col,
                       gpointer            userdata)
  {
    GtkTreeModel *model;
    GtkTreeIter   iter;
 
    g_print ("A row has been double-clicked!\n");
 
    model = gtk_tree_view_get_model(treeview);
 
    if (gtk_tree_model_get_iter(model, &iter, path))
    {
       gchar *name;
 
       gtk_tree_model_get(model, &iter, COLUMN_PATHNAME, &name, -1);
 
       g_print ("Double-clicked row contains name %s\n", name);
 
       g_free(name);
    }
}
  
extern "C" int addToGui(gpointer data)
{
	plugData*			plugdata=(plugData*)data;
	GtkTreeViewColumn*	column;
	GtkTreeModel*		model=NULL;
	GtkCellRenderer*	renderer;
	GtkTreeIter			iter;

	folderPath=strdup("/");
	store=gtk_tree_store_new(NUM_COLUMNS,G_TYPE_STRING,G_TYPE_STRING);
	addFolderContents(folderPath,&iter,true);
	model=GTK_TREE_MODEL(store);
	treeview=gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_enable_tree_lines((GtkTreeView*)treeview,true);
	scrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrollbox,GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);

	gtk_container_add(GTK_CONTAINER(scrollbox),(GtkWidget*)treeview);
	gtk_container_add(GTK_CONTAINER(plugdata->leftUserBox),(GtkWidget*)scrollbox);

//colom
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("StarBug",renderer,"text",COLUMN_FILENAME,NULL);
	gtk_tree_view_column_set_resizable (column,true);
	gtk_tree_view_column_set_sizing(column,GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);

	gtk_widget_show_all((GtkWidget*)plugdata->leftUserBox);
	g_signal_connect(treeview,"row-expanded",G_CALLBACK(expandRow),column);
	g_signal_connect(treeview,"row-collapsed",G_CALLBACK(collapseRow),column);
	g_signal_connect((GtkWidget*)column,"notify::width",G_CALLBACK(onColWidthChange),column);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(onRowActivated),NULL);

	leftBox=(GtkWidget*)plugdata->leftUserBox;
	return(0);
}

//extern "C" int plugPrefs(gpointer data)
//{
//	GtkWidget*	dialog;
//	GtkWidget*	dialogbox;
//	GtkWidget*	projects;
//	GtkWidget*	svn;
//	GtkWidget*	vbox;
//	int			response;
//	char*		command;
//	plugData*	plugdata=(plugData*)data;
//
//	vbox=gtk_vbox_new(false,0);
//
//	dialog=gtk_dialog_new_with_buttons("FileBrowser",NULL,GTK_DIALOG_MODAL,GTK_STOCK_APPLY,GTK_RESPONSE_APPLY,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
//	gtk_window_set_default_size((GtkWindow*)dialog,300,120);
//	dialogbox=gtk_dialog_get_content_area((GtkDialog*)dialog);
//	gtk_container_add(GTK_CONTAINER(dialogbox),vbox);
//
//	projects=gtk_entry_new();
//	svn=gtk_entry_new();
//
//	gtk_entry_set_text((GtkEntry*)projects,projectsPath);
//	gtk_entry_set_text((GtkEntry*)svn,SVNRepoPath);
//	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Projects Folder"),true,true,4);
//	gtk_box_pack_start((GtkBox*)vbox,projects,true,true,4);
//	gtk_box_pack_start((GtkBox*)vbox,gtk_label_new("Subversion Folder"),true,true,4);
//	gtk_box_pack_start((GtkBox*)vbox,svn,true,true,4);
//
//	gtk_widget_show_all(dialog);
//	response=gtk_dialog_run(GTK_DIALOG(dialog));
//	if(response==GTK_RESPONSE_APPLY);
//		{
//			asprintf(&command,"echo %s>%s/filebrowser.rc;echo %s>>%s/filebrowser.rc",gtk_entry_get_text((GtkEntry*)projects),plugdata->lPlugFolder,gtk_entry_get_text((GtkEntry*)svn),plugdata->lPlugFolder);
//			system(command);
//			free(command);
//		}
//	gtk_widget_destroy((GtkWidget*)dialog);
//	return(0);
//}

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
	free(licence);
	free(licencepath);
	return(0);
}

extern "C" int enablePlug(gpointer data)
{
	plugData*		plugdata=(plugData*)data;

	if(plugdata->modData->unload==true)
		{
			gtk_widget_destroy(scrollbox);
		}
	else
		{
			if(g_module_symbol(plugdata->modData->module,"addToGui",(gpointer*)&module_plug_function))
				module_plug_function(data);
		}
	return(0);
}
