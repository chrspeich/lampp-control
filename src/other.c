#include "include.h"

void my_error(char *format, GtkWindow *win) {
	GtkWidget *dialog;
 		dialog = gtk_message_dialog_new(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, format);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
}

void my_info(char *format, GtkWindow *win) {
	GtkWidget *dialog;
 		dialog = gtk_message_dialog_new(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, format);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
}

void license(void) {
	GtkWidget *dialog;
	GtkTextBuffer *text;
	GtkTextView *text_view;
	gchar *gpl_notice;

	dialog = gtk_dialog_new_with_buttons ("License", main_data.win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

	text = gtk_text_buffer_new(NULL);
	text_view = g_object_new(GTK_TYPE_TEXT_VIEW, "editable", FALSE, "cursor-visible", FALSE, "buffer", text, NULL);

	gpl_notice = "Lampp Control 1.1,\nCopyright (C) 2006 Christian Hartig <kleinweby@gmx.de>\n\nLampp Control comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome to redistribute\nit under certain conditions;\n\nSee http://www.gnu.org/licenses/gpl.html for details.";


	gtk_text_buffer_set_text(text, gpl_notice, strlen(gpl_notice));

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), GTK_WIDGET(text_view));
	gtk_widget_show_all(GTK_WIDGET(dialog));
	switch(gtk_dialog_run(GTK_DIALOG(dialog))) {
	case GTK_RESPONSE_OK:
		close(creat("/opt/lampp/share/lampp_control/.license", 755));
		gtk_widget_destroy(dialog);
		return;
	}
	gtk_widget_destroy(dialog);
	exit(1);
}

void copyfile(char *from, char *to) {
	FILE *file[1];
	char tmp[512];

	file[0] = fopen(from, "r");
	if(file[0] == NULL) {
		my_error(_("Action could not be implement.\nSee Console for Details."), einstellungen_win);
		perror("fopen");
		return;
	}

	file[1] = fopen(to, "w");
	if(file[1] == NULL) {
		my_error(_("Action could not be implement.\nSee Console for Details."), einstellungen_win);
		perror("fopen");
		fclose(file[0]);
		return;
	}

        while (fgets(tmp, sizeof(tmp), file[0])) {
		fputs(tmp, file[1]);
	}

	fclose(file[0]);
	fclose(file[1]);

	return;
}
