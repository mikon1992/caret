#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <gtk/gtk.h>

// Deklarasi fungsi
G_MODULE_EXPORT void on_menu_save_activate(GtkMenuItem *menuitem, gpointer user_data);
G_MODULE_EXPORT void on_menu_new_activate(GtkMenuItem *menuitem, gpointer user_data);
G_MODULE_EXPORT void on_menu_merge_activate(GtkMenuItem *menuitem, gpointer user_data);
G_MODULE_EXPORT void on_menu_open_activate(GtkMenuItem *menuitem, gpointer user_data);
G_MODULE_EXPORT void on_menu_save_as_activate(GtkMenuItem *menuitem, gpointer user_data);
G_MODULE_EXPORT void on_menu_daily_notes_activate(GtkMenuItem *menuitem, gpointer user_data);

#endif