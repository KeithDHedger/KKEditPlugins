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
#define VERSION "0.0.1"
#define NUM_COLUMNS 1
#define COLUMN_FILENAME 0

int	(*module_plug_function)(gpointer globaldata);
GtkWidget*		leftButton;
GtkTreeStore*	store;
GtkWidget*		treeview;
char*			folderPath;
GtkWidget*		scrollbox;

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

void theCallBack(GtkWidget* widget,gpointer data)
{
	plugData*	plugdata=(plugData*)data;

	showToolOutput(true);
	runCommandAndOut((char*)"echo clicked",plugdata);
}

void addFolderContents(char* folder,GtkTreeIter* iter,bool root)
{
//	GtkTreeIter		iter;
	FILE*		fp=NULL;
	char		line[1024];
//	GtkTextIter	iter;
	char*		command;
	GtkTreeIter child_iter;
	GtkTreeModel*		model;
GtkTreePath *	path;

	asprintf(&command,"find %s -maxdepth 1|sort",folder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					line[strlen(line)-1]=0;
//					if(root==true)
//					{
					gtk_tree_store_append ((GtkTreeStore*)store,iter,NULL);
					gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,line,-1);
//					if((g_file_test(line,G_FILE_TEST_IS_DIR)==true) || (g_file_test(line,G_FILE_TEST_IS_DIR)==true && g_file_test(line,G_FILE_TEST_IS_SYMLINK)==true))
//						{
							gtk_tree_store_append(store,&child_iter,iter);
							gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,line,-1);
//						}
//					}
//					else
//						{
//							//gtk_tree_store_clear(store);
//	model=gtk_tree_view_get_model((GtkTreeView*)treeview);
//							gtk_tree_model_iter_children(model,&child_iter,iter);
////							gtk_tree_store_prepend(store,&child_iter,iter);
//							//GtkTreeIter  iter;
// //path= gtk_tree_model_get_path(model,&child_iter);
//   //        while(gtk_tree_model_get_iter(model, &child_iter, path))
//     //      {
//          //   gtk_tree_store_remove(store, &child_iter);
//       //    }
//           //gtk_tree_store_append(store,iter,NULL);
////           gtk_tree_store_remove(store, &child_iter);
////							while(gtk_tree_store_remove(store,&child_iter));
//							//gtk_tree_store_remove
////gtk_tree_store_append(store,&child_iter,iter);
//						//	gtk_tree_store_append(store,&child_iter,iter);
//							
//							//gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,line,-1);
//							gtk_tree_store_append(store,&child_iter,iter);
//							
//							gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,line,-1);
//							//printf("%sn",line);
//							//gtk_tree_view_expand_all            ((GtkTreeView*)treeview);
//						}
				}
			pclose(fp);
		}

//	plugdata=(moduleData*)data;
//	gtk_tree_store_append ((GtkTreeStore*)store,&iter,NULL);
//	gtk_tree_store_set((GtkTreeStore*)store,&iter,COLUMN_FILENAME,"data 1",-1);
//	gtk_tree_store_append ((GtkTreeStore*)store,&iter,NULL);
//	gtk_tree_store_set((GtkTreeStore*)store,&iter,COLUMN_FILENAME,"data 2",-1);
//	gtk_tree_store_append ((GtkTreeStore*)store,&iter,NULL);
//	gtk_tree_store_set((GtkTreeStore*)store,&iter,COLUMN_FILENAME,"data 3",-1);
//
//GtkTreeIter child_iter;
//
//	 gtk_tree_store_append (store, &child_iter, &iter);
////	gtk_tree_store_append ((GtkTreeStore*)store,&iter,NULL);
//	gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,"data 4",-1);
	
//	gtk_list_store_append((GtkListStore*)store,&iter);
//	gtk_list_store_set((GtkListStore*)store,&iter,COLUMN_FILENAME,"data 1",-1);
//	gtk_list_store_append((GtkListStore*)store,&iter);
//	
//
//	gtk_list_store_set((GtkListStore*)store,&iter,COLUMN_FILENAME,"data 2",-1);
//	gtk_list_store_append((GtkListStore*)store,&iter);
//	gtk_list_store_set((GtkListStore*)store,&iter,COLUMN_FILENAME,"data 3",-1);
}

gboolean foreach_func (GtkTreeModel *model,
                GtkTreePath  *path,
                GtkTreeIter  *iter,
                GList       **rowref_list)
  {
    guint  year_of_birth;
 
    g_assert ( rowref_list != NULL );
 
 //   gtk_tree_model_get (model, iter, COL_YEAR_BORN, &year_of_birth, -1);
 
//    if ( year_of_birth > 1980 )
//    {
      GtkTreeRowReference  *rowref;
 
      rowref = gtk_tree_row_reference_new(model, path);
 
      *rowref_list = g_list_append(*rowref_list, rowref);
 //   }
 
    return FALSE; /* do not stop walking the store, call us with next row */
  }

