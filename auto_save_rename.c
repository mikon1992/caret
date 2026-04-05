#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "file_ops.h"


// AUTO SAVE

void autoSave() {
    if (lokasi_file_sekarang == NULL) return; 

    singkronisasi_layar_ke_array(); 

    FILE *file = fopen(lokasi_file_sekarang, "w");
    if (file != NULL) {
        for (int i = 0; i < jumlah_baris; i++) {
            fputs(text_editor[i], file);
            if (i < jumlah_baris - 1) fputs("\n", file);
        }
        fclose(file);
        g_print("auto save ke: %s\n", lokasi_file_sekarang);
    }
}


// NEW FILE DENGAN NAMA

G_MODULE_EXPORT void on_menu_new_named_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Buat File Baru",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        "Buat", GTK_RESPONSE_OK,
        "Batal", GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Nama file (contoh: catatan.txt)");
    gtk_box_pack_start(
        GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
        gtk_label_new("Nama file baru:"),
        FALSE, FALSE, 5
    );
    gtk_box_pack_start(
        GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
        entry,
        FALSE, FALSE, 5
    );
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *nama = gtk_entry_get_text(GTK_ENTRY(entry));
        if (strlen(nama) == 0) nama = "untitled.txt";

        
        GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
        gtk_text_buffer_set_text(buf, "", -1);

        
        if (lokasi_file_sekarang != NULL) g_free(lokasi_file_sekarang);
        lokasi_file_sekarang = g_strdup(nama);

        
        FILE *f = fopen(lokasi_file_sekarang, "w");
        if (f != NULL) fclose(f);

        g_print("new file dibuat: %s\n", lokasi_file_sekarang);
    }
    gtk_widget_destroy(dialog);
}


// RENAME FILE

G_MODULE_EXPORT void on_menu_rename_activate(GtkMenuItem *menuitem, gpointer user_data) {
    if (lokasi_file_sekarang == NULL) {
        GtkWidget *info = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Belum ada file yang dibuka!"
        );
        gtk_dialog_run(GTK_DIALOG(info));
        gtk_widget_destroy(info);
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Rename File",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        "Simpan", GTK_RESPONSE_OK,
        "Batal", GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), lokasi_file_sekarang); // prefill nama lama
    gtk_box_pack_start(
        GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
        gtk_label_new("Nama file baru:"),
        FALSE, FALSE, 5
    );
    gtk_box_pack_start(
        GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
        entry,
        FALSE, FALSE, 5
    );
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *nama_baru = gtk_entry_get_text(GTK_ENTRY(entry));
        if (strlen(nama_baru) > 0) {
            if (rename(lokasi_file_sekarang, nama_baru) == 0) {
                g_free(lokasi_file_sekarang);
                lokasi_file_sekarang = g_strdup(nama_baru);
                g_print("file direname jadi: %s\n", lokasi_file_sekarang);
            } else {
                g_print("rename gagal!\n");
            }
        }
    }
    gtk_widget_destroy(dialog);
}
