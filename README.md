# caret
 our text editor project in our college task
<<<<<<< HEAD
 # Instalasi mingw dan msys2
 install bahasa C pada Visual Studio Code
 Install terlebih dahulu MinGW pada desktop 
 pilih mingw32-base dan mingw32-gcc-g++
 ini yang akan menginstal compiler untuk bahasa C
# how to run
untuk run projek kami kalian memerlukan beberapa hal ini:
1. mingw
2. msys2
3. GTK-3
4. zip project kami
setelah kamu mendownload mingw seperti panduan di atas,

1. buka aplikasi msys2 minGW
2. install compiler C dan GTK-3 dengan code ini
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkgconf
3. extract file zip ke folder
4. di msys2 ketik cd dan drag folder yang sudah di extract ke dalam layar hitam msys2
5. jika tidak ada error jalankan program ini
gcc main.c GUI.c -o text_editor.exe `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
>>>>>>> 87c87555a546c3b0c50a617f49abac9f27f7dd9b
