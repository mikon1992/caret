#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtk/gtk.h>

// Ini ngasih tau file lain
extern GtkWidget *window;
extern GtkWidget *textview;
extern GtkWidget *notebook;
extern GtkWidget *bookmark_list;
extern GtkWidget *label_word_count;
extern char *lokasi_file_sekarang;
extern void autoSave();
extern GtkWidget *label_cursor_pos;
extern void tutup_tab(GtkButton *btn, gpointer data);

// array 2d
extern char **text_editor;
extern int jumlah_baris;
extern int kapasitas_baris;

#endif