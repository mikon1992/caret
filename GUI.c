#include <gtk/gtk.h>
#include "globals.h"
#include "file_ops.h"

GtkWidget *window;
GtkWidget *textview;

void setup_gui(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkCssProvider *css_provider;

    gtk_init(&argc, &argv);
    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    //load CSS

    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "GUI_code.glade", NULL) == 0) {
        g_print("Waduh, gagal memuat file GUI_code.glade!\n");
        return;
    }

    // Ambil widget
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main"));
    textview = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    
}