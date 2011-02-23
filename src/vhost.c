#include "include.h"
#include "fnmatch.h"

char *selected_vhost = "";
int vhost_status = 0;
GtkListStore *store;
enum {
	AKTIV_COLUMN,
	VHOST_COLUMN,
	N_COLUMN
};

int set_vhost(Vhost *vhost) { /* Return 1 =  OK-Vhost create, 2 = OK-Vhost edit and -1 = NOK-Vhost dont create/edit */
	char *tmp, *tmp2, line[512], var[512], value[512];
	FILE *read_fp;
	int write_fp;
	
	tmp = malloc(strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(vhost->ServerName) + 1);
	snprintf(tmp, strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(vhost->ServerName) + 1, "/opt/lampp/etc/vhost_available/%s.conf", vhost->ServerName);
	if(access(tmp, F_OK) == 0) { /* Edit Vhost*/
		read_fp = fopen(tmp, "r");
		if(read_fp == NULL) {
			my_perror("fopen");
			return -1;
		}
				
		tmp2 = malloc(strlen("/opt/lampp/etc/vhost_available/.conf_") + strlen(vhost->ServerName) + 1);
		snprintf(tmp2, strlen("/opt/lampp/etc/vhost_available/.conf_") + strlen(vhost->ServerName) + 1, "/opt/lampp/etc/vhost_available/%s.conf_", vhost->ServerName);
		write_fp = open(tmp2, O_WRONLY | O_CREAT, 644);
		if(write_fp == -1) {
			my_perror("open");
			return -1;
		}
		
		while(fgets(line, sizeof(line), read_fp)) {
			memset(var, 0, sizeof(var));
			memset(value, 0, sizeof(value));
			
			if(sscanf(line, "<Directory %[a-zA-Z1-9/]>%[^\n]", value) == 1) {
				write(write_fp, "<Directory ", strlen("<Directory "));
				write(write_fp, vhost->DocumentRoot, strlen(vhost->DocumentRoot));
				write(write_fp, ">\n", strlen(">\n"));
			} else if(sscanf(line, "%[^ \t=]%s[\t ]=%s[\t ]%[^\n]", var, value) == 2) {
				if(strcmp(var, "ServerName") == 0) {
					write(write_fp, "ServerName ", strlen("ServerName "));
					write(write_fp, vhost->ServerName, strlen(vhost->ServerName));
					write(write_fp, "\n", strlen("\n"));
				} else if(strcmp(var, "ServerAdmin") == 0) {
					write(write_fp, "ServerAdmin ", strlen("ServerAdmin "));
					write(write_fp, vhost->ServerAdmin, strlen(vhost->ServerAdmin));
					write(write_fp, "\n", strlen("\n"));
				} else if(strcmp(var, "DocumentRoot") == 0) {
					write(write_fp, "DocumentRoot ", strlen("DocumentRoot "));
					write(write_fp, vhost->DocumentRoot, strlen(vhost->DocumentRoot));
					write(write_fp, "\n", strlen("\n"));
				} else if(strcmp(var, "Options") == 0) {
					write(write_fp, "Options ", strlen("Options "));
					write(write_fp, vhost->options, strlen(vhost->options));
					write(write_fp, "\n", strlen("\n"));
				} else {
					write(write_fp, line, strlen(line));
				}
			} else {
				write(write_fp, line, strlen(line));
			}
		}
		fclose(read_fp);
		close(write_fp);
		rename(tmp2, tmp);
		
		free(tmp2);
		free(tmp);
		return 2;
	} else { /*Create new Vhost*/
		write_fp = open(tmp, O_WRONLY | O_CREAT, 644);
		if(write_fp == -1) {
			my_perror("open");
			return -1;
		}
		
		write(write_fp, "<VirtualHost *:80>\n", strlen("<VirtualHost *:80>\n"));
		write(write_fp, "ServerName ", strlen("ServerName "));
		write(write_fp, vhost->ServerName, strlen(vhost->ServerName));
		write(write_fp, "\n", strlen("\n"));
		write(write_fp, "ServerAdmin ", strlen("ServerAdmin "));
		write(write_fp, vhost->ServerAdmin, strlen(vhost->ServerAdmin));
		write(write_fp, "\n", strlen("\n"));
		write(write_fp, "DocumentRoot ", strlen("DocumentRoot "));
		write(write_fp, vhost->DocumentRoot, strlen(vhost->DocumentRoot));
		write(write_fp, "\n", strlen("\n"));
		write(write_fp, "ErrorLog logs/", strlen("ErrorLog logs/"));
		write(write_fp, vhost->ServerName, strlen(vhost->ServerName));
		write(write_fp, "-error_log\n", strlen("-error_log\n"));
		write(write_fp, "CustomLog logs/", strlen("CustomLog logs/"));
		write(write_fp, vhost->ServerName, strlen(vhost->ServerName));
		write(write_fp, "-common-log common\n", strlen("-common-log common\n"));
		write(write_fp, "<Directory ", strlen("<Directory "));
		write(write_fp, vhost->DocumentRoot, strlen(vhost->DocumentRoot));
		write(write_fp, ">\n", strlen(">\n"));
		write(write_fp, "Options ", strlen("Options "));
		write(write_fp, vhost->options, strlen(vhost->options));
		write(write_fp, "\n", strlen("\n"));
		write(write_fp, "AllowOverride All\nOrder allow,deny\nallow from All\ndeny from none\n</Directory>\n</VirtualHost>", strlen("AllowOverride All\nOrder Allow, Deny\nallow from All\ndeny from none\n</Directory>\n</VirtualHost>"));	
		close(write_fp);
		
		free(tmp);
		return 1;
	}
}
Vhost *get_vhost(char *name) {
	Vhost *read_vhost;
	FILE *fp;
	char *tmp, line[512], var[512], value[512],  *tmp2_1, *tmp2_2;
	
	if(fnmatch("*.conf", name, 0) == 0) {
		tmp = malloc(strlen("/opt/lampp/etc/vhost_available/") + strlen(name) + 1);
		snprintf(tmp, strlen("/opt/lampp/etc/vhost_available/") + strlen(name) + 1, "/opt/lampp/etc/vhost_available/%s", name);
	} else {
		tmp = malloc(strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(name) + 1);
		snprintf(tmp, strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(name) + 1, "/opt/lampp/etc/vhost_available/%s.conf", name);
	}

	fp = fopen(tmp, "r");
	if(fp == NULL) {
		my_perror("fopen");
		return NULL;
	}
	
	read_vhost = (Vhost *)malloc(sizeof(Vhost));
	
	while(fgets(line, sizeof(line), fp)) {
		memset(var, 0, sizeof(var));
		memset(value, 0, sizeof(value));
		if(sscanf(line, "%[^ \t=]%s[\t ]=%*[\t ]%[^\n]", var, value) == 2) {
			if(strcmp(var, "ServerName") == 0) {
				strcpy(read_vhost->ServerName, (char *)value);
			} else if(strcmp(var, "ServerAdmin") == 0) {
				strcpy(read_vhost->ServerAdmin, (char *)value);
			} else if(strcmp(var, "DocumentRoot") == 0) {
				strcpy(read_vhost->DocumentRoot, (char *)value);
			} else if(strcmp(var,"Options") == 0) {
				tmp2_1 = line;
				tmp2_2 = strstr(tmp2_1, " ");
				*tmp2_2++ = '\0';
				tmp2_2[strlen(tmp2_2) - 1] = '\0';
				strcpy(read_vhost->options, (char *)tmp2_2);
			}/* else if(strcmp(var, "ErrorLog") == 0) {
				strcpy(read_vhost->ErrorLog, (char *)value);
			} else if(strcmp(var, "CustomLog") == 0) {
				strcpy(read_vhost->CustomLog, (char *)value);
			}*/
		}
	}
	fclose(fp);
	free(tmp);
	return read_vhost;
}

