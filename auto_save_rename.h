#ifndef AUTO_SAVE_RENAME_H
#define AUTO_SAVE_RENAME_H

#include <gtk/gtk.h>

void autoSave();
void trigger_idle_save(); // <--- Tambahkan ini
G_MODULE_EXPORT void on_menu_new_named_activate(GtkMenuItem *menuitem, gpointer user_data);
G_MODULE_EXPORT void on_menu_rename_activate(GtkMenuItem *menuitem, gpointer user_data);

#endif