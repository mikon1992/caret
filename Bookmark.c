#include <gtk/gtk.h>
#include <stdio.h>
#include "globals.h"

// Ambil text dari TextView
char* get_text(GtkWidget *text_view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

// Tambah bookmark
void tambah_bookmark(GtkButton *btn, gpointer data) {
    int current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (current < 0) return;

    GtkWidget *scroll = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
    GtkWidget *text_view = gtk_bin_get_child(GTK_BIN(scroll));

    char *text = get_text(text_view);

    // Pastikan variabel 'no' ini tetap statis agar nilainya bertambah terus
    static int no = 1; 
    char judul[30];
    sprintf(judul, "Bookmark %d", no++); // Ini akan menghasilkan Bookmark 1, 2, dst

    // Buat label dengan nama unik tersebut
    GtkWidget *label = gtk_label_new(judul);
    
    // Simpan isi teksnya ke dalam objek label agar bisa diambil saat diklik
    g_object_set_data_full(G_OBJECT(label), "text", text, g_free);

    // Masukkan ke dalam list box
    gtk_list_box_insert(GTK_LIST_BOX(bookmark_list), label, -1);
    gtk_widget_show_all(bookmark_list);
}

// Klik bookmark
void buka_bookmark(GtkListBox *box, GtkListBoxRow *row, gpointer data) {
    GtkWidget *label_dlm_list = gtk_bin_get_child(GTK_BIN(row));
    const char *text = g_object_get_data(G_OBJECT(label_dlm_list), "text");
    
    const char *judul_bookmark = gtk_label_get_text(GTK_LABEL(label_dlm_list));

    if (!text) return;

    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), text_view);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, text, -1);

    // Gunakan judul_bookmark di sini, bukan teks statis
    GtkWidget *tab_label = gtk_label_new(judul_bookmark); 
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, tab_label);

    gtk_widget_show_all(notebook);
}