void selected_vhost_changed(GtkTreeView *view, gpointer nix) {
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gchar *tmp;
		gtk_tree_model_get(model, &iter, VHOST_COLUMN, &tmp, -1);
		selected_vhost = tmp;

		gtk_tree_model_get(model, &iter, AKTIV_COLUMN, &tmp, -1);
		if(strcmp(tmp, "Yes") == 0) {
			gtk_button_set_label(GTK_BUTTON(nix), _("Deactivate"));
			vhost_status = 1;
		} else {
			gtk_button_set_label(GTK_BUTTON(nix), _("Activate"));
			vhost_status = 0;
		}
		g_free(tmp);
	}
}

void list_vhost() {
	GtkTreeIter iter;
	DIR *available;
	struct dirent *content;
	char *tmp;
	Vhost *vhost;
	
	available = opendir("/opt/lampp/etc/vhost_available");
	if(available == NULL) {
		my_perror("opendir");
		return;
	}
	
	gtk_list_store_clear(store);
	
	while((content = readdir(available)) != NULL) {
		if(fnmatch("*.conf", content->d_name, 0) == 0) {
			vhost = get_vhost(content->d_name);
			gtk_list_store_append(store, &iter);
			
			tmp = malloc(strlen("/opt/lampp/etc/vhost_enable/") + strlen(content->d_name) + 1);
			snprintf(tmp, strlen("/opt/lampp/etc/vhost_enable/") + strlen(content->d_name) + 1, "/opt/lampp/etc/vhost_enable/%s", content->d_name);
			if(access(tmp, F_OK) == 0) {
				gtk_list_store_set(store, &iter, AKTIV_COLUMN, "Yes", VHOST_COLUMN, vhost->ServerName, -1);
			} else {
				gtk_list_store_set(store, &iter, AKTIV_COLUMN, "No", VHOST_COLUMN, vhost->ServerName, -1);
			}
		}
	}
	closedir(available);
}