//void expandRow(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
//{
//	GtkTreeIter real_it;
//	GtkTreePath* real_path;
//	GtkTreeModel* filter = gtk_tree_view_get_model( treeview );
//  //  PtkDirTree* tree = PTK_DIR_TREE( user_data );
//	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER( filter ), &real_it, iter );
////    real_path = gtk_tree_model_filter_convert_path_to_child_path(GTK_TREE_MODEL_FILTER( filter ),path );
//    //ptk_dir_tree_expand_row( tree, &real_it, real_path );
// //   gtk_tree_path_free( real_path );
//
//}
void expandRow2x(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
{

	GtkTreeModel*		model;
	bool				gotchild;
	GtkTreeIter			childiter;
	GtkTreeIter			parentiter;

	model=gtk_tree_view_get_model(treeview);
//	gtk_tree_model_iter_parent(model,&parentiter,iter);
//gtk_tree_model_iter_nth_child       (model,&childiter,&parentiter,0);
	//childiter=iter;
	//gotchild=gtk_tree_model_iter_next(model,&childiter);
//	gtk_tree_store_remove((GtkTreeStore*)store,iter);
	gotchild=gtk_tree_model_iter_children(model,&childiter,iter);
//gotchild=true;
//		printf("%s\n",gtk_tree_path_to_string(path));

	while(gotchild)
		{
		printf("YYYY\n");
		printf("%s\n",gtk_tree_path_to_string(path));
			gtk_tree_store_remove((GtkTreeStore*)store,&childiter);
			//gotchild=gtk_tree_model_iter_next(model,&childiter);
			gotchild=gtk_tree_model_iter_children(model,&childiter,iter);
		}

	gtk_tree_store_append((GtkTreeStore*)store,&childiter,iter);
	gtk_tree_store_set((GtkTreeStore*)store,&childiter,COLUMN_FILENAME,"XXXXXXXXXXX",-1);
}

void expandRow2(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
{

	GtkTreeModel*		model;
	bool				gotchild;
	GtkTreeIter			childiter;
	GtkTreeIter			parentiter;
	model=gtk_tree_view_get_model(treeview);
	gtk_tree_model_iter_parent(model,&parentiter,iter);
	childiter=*iter;
//	gtk_tree_store_append((GtkTreeStore*)store,iter,NULL);
	gtk_tree_store_set((GtkTreeStore*)store,iter,COLUMN_FILENAME,"XXXXXXXXXXX",-1);
	if(gtk_tree_model_iter_next(model,&childiter)==false)
		gtk_tree_store_append((GtkTreeStore*)store,&childiter,iter);
}


void expandRow(GtkTreeView* treeview,GtkTreeIter* iter,GtkTreePath* path,gpointer user_data)
{
	GtkTreeModel*		model;
	bool				gotchild;
	GtkTreeIter			childiter;
	GtkTreeIter			parentiter;

	model=gtk_tree_view_get_model(treeview);
	gotchild=gtk_tree_model_iter_children(model,&childiter,iter);
//	while(gtk_tree_store_remove((GtkTreeStore*)store,&childiter));
//	gtk_tree_store_append((GtkTreeStore*)store,&childiter,iter);
//	gtk_tree_store_set((GtkTreeStore*)store,&childiter,COLUMN_FILENAME,"XXXXXXXXXXX",-1);
//	gtk_tree_store_append((GtkTreeStore*)store,&childiter,iter);
//	gtk_tree_store_set((GtkTreeStore*)store,&childiter,COLUMN_FILENAME,"XXXXXXXXXXX",-1);
		printf("outer %s\n",gtk_tree_path_to_string(path));
	while(gotchild)
		{
		printf("XXXXX\n");
			//gtk_tree_model_get_iter_from_string(model,&childiter,"6:1");
			expandRow2(treeview,&childiter,path,user_data);
			gotchild=gtk_tree_model_iter_next(model,&childiter);
		}

//	gotchild=gtk_tree_model_iter_children(model,&childiter,iter);
//	while(gotchild)
//		{
//		printf("XXXXXn");
//			//gtk_tree_model_get_iter_from_string(model,&childiter,"6:1");
//			expandRow2(treeview,iter,path,user_data);
//			gotchild=gtk_tree_model_iter_next(model,iter);
//		}


//	gtk_tree_model_iter_parent(model,&parentiter,iter);
//	gtk_tree_model_iter_nth_child       (model,&childiter,&parentiter,0);
//	gtk_tree_store_remove((GtkTreeStore*)store,&childiter);

}

void expandRowXX(GtkTreeView* treeview,GtkTreeIter* iterXX,GtkTreePath* path,gpointer user_data)
{
	GtkTreeModel*		model;
	GtkTreeSelection*	selection=NULL;
	GtkTreeIter			iters;
	GtkTreeIter			child_iter;
	char*				folder;
	GtkTreeIter			c2;

//	selection=gtk_tree_view_get_selection(treeview);
//	if((selection!=NULL) && (gtk_tree_selection_get_selected(selection,&model,&iters)))
//		{
//			gtk_tree_model_get(model,&iters,COLUMN_FILENAME,&folder,-1);
	//folder=gtk_tree_path_to_string(path);
//	 gtk_tree_model_get(GtkTreeModel *tree_model,
 //                                                        GtkTreeIter *iter,
//   
printf("%s\n",gtk_tree_path_to_string(path));
	model=gtk_tree_view_get_model(treeview);
	gtk_tree_model_get(model,iterXX,COLUMN_FILENAME,&folder,-1);
	//gtk_tree_model_iter_children(model,&child_iter,iter);

//while(gtk_tree_store_remove((GtkTreeStore*)store,&child_iter));
//while(gtk_tree_store_remove((GtkTreeStore*)treeview,&child_iter)!=true);
////	gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,folder,-1);

//	gtk_tree_store_append(store,&child_iter,iter);
//	gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,folder,-1);
//	gtk_tree_store_append(store,&child_iter,iter);
//	gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,folder,-1);

//gtk_tree_model_get_iter_first       (tree_model,
 //                                                        GtkTreeIter *iter);
//	gtk_widget_show_all(scrollbox);

 
//  void
//  remove_people_born_after_1980 (void)
//  {
//     GList *rr_list = NULL;    /* list of GtkTreeRowReferences to remove */
//     GList *node;
// 
//     gtk_tree_model_foreach(GTK_TREE_MODEL(store),
//                            (GtkTreeModelForeachFunc) foreach_func,
//                            &rr_list);
// 
////     for ( node = rr_list;  node != NULL;  node = node->next )
////     {
// //       GtkTreePath *path;
// 
// //       path = gtk_tree_row_reference_get_path((GtkTreeRowReference*)node->data);
// 
//        if (path)
//        {
//           GtkTreeIter  iter;
// 
//           if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path))
//           {
//             gtk_list_store_remove(store, &iter);
//           }
// 
//           /* FIXME/CHECK: Do we need to free the path here? */
//        }
//     }
// 
//     g_list_foreach(rr_list, (GFunc) gtk_tree_row_reference_free, NULL);
//     g_list_free(rr_list);
//  }

GtkTreePath* childpath;
childpath=gtk_tree_path_copy(path);
gtk_tree_path_down(childpath);
printf("PATH %s\n",gtk_tree_path_to_string(childpath));
gtk_tree_model_get_iter(model,&c2,childpath);
//bool flag=true;
//
while(gtk_tree_model_get_iter(model,&c2,childpath)==true)
{
//if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &child_iter, path))
//           {
printf("PATH %s\n",gtk_tree_path_to_string(childpath));
            gtk_tree_store_remove(store,&c2);
            gtk_tree_path_next(childpath);
            
//           }
//          else
//          	flag=false;
}
// gtk_tree_path_down                  (path);
//	gtk_tree_model_get_iter(model,&c2,path);
//							gtk_tree_store_append(store,iter,NULL);
//
							gtk_tree_store_append(store,&child_iter,iterXX);
							gtk_tree_store_set((GtkTreeStore*)store,&child_iter,COLUMN_FILENAME,folder,-1);

printf("PATH EMPTY%s\n",gtk_tree_path_to_string(path));
//gtk_tree_model_get_iter(model,&c2,path);
//gtk_tree_model_get_iter_from_string (model,&c2,
 //                                                        const gchar *path_string);
//gtk_tree_store_append(store,&c2,iterXX);
//	addFolderContents(folder,iterXX,true);
	printf("%s\n",folder);
//		}
}

extern "C" int addToGui(gpointer data)
{
	plugData*	plugdata=(plugData*)data;
	GtkTreeViewColumn *column;
	GtkTreeModel*	model=NULL;
	GtkCellRenderer *renderer;
	GtkTreeIter		iter;

	folderPath=strdup("/");
	store=gtk_tree_store_new(NUM_COLUMNS,G_TYPE_STRING);
	addFolderContents(folderPath,&iter,true);
	model=GTK_TREE_MODEL(store);
	treeview=gtk_tree_view_new_with_model(model);
	scrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrollbox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

	gtk_widget_set_size_request((GtkWidget*)plugdata->leftUserBox,150,-1);
	gtk_container_add(GTK_CONTAINER(scrollbox),(GtkWidget*)treeview);
	gtk_container_add(GTK_CONTAINER(plugdata->leftUserBox),scrollbox);
//colom
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("Plug In",renderer,"text",COLUMN_FILENAME,NULL);
//	gtk_tree_view_column_set_sort_column_id(column,COLUMN_FILENAME);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);

//	leftButton=gtk_button_new_with_label("left side button\nat top");
//	gtk_box_pack_start(GTK_BOX(plugdata->leftUserBox),leftButton,false,false,0);
	
//	gtk_widget_set_name(leftButton,"echo Left Button Clicked");
//	gtk_signal_connect(GTK_OBJECT(leftButton),"clicked",G_CALLBACK(theCallBack),plugdata);
//	gtk_widget_show_all(plugdata->leftUserBox);
	gtk_widget_show_all((GtkWidget*)plugdata->leftUserBox);
//	g_signal_connect_after(treeview,"expand-collapse-cursor-row",G_CALLBACK(expandRow),NULL);
//	g_signal_connect(treeview,"toggle-cursor-row",G_CALLBACK(expandRow),NULL);
	g_signal_connect(treeview,"row-expanded",G_CALLBACK(expandRow),NULL);

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
