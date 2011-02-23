/* Fenster */
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <gtk/gtk.h>
static GtkWindow *win;
static GtkLabel *status_apache, *status_mysql, *status_ftp, *status_php;
static GtkStatusbar *status;
void ceck_status(void);
static gint delete_Event(GtkWidget * widget, gpointer daten) {
	return FALSE;
}

static void end(GtkWidget * widget, gpointer daten) {
	gtk_main_quit();
}

static void lampp(GtkWidget * widget, gpointer daten) {
	FILE *file;
	char puffer[PIPE_BUF * 8];
	if(daten == "start") {
		if ((file = popen ("gksudo '/opt/lampp/lampp start'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "Lampp gestartet");
	} else if(daten == "stop") {
		if ((file = popen ("gksudo '/opt/lampp/lampp stop'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "Lampp gestoppt");
	} else if(daten == "apache_start") {
		if ((file = popen ("gksudo '/opt/lampp/lampp startapache'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "Apache gestartet");
	} else if(daten == "apache_stop") {
		if ((file = popen ("gksudo '/opt/lampp/lampp stopapache'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "Apache gestoppt");
	} else if(daten == "reload") {
		if ((file = popen ("gksudo '/opt/lampp/lampp reload'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "Lampp reloaded");
	} else if(daten == "mysql_start") {
		if ((file = popen ("gksudo '/opt/lampp/lampp startmysql'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "MySql gestartet");
	} else if(daten == "mysql_stop") {
		if ((file = popen ("gksudo '/opt/lampp/lampp stopmysql'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "MySql gestoppt");
	} else if(daten == "ftp_start") {
		if ((file = popen ("gksudo '/opt/lampp/lampp startftp'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "FTP gestartet");
	} else if(daten == "ftp_stop") {
		if ((file = popen ("gksudo '/opt/lampp/lampp stopftp'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "FTP gestoppt");
	} else if(daten == "php_4") {
		if ((file = popen ("gksudo '/opt/lampp/lampp php4'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "Auf PHP4 geswitcht");
	} else if(daten == "php_5") {
		if ((file = popen ("gksudo '/opt/lampp/lampp php5'", "r")) == NULL)
			perror("popen");
		while (fgets(puffer, PIPE_BUF * 8, file) != NULL) {
			g_print(puffer);
		}
		gtk_statusbar_push(status, 0, "Auf PHP5 geswitcht");
	}
	sleep(2);
	ceck_status();
}

void ceck_status(void) {
	char *pfad, *php, puffer[PIPE_BUF * 8];
	FILE *file;
	/* APACHE */
	if(access("/opt/lampp/logs/httpd.pid", F_OK) == 0) {
		g_object_set(status_apache, "label", "<span color=\"green\">läuft</span>", NULL);
	} else {
		g_object_set(status_apache, "label", "<span color=\"red\">läuft nicht</span>", NULL);
	}

	/* MYSQL */
	pfad = (char *) malloc(2024);
	snprintf(pfad, strlen("/opt/lampp/var/mysql/") + strlen(getenv("HOSTNAME")) + strlen(".pid") + 1, "%s%s%s", "/opt/lampp/var/mysql/", getenv("HOSTNAME"), ".pid");
	if(access(pfad, F_OK) == 0) {
		g_object_set(status_mysql, "label", "<span color=\"green\">läuft</span>", NULL);
	} else {
		g_object_set(status_mysql, "label", "<span color=\"red\">läuft nicht</span>", NULL);
	}
	free(pfad);

	/* ftp */
	if(access("/opt/lampp/var/proftpd.pid", F_OK) == 0) {
		g_object_set(status_ftp, "label", "<span color=\"green\">läuft</span>", NULL);
	} else {
		g_object_set(status_ftp, "label", "<span color=\"red\">läuft nicht</span>", NULL);
	}

	/* PHP Version */
	if ((file = popen ("gksudo '/opt/lampp/lampp phpstatus'", "r")) == NULL)
		perror("popen");
	fgets(puffer, PIPE_BUF * 8, file);
	php = (char *) malloc(1024);
	puffer[strlen(puffer)-1] = '\0';
	snprintf(php, strlen("<span color=\"green\">") + strlen(puffer) + strlen("</span>") + 1, "%s%s%s", "<span color=\"green\">", puffer, "</span>");
	g_object_set(status_php, "label", php);
	free(php);
}

int main(int argc, char **argv) {
	GdkPixbuf *pic;
	GtkButton *lampp_start, *lampp_stop, *lampp_reload, *apache_start, *apache_stop,  *mysql_start, *mysql_stop, *ftp_start, *ftp_stop, *php_4, *php_5;
	GtkTable *table;
	GtkVBox *box;
	GtkAlignment *statusbar;
	gtk_init(&argc, &argv);
	pic = gdk_pixbuf_new_from_file("logo.gif", NULL);
	win = g_object_new(GTK_TYPE_WINDOW, "title", "Xampp", "default-width", 284, "default-height", 200, "resizable",  FALSE, "window-position", GTK_WIN_POS_CENTER, "border-width", 5, "icon", pic, NULL);
	box = g_object_new(GTK_TYPE_VBOX, "spacing", 0, NULL);
	statusbar = g_object_new(GTK_TYPE_ALIGNMENT, "yscale", 0.0, "yalign", 1.0, NULL);
	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 5, "n-rows",  3, NULL);

	lampp_start = g_object_new(GTK_TYPE_BUTTON, "label", "Lampp start", NULL);
	lampp_stop = g_object_new(GTK_TYPE_BUTTON, "label", "Lampp stop", NULL);
	lampp_reload = g_object_new(GTK_TYPE_BUTTON, "label", "Lampp reload", NULL);
	apache_start = g_object_new(GTK_TYPE_BUTTON, "label", "Apache start", NULL);
	apache_stop = g_object_new(GTK_TYPE_BUTTON, "label", "Apache stop", NULL);
	mysql_start = g_object_new(GTK_TYPE_BUTTON, "label", "MySql start", NULL);
	mysql_stop = g_object_new(GTK_TYPE_BUTTON, "label", "MySql stop", NULL);
	ftp_start = g_object_new(GTK_TYPE_BUTTON, "label", "FTP start", NULL);
	ftp_stop = g_object_new(GTK_TYPE_BUTTON, "label", "FTP stop", NULL);
	php_4 = g_object_new(GTK_TYPE_BUTTON, "label", "PHP4", NULL);
	php_5 = g_object_new(GTK_TYPE_BUTTON, "label", "PHP5", NULL);

	status_apache = g_object_new(GTK_TYPE_LABEL, "justify", GTK_JUSTIFY_CENTER, "use-markup", TRUE, "label", "", NULL);
	status_mysql = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", "", NULL);
	status_ftp = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", "", NULL);
	status_php = g_object_new(GTK_TYPE_LABEL, "use-markup", TRUE, "label", "", NULL);
	status = g_object_new(GTK_TYPE_STATUSBAR, NULL);
	gtk_statusbar_push(status, 0, "");
	gtk_statusbar_set_has_resize_grip(status, FALSE);
	ceck_status();

	g_signal_connect(lampp_start, "clicked", G_CALLBACK(lampp), "start");
	g_signal_connect(lampp_stop, "clicked", G_CALLBACK(lampp), "stop");
	g_signal_connect(lampp_reload, "clicked", G_CALLBACK(lampp), "reload");
	g_signal_connect(apache_start, "clicked", G_CALLBACK(lampp), "apache_start");
	g_signal_connect(apache_stop, "clicked", G_CALLBACK(lampp), "apache_stop");
	g_signal_connect(mysql_start, "clicked", G_CALLBACK(lampp), "mysql_start");
	g_signal_connect(mysql_stop, "clicked", G_CALLBACK(lampp), "mysql_stop");
	g_signal_connect(ftp_start, "clicked", G_CALLBACK(lampp), "ftp_start");
	g_signal_connect(ftp_stop, "clicked", G_CALLBACK(lampp), "ftp_stop");
	g_signal_connect(php_4, "clicked", G_CALLBACK(lampp), "php_4");
	g_signal_connect(php_5, "clicked", G_CALLBACK(lampp), "php_5");
	g_signal_connect(win, "delete-event", G_CALLBACK(delete_Event), NULL);
	g_signal_connect(win, "destroy", G_CALLBACK(end), NULL);

	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(lampp_start), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(lampp_stop), 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(lampp_reload), 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(apache_start), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(apache_stop), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status_apache), 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(mysql_start), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(mysql_stop), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status_mysql), 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(ftp_start), 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(ftp_stop), 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status_ftp), 2, 3, 3, 4, GTK_FILL, GTK_FILL, 0, 1);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_4), 0, 1, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_5), 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(status_php), 2, 3, 4, 5, GTK_FILL, GTK_FILL, 0, 0);

	gtk_container_add(GTK_CONTAINER(statusbar), GTK_WIDGET(status));
	gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(box));
	gtk_box_pack_start_defaults(GTK_BOX(box), GTK_WIDGET(table));
	gtk_box_pack_start_defaults(GTK_BOX(box), GTK_WIDGET(statusbar));
	gtk_widget_show_all(GTK_WIDGET(win));
	gtk_main();
	return 0;
}
