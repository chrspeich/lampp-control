#include "include.h"

void xampp_pw(void);
void mysql_pw(void);
void phpini(void);

void einstellungen(void) {

	GtkTable *table;
	GtkButton *pw_xampp, *pw_mysql, *phpini_;
	einstellungen_win = g_object_new(GTK_TYPE_WINDOW, "title", _("Settings"), "default-width", 284, "default-height", 200, "resizable",  FALSE, "window-position", GTK_WIN_POS_CENTER, "border-width", 5, "icon", main_data.pic, "skip-pager-hint", TRUE, "skip-taskbar-hint", TRUE, NULL);
	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 4, "n-rows",  1, NULL);

	pw_xampp = g_object_new(GTK_TYPE_BUTTON, "label", _("Xammpside with password protect"), NULL);
	pw_mysql = g_object_new(GTK_TYPE_BUTTON, "label", _("Set new mysql-pw"), NULL);
	phpini_ = g_object_new(GTK_TYPE_BUTTON, "label", _("PHP Settings"), NULL);

	g_signal_connect(pw_xampp, "clicked", G_CALLBACK(xampp_pw), NULL);
	g_signal_connect(pw_mysql, "clicked", G_CALLBACK(mysql_pw), NULL);
	g_signal_connect(phpini_, "clicked", G_CALLBACK(phpini), NULL);

	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw_xampp), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw_mysql), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(phpini_), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_container_add(GTK_CONTAINER(einstellungen_win), GTK_WIDGET(table));
	gtk_widget_show_all(GTK_WIDGET(einstellungen_win));
}

void xampp_pw(void) {
	GtkTable *table;
	GtkLabel *pw1, *pw2, *user1, *hinweis;
	GtkButton *pw, *wpw, *user;
	GtkWidget *xdialog;
	gchar *tmp, *tmp2, *tmp5;
	gint result;
	FILE *file;

	xdialog = gtk_dialog_new_with_buttons(_("New Password"), einstellungen_win, GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_ACCEPT, _("Cancel"), GTK_RESPONSE_REJECT, NULL);
	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 5, "n-rows",  2, NULL);

	hinweis = g_object_new(GTK_TYPE_LABEL, "label", _("This protect the Xamppsides with a Password."), NULL);
	user1 = g_object_new(GTK_TYPE_LABEL, "label", _("User:"), NULL);
	user = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0, NULL);
	pw1 = g_object_new(GTK_TYPE_LABEL, "label", _("Password:"), NULL);
	pw = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0, "visibility", FALSE,  NULL);
	pw2 = g_object_new(GTK_TYPE_LABEL, "label", _("Again:"), NULL);
	wpw = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0, "visibility", FALSE,  NULL);

	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(hinweis), 0, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(user1), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(user), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw1), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw2), 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(wpw), 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(xdialog)->vbox), GTK_WIDGET(table));
	gtk_widget_show_all(xdialog);

	result = gtk_dialog_run(GTK_DIALOG(xdialog));

	switch(result) {
	case GTK_RESPONSE_ACCEPT:
		g_object_get(pw, "text", &tmp, NULL);
		g_object_get(wpw, "text", &tmp2, NULL);
		g_object_get(user, "text", &tmp5, NULL);
		gtk_widget_destroy(xdialog);
		if(strcmp(tmp5, "") == 0) {
			my_info(_("No Username entered."), einstellungen_win);
		} else if(strcmp(tmp, tmp2) != 0) {
			my_info(_("The passwords do not agree."), einstellungen_win);
		} else if(strcmp(tmp, "") == 0) {
			my_info(_("No password entered."), einstellungen_win);
		} else {
			FILE *file;
			char puffer[PIPE_BUF * 8], *tmp3, *tmp4;
			tmp3 = (char *) malloc(1024);
			snprintf(tmp3, strlen("/opt/lampp/share/lampp/crypt ") + strlen(tmp) + 1, "%s%s", "/opt/lampp/share/lampp/crypt ", tmp);
			file = popen (tmp3, "r");
			if (file == NULL)
				perror("popen");
			fgets(puffer, PIPE_BUF * 8, file);
			tmp4= (char *) malloc(1024);
			puffer[strlen(puffer)-1] = '\0';
			file = fopen("/opt/lampp/lib/xampp.users", "w");
			if(file == NULL) {
				my_error(_("Action could not be implement.\nSee Console for Details."), einstellungen_win);
				perror("fopen");
				return;
			}
			snprintf(tmp4, strlen(tmp5) + strlen(":") + strlen(puffer) + 1, "%s%s%s", tmp5, ":",  puffer);
			fputs(tmp4, file);
			free(tmp3);
			fclose(file);

			file = fopen("/opt/lampp/htdocs/xampp/.htaccess", "w");
			if(file == NULL) {
				my_error(_("Action could not be implement.\nSee Console for Details."), einstellungen_win);
				perror("fopen");
				return;
			}
			fputs("AuthName \"xampp user\"\n", file);
			fputs("AuthType Basic\n", file);
			fputs("AuthUserFile /opt/lampp/lib/xampp.users\n", file);
			fputs("require valid-user\n", file);
			fclose(file);

			copyfile("/opt/lampp/htdocs/xampp/.htaccess", "/opt/lampp/htdocs/webalizer/.htaccess");
			copyfile("/opt/lampp/htdocs/xampp/.htaccess", "/opt/lampp/phpmyadmin/.htaccess");
			copyfile("/opt/lampp/htdocs/xampp/.htaccess", "/opt/lampp/phpsqliteadmin/.htaccess");
			chmod("/opt/lampp/lib/xampp.users", 644);
			chmod("/opt/lampp/htdocs/xampp/.htaccess", 644);
			chmod("/opt/lampp/htdocs/webalizer/.htaccess", 644);
			chmod("/opt/lampp/phpmyadmin/.htaccess", 644);
			chmod("/opt/lampp/phpsqliteadmin/.htaccess", 644);
			my_info(_("Password successfully set."), einstellungen_win);
		}
		break;
	case GTK_RESPONSE_REJECT:
		gtk_widget_destroy(xdialog);
		break;
	case GTK_RESPONSE_DELETE_EVENT:
		gtk_widget_destroy(xdialog);
		break;
	}
}

