static void press(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	(void) widget;
	(void) event;

	printf("int value: %d ->", *(int *)user_data);
	*(int *)user_data = 1;
	printf(" %d\n", *(int *)user_data);
}

static void release(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	(void) widget;
	(void) event;

	printf("int value: %d ->", *(int *)user_data);
	*(int *)user_data = 0;
	printf(" %d\n", *(int *)user_data);
}

static void activate(GtkApplication *app, gpointer user_data) {
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *button_box;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "PTT");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(window), button_box);

	button = gtk_button_new_with_label("Push-to-talk");
	g_signal_connect(
	  button, "button-press-event", G_CALLBACK(press), user_data);
	g_signal_connect(
	  button, "button-release-event", G_CALLBACK(release), user_data);
	gtk_container_add(GTK_CONTAINER(button_box), button);

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
	int value = 0;
	pthread_t t;
	pthread_create(&t, 0, gui_run, &value);
	pthread_join(t, NULL);
}
