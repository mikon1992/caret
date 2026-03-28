#include <gtk/gtk.h>
#include "globals.h"
#include "file_ops.h"

GtkWidget *window;
GtkWidget *textview;
GtkWidget *label_word_count;

// Callback: hitung kata setiap kali isi textview berubah
static void on_text_changed(GtkTextBuffer *buffer, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Hitung kata
    int count = 0;
    int in_word = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
            if (!in_word) { count++; in_word = 1; }
        } else {
            in_word = 0;
        }
    }
    g_free(text);

    // Update label status bar
    char hasil[64];
    snprintf(hasil, sizeof(hasil), "word count : %d", count);
    gtk_label_set_text(GTK_LABEL(label_word_count), hasil);
}

void setup_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    g_object_set(gtk_settings_get_default(),
                 "gtk-application-prefer-dark-theme", TRUE, NULL);

    // Muat file UI
    GtkBuilder *builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "GUI_code.glade", NULL) == 0) {
        g_printerr("Gagal memuat GUI_code.glade!\n");
        return;
    }

    // Ambil semua widget yang dibutuhkan
    window            = GTK_WIDGET(gtk_builder_get_object(builder, "main"));
    textview          = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
    label_word_count  = GTK_WIDGET(gtk_builder_get_object(builder, "word_count"));

    // Hubungkan sinyal menu (on_menu_save_activate, dsb.) dari file_ops.c
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    // Hubungkan sinyal tutup jendela
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Hubungkan word-count ke perubahan teks secara real-time
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}