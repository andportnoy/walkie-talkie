struct toggle_button_state {
	int on;
	char *son;
	char *soff;
};

static void toggle(GtkToggleButton *togglebutton, gpointer user_data) {
	struct toggle_button_state *tbs = user_data;
	tbs->on = !tbs->on;
	gtk_button_set_label(
	  GTK_BUTTON(togglebutton), tbs->on? tbs->son: tbs->soff);
}

static void activate(GtkApplication *app, gpointer user_data) {
	GtkWidget *window;
	GtkWidget *toggle_button;
	GtkWidget *button_box;

	struct toggle_button_state *tbs = user_data;
	char *initial_label = tbs->on? tbs->son: tbs->soff;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "PTT");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(window), button_box);

	toggle_button = gtk_toggle_button_new_with_label(initial_label);
	g_signal_connect(toggle_button, "toggled", G_CALLBACK(toggle), tbs);
	gtk_container_add(GTK_CONTAINER(button_box), toggle_button);

	gtk_widget_show_all(window);
}

void *gui_run(void *arg) {
	GtkApplication *app = gtk_application_new(
	  "com.aportnoy.ptt", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), arg);
	g_application_run(G_APPLICATION(app), 0, NULL);
	g_object_unref(app);

	return NULL;
}

int main(void) {
	struct toggle_button_state tbs = {
		.on   = 0,
		.son  = "Stop",
		.soff = "Start",
	};
	gui_run(&tbs);
}
