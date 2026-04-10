#include <gtk/gtk.h>
#include <stdio.h>
#include "globals.h"

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
}

// Fungsi baru untuk menutup tab yang sedang aktif
void tutup_tab(GtkButton *btn, gpointer data) {
    int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    
    // Pastikan ada tab yang bisa ditutup (jangan sampai menutup tab terakhir jika tidak diinginkan)
    if (current_page >= 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), current_page);
    }
}