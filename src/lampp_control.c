/* lampp_contorl.c */
#include "include.h"
#include <pthread.h>

static GtkTextBuffer *terminal;
static GtkWidget *expander;

gboolean expander_text(gpointer user_data) {
	if(gtk_expander_get_expanded(GTK_EXPANDER(expander)) == TRUE) {
		gtk_expander_set_label(GTK_EXPANDER(expander), _("Hide info window"));
	} else {
		gtk_expander_set_label(GTK_EXPANDER(expander), _("Show info window"));
	}
	return  TRUE;
}

static gint delete_Event(GtkWidget * widget, gpointer daten) {
	return FALSE;
}

static void end(GtkWidget * widget, gpointer daten) {
	gtk_main_quit();
}

static gboolean file_to_terminal(gpointer file) {
	int i = 0, result, status = TRUE;
	struct timeval timeout;
	char *tmp = malloc(2048);
	fd_set read_fds;

	FD_ZERO(&read_fds);
	FD_SET(fileno(file), &read_fds);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	result = select(fileno(file) + 1, &read_fds, NULL, NULL, &timeout);
	if(result == 0) {
		return status;
	} else if(result != 1) {
		perror("select");
	}

	while(((tmp[i] = fgetc(file))) && (i < 2048) ) {
		if(tmp[i] == EOF) {
			pclose(file);
			status = FALSE;
			break;
		}
		i++;
		if(strlen(tmp) != 0 && tmp[strlen(tmp) -1] == '\n') {
			break;
		}
	}
	
	tmp[i] = 0;
	gtk_text_buffer_insert_at_cursor(terminal, uft8(tmp), strlen(uft8(tmp)));
	free(tmp);
	return status;
}

void lampp(GtkWidget *tmppp, gpointer daten) {
	FILE *file;
	char *puffer;
	gtk_text_buffer_set_text(terminal, "", 0);

	puffer = malloc(PIPE_BUF * 8);
	if(daten == "start") {
		file = popen ("/opt/lampp/lampp start", "r");
	} else if(daten == "stop") {
		file = popen ("/opt/lampp/lampp stop", "r");
	} else  if(daten == "reload") {
		file = popen ("/opt/lampp/lampp reload", "r");
	} else if(daten == "apache_start") {
		file = popen ("/opt/lampp/lampp startapache", "r");
	} else if(daten == "apache_stop") {
		file = popen ("/opt/lampp/lampp stopapache", "r");
	} else if(daten == "mysql_start") {
		file = popen ("/opt/lampp/lampp startmysql", "r");
	} else if(daten == "mysql_stop") {
		file = popen ("/opt/lampp/lampp stopmysql", "r");
	} else if(daten == "ftp_start") {
		file = popen ("/opt/lampp/lampp startftp", "r");
	} else if(daten == "ftp_stop") {
		file = popen ("/opt/lampp/lampp stopftp", "r");
	} else if(daten == "php_4") {
		file = popen ("/opt/lampp/lampp php4", "r");
	} else if(daten == "php_5") {
		file = popen ("/opt/lampp/lampp php5", "r");
	} else {
		free(puffer);
		g_print("ne");
		return;
	}

	if(file == NULL) {
		perror("popen");
		my_error(_("Action could not be implement.\nSee Console for Details."), main_data.win);
		free(puffer);
		return;
	}

	gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
	g_timeout_add(300, file_to_terminal, file);
	free(puffer);
}

