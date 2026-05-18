#include "file_ops.h"
#include "globals.h"
#include "auto_save_rename.h" 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// var global
char *lokasi_file_sekarang = NULL;
char **text_editor = NULL;
LineNode *head_editor = NULL;
LineNode *tail_editor = NULL;
TabNode *head_tab = NULL;

void tambah_info_tab(int index, const char *path) {
    TabNode *current = head_tab;
    while (current != NULL) {
        if (current->tab_index == index) {
            g_free(current->filepath);
            current->filepath = g_strdup(path);
            return;
        }
        current = current->next;
    }
    TabNode *new_node = malloc(sizeof(TabNode));
    new_node->tab_index = index;
    new_node->filepath = g_strdup(path);
    new_node->next = head_tab;
    head_tab = new_node;
}

char* dapatkan_path_tab(int index) {
    TabNode *current = head_tab;
    while (current != NULL) {
        if (current->tab_index == index) {
            return current->filepath;
        }
        current = current->next;
    }
    return NULL;
}

void free_linked_list(){
    LineNode *current = head_editor;
    while (current != NULL) {
        LineNode *next_node = current->next;
        g_free(current->text);
        free(current);
        current = next_node;
    }
    head_editor = NULL;
    tail_editor = NULL;
}

void append_line(const char *text) {
    LineNode *new_node = malloc(sizeof(LineNode));
    new_node->text = g_strdup(text);
    new_node->next = NULL;
    new_node->prev = tail_editor;

    if (head_editor == NULL) {
        head_editor = new_node;
    } else {
        tail_editor->next = new_node;
    }
    tail_editor = new_node;
}

void sinkronisasi_layar_ke_list() {
    int current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (current < 0) return;
    GtkWidget *scroll = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
    GtkWidget *active_textview = gtk_bin_get_child(GTK_BIN(scroll));
    if (!GTK_IS_TEXT_VIEW(active_textview)) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(active_textview));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text_utuh = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    free_linked_list();

    char **baris_array = g_strsplit(text_utuh, "\n", -1);
    for (int i = 0; baris_array[i] != NULL; i++) {
        append_line(baris_array[i]);
    }
    g_strfreev(baris_array);
    g_free(text_utuh);
}

void update_nama_tab(const char *path) {
    if (path == NULL || notebook == NULL) return;
    int current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (current < 0) return;
    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
    char *nama_file = g_path_get_basename(path);
    GtkWidget *label = gtk_label_new(nama_file);
    gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), page, label);
    g_object_set_data_full(G_OBJECT(page), "file_path", g_strdup(path), g_free);
    tambah_info_tab(current, path);
    g_free(nama_file);
}

void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
    extern void trigger_idle_save(); 
    trigger_idle_save();
}

void tulis_ke_file(const char *filepath) {
    sinkronisasi_layar_ke_list();
    FILE *file = fopen(filepath, "w");
   if (file != NULL) {
        LineNode *current = head_editor;
        while (current != NULL) {
            fputs(current->text, file);
            if (current->next != NULL) fputs("\n", file);
            current = current->next;
        }
        fclose(file);
        update_nama_tab(filepath); 
        g_print("File tersimpan: %s\n", filepath);
    }
}

GtkWidget* get_active_textview() {
    if (!notebook) return NULL;
    int current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (current < 0) return NULL;
    GtkWidget *scroll = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
    GtkWidget *active_tv = gtk_bin_get_child(GTK_BIN(scroll));
    if (active_tv && GTK_IS_TEXT_VIEW(active_tv)) return active_tv;
    return NULL;
}

