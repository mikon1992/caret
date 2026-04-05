#include <gtk/gtk.h>
#include "globals.h"

// Deklarasi variabel global HANYA DI SINI (jangan pakai extern)
GtkWidget *window;
GtkWidget *textview;
GtkWidget *label_word_count;

// Beri tahu kompilator bahwa fungsi ini ada di Word_Count.c
extern void on_text_changed(GtkTextBuffer *buffer, gpointer user_data); 
extern void on_buffer_changed_array(GtkTextBuffer *buffer, gpointer user_data);

void setup_gui(int argc, char *argv[]) {
    GtkBuilder *builder;

    gtk_init(&argc, &argv);
    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "GUI_code.glade", NULL) == 0) {
        g_print("Waduh, gagal memuat file GUI_code.glade!\n");
        return;
    }

    // Ambil widget dari Glade
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main"));
    textview = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
    
    // Ambil widget label word count (pastikan id-nya di Glade adalah "word_count")
    label_word_count = GTK_WIDGET(gtk_builder_get_object(builder, "word_count"));

    // Hubungkan sinyal dari Glade (termasuk menu-menu dari file_ops.c)
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    // Hubungkan tombol close (X) di pojok kanan atas
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Hubungkan fungsi hitung kata agar berjalan setiap kali teks berubah
   GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);
    g_signal_connect(buffer, "changed", G_CALLBACK(on_buffer_changed_array), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}