static void *ceck_phpversion(void *name) { 
	FILE *file;
	char *php, puffer[PIPE_BUF * 8];
	while(1) {
		/* PHP Version */
		if ((file = popen ("/opt/lampp/share/lampp/phpstatus", "r")) == NULL)
			perror("popen");
		fgets(puffer, PIPE_BUF * 8, file);
		pclose(file);
		//php = (char *) malloc(1024);
		puffer[strlen(puffer)-1] = '\0';
		//snprintf(php, strlen("<span color=\"#0cff00\">") + strlen(puffer) + strlen("</span>") + 1, "%s%s%s", "<span color=\"green\">", puffer, "</span>");
		g_object_set(status.php, "label", puffer);
		//free(php);
		sleep(1);
	}
}
static void *ceck_status(void *name) {
	char *mysql, *laeuft, *laeuft_n, tmp[512];
	FILE *file;
	laeuft = (char *) malloc(1024);
	snprintf(laeuft, strlen("<span foreground=\"#0cff00\"></span>") + strlen(_("runnig")) + 1, "%s%s%s", "<span color=\"green\">", _("runnig"), "</span>");
	laeuft_n = (char *) malloc(1024);
	snprintf(laeuft_n, strlen("<span color=\"red\"></span>") + strlen(_("not runnig")) + 1, "%s%s%s", "<span color=\"red\">", _("not runnig"), "</span>");

	file = fopen("/etc/hostname", "r");
	if(!file) {
		g_print(_("Can`t open /etc/hostname"));
		return;
	}
	fgets(tmp, sizeof(tmp), file);
	tmp[strlen(tmp) - 1] = 0;
	mysql = (char *) malloc(2024);
	snprintf(mysql, strlen("/opt/lampp/var/mysql/") + strlen(tmp) + strlen(".pid") + 1, "%s%s%s", "/opt/lampp/var/mysql/", tmp, ".pid");
	pclose(file);
	while(1) {
		/* APACHE */
		if(access("/opt/lampp/logs/httpd.pid", F_OK) == 0) {
			g_object_set(status.apache, "label", laeuft, NULL);
		} else {
			g_object_set(status.apache, "label", laeuft_n, NULL);
		}

		/* MYSQL */
		if(access(mysql, F_OK) == 0) {
			g_object_set(status.mysql, "label", laeuft, NULL);
		} else {
			g_object_set(status.mysql, "label", laeuft_n, NULL);
		}
	
		/* ftp */
		if(access("/opt/lampp/var/proftpd.pid", F_OK) == 0) {
			g_object_set(status.ftp, "label", laeuft, NULL);
		} else {
			g_object_set(status.ftp, "label", laeuft_n, NULL);
		}
	
		sleep(1);
	}
	free(laeuft); free(laeuft_n); free(mysql);
}

