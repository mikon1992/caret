#include <gtk/gtk.h>

extern void setup_gui(int argc, char *argv[]);
extern void inisialisasi_array_dinamis();

int main(int argc, char *argv[]) {
    inisialisasi_array_dinamis();
    setup_gui(argc, argv);
    return 0;
}