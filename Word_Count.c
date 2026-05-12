#include <gtk/gtk.h>
#include <stdio.h>
#include "globals.h"
#include "file_ops.h"

int hitung_kata(const char *text) {
    int count = 0, in_word = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }
    return count;
}

// Callback
void on_text_changed(GtkTextBuffer *buffer, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);

    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    int jumlah = hitung_kata(text);

    char hasil[64];
    snprintf(hasil, sizeof(hasil), "Word Count: %d", jumlah);
    gtk_label_set_text(GTK_LABEL(label_word_count), hasil);


    g_free(text);
}