void mysql_pw(void) {
	GtkTable *table;
	GtkLabel *pw1, *pw2, *old_pw1, *hinweis;
	GtkButton *pw, *wpw, *old_pw;
	GtkWidget *xdialog;
	gint result;
	gchar *tmp, *tmp2, *tmp3;

	xdialog = gtk_dialog_new_with_buttons(_("New Password"), einstellungen_win, GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_ACCEPT, _("Cancel"), GTK_RESPONSE_REJECT, NULL);

	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 5, "n-rows",  2, NULL);
 
	hinweis = g_object_new(GTK_TYPE_LABEL, "label", _("If you has not set a root-password(Mysql),\nthen you can empty Old Password."), NULL);
	old_pw1 = g_object_new(GTK_TYPE_LABEL, "label", _("Old Password:"), NULL);
	old_pw = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0, "visibility", FALSE, NULL);
	pw1 = g_object_new(GTK_TYPE_LABEL, "label", _("Password:"), NULL);
	pw = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0, "visibility", FALSE,  NULL);
	pw2 = g_object_new(GTK_TYPE_LABEL, "label", _("Again:"), NULL);
	wpw = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0, "visibility", FALSE,  NULL);

	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(hinweis), 0, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(old_pw1), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(old_pw), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw1), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(pw2), 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(wpw), 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(xdialog)->vbox), GTK_WIDGET(table));
	gtk_widget_show_all(xdialog);

	result = gtk_dialog_run(GTK_DIALOG(xdialog));

	switch(result) {
	case GTK_RESPONSE_ACCEPT:
		g_object_get(pw, "text", &tmp, NULL);
		g_object_get(wpw, "text", &tmp2, NULL);
		g_object_get(old_pw, "text", &tmp3, NULL);
		gtk_widget_destroy(xdialog);
		if(strcmp(tmp, tmp2) != 0) {
			my_info(_("The passwords do not agree."), einstellungen_win);
		} else if(strcmp(tmp, "") == 0) {
			my_info(_("No password entered."), einstellungen_win);
		} else {
			char *tmp4, *tmp5, *tmp6;
			FILE *file;
			char puffer[PIPE_BUF * 8];
			if(strcmp(tmp3, "") == 0) {
				tmp4 = malloc(1024);
				snprintf(tmp4, strlen("echo \"update user set Password=password('") + strlen(tmp) + strlen("') where User = 'root';\" | /opt/lampp/bin/mysql -uroot mysql") + 1, "%s%s%s", "echo \"update user set Password=password('", tmp, "') where User = 'root';\" | /opt/lampp/bin/mysql -uroot mysql");

				tmp5 = malloc(strlen("gksudo /opt/lampp/bin/mysqladmin reload"));
				tmp5 = "gksudo /opt/lampp/bin/mysqladmin reload";
			} else {
				tmp4 = malloc(1024);
				snprintf(tmp4, strlen("echo \"update user set Password=password('") + strlen(tmp) + strlen("') where User = 'root';\" | /opt/lampp/bin/mysql -uroot -p") + strlen(tmp3) + strlen(" mysql") + 1, "%s%s%s%s%s", "echo \"update user set Password=password('", tmp, "') where User = 'root';\" | /opt/lampp/bin/mysql -uroot -p", tmp3, " mysql");

				tmp5 = malloc(1024);
				snprintf(tmp5, strlen("/opt/lampp/bin/mysqladmin -p") + strlen(tmp3) + strlen(" reload") + 1, "%s%s%s", "/opt/lampp/bin/mysqladmin -p", tmp3, " reload");
			}
			if ((file = popen (tmp4, "r")) == NULL)
				perror("popen");

			tmp6 = (char *) malloc(2048);
			while(fgets(puffer, 2048, file) != NULL) {
				strcat(tmp6, uft8(puffer));
			}
			system(tmp5);

			free(tmp4);
		}
		break;
	case GTK_RESPONSE_REJECT:
		gtk_widget_destroy(xdialog);
		break;
	case GTK_RESPONSE_DELETE_EVENT:
		gtk_widget_destroy(xdialog);
		break;
	}
}


