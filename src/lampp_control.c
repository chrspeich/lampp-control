/* lampp_contorl.c */
#include "include.h"
#include <pthread.h>
#include "fnmatch.h"
#include "eggtrayicon/eggtrayicon.h"

static GtkTextBuffer *terminal;
static GtkWidget *expander;
static void ceck_phpversion();

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
		my_perror("select");
	}

	while(((tmp[i] = fgetc(file))) && (i < 2048) ) {
		if(tmp[i] == EOF) {
			pclose(file);
			status = FALSE;
			g_object_set(Modules.Apache.start, "sensitive", TRUE, NULL);
			g_object_set(Modules.Apache.stop, "sensitive", TRUE, NULL);
			g_object_set(Modules.Mysql.start, "sensitive", TRUE, NULL);
			g_object_set(Modules.Mysql.stop, "sensitive", TRUE, NULL);
			g_object_set(Modules.Ftp.start, "sensitive", TRUE, NULL);
			g_object_set(Modules.Ftp.stop, "sensitive", TRUE, NULL);
			g_object_set(Modules.php_4, "sensitive", TRUE, NULL);
			g_object_set(Modules.php_5, "sensitive", TRUE, NULL);
			g_object_set(Modules.lampp_start, "sensitive", TRUE, NULL);
			g_object_set(Modules.lampp_stop, "sensitive", TRUE, NULL);
			g_object_set(Modules.lampp_reload, "sensitive", TRUE, NULL);
			Modules.work = 0;
			break;
		}
		i++;
		if(strlen(tmp) != 0 && tmp[strlen(tmp) -1] == '\n') {
			break;
		}
	}
	
	tmp[i] = 0;
	if(uft8(tmp) != NULL) {
		gtk_text_buffer_insert_at_cursor(terminal, uft8(tmp), strlen(uft8(tmp)));
	}
	free(tmp);
	return status;
}

void lampp(GtkWidget *tmppp, gpointer daten) {
	FILE *file;
	char *tmp;

	if(Modules.work == 1) return;
	g_object_set(Modules.Apache.start, "sensitive", FALSE, NULL);
	g_object_set(Modules.Apache.stop, "sensitive", FALSE, NULL);
	g_object_set(Modules.Mysql.start, "sensitive", FALSE, NULL);
	g_object_set(Modules.Mysql.stop, "sensitive", FALSE, NULL);
	g_object_set(Modules.Ftp.start, "sensitive", FALSE, NULL);
	g_object_set(Modules.Ftp.stop, "sensitive", FALSE, NULL);
	g_object_set(Modules.php_4, "sensitive", FALSE, NULL);
	g_object_set(Modules.php_5, "sensitive", FALSE, NULL);
	g_object_set(Modules.lampp_start, "sensitive", FALSE, NULL);
	g_object_set(Modules.lampp_stop, "sensitive", FALSE, NULL);
	g_object_set(Modules.lampp_reload, "sensitive", FALSE, NULL);
	Modules.work = 1;
	gtk_text_buffer_set_text(terminal, "", 0);

	if(strcmp(daten, "") == 0) {
		return;
	}

	tmp = malloc(strlen("/opt/lampp/lampp ") + strlen(daten) + 1);
	snprintf(tmp, strlen("/opt/lampp/lampp ") + strlen(daten) + 1, "/opt/lampp/lampp %s", daten);
	file = popen(tmp, "r");
	free(tmp);

	if(file == NULL) {
		my_perror("popen");
		return;
	}

	g_timeout_add(200, file_to_terminal, file);
}

