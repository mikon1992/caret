#include <gtk/gtk.h>
#include <string.h>

// Global
GtkWidget *notebook;
GtkWidget *bookmark_list;

// Ambil text dari TextView
char* get_text_from_view(GtkWidget *text_view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

// Tambah Tab baru
void tambah_tab(GtkButton *button, gpointer data) {
    static int nomor = 1;

    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *scroll_text = gtk_scrolled_window_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER(scroll_text), text_view);

    char judul[20];
    sprintf(judul, "Tab %d", nomor++);
    GtkWidget *label = gtk_label_new(judul);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_text, label);
    gtk_widget_show_all(notebook);
}

// Tambah Bookmark
void tambah_bookmark(GtkButton *button, gpointer data) {
    int current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (current < 0) {
        g_print("Tidak ada tab!\n");
        return;
    }

    GtkWidget *scroll = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
    GtkWidget *text_view = gtk_bin_get_child(GTK_BIN(scroll));

    char *text = get_text_from_view(text_view);

    static int num_bookmark = 1;
    char judul_bm[30];
    sprintf(judul_bm, "Bookmark %d", num_bookmark++);

    GtkWidget *row = gtk_label_new(judul_bm);

    // Simpan isi text
    g_object_set_data_full(G_OBJECT(row), "full_text", text, g_free);

    gtk_list_box_insert(GTK_LIST_BOX(bookmark_list), row, -1);
    gtk_widget_show_all(bookmark_list);
}

// Klik Bookmark
void on_bookmark_selected(GtkListBox *box, GtkListBoxRow *row, gpointer data) {
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));
    const char *text = g_object_get_data(G_OBJECT(label), "full_text");

    if (!text) return;

    GtkWidget *text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, text, -1);

    GtkWidget *scroll_text = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll_text), text_view);

    GtkWidget *tab_label = gtk_label_new("Bookmark");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_text, tab_label);

    gtk_widget_show_all(notebook);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *hbox, *vbox;
    GtkWidget *btn_add_tab, *btn_bookmark;
    GtkWidget *scroll_bookmark;
    GtkWidget *button_box;

    gtk_init(&argc, &argv);

    // Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tab + Bookmark GTK");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout utama
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    // === 📚 SIDEBAR BOOKMARK (SCROLLABLE) ===
    scroll_bookmark = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll_bookmark, 200, -1);

    bookmark_list = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scroll_bookmark), bookmark_list);

    gtk_box_pack_start(GTK_BOX(hbox), scroll_bookmark, FALSE, FALSE, 0);

    g_signal_connect(bookmark_list, "row-activated",
                     G_CALLBACK(on_bookmark_selected), NULL);

    // === 📄 AREA KANAN ===
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

    // Notebook
    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // === 🔘 BUTTON AREA (RAPI) ===
    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    btn_add_tab = gtk_button_new_with_label("Tambah Tab");
    btn_bookmark = gtk_button_new_with_label("Tambah Bookmark");

    gtk_box_pack_start(GTK_BOX(button_box), btn_add_tab, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), btn_bookmark, TRUE, TRUE, 0);

    g_signal_connect(btn_add_tab, "clicked", G_CALLBACK(tambah_tab), NULL);
    g_signal_connect(btn_bookmark, "clicked", G_CALLBACK(tambah_bookmark), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}