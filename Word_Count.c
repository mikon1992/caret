#include <gtk/gtk.h>
#include <stdio.h>
#include "globals.h"

// Callback: hitung kata setiap kali isi textview berubah
G_MODULE_EXPORT void on_text_changed(GtkTextBuffer *buffer, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Hitung kata
    int count = 0;
    int in_word = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
            if (!in_word) { count++; in_word = 1; }
        } else {
            in_word = 0;
        }
    }
    g_free(text);

    // Update label status bar
    char hasil[64];
    snprintf(hasil, sizeof(hasil), "word count : %d", count);
    gtk_label_set_text(GTK_LABEL(label_word_count), hasil);
}