static void *ceck_status(void *name) {
	char *mysql, *laeuft, *laeuft_n, tmp[512];
	FILE *file;
	char puffer[PIPE_BUF * 8];
	int status;

	laeuft = (char *) malloc(1024);
	snprintf(laeuft, strlen("<span foreground=\"#0cff00\"></span>") + strlen(_("runnig")) + 1, "%s%s%s", "<span color=\"green\">", _("runnig"), "</span>");
	laeuft_n = (char *) malloc(1024);
	snprintf(laeuft_n, strlen("<span color=\"red\"></span>") + strlen(_("not runnig")) + 1, "%s%s%s", "<span color=\"red\">", _("not runnig"), "</span>");

	file = fopen("/etc/hostname", "r");
	if(!file) {
		g_print(_("Can't open /etc/hostname"));
		return;
	}
	fgets(tmp, sizeof(tmp), file);
	tmp[strlen(tmp) - 1] = 0;
	mysql = (char *) malloc(2024);
	snprintf(mysql, strlen("/opt/lampp/var/mysql/") + strlen(tmp) + strlen(".pid") + 1, "%s%s%s", "/opt/lampp/var/mysql/", tmp, ".pid");
	pclose(file);
	while(1) {
		/* APACHE */
		status = kom_status("/opt/lampp/logs/httpd.pid");

		if(status != Modules.Apache.raw_status) { //Nachschauen ob sich der Status geändert hat
			if(status == 1) { //läuft
				gtk_widget_hide(GTK_WIDGET(Modules.Apache.start));
				gtk_widget_show(GTK_WIDGET(Modules.Apache.stop));
				g_object_set(Modules.Apache.status, "label", laeuft, NULL);
				Modules.Apache.raw_status = 1;
			} else { //läuft nicht
				gtk_widget_hide(GTK_WIDGET(Modules.Apache.stop));
				gtk_widget_show(GTK_WIDGET(Modules.Apache.start));
				g_object_set(Modules.Apache.status, "label", laeuft_n, NULL);
				Modules.Apache.raw_status = 0;
			}
		}

		/* MYSQL */
		status = kom_status(mysql);

		if(status != Modules.Mysql.raw_status) {
			if(status == 1) {
				gtk_widget_hide(GTK_WIDGET(Modules.Mysql.start));
				gtk_widget_show(GTK_WIDGET(Modules.Mysql.stop));
				g_object_set(Modules.Mysql.status, "label", laeuft, NULL);
				Modules.Mysql.raw_status = 1;
			} else {
				gtk_widget_hide(GTK_WIDGET(Modules.Mysql.stop));
				gtk_widget_show(GTK_WIDGET(Modules.Mysql.start));
				g_object_set(Modules.Mysql.status, "label", laeuft_n, NULL);
				Modules.Mysql.raw_status = 0;
			}
		}
	
		/* ftp */
		status = kom_status("/opt/lampp/var/proftpd.pid");

		if(status != Modules.Ftp.raw_status) {
			if(status == 1) {
				gtk_widget_hide(GTK_WIDGET(Modules.Ftp.start));
				gtk_widget_show(GTK_WIDGET(Modules.Ftp.stop));
				g_object_set(Modules.Ftp.status, "label", laeuft, NULL);
				Modules.Ftp.raw_status = 1;
			} else {
				gtk_widget_hide(GTK_WIDGET(Modules.Ftp.stop));
				gtk_widget_show(GTK_WIDGET(Modules.Ftp.start));
				g_object_set(Modules.Ftp.status, "label", laeuft_n, NULL);
				Modules.Ftp.raw_status = 0;
			}
		}

		/* PHP */
		if ((file = popen ("/opt/lampp/share/lampp/phpstatus", "r")) == NULL) //Öffne PHPstatus Prozess von lampp
			my_perror("popen");
		fgets(puffer, PIPE_BUF * 8, file);
		pclose(file);
		puffer[strlen(puffer)-1] = '\0';
		if(fnmatch("5.*", (char *)puffer, 0) == 0) { //Nachschauen ob die PHP Version mit 5.* anfängt
			status = 5;
		} else {
			status = 4;
		}
		if(status != Modules.php_raw_status) { //Nachschauen ob sich die Version geändert hat
			if(status == 5) { //PHP 5
				gtk_widget_hide(GTK_WIDGET(Modules.php_5));
				gtk_widget_show(GTK_WIDGET(Modules.php_4));
				Modules.php_raw_status = 5;
			} else { //PHP 4
				gtk_widget_hide(GTK_WIDGET(Modules.php_4));
				gtk_widget_show(GTK_WIDGET(Modules.php_5));
				Modules.php_raw_status = 4;
			}
			g_object_set(Modules.php_status, "label", puffer, NULL);
		}

		sleep(1);
	}
	free(laeuft); free(laeuft_n); free(mysql);
}