void akti_deakti(GtkWidget *button, gpointer *nix) {
	char *tmp, *tmp2;

	if(strcmp(selected_vhost,  "") == 0) {
		return;
	}
	if(vhost_status == 1) {
		tmp = malloc(strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(selected_vhost) + 1);
		snprintf(tmp, strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(selected_vhost) + 1, "/opt/lampp/etc/vhost_enable/%s.conf", selected_vhost);
		unlink(tmp);
		list_vhost(GTK_LIST_STORE(store));
		free(tmp);
	} else {
		tmp = malloc(strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(selected_vhost) + 1);
		snprintf(tmp, strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(selected_vhost) + 1, "/opt/lampp/etc/vhost_enable/%s.conf", selected_vhost);
		tmp2 = malloc(strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(selected_vhost) + 1);
		snprintf(tmp2, strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(selected_vhost) + 1, "/opt/lampp/etc/vhost_available/%s.conf", selected_vhost);
		symlink(tmp2, tmp);
		list_vhost(GTK_LIST_STORE(store));
		free(tmp);
		free(tmp2);
	}
	lampp(NULL, "reload");
}

void vhost_add(GtkWidget *button, gpointer *modus) {
	GtkWidget *dialog, *name, *name_, *docroot, *docroot_, *admin, *admin_, *table, *options, *options_;
	int result;
	char *tmp, *tmp2, *info_text;
	Vhost *write_vhost;
	int ok;
	if(strcmp((char *)modus, "edit") == 0) {
		dialog = gtk_dialog_new_with_buttons(_("Vhost Edit"), einstellungen_win, GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_ACCEPT, _("Cancel"), GTK_RESPONSE_REJECT, NULL);
	} else {
		dialog = gtk_dialog_new_with_buttons(_("Vhost ADD"), einstellungen_win, GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_ACCEPT, _("Cancel"), GTK_RESPONSE_REJECT, NULL);
	}
	table = g_object_new(GTK_TYPE_TABLE, "n-columns", 3, "n-rows",  9, NULL);

	
	name = g_object_new(GTK_TYPE_LABEL, "label", _("Server Name:"), NULL);
	name_ = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0,  NULL);

	docroot = g_object_new(GTK_TYPE_LABEL, "label", _("Document Root:"), NULL);
	docroot_ = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0,  NULL);

	admin = g_object_new(GTK_TYPE_LABEL, "label", _("Server Admin:"), NULL);
	admin_ = g_object_new(GTK_TYPE_ENTRY, "text", "", "max-length", 0,  NULL);
	
	options = g_object_new(GTK_TYPE_LABEL, "label", _("Options"), NULL);
	options_ = g_object_new(GTK_TYPE_ENTRY, "text", "All", "max-length", 0, NULL);

	if(strcmp((char *)modus, "edit") == 0) {
		Vhost *read_vhost;
		
		if(strcmp(selected_vhost,  "") == 0) {
			return;
		}
	
		read_vhost = get_vhost(selected_vhost);
		g_object_set(name_, "text", uft8(read_vhost->ServerName), "editable", FALSE, NULL);
		g_object_set(admin_, "text", uft8(read_vhost->ServerAdmin), NULL);
		g_object_set(docroot_, "text", uft8(read_vhost->DocumentRoot), NULL);
		g_object_set(options_, "text", uft8(read_vhost->options), NULL);
		free(read_vhost);
	}
	
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(name), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(name_), 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(docroot), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(docroot_), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(admin), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(admin_), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(options), 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(options_), 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), GTK_WIDGET(table));
	gtk_widget_show_all(dialog);

	ok = 0;
	while(ok != 1) {
		ok = 1;
		result = gtk_dialog_run(GTK_DIALOG(dialog));
		if(result == GTK_RESPONSE_ACCEPT) {
			g_object_get(name_, "text", &tmp, NULL);
			if(strcmp(tmp, "") == 0) {
				my_info(_("Please enter a Servername!"), GTK_WINDOW(dialog));
				ok = 0;
				continue;
			}
			g_object_get(docroot_, "text", &tmp, NULL);
			if(strcmp(tmp, "") == 0) {
				my_info(_("Please enter a Documentroot!"), GTK_WINDOW(dialog));
				ok = 0;
				continue;
			}
		}
	}

	
	switch(result) {
		case GTK_RESPONSE_ACCEPT:
		
			write_vhost = (Vhost *)malloc(sizeof(Vhost));
			g_object_get(name_, "text", &tmp, NULL);
			strcpy(write_vhost->ServerName, tmp);
			g_object_get(admin_, "text", &tmp, NULL);
			strcpy(write_vhost->ServerAdmin, tmp);
			g_object_get(docroot_, "text", &tmp, NULL);
			strcpy(write_vhost->DocumentRoot, tmp);
			g_object_get(options_, "text", &tmp, NULL);
			strcpy(write_vhost->options, tmp);

			if(strcmp(write_vhost->options, "") == 0) {
				strcpy(write_vhost->options, "none");
			}
			
			switch(set_vhost(write_vhost)) {
				case 1: /*Vhost create*/
					info_text = malloc(strlen(_("The new Vhost \"%s\" was create!")) + strlen(write_vhost->ServerName) + 1);
					snprintf(info_text, strlen(_("The new Vhost \"%s\" was create!")) + strlen(write_vhost->ServerName) + 1, _("The new Vhost \"%s\" was create!"), write_vhost->ServerName);
					tmp = malloc(strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(write_vhost->ServerName) + 1);
					snprintf(tmp, strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(write_vhost->ServerName) + 1, "/opt/lampp/etc/vhost_enable/%s.conf", write_vhost->ServerName);
					tmp2 = malloc(strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(write_vhost->ServerName) + 1);
					snprintf(tmp2, strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(write_vhost->ServerName) + 1, "/opt/lampp/etc/vhost_available/%s.conf", write_vhost->ServerName);
					symlink(tmp2, tmp);
					free(tmp);
					free(tmp2);
					lampp(NULL, "reload");
					break;
				case 2: /*Vhost edit*/
					info_text = malloc(strlen(_("The Vhost \"%s\" was changed!")) + strlen(write_vhost->ServerName) + 1);
					snprintf(info_text, strlen(_("The Vhost \"%s\" was changed!")) + strlen(write_vhost->ServerName) + 1, _("The Vhost \"%s\" was changed!"), write_vhost->ServerName);
					lampp(NULL, "reload");
					break;
				case -1: /*Error*/
					break;
			}
			
			if(access(write_vhost->DocumentRoot, F_OK) != 0) {
				mkdir(write_vhost->DocumentRoot, 644);
			}
			my_info(info_text, einstellungen_win);
			free(write_vhost);
			break;
		case GTK_RESPONSE_REJECT:
		case GTK_RESPONSE_DELETE_EVENT:
			break;
	}

	list_vhost(GTK_LIST_STORE(store));
	gtk_widget_destroy(dialog);
}

