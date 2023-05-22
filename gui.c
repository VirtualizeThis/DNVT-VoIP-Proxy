#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <errno.h>
#include <gtk-4.0/gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkBuilder *builder;
    GObject *window;

    // Create a new GtkBuilder instance
    builder = gtk_builder_new_from_file("ui.xml");

    // Get the window object from the UI definition
    window = gtk_builder_get_object(builder, "window");

    // Connect the "destroy" signal to terminate the application
    g_signal_connect(window, "destroy", G_CALLBACK(g_application_quit), app);

    // Show the window
    gtk_widget_show(GTK_WIDGET(window));
}

int main(int argc, char **argv)
{
    gtk_init();
    GtkApplication *app;
    int status;

    // Create a new GtkApplication instance
    app = gtk_application_new("com.example.myapp", G_APPLICATION_FLAGS_NONE);

    // Connect the "activate" signal to the activate() function
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run the application
    status = g_application_run(G_APPLICATION(app), 0, NULL);

    sleep(10);

    // Clean up
    g_object_unref(app);

    return status;
}