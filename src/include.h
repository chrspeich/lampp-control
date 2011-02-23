#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include <dirent.h>


#ifndef __NLS_H__
#define __NLS_H__ 
#define _(String) g_locale_to_utf8(gettext(String), strlen(gettext(String)), NULL, NULL, NULL)
#define N_(String) (String)
#endif
#define uft8(String) g_locale_to_utf8(String, strlen(String), NULL, NULL, NULL)
#define my_perror(tmp) _my_perror(tmp, __FILE__, __LINE__)

struct main_data {
	GtkWindow *win;
	GdkPixbuf *pic;
	int show;
} main_data;

struct module {
	GtkLabel *name;
	GtkLabel *status;
	GtkButton *start;
	GtkButton *stop;
	int raw_status;
};

struct Modules {
	struct module Apache;
	struct module Mysql;
	struct module Ftp;
	GtkLabel *php_name;
	GtkLabel *php_status;
	GtkButton *php_4;
	GtkButton *php_5;
	int php_raw_status;
	GtkButton *lampp_reload, *lampp_stop, *lampp_start;
	int work;
} Modules;

struct status {
	GtkLabel *apache;
	GtkLabel *mysql;
	GtkLabel *ftp;
	GtkLabel *php;
} status;

struct status_win {
	GtkWindow *win;
	GtkLabel *status_label;
} status_win;

struct _vhost {
	char ServerName[512];
	char ServerAdmin[512];
	char DocumentRoot[512];
	char options[1024];
/*	char ErrorLog[512];
	char CustomLog[512];*/
};

typedef struct _vhost Vhost;

void einstellungen(void);
void my_error(char *fromat, GtkWindow *win);
void _my_perror(char *tmp, char *quelldatei, int line);
void my_info(char *format, GtkWindow *win);
void copyfile(char *from, char *to);
void license(void);
void lampp(GtkWidget *tmppp, gpointer daten);
void tray_init();
void vhosts(void);
int kom_status(char *pid_file);

GtkWindow *einstellungen_win;
GtkWidget *trayicon;
char *xampp_version;
