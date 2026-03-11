# caret
 Our text editor project in our college task
<<<<<<< HEAD

 # Instalasi mingw dan msys2
 1. Install bahasa C pada Visual Studio Code
 2. Install terlebih dahulu MinGW pada desktop 
 3. Pilih mingw32-base dan mingw32-gcc-g++
 4. Ini yang akan menginstal compiler untuk bahasa C

# Aplikasi yang diperlukan
Untuk run projek kami kalian memerlukan beberapa hal ini:
1. minGW
2. msys2
3. GTK-3
4. Zip project kami
Setelah kamu mendownload minGW seperti panduan di atas


# Cara 
1. Buka aplikasi msys2 minGW
2. Install compiler C dan GTK-3 dengan code ini
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkgconf
```
3. Extract file zip ke folder
4. Di msys2 ketik cd dan drag folder yang sudah di extract ke dalam layar hitam msys2
5. Jika tidak ada error jalankan program ini
```bash
gcc main.c GUI.c -o text_editor.exe `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
```


# Identitas
Nama | Nim | Id Git Hub
- Adrian Madani Radinka (032) | T-RexXeon
- Muhammad Sidiq Mufti Ksatriatama (055) | CeriuxD
- Rafi Akmal Maulana (058) | mikon1992

# Manager
- Pa Yudi Widhiyasana, S.Si., M.T.
- NIP. 197407182001121002