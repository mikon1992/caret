#include "file_ops.h"
#include "globals.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// var global
char *lokasi_file_sekarang = NULL;
char **text_editor = NULL;
int jumlah_baris = 0;
int kapasitas_baris = 100;

void inisialisasi_array_dinamis() {
    text_editor = malloc(kapasitas_baris * sizeof(char*));
    for(int i =0; i < kapasitas_baris; i++) {
        text_editor[i] = NULL;
    }
}

// Fungsi khusus untuk ngintip isi Array di Terminal
void print_array_ke_terminal() {
    g_print("\n=== DEBUG: ISI ARRAY SAAT INI ===\n");
    g_print("Total baris terisi: %d\n", jumlah_baris);
    
    // Looping untuk nge-print setiap indeks array
    for (int i = 0; i < jumlah_baris; i++) {
        g_print("Index [%d]: %s\n", i, text_editor[i]);
    }
    
    g_print("=================================\n\n");
}

void singkronisasi_layar_ke_array() {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text_utuh = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    for (int i = 0; i < jumlah_baris; i++) {
        if (text_editor[i] != NULL) {
            free(text_editor[i]);
            text_editor[i] = NULL;
        }
    }
    jumlah_baris = 0;

    
void on_buffer_changed_array(GtkTextBuffer *buffer, gpointer user_data) ;
{
    singkronisasi_layar_ke_array();
}

    char *baris_text = strtok(text_utuh, "\n");
    while (baris_text != NULL) {
        if (jumlah_baris >= kapasitas_baris) {
            kapasitas_baris *= 2;
            text_editor = realloc(text_editor, kapasitas_baris * sizeof(char));

            for (int i = jumlah_baris; i < kapasitas_baris; i++) {
                text_editor[i] = NULL;
            }
            g_print("nambah baris jadi %d\n", kapasitas_baris);
        }
        text_editor[jumlah_baris] = strdup(baris_text);

        jumlah_baris++;
        baris_text = strtok(NULL, "\n");
    }

    g_free(text_utuh);
    print_array_ke_terminal();
}

// fungsi pembantu
void tulis_ke_file(const char *filepath)
{
    singkronisasi_layar_ke_array();

    FILE *file = fopen(filepath, "w");
    if (file != NULL) {
        for (int i = 0; i < jumlah_baris; i++) {
            fputs(text_editor[i], file);
            if (i < jumlah_baris - 1) {
                fputs("\n", file);
            }
        }
        fclose(file);
        g_print("kesave!");
    } else {
        g_print("gagal save");
    }
}

G_MODULE_EXPORT void on_menu_save_activate(GtkMenuItem *menuitem, gpointer user_data) {
    if (lokasi_file_sekarang == NULL){
        GtkFileChooserNative *native;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
        gint res;

        native = gtk_file_chooser_native_new( "simpen file baru jir",
                                             GTK_WINDOW(window), action,
                                             "_save", "_Cancel");
    
        res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
        
        if (res == GTK_RESPONSE_ACCEPT) {
            GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
            lokasi_file_sekarang = gtk_file_chooser_get_filename(chooser);
            tulis_ke_file(lokasi_file_sekarang);
        }
        g_object_unref(native);
    } else {
        tulis_ke_file(lokasi_file_sekarang);
    }
}

G_MODULE_EXPORT void on_menu_new_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, "", -1);

    if (lokasi_file_sekarang != NULL){
        g_free(lokasi_file_sekarang);
        lokasi_file_sekarang = NULL;
    }
    g_print("new file!\n");
}

G_MODULE_EXPORT void on_menu_merge_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    native = gtk_file_chooser_native_new("pilih file untuk di merge",
                                         GTK_WINDOW(window), action,
                                         "_merge", "_cancel");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    
    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        char *target_filename = gtk_file_chooser_get_filename(chooser);

        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        FILE *file = fopen(target_filename, "a");
        if (file != NULL) {
            fputs("\n", file);
            fputs(text, file);
            fclose(file);
            g_print("teks merge complete");
        } else {
            g_print("merge failed\n");
        }

        g_free(text);
        g_free(target_filename);
    }
    g_object_unref(native);
}

G_MODULE_EXPORT void on_menu_open_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    native = gtk_file_chooser_native_new("buka file",
                                         GTK_WINDOW(window), action,
                                         "_buka", "_batal");
    
    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));

    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        char *filename = gtk_file_chooser_get_filename(chooser);

        FILE *file = fopen(filename, "r");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            long length = ftell(file);
            fseek(file, 0, SEEK_SET);

            char *buffer_text = malloc(length + 1);
            if (buffer_text) {
                fread(buffer_text, 1, length, file);
                buffer_text[length] = '\0';

                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                gtk_text_buffer_set_text(buffer, buffer_text, -1);
                
                if (lokasi_file_sekarang != NULL) g_free(lokasi_file_sekarang);
                lokasi_file_sekarang = g_strdup(filename);

                g_print("berhasil buka file %s\n", filename);
                free(buffer_text);
            }
            fclose(file);
        } else {
            g_print("gagal jir");
        }
        g_free(filename);
    }
    g_object_unref(native);
}
G_MODULE_EXPORT void on_menu_save_as_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    native = gtk_file_chooser_native_new("Simpan Sebagai...",
                                         GTK_WINDOW(window), action,
                                         "_Simpan", "_Batal");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    
    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (lokasi_file_sekarang != NULL) g_free(lokasi_file_sekarang);
        lokasi_file_sekarang = g_strdup(filename);

        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        FILE *file = fopen(lokasi_file_sekarang, "w");
        if (file != NULL) {
            fputs(text, file);
            fclose(file);
            g_print("File berhasil di-Save As ke: %s\n", lokasi_file_sekarang);
        }

        g_free(text);
        g_free(filename);
    }
    g_object_unref(native);
}

G_MODULE_EXPORT void on_menu_daily_notes_activate(GtkMenuItem *menuitem, gpointer user_data) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char filename[256];
    
    snprintf(filename, sizeof(filename), "%04d-%02d-%02d.md", 
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    FILE *file = fopen(filename, "r");
    
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *buffer_text = malloc(length + 1);
        if (buffer_text) {
            fread(buffer_text, 1, length, file);
            buffer_text[length] = '\0';
            gtk_text_buffer_set_text(buffer, buffer_text, -1);
            free(buffer_text);
        }
        fclose(file);
        g_print("Membuka Daily Note hari ini: %s\n", filename);
        
    } else {
        char template_teks[512];
        snprintf(template_teks, sizeof(template_teks), "# Jurnal Hari Ini: %04d-%02d-%02d\n\n- ", 
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
                 
        gtk_text_buffer_set_text(buffer, template_teks, -1);
        
        file = fopen(filename, "w");
        if (file != NULL) {
            fputs(template_teks, file);
            fclose(file);
        }
        g_print("Membuat Daily Note baru: %s\n", filename);
    }

    if (lokasi_file_sekarang != NULL) g_free(lokasi_file_sekarang);
    lokasi_file_sekarang = g_strdup(filename);
} 