#include <gtk/gtk.h>

// Nambah tab baru
void tambah_tab(GtkButton *button, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);

    // Buat TextView baru
    GtkWidget *text_view = gtk_text_view_new();

    // Buat label tab
    static int nomor_tab = 2;
    char judul[20];
    sprintf(judul, "Tab %d", nomor_tab++);

    GtkWidget *label = gtk_label_new(judul);

    // Tambah ke notebook
    gtk_notebook_append_page(notebook, text_view, label);

    gtk_widget_show_all(GTK_WIDGET(notebook));
}

// Untuk pindah ke tab berikutnya
void next_tab(GtkButton *button, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);

    int current = gtk_notebook_get_current_page(notebook);
    int total = gtk_notebook_get_n_pages(notebook);

    if (current < total - 1) {
        gtk_notebook_set_current_page(notebook, current + 1);
    }
}

// Untuk pindah ke tab sebelumnya
void prev_tab(GtkButton *button, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);

    int current = gtk_notebook_get_current_page(notebook);

    if (current > 0) {
        gtk_notebook_set_current_page(notebook, current - 1);
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *notebook;
    GtkWidget *button_add, *button_next, *button_prev;
    GtkWidget *hbox;

    gtk_init(&argc, &argv);

    // Window utama
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tab GTK Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout utama (vertical)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Notebook (tab container)
    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // Tambah 1 tab awal
    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *label = gtk_label_new("Tab 1");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), text_view, label);

    // Box untuk tombol
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Tombol
    button_add = gtk_button_new_with_label("Tambah Tab");
    button_next = gtk_button_new_with_label("Next Tab");
    button_prev = gtk_button_new_with_label("Prev Tab");

    gtk_box_pack_start(GTK_BOX(hbox), button_add, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button_next, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button_prev, TRUE, TRUE, 0);

    // Hubungkan tombol ke fungsi
    g_signal_connect(button_add, "clicked", G_CALLBACK(tambah_tab), notebook);
    g_signal_connect(button_next, "clicked", G_CALLBACK(next_tab), notebook);
    g_signal_connect(button_prev, "clicked", G_CALLBACK(prev_tab), notebook);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}