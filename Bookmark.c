#include <gtk/gtk.h>
#include <stdio.h>
#include "globals.h"
#include "file_ops.h" 
#include "wordnotemark.h"

char* get_text(GtkWidget *text_view) {
    if (!text_view || !GTK_IS_TEXT_VIEW(text_view)) {
        g_print("Waduh: Gagal ambil teks, widget bukan TextView!\n");
        return g_strdup(""); 
    }
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

void tambah_bookmark(GtkButton *btn, gpointer data) {
    GtkWidget *text_view = get_active_textview();
    if (!text_view) return;

    char *text = get_text(text_view);

    static int no = 1; 
    char judul[30];
    sprintf(judul, "Bookmark %d", no++); 

    FILE *f = fopen("bookmarks.txt", "a"); 
    if (f) {
        fprintf(f, "[[%s]]\n%s\n[END_BK]\n", judul, text);
        fclose(f);
    } else {
        g_print("Waduh, gagal buka file bookmarks.txt buat nulis!\n");
    }
    GtkWidget *label = gtk_label_new(judul);
    g_object_set_data_full(G_OBJECT(label), "text", text, g_free);
    if (bookmark_list) {
        gtk_list_box_insert(GTK_LIST_BOX(bookmark_list), label, -1);
        gtk_widget_show_all(bookmark_list);
    }
    
    g_print("Bookmark '%s' berhasil disimpan ke file dan daftar!\n", judul);
}

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

    GtkWidget *tab_label = gtk_label_new(judul_bookmark); 
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, tab_label);

    gtk_widget_show_all(notebook);
}
void muat_bookmark_dari_file() {
    FILE *f = fopen("bookmarks.txt", "r");
    if (!f) {
        g_print("Info: Belum ada file bookmarks.txt, dilewati saja.\n");
        return;
    }

    char line[1024];
    char judul[100];
    GString *isi = g_string_new("");
    gboolean sedang_baca_isi = FALSE;
    
    while (fgets(line, sizeof(line), f)) {
        // Cari penanda judul [[Judul]]
        if (strncmp(line, "[[", 2) == 0) {
            sscanf(line, "[[%[^]]", judul);
            g_string_assign(isi, "");
            sedang_baca_isi = TRUE;
        } 
        // Cari penanda akhir bookmark
        else if (strncmp(line, "[END_BK]", 8) == 0) {
            GtkWidget *label = gtk_label_new(judul);
            // Simpan teks ke dalam label
            g_object_set_data_full(G_OBJECT(label), "text", g_strdup(isi->str), g_free);
            
            // Masukkan ke ListBox di GUI
            if (bookmark_list) {
                gtk_list_box_insert(GTK_LIST_BOX(bookmark_list), label, -1);
            }
            sedang_baca_isi = FALSE;
        } 
        // Jika sedang di tengah-tengah isi teks, tambahkan ke string
        else if (sedang_baca_isi) {
            g_string_append(isi, line);
        }
    }

    g_string_free(isi, TRUE);
    fclose(f);
    
    if (bookmark_list) {
        gtk_widget_show_all(bookmark_list);
    }
    g_print("Bookmark lama berhasil dimuat dari file.\n");
}