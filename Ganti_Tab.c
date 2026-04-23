#include <gtk/gtk.h>
#include <stdio.h>
#include "globals.h"
#include "wordnotemark.h"

// dari word_count.c
extern void on_text_changed(GtkTextBuffer *buffer, gpointer user_data);

void tambah_tab(GtkButton *btn, gpointer data) {
    static int nomor = 1;

    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER(scroll), text_view);

    // hubungkan word count
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);

    char judul[20];
    sprintf(judul, "Tab %d", nomor++);
    GtkWidget *label = gtk_label_new(judul);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, label);
    gtk_widget_show_all(notebook);
    
    // Coba aktifkan fitur undo bawaan (Hanya jalan di versi GTK 3 terbaru)
    #if GTK_CHECK_VERSION(3, 24, 30)
    g_object_set(G_OBJECT(buffer), "enable-undo", TRUE, NULL);
    #endif
}

// Fungsi baru untuk menutup tab yang sedang aktif
void tutup_tab(GtkButton *btn, gpointer data) {
    // Ambil indeks halaman yang sedang aktif/terbuka
    int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    
    // Jika ada tab yang terbuka (indeks bukan -1), maka hapus
    if (current_page >= 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), current_page);
    }
}