void vhost_del(GtkWidget *button, gpointer *nix) {
	char *tmp;

	if(strcmp(selected_vhost,  "") == 0) {
		return;
	}
	
	tmp = malloc(strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(selected_vhost) + 1);
	snprintf(tmp, strlen("/opt/lampp/etc/vhost_available/.conf") + strlen(selected_vhost) + 1, "/opt/lampp/etc/vhost_available/%s.conf", selected_vhost);
	unlink(tmp);
	free(tmp);
	tmp = malloc(strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(selected_vhost) + 1);
	snprintf(tmp, strlen("/opt/lampp/etc/vhost_enable/.conf") + strlen(selected_vhost) + 1, "/opt/lampp/etc/vhost_enable/%s.conf", selected_vhost);
	unlink(tmp);
	free(tmp);
	list_vhost(GTK_LIST_STORE(store));
	lampp(NULL, "reload");
}

void vhosts(void) {
	GtkWidget *dialog;
	GtkCellRenderer *renderer;
	GtkWidget *view, *vbox, *hbox, *akti, *del, *edit, *add;
	GtkTreeSelection  *selection;
	
	dialog = gtk_dialog_new_with_buttons(_("Vhost"), einstellungen_win, GTK_DIALOG_DESTROY_WITH_PARENT, _("Close"), GTK_RESPONSE_REJECT, NULL);

	view = gtk_tree_view_new();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1, "Aktivate", renderer, "text", AKTIV_COLUMN, NULL);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1, "VHost", renderer, "text", VHOST_COLUMN, NULL);
	store = gtk_list_store_new(N_COLUMN, G_TYPE_STRING, G_TYPE_STRING);
	akti = g_object_new(GTK_TYPE_BUTTON, "label", _("Activate"), NULL);
	del = g_object_new(GTK_TYPE_BUTTON, "label", _("Delete"), NULL);
	edit = g_object_new(GTK_TYPE_BUTTON, "label", _("Edit"), NULL);
	add = g_object_new(GTK_TYPE_BUTTON, "label", _("Add"), NULL);
	vbox = gtk_vbutton_box_new();
	hbox = gtk_hbox_new(FALSE, 2);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	
	list_vhost(store);
	gtk_tree_view_set_model(GTK_TREE_VIEW (view), GTK_TREE_MODEL (store));
	g_object_unref (GTK_TREE_MODEL (store));
	
	
	g_signal_connect(add, "clicked", G_CALLBACK(vhost_add), "add");
	g_signal_connect(edit, "clicked", G_CALLBACK(vhost_add), "edit");
	g_signal_connect(del, "clicked", G_CALLBACK(vhost_del), NULL);
	g_signal_connect(akti, "clicked", G_CALLBACK(akti_deakti), store);
	g_signal_connect(GTK_WIDGET(view), "cursor-changed", G_CALLBACK(selected_vhost_changed), akti);
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(akti));
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(add));
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(edit));
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(del));
	gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(view));
	gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(vbox));
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), GTK_WIDGET(hbox));
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
