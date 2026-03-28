#include <gtk/gtk.h>

extern void setup_gui(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    setup_gui(argc, argv);

    return 0;
}