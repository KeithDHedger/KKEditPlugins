/******************************************************
*
*     ©keithhedger Thu 14 Aug 14:57:06 BST 2014
*     kdhedger68713@gmail.com
*
*     askpass.cpp
* 
******************************************************/

#include <gtk/gtk.h>

GtkWidget*	dialog;

void doEnter(GtkWidget* widget,gpointer data)
{
	gtk_dialog_response((GtkDialog*)dialog,GTK_RESPONSE_ACCEPT);
}

int main(int argc,char **argv)
{
	GtkWidget*	content;
	GtkWidget*	entry;

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new_with_buttons("",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,GTK_RESPONSE_REJECT,NULL);
	gtk_window_set_resizable((GtkWindow*)dialog,false);
	content=gtk_dialog_get_content_area((GtkDialog*)dialog);
	entry=gtk_entry_new();
	g_signal_connect_after(G_OBJECT(entry),"activate",G_CALLBACK(doEnter),(void*)entry);
	gtk_entry_set_visibility((GtkEntry*)entry,false);
	gtk_box_pack_start((GtkBox*)content,gtk_label_new("Enter Password"),true,true,4);
	gtk_box_pack_start((GtkBox*)content,entry,true,true,4);
	gtk_widget_show_all(content);
	gtk_window_set_keep_above((GtkWindow*)dialog,true);
	if(gtk_dialog_run((GtkDialog*)dialog)==GTK_RESPONSE_ACCEPT)
		printf("%s\n",gtk_entry_get_text((GtkEntry*)entry));
	else
		printf("\n");
	return(0);
}