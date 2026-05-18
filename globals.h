#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtk/gtk.h>

typedef struct LineNode {
    char *text;
    struct LineNode *prev;
    struct LineNode *next;
} LineNode;

typedef struct TabNode {
    int tab_index;
    char *filepath;
    struct TabNode *next;
} TabNode;

extern GtkWidget *window;
extern GtkWidget *textview;
extern GtkWidget *notebook;
extern GtkWidget *bookmark_list;
extern GtkWidget *label_word_count;
extern GtkWidget *listbox_riwayat;
extern char *lokasi_file_sekarang;
extern void autoSave();
extern GtkWidget *label_cursor_pos;
extern void tutup_tab(GtkButton *btn, gpointer data);
extern void tambah_tab(GtkButton *btn, gpointer data);
extern TabNode *head_tab;
extern void free_linked_list();
extern void tambah_info_tab(int index, const char *path);
extern char* dapatkan_path_tab(int index);
extern LineNode *head_editor;
extern LineNode *tail_editor;
extern char **text_editor;
extern int jumlah_baris;
extern int kapasitas_baris;
extern void update_nama_tab(const char *path);

#endif
