#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <dlfcn.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#ifndef __NLS_H__
#define __NLS_H__ 
#define _(String) g_locale_to_utf8(gettext(String), strlen(gettext(String)), NULL, NULL, NULL)
#define N_(String) (String) 
#endif
#define uft8(String) g_locale_to_utf8(String, strlen(String), NULL, NULL, NULL)


struct main_data {
	GtkWindow *win;
	GdkPixbuf *pic;
} main_data;

struct status {
	GtkLabel *apache;
	GtkLabel *mysql;
	GtkLabel *ftp;
	GtkLabel *php;
} status;

void einstellungen(void);
void my_error(char *fromat, GtkWindow *win);
void my_info(char *format, GtkWindow *win);
void copyfile(char *from, char *to);
void license(void);
void lampp(GtkWidget *tmppp, gpointer daten);

GtkWindow *einstellungen_win;
