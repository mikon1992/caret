#ifndef AUTO_SAVE_RENAME_H
#define AUTO_SAVE_RENAME_H

#include <gtk/gtk.h>

typedef struct RiwayatRename {
    char *nama_lama;
    char *nama_baru;
    struct RiwayatRename *next;
} RiwayatRename;

void tambah_riwayat(const char *nama_lama,const char *nama_baru);
void print_riwayat();
void hapus_semua_riwayat();

void autoSave();
void trigger_idle_save(); 
G_MODULE_EXPORT void on_menu_new_named_activate(GtkMenuItem *menuitem, gpointer user_data);
G_MODULE_EXPORT void on_menu_rename_activate(GtkMenuItem *menuitem, gpointer user_data);

#endif