G_MODULE_EXPORT void on_menu_save_activate(GtkMenuItem *menuitem, gpointer user_data) {
    int current_tab = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    char *path_aktif = dapatkan_path_tab(current_tab);

    if (path_aktif == NULL){
        GtkFileChooserNative *native;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
        gint res;
        native = gtk_file_chooser_native_new("Simpan file baru",
                                             GTK_WINDOW(window), action,
                                             "_Save", "_Cancel");
    
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        
        GtkFileFilter *filter_txt = gtk_file_filter_new();
        gtk_file_filter_set_name(filter_txt, "Text Document (*.txt)");
        gtk_file_filter_add_pattern(filter_txt, "*.txt");
        gtk_file_chooser_add_filter(chooser, filter_txt);

        GtkFileFilter *filter_md = gtk_file_filter_new();
        gtk_file_filter_set_name(filter_md, "Markdown File (*.md)");
        gtk_file_filter_add_pattern(filter_md, "*.md");
        gtk_file_chooser_add_filter(chooser, filter_md);
        
        gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
        
        res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
        
        if (res == GTK_RESPONSE_ACCEPT) {
            char *filename = gtk_file_chooser_get_filename(chooser);
            tulis_ke_file(filename);
            g_free(filename);
        }
        g_object_unref(native);
    } else {
        tulis_ke_file(path_aktif);
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
            update_nama_tab(lokasi_file_sekarang);
            char *buffer_text = malloc(length + 1);
            if (buffer_text) {
                fread(buffer_text, 1, length, file);
                buffer_text[length] = '\0';
                GtkWidget *current_tv = get_active_textview();
                if (!current_tv) return;
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(current_tv));
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

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
    GtkFileFilter *filter_txt = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_txt, "Text Document (*.txt)");
    gtk_file_filter_add_pattern(filter_txt, "*.txt");
    gtk_file_chooser_add_filter(chooser, filter_txt);
    GtkFileFilter *filter_md = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_md, "Markdown File (*.md)");
    gtk_file_filter_add_pattern(filter_md, "*.md");
    gtk_file_chooser_add_filter(chooser, filter_md);
    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native)); 
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(chooser);
        
        if (lokasi_file_sekarang != NULL) g_free(lokasi_file_sekarang);
        lokasi_file_sekarang = g_strdup(filename);
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(get_active_textview()));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        
        FILE *file = fopen(lokasi_file_sekarang, "w");
        if (file != NULL) {
            fputs(text, file);
            fclose(file);
            update_nama_tab(lokasi_file_sekarang);
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

G_MODULE_EXPORT void on_btn_clicked(GtkButton *btn, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Tanya AI",
        GTK_WINDOW(window), 
        GTK_DIALOG_MODAL,
        "Tanya", GTK_RESPONSE_OK,
        "Batal", GTK_RESPONSE_CANCEL,
        NULL
    );
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Mau tanya apa ke AI?");
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry, FALSE, FALSE, 5);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *prompt = gtk_entry_get_text(GTK_ENTRY(entry));
        if (strlen(prompt) > 0) {
            char command[1024];
            snprintf(command, sizeof(command), "C:/Users/rafia/AppData/Local/Python/pythoncore-3.14-64/python.exe gemini_bridge.py \"%s\" 2>&1", prompt);
            FILE *fp = popen(command, "r");
            if (fp != NULL) {
                char buffer[256];
                GString *hasil_ai = g_string_new("");
                while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                    g_string_append(hasil_ai, buffer);
                }
                pclose(fp);
                GtkWidget *active_tv = get_active_textview(); 
                if (active_tv) {
                    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(active_tv));
                    GtkTextIter iter;
                    
                    gtk_text_buffer_get_iter_at_mark(buf, &iter, gtk_text_buffer_get_insert(buf));
                    
                    gtk_text_buffer_insert(buf, &iter, "\n\n🤖 --- Jawaban AI ---\n", -1);
                    gtk_text_buffer_insert(buf, &iter, hasil_ai->str, -1);
                    gtk_text_buffer_insert(buf, &iter, "\n----------------------\n", -1);
                }
                g_string_free(hasil_ai, TRUE);
            } else {
                g_print("Waduh, gagal ngejalanin script AI!\n");
            }
        }
    }
    gtk_widget_destroy(dialog);
}