/*
 *
 * ©K. D. Hedger. Tue 31 Jul 12:39:41 BST 2018 keithdhedger@gmail.com

 * This file (askpass.cpp) is part of KKEditPlugins.

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

#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>

#include "../common.h"
#ifdef _DEVMODE_
#include "../../KKEdit/gtk-kkedit/KKEdit/src/kkedit-plugins.h"
#else
#include <kkedit-plugins.h>
#endif

#define TEXTDOMAIN "RemoteEdit"

GtkWidget*	dialog;

void doEnter(GtkWidget* widget,gpointer data)
{
	gtk_dialog_response((GtkDialog*)dialog,GTK_RESPONSE_ACCEPT);
}

int main(int argc,char **argv)
{
	GtkWidget*	content;
	GtkWidget*	entry;

	setlocale(LC_ALL,"");
	bindtextdomain(TEXTDOMAIN,LOCALEDIR);
	textdomain(TEXTDOMAIN);
	bind_textdomain_codeset(TEXTDOMAIN,"UTF-8");

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new_with_buttons("",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,GTK_RESPONSE_REJECT,NULL);
	gtk_window_set_resizable((GtkWindow*)dialog,false);
	content=gtk_dialog_get_content_area((GtkDialog*)dialog);
	entry=gtk_entry_new();
	g_signal_connect_after(G_OBJECT(entry),"activate",G_CALLBACK(doEnter),(void*)entry);
	gtk_entry_set_visibility((GtkEntry*)entry,false);
	gtk_box_pack_start((GtkBox*)content,gtk_label_new(gettext("Enter Password")),true,true,4);
	gtk_box_pack_start((GtkBox*)content,entry,true,true,4);
	gtk_widget_show_all(content);
	gtk_window_set_keep_above((GtkWindow*)dialog,true);
	if(gtk_dialog_run((GtkDialog*)dialog)==GTK_RESPONSE_ACCEPT)
		printf("%s\n",gtk_entry_get_text((GtkEntry*)entry));
	else
		printf("\n");
	return(0);
}