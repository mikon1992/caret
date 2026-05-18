#include <gtk/gtk.h>
#include "globals.h"


GtkWidget *window;
GtkWidget *textview;
GtkWidget *label_word_count;
GtkWidget *notebook;
GtkWidget *bookmark_list;
GtkWidget *label_cursor_pos;
GtkWidget *listbox_riwayat;

extern gboolean autosave_cb(gpointer data);
extern void on_text_changed(GtkTextBuffer *buffer, gpointer user_data); 
extern void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data);
extern void on_cursor_moved(GtkTextBuffer *buffer, const GtkTextIter *location, GtkTextMark *mark, gpointer user_data);
extern void muat_bookmark_dari_file();

void setup_gui(int argc, char *argv[]) {
    GtkBuilder *builder;
    GError *error = NULL; 

    gtk_init(&argc, &argv);

    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "GUI_code.glade", &error) == 0) {
        g_print("Waduh, gagal muat Glade: %s\n", error->message);
        g_error_free(error);
        return;
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main"));
    if (window == NULL) {
        g_print("Error: Widget 'main' tidak ketemu di Glade!\n");
        return;
    }

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

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

    textview = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
    label_word_count = GTK_WIDGET(gtk_builder_get_object(builder, "word_count"));
    label_cursor_pos = GTK_WIDGET(gtk_builder_get_object(builder, "cursor_pos"));
    notebook      = GTK_WIDGET(gtk_builder_get_object(builder, "notebook"));
    bookmark_list = GTK_WIDGET(gtk_builder_get_object(builder, "bookmark_list"));
    listbox_riwayat = GTK_WIDGET(gtk_builder_get_object(builder, "listbox_riwayat"));
    if (notebook) {
        GtkWidget *add_tab_btn = gtk_button_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_MENU);
        gtk_button_set_relief(GTK_BUTTON(add_tab_btn), GTK_RELIEF_NONE);
        gtk_widget_set_focus_on_click(add_tab_btn, FALSE);
        gtk_widget_show(add_tab_btn);
        
        g_signal_connect(add_tab_btn, "clicked", G_CALLBACK(tambah_tab), NULL);
        
        gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook), add_tab_btn, GTK_PACK_END);
    }

    if (textview) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
        g_signal_connect(buffer, "changed", G_CALLBACK(on_text_changed), NULL);
        g_signal_connect(buffer, "changed", G_CALLBACK(on_buffer_changed), NULL);
        g_signal_connect(buffer, "mark-set", G_CALLBACK(on_cursor_moved), NULL);
        
        #if GTK_CHECK_VERSION(3, 24, 30)
        g_object_set(G_OBJECT(buffer), "enable-undo", TRUE, NULL);
        #endif
    }

    g_timeout_add_seconds(60, autosave_cb, NULL); 
    
    muat_bookmark_dari_file();

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();
}

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
