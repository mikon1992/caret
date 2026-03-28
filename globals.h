#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtk/gtk.h>

// Ini ngasih tau file lain
extern GtkWidget *window;
extern GtkWidget *textview;
extern GtkWidget *label_word_count;
extern char *lokasi_file_sekarang;

// array 2d
extern char **text_editor;
extern int jumlah_baris;
extern int kapasitas_baris;

#endif