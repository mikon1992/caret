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
    GError *error = NULL; // Tambahkan variabel penyimpan error
    
    if (gtk_builder_add_from_file(builder, "GUI_code.glade", &error) == 0) {
        g_print("Waduh, gagal memuat file GUI_code.glade!\n");
        if (error != NULL) {
            // Print pesan error asli dari GTK
            g_print("Detail Error GTK: %s\n", error->message); 
            g_error_free(error);
        }
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