int main(int argc, char **argv) {
	GtkButton *lampp_start, *lampp_stop, *lampp_reload, *apache_start, *apache_stop,  *mysql_start, *mysql_stop, *ftp_start, *ftp_stop, *php_4, *php_5, *settings;
	GtkTable *table;
	GtkVBox *box;
	GtkAlignment *statusbar, *scroll;
	GtkTextView *text;
	int i;
	pthread_t th[1];
	uid_t uid;
	char *tmp;

	bindtextdomain ("lampp_control", "/opt/lampp/share/locale/");
	textdomain("lampp_control");
	setlocale(LC_ALL, "");
	setlocale(LC_TIME, "" );
	setlocale(LC_MESSAGES, "");

	uid = getuid();
	if(uid != 0) {
		tmp = malloc(1024);
#ifdef UBUNTU
		strcpy(tmp, "gksudo \"");
#else
		strcpy(tmp, "gksu \"");
#endif
		strcat(tmp, argv[0]);
		strcat(tmp, "\"");
		system(tmp);
		free(tmp);
		return;
	}

	gtk_init(&argc, &argv);

	if(access("/opt/lampp/share/lampp_control/.license", F_OK) != 0) {
		license();
	}

	main_data.pic = gdk_pixbuf_new_from_file("/opt/lampp/share/lampp_control/logo.gif", NULL);
	main_data.win = g_object_new(GTK_TYPE_WINDOW, "title", "Lampp Control", "resizable",  FALSE, "window-position", GTK_WIN_POS_CENTER, "border-width", 5, "icon", main_data.pic, NULL);
	box = g_object_new(GTK_TYPE_VBOX, "spacing", 0, NULL);
	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 6, "n-rows",  4, NULL);

	lampp_start = g_object_new(GTK_TYPE_BUTTON, "label", _("Lampp start"), NULL);
	lampp_stop = g_object_new(GTK_TYPE_BUTTON, "label", _("Lampp stop"), NULL);
	lampp_reload = g_object_new(GTK_TYPE_BUTTON, "label", _("Lampp reload"), NULL);
	apache_start = g_object_new(GTK_TYPE_BUTTON, "label", _("Apache start"), NULL);
	apache_stop = g_object_new(GTK_TYPE_BUTTON, "label", _("Apache stop"), NULL);
	mysql_start = g_object_new(GTK_TYPE_BUTTON, "label", _("MySql start"), NULL);
	mysql_stop = g_object_new(GTK_TYPE_BUTTON, "label", _("MySql stop"), NULL);
	ftp_start = g_object_new(GTK_TYPE_BUTTON, "label", _("FTP start"), NULL);
	ftp_stop = g_object_new(GTK_TYPE_BUTTON, "label", _("FTP stop"), NULL);
	settings = g_object_new(GTK_TYPE_BUTTON, "label", _("Settings"), NULL);
	php_4 = g_object_new(GTK_TYPE_BUTTON, "label", "PHP4", NULL);
	php_5 = g_object_new(GTK_TYPE_BUTTON, "label", "PHP5", NULL);
	expander = gtk_expander_new("");
	scroll = g_object_new(GTK_TYPE_SCROLLED_WINDOW, "hscrollbar-policy", GTK_POLICY_AUTOMATIC, "vscrollbar-policy", GTK_POLICY_AUTOMATIC, NULL);
	terminal = gtk_text_buffer_new(NULL);
	text = g_object_new(GTK_TYPE_TEXT_VIEW, "editable", FALSE, "cursor-visible", FALSE, "buffer", terminal, NULL);

	status.apache = g_object_new(GTK_TYPE_LABEL, "justify", GTK_JUSTIFY_CENTER, "use-markup", TRUE, "label", "", NULL);
	status.mysql = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", "", NULL);
	status.ftp = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", "", NULL);
	status.php = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", "", NULL);

	pthread_create(&th[0], NULL, &ceck_status, NULL);

	g_timeout_add(100, expander_text, NULL);

	g_signal_connect(lampp_start, "clicked", G_CALLBACK(lampp), "start");
	g_signal_connect(lampp_stop, "clicked", G_CALLBACK(lampp), "stop");
	g_signal_connect(lampp_reload, "clicked", G_CALLBACK(lampp), "reload");
	g_signal_connect(settings, "clicked", G_CALLBACK(einstellungen), NULL);
	g_signal_connect(apache_start, "clicked", G_CALLBACK(lampp), "apache_start");
	g_signal_connect(apache_stop, "clicked", G_CALLBACK(lampp), "apache_stop");
	g_signal_connect(mysql_start, "clicked", G_CALLBACK(lampp), "mysql_start");
	g_signal_connect(mysql_stop, "clicked", G_CALLBACK(lampp), "mysql_stop");
	g_signal_connect(ftp_start, "clicked", G_CALLBACK(lampp), "ftp_start");
	g_signal_connect(ftp_stop, "clicked", G_CALLBACK(lampp), "ftp_stop");
	g_signal_connect(php_4, "clicked", G_CALLBACK(lampp), "php_4");
	g_signal_connect(php_5, "clicked", G_CALLBACK(lampp), "php_5");
	g_signal_connect(main_data.win, "delete-event", G_CALLBACK(delete_Event), NULL);
	g_signal_connect(main_data.win, "destroy", G_CALLBACK(end), NULL);

	gtk_container_add(GTK_CONTAINER(scroll), GTK_WIDGET(text));
	gtk_container_add(GTK_CONTAINER(expander), GTK_WIDGET(scroll));

	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(lampp_start), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(lampp_stop), 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(lampp_reload), 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(settings), 3, 4, 1, 2, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(apache_start), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(apache_stop), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status.apache), 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(mysql_start), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(mysql_stop), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status.mysql), 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(ftp_start), 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(ftp_stop), 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status.ftp), 2, 3, 3, 4, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_4), 0, 1, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_5), 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status.php), 2, 3, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(expander), 0, 4, 7, 8, GTK_FILL, GTK_FILL, 0, 0);

	gtk_container_add(GTK_CONTAINER(main_data.win), GTK_WIDGET(box));
	gtk_box_pack_start_defaults(GTK_BOX(box), GTK_WIDGET(table));
	gtk_widget_show_all(GTK_WIDGET(main_data.win));

	pthread_create(&th[1], NULL, &ceck_phpversion, NULL);

	gtk_main();
	return 0;
}
