#include <gtk/gtk.h>
#include "globals.h"

// Variabel Global (Hanya deklarasi, jangan double)
GtkWidget *window;
GtkWidget *textview;
GtkWidget *label_word_count;
GtkWidget *notebook;
GtkWidget *bookmark_list;
GtkWidget *label_cursor_pos;

// Fungsi-fungsi dari file lain
extern gboolean autosave_cb(gpointer data);
extern void on_text_changed(GtkTextBuffer *buffer, gpointer user_data); 
extern void on_buffer_changed_array(GtkTextBuffer *buffer, gpointer user_data);
extern void inisialisasi_array_dinamis();
extern void on_cursor_moved(GtkTextBuffer *buffer, const GtkTextIter *location, GtkTextMark *mark, gpointer user_data);
extern void muat_bookmark_dari_file();

void setup_gui(int argc, char *argv[]) {
    GtkBuilder *builder;
    GError *error = NULL; // KUNCI: Harus ada deklarasi error agar tidak merah

    gtk_init(&argc, &argv);

    // Tema Dark
    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "GUI_code.glade", &error) == 0) {
        g_print("Waduh, gagal muat Glade: %s\n", error->message);
        g_error_free(error);
        return;
    }

    // --- 1. AMBIL WIDGET WINDOW DULU ---
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main"));
    if (window == NULL) {
        g_print("Error: Widget 'main' tidak ketemu di Glade!\n");
        return;
    }

    // Hubungkan tombol X di pojok kanan
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // --- 2. PASANG SHORTCUT (CTRL+S, CTRL+O, DLL) ---
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    struct { const char *id; guint key; GdkModifierType mod; } shortcuts[] = {
        {"menu_new",  GDK_KEY_n, GDK_CONTROL_MASK},
        {"menu_open", GDK_KEY_o, GDK_CONTROL_MASK},
        {"menu_save", GDK_KEY_s, GDK_CONTROL_MASK},
        {"menu_save-as", GDK_KEY_s, GDK_CONTROL_MASK | GDK_SHIFT_MASK}
    };

    for (int i = 0; i < 4; i++) {
        GtkWidget *item = GTK_WIDGET(gtk_builder_get_object(builder, shortcuts[i].id));
        if (item) {
            gtk_widget_add_accelerator(item, "activate", accel_group, 
                                     shortcuts[i].key, shortcuts[i].mod, GTK_ACCEL_VISIBLE);
        }
    }

    // --- 3. AMBIL WIDGET LAINNYA ---
    textview = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
    label_word_count = GTK_WIDGET(gtk_builder_get_object(builder, "word_count"));
    label_cursor_pos = GTK_WIDGET(gtk_builder_get_object(builder, "cursor_pos"));
    notebook      = GTK_WIDGET(gtk_builder_get_object(builder, "notebook"));
    bookmark_list = GTK_WIDGET(gtk_builder_get_object(builder, "bookmark_list"));

    // --- 4. HUBUNGKAN SIGNAL BUFFER (TAB PERTAMA) ---
    if (textview) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
        g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);
        g_signal_connect(buffer, "changed", G_CALLBACK(on_buffer_changed_array), NULL);
        g_signal_connect(buffer, "mark-set", G_CALLBACK(on_cursor_moved), NULL);
        
        // Aktifkan Undo di tab pertama
        g_object_set(G_OBJECT(buffer), "enable-undo", TRUE, NULL);
    }

    // --- 5. TIMER & PERMANEN DATA ---
    // Autosave tiap 60 detik (1 menit)
    g_timeout_add_seconds(60, autosave_cb, NULL); 
    
    // Muat bookmark lama
    muat_bookmark_dari_file();

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();
}

// Fungsi kursor (Tetap sama)
void on_cursor_moved(GtkTextBuffer *buffer, const GtkTextIter *location, GtkTextMark *mark, gpointer user_data) {
    if (mark != gtk_text_buffer_get_insert(buffer)) return;

    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark);
    
    gint baris = gtk_text_iter_get_line(&iter) + 1;
    gint kolom = gtk_text_iter_get_line_offset(&iter) + 1;
    
    char tulisan[100];
    snprintf(tulisan, sizeof(tulisan), "Baris: %d, Kolom: %d  |  ", baris, kolom);
    
    if (label_cursor_pos) {
        gtk_label_set_text(GTK_LABEL(label_cursor_pos), tulisan);
    }
}