struct on_off {
	GtkRadioButton *on;
	GtkRadioButton *off;
};

struct php_ini_data {
	struct on_off short_open_tag;
	struct on_off asp_tags;
	GtkButton *max_execution_time;
	GtkButton *memory_limit;
	struct on_off display_errors;
	struct on_off log_errors;
	GtkButton *post_max_size;
	struct on_off magic_quotes_gpc;
	struct on_off file_uploads;
	//GtkButton *upload_max_filesize;
} php_ini_data;

void phpini(void) {
	GtkWidget *xdialog;
	gint result;
	GtkTable *table;
	gpointer daten;
	GtkLabel *short_open_tag_, *asp_tags_, *max_execution_time_, *memory_limit_, *display_errors_, *log_errors_, *post_max_size_, *magic_quotes_gpc_, *file_uploads_, *upload_max_filesize_;
	gchar *short_open_tag, *asp_tags, *max_execution_time, *memory_limit, *display_errors, *log_errors, *post_max_size, *magic_quotes_gpc, *file_uploads, *upload_max_filesize;
	char *tmp;
	FILE *file;
	int file1;

	xdialog = gtk_dialog_new_with_buttons(_("PHP Settings"), einstellungen_win, GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_ACCEPT, _("Cancel"), GTK_RESPONSE_REJECT, NULL);
	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 10, "n-rows",  3, NULL);

	short_open_tag_ = g_object_new(GTK_TYPE_LABEL, "label", "short_open_tag", NULL);
	php_ini_data.short_open_tag.on = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "On", NULL);
	php_ini_data.short_open_tag.off = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "Off", "group", php_ini_data.short_open_tag.on, NULL);

	asp_tags_ = g_object_new(GTK_TYPE_LABEL, "label", "asp_tags", NULL);
	php_ini_data.asp_tags.on = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "On", NULL);
	php_ini_data.asp_tags.off = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "Off", "group", php_ini_data.asp_tags.on, NULL);

	max_execution_time_ = g_object_new(GTK_TYPE_LABEL, "label", "max_execution_time", NULL);
	php_ini_data.max_execution_time = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0,  NULL);

	memory_limit_ = g_object_new(GTK_TYPE_LABEL, "label", "memory_limit", NULL);
	php_ini_data.memory_limit = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0,  NULL);

	display_errors_ = g_object_new(GTK_TYPE_LABEL, "label", "display_errors", NULL);
	php_ini_data.display_errors.on = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "On", NULL);
	php_ini_data.display_errors.off = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "Off", "group", php_ini_data.display_errors.on, NULL);

	log_errors_ = g_object_new(GTK_TYPE_LABEL, "label", "log_errors", NULL);
	php_ini_data.log_errors.on = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "On", NULL);
	php_ini_data.log_errors.off = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "Off", "group", php_ini_data.log_errors.on, NULL);

	post_max_size_ = g_object_new(GTK_TYPE_LABEL, "label", "post_max_size", NULL);
	php_ini_data.post_max_size = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0,  NULL);

	magic_quotes_gpc_ = g_object_new(GTK_TYPE_LABEL, "label", "magic_quotes_gpc", NULL);
	php_ini_data.magic_quotes_gpc.on = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "On", NULL);
	php_ini_data.magic_quotes_gpc.off = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "Off", "group", php_ini_data.magic_quotes_gpc.on, NULL);

	file_uploads_ = g_object_new(GTK_TYPE_LABEL, "label", "file_uploads", NULL);
	php_ini_data.file_uploads.on = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "On", NULL);
	php_ini_data.file_uploads.off = g_object_new(GTK_TYPE_RADIO_BUTTON, "label", "Off", "group", php_ini_data.file_uploads.on, NULL);

	//upload_max_filesize_ = g_object_new(GTK_TYPE_LABEL, "label", "upload_max_filesize", NULL);
	//php_ini_data.upload_max_filesize = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0,  NULL);

	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(short_open_tag_), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.short_open_tag.on), 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.short_open_tag.off), 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(asp_tags_), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.asp_tags.on), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.asp_tags.off), 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(max_execution_time_), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.max_execution_time), 1, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(memory_limit_), 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.memory_limit), 1, 3, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(display_errors_), 0, 1, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.display_errors.on), 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.display_errors.off), 2, 3, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(log_errors_), 0, 1, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.log_errors.on), 1, 2, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.log_errors.off), 2, 3, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(post_max_size_), 0, 1, 6, 7, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.post_max_size), 1, 3, 6, 7, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(magic_quotes_gpc_), 0, 1, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.magic_quotes_gpc.on), 1, 2, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.magic_quotes_gpc.off), 2, 3, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(file_uploads_), 0, 1, 8, 9, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.file_uploads.on), 1, 2, 8, 9, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.file_uploads.off), 2, 3, 8, 9, GTK_FILL, GTK_FILL, 0, 0);
	//gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(upload_max_filesize_), 0, 1, 9, 10, GTK_FILL, GTK_FILL, 0, 0);
	//gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(php_ini_data.upload_max_filesize), 1, 3, 9, 10, GTK_FILL, GTK_FILL, 0, 0);

	FILE *fp = fopen("/opt/lampp/etc/php.ini", "r");
	char var[512], value[512], line[512], tmp1[512], tmp2[512];
	int i, i2;
	if (fp) {
		while (fgets(line, sizeof(line), fp)) {
			if (sscanf(line, "%[^ \t=]%*[\t ]=%*[\t ]%[^\n]", var, value) == 2) {
				if (sscanf(value, "%[^ \t=]%*[\t ];%*[\t ]%[^\n]", tmp1, tmp2) == 2) {
					strcpy(value, tmp1);
				}
				if(strcmp(var, "short_open_tag") == 0) {
					if(strcmp(value, "On") == 0) {
						g_object_set(php_ini_data.short_open_tag.on, "active", 1, NULL);
					} else {
						g_object_set(php_ini_data.short_open_tag.off, "active", 1, NULL);
					}
				} else if(strcmp(var, "asp_tags") == 0) {
					if(strcmp(value, "On") == 0) {
						g_object_set(php_ini_data.asp_tags.on, "active", 1, NULL);
					} else {
						g_object_set(php_ini_data.asp_tags.off, "active", 1, NULL);
					}
				} else if(strcmp(var, "max_execution_time") == 0) {
					g_object_set(php_ini_data.max_execution_time, "text", value, NULL);
				} else if(strcmp(var, "memory_limit") == 0) {
					g_object_set(php_ini_data.memory_limit, "text", value, NULL);
				} else if(strcmp(var, "display_errors") == 0) {
					if(strcmp(value, "On") == 0) {
						g_object_set(php_ini_data.display_errors.on, "active", 1, NULL);
					} else {
						g_object_set(php_ini_data.display_errors.off, "active", 1, NULL);
					}
				} else if(strcmp(var, "log_errors") == 0) {
					if(strcmp(value, "On") == 0) {
						g_object_set(php_ini_data.log_errors.on, "active", 1, NULL);
					} else {
						g_object_set(php_ini_data.log_errors.off, "active", 1, NULL);
					}
				} else if(strcmp(var, "post_max_size") == 0) {
					g_object_set(php_ini_data.post_max_size, "text", uft8(value), NULL);
				} else if(strcmp(var, "magic_quotes_gpc") == 0) {
					if(strcmp(value, "On") == 0) {
						g_object_set(php_ini_data.magic_quotes_gpc.on, "active", 1, NULL);
					} else {
						g_object_set(php_ini_data.magic_quotes_gpc.off, "active", 1, NULL);
					}
				} else if(strcmp(var, "file_uploads") == 0) {
					if(strcmp(value, "On") == 0) {
						g_object_set(php_ini_data.file_uploads.on, "active", 1, NULL);
					} else {
						g_object_set(php_ini_data.file_uploads.off, "active", 1, NULL);
					}
				}/* else if(strcmp(var, "upload_max_filesize") == 0) {
					g_object_set(php_ini_data.upload_max_filesize, "text", value, NULL);
				}*/
			}
		}
		fclose(fp);
	} else {
		my_error(_("Cant open php.ini"), einstellungen_win);
		return;
	}

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(xdialog)->vbox), GTK_WIDGET(table));
	gtk_widget_show_all(xdialog);
	result = gtk_dialog_run(GTK_DIALOG(xdialog));
	switch(result) {
	case GTK_RESPONSE_ACCEPT:
		g_object_get(php_ini_data.short_open_tag.on, "active", &short_open_tag, NULL);
		g_object_get(php_ini_data.asp_tags.on, "active", &asp_tags, NULL);					
		g_object_get(php_ini_data.max_execution_time, "text", &max_execution_time, NULL);
		g_object_get(php_ini_data.memory_limit, "text", &memory_limit, NULL);
		g_object_get(php_ini_data.display_errors.on, "active", &display_errors, NULL);
		g_object_get(php_ini_data.log_errors.on, "active", &log_errors, NULL);
		g_object_get(php_ini_data.post_max_size, "text", &post_max_size, NULL);
		g_object_get(php_ini_data.magic_quotes_gpc.on, "active", &magic_quotes_gpc, NULL);
		g_object_get(php_ini_data.file_uploads.on, "active", &file_uploads, NULL);
		//g_object_get(php_ini_data.upload_max_filesize, "text", &upload_max_filesize, NULL);
		gtk_widget_destroy(xdialog);

		file = fopen("/opt/lampp/etc/php.ini", "r");
		if(file == NULL) {
			my_error(_("Action could not be implement.\nSee Console for Details."), einstellungen_win);
			perror("fopen");
			return;
		}
		/*file[1] = fopen("/opt/lampp/share/lampp_control/tmp.conf", "w+");
		if(file[1] == NULL) {
			my_error(_("Action could not be implement.\nSee Console for Details."), einstellungen_win);
			perror("fopen");
			fclose(file[0]);
			return;
		}*/

		file1 = open("/opt/lampp/share/lampp_control/tmp.conf", O_WRONLY | O_CREAT, 644);
		if(file == -1) {
			perror("open");
		}
		tmp = (char *) malloc(1024);
		while (fgets(line, sizeof(line), file)) {
			if (sscanf(line, "%[^ \t=]%*[\t ]=%*[\t ]%[^\n]", var, value) == 2) {
        			if(strcmp("short_open_tag", var) == 0) {
					snprintf(tmp, strlen("short_open_tag = ") + strlen(short_open_tag ? "On" : "Off") + strlen("\n") + 1, "%s%s%s", "short_open_tag = ", short_open_tag ? "On" : "Off", "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("asp_tags", var) == 0) {
					snprintf(tmp, strlen("asp_tags = ") + strlen(asp_tags ? "On" : "Off") + strlen("\n") + 1, "%s%s%s", "asp_tags = ", asp_tags ? "On" : "Off", "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("max_execution_time", var) == 0) {
					snprintf(tmp, strlen("max_execution_time = ") + strlen(max_execution_time) + strlen("\n") + 1, "%s%s%s", "max_execution_time = ", max_execution_time, "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("memory_limit", var) == 0) {
					snprintf(tmp, strlen("memory_limit = ") + strlen(memory_limit) + strlen("\n") + 1, "%s%s%s", "memory_limit = ", memory_limit, "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("display_errors", var) == 0) {
					snprintf(tmp, strlen("display_errors = ") + strlen(display_errors ? "On" : "Off") + strlen("\n") + 1, "%s%s%s", "display_errors = ", display_errors ? "On" : "Off", "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("log_errors", var) == 0) {
					snprintf(tmp, strlen("log_errors = ") + strlen(log_errors ? "On" : "Off") + strlen("\n") + 1, "%s%s%s", "log_errors = ", log_errors ? "On" : "Off", "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("post_max_size", var) == 0) {
					snprintf(tmp, strlen("post_max_size = ") + strlen(post_max_size) + strlen("\n") + 1, "%s%s%s", "post_max_size = ", post_max_size, "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("magic_quotes_gpc", var) == 0) {
					snprintf(tmp, strlen("magic_quotes_gpc = ") + strlen(magic_quotes_gpc ? "On" : "Off") + strlen("\n") + 1, "%s%s%s", "magic_quotes_gpc = ", magic_quotes_gpc ? "On" : "Off", "\n");
					write(file1, tmp, strlen(tmp));
				} else if(strcmp("file_uploads", var) == 0) {
					snprintf(tmp, strlen("file_uploads = ") + strlen(file_uploads ? "On" : "Off") + strlen("\n") + 1, "%s%s%s", "file_uploads = ", file_uploads ? "On" : "Off", "\n");
					write(file1, tmp, strlen(tmp));
				/*} else if(strcmp("upload_max_filesize", var) == 0) {
					snprintf(tmp, strlen("upload_max_filesize = ") + strlen(upload_max_filesize) + strlen("\n") + 1, "%s%s%s", "upload_max_filesize = ", upload_max_filesize, "\n");
					//fputs(tmp, file[1]);
					g_print(uft8(upload_max_filesize));
					fputs(line, file[1]);*/
				} else {
					write(file1, line, strlen(line));
				}
			} else {
				write(file1, line, strlen(line));
	 		}
		}
		free(tmp);

		fclose(file);
		close(file1);

		if(rename("/opt/lampp/share/lampp_control/tmp.conf", "/opt/lampp/etc/php.ini") == -1) {
			perror("rename");
		}
		
		system("/opt/lampp/lampp reloadapache");
		break;
	case GTK_RESPONSE_REJECT:
		gtk_widget_destroy(xdialog);
		break;
	case GTK_RESPONSE_DELETE_EVENT:
		gtk_widget_destroy(xdialog);
		break;
	}
}
