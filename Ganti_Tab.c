#include <gtk/gtk.h>
#include <stdio.h>
#include "globals.h"
#include "wordnotemark.h"

// Ambil fungsi on_text_changed dari word_count.c
extern void on_text_changed(GtkTextBuffer *buffer, gpointer user_data);

// Fungsi khusus untuk menutup tab saat tombol X di DALAM tab diklik
G_MODULE_EXPORT void on_tab_close_clicked(GtkButton *btn, GtkWidget *child_widget) {
    // Cari index dari halaman tab yang tombol X-nya diklik
    gint page_num = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), child_widget);
    if (page_num >= 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), page_num);
    }
}

// Fungsi untuk menambah tab baru (dipanggil saat tombol + ditekan)
G_MODULE_EXPORT void tambah_tab(GtkButton *btn, gpointer data) {
    static int nomor = 2; // Mulai dari 2 karena Tab 1 sudah ada dari Glade

    // 1. Buat area teks editor baru
    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    
    // Matikan shadow agar terlihat flat dan bersih
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_NONE);
    gtk_container_add(GTK_CONTAINER(scroll), text_view);

    // Atur margin teks biar tidak menempel ke pinggir
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 6);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);

    // Hubungkan penghitung kata
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);

    // 2. Buat Tampilan Tab ala Browser (Teks + Tombol X)
    GtkWidget *tab_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // Teks nama tab
    char judul[20];
    sprintf(judul, "Tab %d", nomor++);
    GtkWidget *tab_label = gtk_label_new(judul);
    gtk_box_pack_start(GTK_BOX(tab_box), tab_label, TRUE, TRUE, 0);

    // Tombol X yang flat
    GtkWidget *close_btn = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);
    gtk_button_set_relief(GTK_BUTTON(close_btn), GTK_RELIEF_NONE);
    gtk_widget_set_focus_on_click(close_btn, FALSE); // Hilangkan garis putus-putus saat diklik

    gtk_box_pack_start(GTK_BOX(tab_box), close_btn, FALSE, FALSE, 0);
    gtk_widget_show_all(tab_box);

    // Hubungkan tombol X ini agar menutup tab-nya sendiri
    g_signal_connect(close_btn, "clicked", G_CALLBACK(on_tab_close_clicked), scroll);

    // 3. Masukkan ke dalam Notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, tab_box);
    gtk_widget_show_all(notebook);
    
    // Pindah fokus langsung ke tab yang baru dibuat
    gint total_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), total_pages - 1);

    // Fitur undo
    #if GTK_CHECK_VERSION(3, 24, 30)
    g_object_set(G_OBJECT(buffer), "enable-undo", TRUE, NULL);
    #endif
}

// Fungsi tutup tab bawaan (bisa diabaikan jika tidak ada shortcut keyboard yang memakainya)
G_MODULE_EXPORT void tutup_tab(GtkButton *btn, gpointer data) {
    int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (current_page >= 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), current_page);
    }
}