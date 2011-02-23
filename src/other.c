#include "include.h"
#include <errno.h>
#include "eggtrayicon/eggtrayicon.h"
#include "fnmatch.h"

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

	gpl_notice = "Lampp Control 1.2a,\nCopyright (C) 2006 Christian Speich <kleinweby@gmx.de>\n\nLampp Control comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome to redistribute\nit under certain conditions;\n\nSee http://www.gnu.org/licenses/gpl.html for details.";


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
		my_perror("fopen");
		return;
	}

	file[1] = fopen(to, "w");
	if(file[1] == NULL) {
		my_perror("fopen");
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

void tray_menu(GdkEventButton *event) {
	GtkWidget *menu, *item;
	
	menu = gtk_menu_new();
	
	item = gtk_image_menu_item_new_with_label(_("LAMPP start"));
	gtk_container_add(GTK_CONTAINER(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(lampp), "start");
	
	item = gtk_image_menu_item_new_with_label(_("LAMPP stop"));
	gtk_container_add(GTK_CONTAINER(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(lampp), "stop");
	
	item = gtk_image_menu_item_new_with_label(_("LAMPP reload"));
	gtk_container_add(GTK_CONTAINER(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(lampp), "reload");
	
	item = gtk_image_menu_item_new_with_label(_("Close"));
	gtk_container_add(GTK_CONTAINER(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time);
}

gboolean tray_clicked (GtkWidget *event_box, GdkEventButton *event, gpointer data) {

	if(event->button == 2) {
				lampp(NULL, "reload");
				return FALSE;
	} else if(event->button == 1) {
        	if(main_data.show == 1) {
				main_data.show = 0;
				gtk_widget_hide(GTK_WIDGET(main_data.win));
			} else {
				main_data.show = 1;
				gtk_widget_show(GTK_WIDGET(main_data.win));
			}
	} else if(event->button == 3) {
		tray_menu(event);
	}

	return FALSE;
}


void tray_init() {
	GtkWidget *eventbox, *icon;
	
	trayicon = GTK_WIDGET(egg_tray_icon_new("Lampp Control System Tray"));
	if(trayicon == NULL) {
		my_error(_("Cant create trayicon"), main_data.win);
		exit(1);
	}

	gtk_widget_set_size_request(trayicon, 22, 22);
	gtk_widget_realize(trayicon);
	
	icon = gtk_image_new_from_file("/opt/lampp/share/lampp_control/logo.gif");
	eventbox = gtk_event_box_new();
	
	g_signal_connect(G_OBJECT(eventbox), "button_press_event" , G_CALLBACK(tray_clicked), NULL);
	g_signal_connect(GTK_OBJECT(trayicon), "destroy" , GTK_SIGNAL_FUNC(gtk_widget_destroyed), &trayicon);
	g_signal_connect(G_OBJECT(trayicon), "destroy" , GTK_SIGNAL_FUNC(gtk_widget_destroyed), NULL);
	
	gtk_container_add(GTK_CONTAINER(eventbox), icon);
	gtk_container_add(GTK_CONTAINER(trayicon), eventbox);
	
	gtk_widget_show_all(GTK_WIDGET(trayicon));
}

void _my_perror(char *tmp, char *quelldatei, int zeile) {
	GtkWidget *dialog;
	char *tmp2;
	tmp2 = malloc(strlen(tmp) + strlen("[:]: ") + strlen(strerror(errno)) + strlen(quelldatei) + sizeof(zeile) + 1);
	snprintf(tmp2, strlen(tmp) + strlen("[:]: ") + strlen(strerror(errno)) + strlen(quelldatei) + sizeof(zeile) + 1, "%s[%s:%d]:%s", tmp, quelldatei, zeile, strerror(errno));
 	dialog = gtk_message_dialog_new(main_data.win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, tmp2);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

int version_required(char *version) {
	int stat;

	if(strcmp(xampp_version, "unkown") == 0) {
		return 1;
	}

	stat = strcmp(xampp_version, version);
	if(stat == 0 || stat == 1) {
		return 1; //Version are match
	} else {
		return 0; //Version are not match
	}
	return 0;
}

int kom_status(char *pid_file) { //Überprüft od der Prozess in der PID Datei wircklich läuft
	FILE *ps, *fpid;
	char *tmp, *pid, *tmp2;
	if(access(pid_file, F_OK) != 0) { //PID Datei gibs garnicht also kann der Prozess auch nicht laufen
		return 0;
	}

	if((fpid = fopen(pid_file, "r")) == NULL)//Lese PID aus
		my_perror("fopen");
	pid = malloc(512);
	fgets(pid, 512, fpid);
	fclose(fpid);

	tmp =malloc(strlen("ps --no-headers -opid -p") + strlen(pid_file) + 1);
	snprintf(tmp, strlen("ps --no-headers -opid -p") + strlen(pid_file) + 1, "ps --no-headers -opid -p%s", pid);

	if((ps = popen (tmp, "r")) == NULL) //Öffne Prozess PS zum lesen
		my_perror("popen");

	tmp = malloc(512);
	fgets(tmp, 511, ps);

	tmp2 = malloc(strlen(pid) + strlen("**") + 1);
	snprintf(tmp2, strlen(pid) + strlen("**") + 1, "*%s*", pid);
	if(fnmatch(tmp2, tmp, 0) == 0) { //Schaue ob zurückgegebene PID von PS die ist nach der gesucht wurde
		pclose(ps);
		free(tmp);
		return 1;
	} else {
		pclose(ps);
		free(tmp);
		return 0;
	}
}
