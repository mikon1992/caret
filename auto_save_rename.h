#ifndef AUTO_SAVE_RENAME_H
#define AUTO_SAVE_RENAME_H

#include <gtk/gtk.h>

// Otomatis simpan file 
void autoSave();

// Buat file baru dengan nama yang diinput user
G_MODULE_EXPORT void on_menu_new_named_activate(GtkMenuItem *menuitem, gpointer user_data);

// Rename file 
G_MODULE_EXPORT void on_menu_rename_activate(GtkMenuItem *menuitem, gpointer user_data);

#endif
