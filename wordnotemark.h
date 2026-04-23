#ifndef WORDNOTEMARK_H
#define WORDNOTEMARK_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

char* get_text(GtkWidget *text_view);
void tambah_bookmark(GtkButton *btn, gpointer data);
void buka_bookmark(GtkListBox *box, GtkListBoxRow *row, gpointer data);
void muat_bookmark_dari_file(void);

void tambah_tab(GtkButton *btn, gpointer data);
void tutup_tab(GtkButton *btn, gpointer data);

int hitung_kata(const char *text);
void on_text_changed(GtkTextBuffer *buffer, gpointer user_data);

#endif