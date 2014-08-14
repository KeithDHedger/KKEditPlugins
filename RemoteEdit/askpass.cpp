/******************************************************
*
*     ©keithhedger Thu 14 Aug 14:57:06 BST 2014
*     kdhedger68713@gmail.com
*
*     askpass.cpp
* 
******************************************************/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc,char **argv)
{
	GtkWidget*	dialog;
	GtkWidget*	content;
	GtkWidget*	entry;

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new_with_buttons("",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,GTK_RESPONSE_REJECT,NULL);
	gtk_window_set_resizable((GtkWindow*)dialog,false);
	content=gtk_dialog_get_content_area((GtkDialog*)dialog);
	entry=gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry*)entry,false);
	gtk_box_pack_start((GtkBox*)content,gtk_label_new("Enter Password"),true,true,4);
	gtk_box_pack_start((GtkBox*)content,entry,true,true,4);
	gtk_widget_show_all(content);
	if(gtk_dialog_run((GtkDialog*)dialog)==GTK_RESPONSE_ACCEPT)
		printf("%s\n",gtk_entry_get_text((GtkEntry*)entry));
	else
		printf("\n");
	return(0);
}