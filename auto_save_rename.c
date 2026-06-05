#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "file_ops.h"
#include "auto_save_rename.h"
#include <time.h>

static time_t waktu_terakhir_ketik = 0;
static guint idle_checker_id = 0;
gboolean autosave_aktif = FALSE;

// RIWAYAT RENAME

static RiwayatRename *head_riwayat = NULL;

void tambah_riwayat(const char *nama_lama, const char *nama_baru) {
    RiwayatRename *node_baru = malloc(sizeof(RiwayatRename));
    node_baru->nama_lama = strdup(nama_lama);
    node_baru->nama_baru = strdup(nama_baru);
    node_baru->next = NULL;

    if (head_riwayat == NULL) {
        head_riwayat = node_baru;
    } else {
        RiwayatRename *sementara = head_riwayat;
        while (sementara->next != NULL) {
            sementara = sementara->next;
        }
        sementara->next = node_baru;
    }
    if (listbox_riwayat) {
        char teks_label[512];
        snprintf(teks_label, sizeof(teks_label), "%s  ->  %s", nama_lama, nama_baru);

        GtkWidget *label = gtk_label_new(teks_label);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_widget_set_margin_start(label, 6);
        gtk_widget_set_margin_end(label, 6);
        gtk_widget_set_margin_top(label, 3);
        gtk_widget_set_margin_bottom(label, 3);
        gtk_list_box_insert(GTK_LIST_BOX(listbox_riwayat), label, -1);
        gtk_widget_show_all(listbox_riwayat);
    }
}

void print_riwayat() {
    g_print("\n=== RIWAYAT RENAME ===\n");
    RiwayatRename *sementara = head_riwayat;
    int nomor = 1;
    while (sementara != NULL) {
        g_print("%d. %s  ->  %s\n", nomor, sementara->nama_lama, sementara->nama_baru);
        nomor++;
        sementara = sementara->next;
    }
    if (nomor == 1) g_print("(belum ada riwayat)\n");
    g_print("======================\n\n");
}

void hapus_semua_riwayat() {
    RiwayatRename *sementara = head_riwayat;
    while (sementara != NULL) {
        RiwayatRename *berikutnya = sementara->next;
        free(sementara->nama_lama);
        free(sementara->nama_baru);
        free(sementara);
        sementara = berikutnya;
    }
    head_riwayat = NULL;
    g_print("Semua riwayat rename dihapus.\n");
}

// AUTO SAVE 

void autoSave() {
    if (!autosave_aktif) return;
    int current_tab = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (current_tab < 0) return;
    char *path_aktif = dapatkan_path_tab(current_tab);
    if (path_aktif == NULL) return;
    extern void tulis_ke_file(const char *filepath);
    tulis_ke_file(path_aktif);
}

//Toggle AutoSave
G_MODULE_EXPORT gboolean on_autosave_toggled(GtkSwitch *widget, gboolean state, gpointer user_data) {
    autosave_aktif = state;
  
    if (Autosave_label) {
        if (autosave_aktif) {
            gtk_label_set_text(GTK_LABEL(Autosave_label), "Autosave: ON");
        } else {
            gtk_label_set_text(GTK_LABEL(Autosave_label), "Autosave: OFF");
        }
    }
    //TAMBAHAN
    if (!autosave_aktif && idle_checker_id > 0) {
        g_source_remove(idle_checker_id);
        idle_checker_id = 0;
        waktu_terakhir_ketik = 0; // Reset waktu
        g_print("Autosave OFF: Timer background dimatikan.\n");
    }
    g_print("Autosave: %s\n", autosave_aktif ? "ON" : "OFF");
    return FALSE;
}

// NEW FILE 

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
    // Ambil path dari tab yang sedang aktif!
    int current_tab = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    char *path_aktif = dapatkan_path_tab(current_tab);
    if (path_aktif == NULL) {
        GtkWidget *info = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Belum ada file yang dibuka atau file belum pernah disave!"
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
    gtk_entry_set_text(GTK_ENTRY(entry), path_aktif);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), gtk_label_new("Nama file baru:"), FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry, FALSE, FALSE, 5);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *nama_baru = gtk_entry_get_text(GTK_ENTRY(entry));
        if (strlen(nama_baru) > 0) {
            if (rename(path_aktif, nama_baru) == 0) {
                tambah_riwayat(path_aktif, nama_baru);
                g_print("Nama lama '%s' disimpan ke riwayat.\n", path_aktif);
                update_nama_tab(nama_baru);
                g_print("File direname jadi: %s\n", nama_baru);
                print_riwayat();
            } else {
                g_print("Rename gagal!\n");
            }
        }
    }
    gtk_widget_destroy(dialog);
}

// TIMER AUTOSAVE 

gboolean autosave_cb(gpointer data) {
    autoSave();
    return TRUE;
}

gboolean idle_save_cb(gpointer data) {
if (waktu_terakhir_ketik > 0 && difftime(time(NULL), waktu_terakhir_ketik) >= 10) {
        g_print("Idle 10 detik terdeteksi... \n");
        autoSave();
        waktu_terakhir_ketik = 0; // Reset biar nggak autoSave terus-terusan
    }
    return TRUE; // Biarkan timer pengecekan ini hidup terus
}

void trigger_idle_save() {
    if (!autosave_aktif) return;
    waktu_terakhir_ketik = time(NULL); 
    if (idle_checker_id == 0) {
        idle_checker_id = g_timeout_add_seconds(1, idle_save_cb, NULL);
    }
}