int main(int argc, char **argv) {
	GtkLabel *logo, *frame_label;
	GtkFrame *Modules_Frame;
	GtkTable *table, *Modules_Table;
	GtkButton *settings;
	GtkHBox *box;
	GtkVBox *vbox;
	GtkAlignment *scroll;
	GtkTextView *text;
	pthread_t th[1];
	uid_t uid;
	char *tmp, tmp2[512];
	FILE *version;
	GtkImage *pic;
	int start_hide = 0, i;

	bindtextdomain ("lampp_control", "/opt/lampp/share/locale/");
	textdomain("lampp_control");
	setlocale(LC_ALL, "");
	setlocale(LC_TIME, "" );
	setlocale(LC_MESSAGES, "");

	for(i = 0; i < argc; i++) {
		if(strcmp(argv[i], "--start-hide") == 0) {
			start_hide = 1;
		}
	}

	uid = getuid();
	if(uid != 0) {
		tmp = malloc(1024);
		if(start_hide == 1) {
			snprintf(tmp, strlen("gksu \" --start-hide\"") + strlen(argv[0]) + 1, "gksu \"%s --start-hide\"", argv[0]);
		} else {
			snprintf(tmp, strlen("gksu \"\"") + strlen(argv[0]) + 1, "gksu \"%s\"", argv[0]);
		}
		system(tmp);
		free(tmp);
		return 0;
	}

	gtk_init(&argc, &argv);

	if(access("/opt/lampp/share/lampp_control/.license", F_OK) != 0) {
		license();
	}

	main_data.pic = gdk_pixbuf_new_from_file("/opt/lampp/share/lampp_control/logo.gif", NULL);
	main_data.win = g_object_new(GTK_TYPE_WINDOW, "title", "Lampp Control", "window-position", GTK_WIN_POS_CENTER, "border-width", 5, "icon", main_data.pic, "default-height", 225, "default-width", 400, NULL);
	box = g_object_new(GTK_TYPE_HBOX, "spacing", 0, NULL);
	vbox = g_object_new(GTK_TYPE_VBOX, "spacing", 0, NULL);
	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 7, "n-rows",  4, NULL);

	version = fopen("/opt/lampp/lib/VERSION", "r");
	if(version != NULL) {
		fgets(tmp2, sizeof(tmp2), version);
		fclose(version);
		xampp_version = malloc(strlen(tmp2) + 1);
		strcpy(xampp_version, tmp2);
	} else {
		perror("fopen");
		my_error(_("Can't dedect the XAMPP version!"), main_data.win);
		xampp_version = malloc(strlen("unkown") + 1);
		strcpy(xampp_version, "unkown");
	}

	if(version_required("1.4.16") == 0) {
		my_error(_("Lampp Control dosn't support your Xampp version."), main_data.win);
		exit(0);
	}

	tmp = malloc(strlen(_("<span font_desc=\"14\" foreground=\"blue\">Lampp Control %s</span>\n<span font_desc=\"13\">for XAMPP %s</span>")) + strlen("1.2a") + strlen(xampp_version) + 1);
	snprintf(tmp, strlen(_("<span font_desc=\"14\" foreground=\"blue\">Lampp Control %s</span>\n<span font_desc=\"13\">for XAMPP %s</span>")) + strlen("1.2a") + strlen(xampp_version) + 1, _("<span font_desc=\"14\" foreground=\"blue\">Lampp Control %s</span>\n<span font_desc=\"13\">for XAMPP %s</span>"), "1.2a", xampp_version);
	logo = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", tmp, NULL);
	pic = g_object_new(GTK_TYPE_IMAGE, "file", "/opt/lampp/share/lampp_control/logo.gif", NULL);
	gtk_misc_set_alignment(GTK_MISC(pic), 1, 0.5);
	gtk_misc_set_alignment(GTK_MISC(logo), 0.1, 0.5);
	frame_label = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", _("<b>Modules</b>"), NULL);
	Modules_Frame = g_object_new(GTK_TYPE_FRAME, "label-widget", GTK_WIDGET(frame_label), NULL);
	Modules_Table = g_object_new(GTK_TYPE_TABLE, "n-columns", 4, "n-rows",  3, NULL);
	settings = g_object_new(GTK_TYPE_BUTTON, "label", _("Settings"), NULL);
	Modules.lampp_start = g_object_new(GTK_TYPE_BUTTON, "label", _("LAMPP start"), NULL);
	Modules.lampp_stop = g_object_new(GTK_TYPE_BUTTON, "label", _("LAMPP stop"), NULL);
	Modules.lampp_reload = g_object_new(GTK_TYPE_BUTTON, "label", _("LAMPP reload"), NULL);

	/* Create Modules */
	/*  Apache */
	Modules.Apache.name = g_object_new(GTK_TYPE_LABEL, "label", _("Apache"), NULL);
	Modules.Apache.status = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", _("<span foreground=\"yellow\">Unkown</span>"), NULL);
	Modules.Apache.start = g_object_new(GTK_TYPE_BUTTON, "label", _("Start"), NULL);
	Modules.Apache.stop = g_object_new(GTK_TYPE_BUTTON, "label", _("Stop"), NULL);
	Modules.Apache.raw_status = -1;
	/*  Mysql */
	Modules.Mysql.name = g_object_new(GTK_TYPE_LABEL, "label", _("MySQL"), NULL);
	Modules.Mysql.status = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", _("<span foreground=\"yellow\">Unkown</span>"), NULL);
	Modules.Mysql.start = g_object_new(GTK_TYPE_BUTTON, "label", _("Start"), NULL);
	Modules.Mysql.stop = g_object_new(GTK_TYPE_BUTTON, "label", _("Stop"), NULL);
	Modules.Mysql.raw_status = -1;
	/*  FTP */
	Modules.Ftp.name = g_object_new(GTK_TYPE_LABEL, "label", _("FTP"), NULL);
	Modules.Ftp.status = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", _("<span foreground=\"yellow\">Unkown</span>"), NULL);
	Modules.Ftp.start = g_object_new(GTK_TYPE_BUTTON, "label", _("Start"), NULL);
	Modules.Ftp.stop = g_object_new(GTK_TYPE_BUTTON, "label", _("Stop"), NULL);
	Modules.Ftp.raw_status = -1;
	/* PHP */
	Modules.php_name = g_object_new(GTK_TYPE_LABEL, "label", _("PHP"), NULL);
	Modules.php_status = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", _("<span foreground=\"yellow\">Unkown</span>"), NULL);
	Modules.php_4 = g_object_new(GTK_TYPE_BUTTON, "label", _("PHP 4"), NULL);
	Modules.php_5 = g_object_new(GTK_TYPE_BUTTON, "label", _("PHP 5"), NULL);
	Modules.php_raw_status = -1;
	/* End */

	expander = gtk_expander_new("");
	scroll = g_object_new(GTK_TYPE_SCROLLED_WINDOW, "hscrollbar-policy", GTK_POLICY_AUTOMATIC, "vscrollbar-policy", GTK_POLICY_AUTOMATIC, NULL);
	gtk_widget_set_size_request(GTK_WIDGET(scroll), 200, -1);
	terminal = gtk_text_buffer_new(NULL);
	text = g_object_new(GTK_TYPE_TEXT_VIEW, "editable", FALSE, "cursor-visible", FALSE, "buffer", terminal, NULL);
	
	tray_init();

	g_signal_connect(Modules.lampp_start, "clicked", G_CALLBACK(lampp), "start");
	g_signal_connect(Modules.lampp_stop, "clicked", G_CALLBACK(lampp), "stop");
	g_signal_connect(Modules.lampp_reload, "clicked", G_CALLBACK(lampp), "reload");
	g_signal_connect(settings, "clicked", G_CALLBACK(einstellungen), NULL);
	g_signal_connect(Modules.Apache.start, "clicked", G_CALLBACK(lampp), "startapache");
	g_signal_connect(Modules.Apache.stop, "clicked", G_CALLBACK(lampp), "stopapache");
	g_signal_connect(Modules.Mysql.start, "clicked", G_CALLBACK(lampp), "startmysql");
	g_signal_connect(Modules.Mysql.stop, "clicked", G_CALLBACK(lampp), "stopmysql");
	g_signal_connect(Modules.Ftp.start, "clicked", G_CALLBACK(lampp), "startftp");
	g_signal_connect(Modules.Ftp.stop, "clicked", G_CALLBACK(lampp), "stopftp");
	g_signal_connect(Modules.php_4, "clicked", G_CALLBACK(lampp), "php4");
	g_signal_connect(Modules.php_5, "clicked", G_CALLBACK(lampp), "php5");
	g_signal_connect(main_data.win, "delete-event", G_CALLBACK(delete_Event), NULL);
	g_signal_connect(main_data.win, "destroy", G_CALLBACK(end), NULL);

	gtk_container_add(GTK_CONTAINER(scroll), GTK_WIDGET(text));

	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Apache.name), 0, 1, 0, 1, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Apache.status), 1, 2, 0, 1, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Apache.start), 2, 3, 0, 1, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Apache.stop), 2, 3, 0, 1, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);

	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Mysql.name), 0, 1, 1, 2, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Mysql.status), 1, 2, 1, 2, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Mysql.start), 2, 3, 1, 2, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Mysql.stop), 2, 3, 1, 2, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);

	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Ftp.name), 0, 1, 2, 3, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Ftp.status), 1, 2, 2, 3, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Ftp.start), 2, 3, 2, 3, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.Ftp.stop), 2, 3, 2, 3, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);

	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.php_name), 0, 1, 3, 4, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.php_status), 1, 2, 3, 4, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.php_4), 2, 3, 3, 4, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(Modules_Table), GTK_WIDGET(Modules.php_5), 2, 3, 3, 4, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND), 0, 0);

	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pic), 0, 1, 0, 2, (GTK_FILL), (GTK_EXPAND  | GTK_FILL), 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(logo), 1, 4, 0, 2, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND  | GTK_FILL), 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(vbox), 3, 4, 2, 6, 0, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(Modules_Frame), 0, 3, 2, 6, (GTK_EXPAND | GTK_FILL), (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(scroll), 0, 4, 7, 8, (GTK_EXPAND | GTK_FILL), GTK_FILL, 0, 0);

	gtk_widget_set_size_request(GTK_WIDGET(Modules_Frame), 280, -1);

	gtk_container_add(GTK_CONTAINER(main_data.win), GTK_WIDGET(box));
	gtk_container_add(GTK_CONTAINER(Modules_Frame), GTK_WIDGET(Modules_Table));

	gtk_box_pack_start_defaults(GTK_BOX(vbox), GTK_WIDGET(settings));
	gtk_box_pack_start_defaults(GTK_BOX(vbox), GTK_WIDGET(Modules.lampp_start));
	gtk_box_pack_start_defaults(GTK_BOX(vbox), GTK_WIDGET(Modules.lampp_stop));
	gtk_box_pack_start_defaults(GTK_BOX(vbox), GTK_WIDGET(Modules.lampp_reload));

	gtk_box_pack_start_defaults(GTK_BOX(box), GTK_WIDGET(table));
	main_data.show = 1;
	gtk_widget_show_all(GTK_WIDGET(main_data.win));
	
	if(start_hide == 1) {
		main_data.show = 0;
		gtk_widget_hide(GTK_WIDGET(main_data.win));
	}

	//ceck_phpversion();
	pthread_create(&th[0], NULL, (void *)ceck_status, NULL);

	gtk_main();
	return 0;
}
