#include <gtk/gtk.h>
#include "globals.h"


GtkWidget *window;
GtkWidget *textview;
GtkWidget *label_word_count;
GtkWidget *notebook;
GtkWidget *bookmark_list;
GtkWidget *label_cursor_pos;

extern void on_text_changed(GtkTextBuffer *buffer, gpointer user_data); 
extern void on_buffer_changed_array(GtkTextBuffer *buffer, gpointer user_data);
extern void inisialisasi_array_dinamis();
extern void on_cursor_moved(GtkTextBuffer *buffer, const GtkTextIter *location, GtkTextMark *mark, gpointer user_data);

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
    label_word_count = GTK_WIDGET(gtk_builder_get_object(builder, "word_count"));
    label_cursor_pos = GTK_WIDGET(gtk_builder_get_object(builder, "cursor_pos"));
    notebook      = GTK_WIDGET(gtk_builder_get_object(builder, "notebook"));
    bookmark_list = GTK_WIDGET(gtk_builder_get_object(builder, "bookmark_list"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    // Hubungkan tombol close (X) di pojok kanan atas
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Ambil buffer SATU KALI SAJA
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    
    // Hubungkan semua fungsi ke buffer tersebut secara berurutan
    g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);
    g_signal_connect(buffer, "changed", G_CALLBACK(on_buffer_changed_array), NULL); // <-- Pengecek array masuk di sini
    g_signal_connect(buffer, "mark-set", G_CALLBACK(on_cursor_moved), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}

void on_cursor_moved(GtkTextBuffer *buffer, const GtkTextIter *location, GtkTextMark *mark, gpointer user_data) {
    // Pastikan yang pindah adalah "insert" mark (kursor tempat kita ngetik)
    if (mark != gtk_text_buffer_get_insert(buffer)) return;

    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark);
    
    // Ambil baris dan kolom (index mulai dari 0, jadi kita +1)
    gint baris = gtk_text_iter_get_line(&iter) + 1;
    gint kolom = gtk_text_iter_get_line_offset(&iter) + 1;
    
    char tulisan[100];
    snprintf(tulisan, sizeof(tulisan), "Baris: %d, Kolom: %d  |  ", baris, kolom);
    
    gtk_label_set_text(GTK_LABEL(label_cursor_pos), tulisan);
}

void tambah_tab(GtkButton *button, gpointer user_data) {
    // Hitung jumlah tab yang sudah ada
    gint jumlah_tab = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    
    // Buat ScrolledWindow baru
    GtkWidget *scroll_baru = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_baru),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_show(scroll_baru);

    // Buat TextView baru
    GtkWidget *textview_baru = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_baru), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textview_baru), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textview_baru), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(textview_baru), 6);
    gtk_widget_show(textview_baru);

    gtk_container_add(GTK_CONTAINER(scroll_baru), textview_baru);

    // Hubungkan signal buffer ke TextView baru
    GtkTextBuffer *buffer_baru = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_baru));
    g_signal_connect(buffer_baru, "changed", G_CALLBACK(on_text_changed), NULL);
    g_signal_connect(buffer_baru, "changed", G_CALLBACK(on_buffer_changed_array), NULL);
    g_signal_connect(buffer_baru, "mark-set", G_CALLBACK(on_cursor_moved), NULL);

    // Buat label tab
    char nama_tab[20];
    snprintf(nama_tab, sizeof(nama_tab), "Tab %d", jumlah_tab + 1);
    GtkWidget *label_tab = gtk_label_new(nama_tab);
    gtk_widget_show(label_tab);

    // Tambahkan tab ke notebook
    gint index = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_baru, label_tab);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), index);
}