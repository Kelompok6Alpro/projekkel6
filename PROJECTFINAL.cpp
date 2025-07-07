#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string.h> 

using namespace std;

const int BatasPengguna = 50;
const int BatasBuku = 50;
const int BatasJudul = 100;
const int BatasHal = 100;
const int PanjangHal = 2000;
const int BatasUlasan = 500;
const int BatasBookmark = 500;

struct Pengguna {
    char nama[50];
    char id[20];        
    char role[10];      
    char tipe[15];      
    bool banned;        
};

struct Buku {
    char judul[BatasJudul];
    int jumlahHalaman;
    char halaman[BatasHal][PanjangHal];
};

struct Ulasan {
    char username[50];
    char judulBuku[BatasJudul];
    int rating;
    char komentar[500];
    char tanggal[20];
};

struct Bookmark {
    char username[50];
    char judulBuku[BatasJudul];
    int halamanTerakhir;
};

Pengguna pengguna[BatasPengguna];
int jumlahPengguna = 0;

Buku daftarBuku[BatasBuku];
int jumlahBuku = 0;

Ulasan daftarUlasan[BatasUlasan];
int jumlahUlasan = 0;

Bookmark daftarBookmark[BatasBookmark];
int jumlahBookmark = 0;

int indexpengguna = -1;

void dapatkanTanggalSekarang(char* buffer) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    sprintf(buffer, "%02d/%02d/%04d", ltm->tm_mday, ltm->tm_mon + 1, 1900 + ltm->tm_year);
}

// --- FUNGSI SORTING ---
void selectionSortBukuByJudul() { // selection short
    for (int i = 0; i < jumlahBuku - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < jumlahBuku; j++) {
            if (strcasecmp(daftarBuku[j].judul, daftarBuku[min_idx].judul) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Buku temp = daftarBuku[i];      
            daftarBuku[i] = daftarBuku[min_idx];
            daftarBuku[min_idx] = temp;    
        }
    }
}

// --- FUNGSI PENCARIAN (LINEAR SEARCH & BINARY SEARCH) ---

// Fungsi Binary Search untuk mencari indeks buku berdasarkan judul
int binarySearchBukuByJudul(const char* judulCari) { // Binary Short = wahyu
    int low = 0;
    int high = jumlahBuku - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strcasecmp(daftarBuku[mid].judul, judulCari);

        if (cmp == 0) {
            return mid;
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

void bacaDataPengguna() {
    ifstream file("pengguna.txt");
    if (!file) return;
    
    jumlahPengguna = 0;
    char buffer[100];
    
    while (jumlahPengguna < BatasPengguna && file.getline(pengguna[jumlahPengguna].nama, 50)) {
        file.getline(pengguna[jumlahPengguna].id, 20);
        file.getline(pengguna[jumlahPengguna].role, 10);
        
        strcpy(pengguna[jumlahPengguna].tipe, "mahasiswa");
        pengguna[jumlahPengguna].banned = false;
        
        if (file.getline(buffer, 100)) {
            strcpy(pengguna[jumlahPengguna].tipe, buffer);
            if (file.getline(buffer, 100)) {
                pengguna[jumlahPengguna].banned = (strcmp(buffer, "1") == 0);
            }
        }
        jumlahPengguna++;
    }
    file.close();
}

void bacaDataBuku() {
    ifstream file("buku.txt");
    if (!file) return;

    jumlahBuku = 0;
    char buffer[PanjangHal];

    while (jumlahBuku < BatasBuku && file.getline(daftarBuku[jumlahBuku].judul, BatasJudul)) {
        file >> daftarBuku[jumlahBuku].jumlahHalaman;
        file.ignore();

        for (int i = 0; i < daftarBuku[jumlahBuku].jumlahHalaman; i++) {
            daftarBuku[jumlahBuku].halaman[i][0] = '\0';
            
            while (true) {
                file.getline(buffer, PanjangHal);
                if (file.eof() || strcmp(buffer, "::END::") == 0) break;
                
                if (daftarBuku[jumlahBuku].halaman[i][0] != '\0') {
                    strcat(daftarBuku[jumlahBuku].halaman[i], "\n");
                }
                strcat(daftarBuku[jumlahBuku].halaman[i], buffer);
            }
        }
        jumlahBuku++;
    }
    file.close();
}

void bacaDataUlasan() {
    ifstream file("ulasan.txt");
    if (!file) return;

    jumlahUlasan = 0;
    while (jumlahUlasan < BatasUlasan && file.getline(daftarUlasan[jumlahUlasan].username, 50)) {
        file.getline(daftarUlasan[jumlahUlasan].judulBuku, BatasJudul);
        file >> daftarUlasan[jumlahUlasan].rating;
        file.ignore();
        file.getline(daftarUlasan[jumlahUlasan].komentar, 500);
        file.getline(daftarUlasan[jumlahUlasan].tanggal, 20);
        jumlahUlasan++;
    }
    file.close();
}

void bacaDataBookmark() {
    ifstream file("bookmark.txt");
    if (!file) return;

    jumlahBookmark = 0;
    while (jumlahBookmark < BatasBookmark && file.getline(daftarBookmark[jumlahBookmark].username, 50)) {
        file.getline(daftarBookmark[jumlahBookmark].judulBuku, BatasJudul);
        file >> daftarBookmark[jumlahBookmark].halamanTerakhir;
        file.ignore();
        jumlahBookmark++;
    }
    file.close();
}

void simpanDataPengguna() {
    ofstream file("pengguna.txt");
    for (int i = 0; i < jumlahPengguna; i++) {
        file << pengguna[i].nama << '\n' << pengguna[i].id << '\n' << pengguna[i].role << '\n' << pengguna[i].tipe << '\n' << (pengguna[i].banned ? "1" : "0") << '\n';
    }
    file.close();
}

void simpanDataBuku() {
    ofstream file("buku.txt");
    for (int i = 0; i < jumlahBuku; i++) {
        file << daftarBuku[i].judul << '\n'
             << daftarBuku[i].jumlahHalaman << '\n';
        
        for (int j = 0; j < daftarBuku[i].jumlahHalaman; j++) {
            file << daftarBuku[i].halaman[j] << '\n';
            file << "::END::\n";
        }
    }
    file.close();
}

void simpanDataUlasan() {
    ofstream file("ulasan.txt");
    for (int i = 0; i < jumlahUlasan; i++) {
        file << daftarUlasan[i].username << '\n'
             << daftarUlasan[i].judulBuku << '\n'
             << daftarUlasan[i].rating << '\n'
             << daftarUlasan[i].komentar << '\n'
             << daftarUlasan[i].tanggal << '\n';
    }
    file.close();
}

void simpanDataBookmark() {
    ofstream file("bookmark.txt");
    for (int i = 0; i < jumlahBookmark; i++) {
        file << daftarBookmark[i].username << '\n'
             << daftarBookmark[i].judulBuku << '\n'
             << daftarBookmark[i].halamanTerakhir << '\n';
    }
    file.close();
}

void updateBookmark(const char* username, const char* judulBuku, int halaman) { // linear short = dzakwan
    for (int i = 0; i < jumlahBookmark; i++) {
        if (strcmp(daftarBookmark[i].username, username) == 0 && 
            strcmp(daftarBookmark[i].judulBuku, judulBuku) == 0) {
            daftarBookmark[i].halamanTerakhir = halaman;
            simpanDataBookmark();
            return;
        }
    }

    if (jumlahBookmark < BatasBookmark) {
        strcpy(daftarBookmark[jumlahBookmark].username, username);
        strcpy(daftarBookmark[jumlahBookmark].judulBuku, judulBuku);
        daftarBookmark[jumlahBookmark].halamanTerakhir = halaman;
        jumlahBookmark++;
        simpanDataBookmark();
    }
}

int getBookmark(const char* username, const char* judulBuku) { // linear short = dzakwan
    for (int i = 0; i < jumlahBookmark; i++) {
        if (strcmp(daftarBookmark[i].username, username) == 0 && 
            strcmp(daftarBookmark[i].judulBuku, judulBuku) == 0) {
            return daftarBookmark[i].halamanTerakhir;
        }
    }
    return -1;
}

bool isUsernameTaken(const char* nama) {
    for (int i = 0; i < jumlahPengguna; i++) {
        if (strcmp(nama, pengguna[i].nama) == 0) {
            return true;
        }
    }
    return false;
}

void registrasiPengguna() {
    if (jumlahPengguna >= BatasPengguna) {
        cout << "Kapasitas pengguna penuh!\n";
        return;
    }

    Pengguna p;
    cin.ignore();

    while (true) {
        cout << "Masukkan Username: ";
        cin.getline(p.nama, 50);
        
        if (isUsernameTaken(p.nama)) {
            cout << "Username sudah digunakan. Silakan pilih username lain.\n";
        } else {
            break;
        }
    }

    cout << "Pilih tipe pengguna:\n";
    cout << "1. Mahasiswa\n2. Publik\nPilihan: ";
    int tipe;
    cin >> tipe;
    cin.ignore();

    if (tipe == 1) {
        strcpy(p.tipe, "mahasiswa");
        cout << "Masukkan NIM: ";
        cin.getline(p.id, 20);
    } else {
        strcpy(p.tipe, "publik");
        cout << "Masukkan NIK: ";
        cin.getline(p.id, 20);
    }

    strcpy(p.role, "user");
    p.banned = false;

    pengguna[jumlahPengguna++] = p;
    simpanDataPengguna();
    cout << "Registrasi berhasil!\n";
}

bool loginPengguna() { // linear short = gevan
    char nama[50];
    char id[20];
    cin.ignore();

    cout << "Masukkan Username: ";
    cin.getline(nama, 50);
    cout << "Masukkan ID (NIM/NIK/Password): ";
    cin.getline(id, 20);

    for (int i = 0; i < jumlahPengguna; i++) {
        if (strcmp(nama, pengguna[i].nama) == 0 && strcmp(id, pengguna[i].id) == 0) {
            if (pengguna[i].banned) {
                cout << "Akun ini dibanned! Tidak dapat login.\n";
                return false;
            }
            
            cout << "Login berhasil! Sebagai " << pengguna[i].role << "\n";
            indexpengguna = i;
            return true;
        }
    }

    cout << "Login gagal! Username atau ID tidak ditemukan.\n";
    return false;
}

void lihatPengguna() {
    if (jumlahPengguna == 0) {
        cout << "Belum ada pengguna yang terdaftar.\n";
        return;
    }

    cout << "Daftar Pengguna:\n";
    for (int i = 0; i < jumlahPengguna; i++) {
        cout << i+1 << ". Username: " << pengguna[i].nama << ", ID: " << pengguna[i].id 
             << ", Role: " << pengguna[i].role << ", Tipe: " << pengguna[i].tipe 
             << ", Status: " << (pengguna[i].banned ? "1" : "Aktif") << endl;
    }
}

void tambahBukuDefault() {
    strcpy(daftarBuku[jumlahBuku].judul, "Bahasa pemrograman C++");
    daftarBuku[jumlahBuku].jumlahHalaman = 2;
    strcpy(daftarBuku[jumlahBuku].halaman[0], "Bab 1: Pengenalan C++\nC++ adalah bahasa pemrograman tingkat tinggi yang dikembangkan oleh Bjarne Stroustrup sebagai pengembangan dari bahasa C. C++ mendukung paradigma prosedural dan berorientasi objek (OOP).");
    strcpy(daftarBuku[jumlahBuku].halaman[1], "Bab 2: Kelebihan: Performa tinggi dan efisien (dekat dengan mesin). Cocok untuk sistem operasi, game engine, dan perangkat lunak berat.");
    jumlahBuku++;

    strcpy(daftarBuku[jumlahBuku].judul, "Bahasa pemrograman Python");
    daftarBuku[jumlahBuku].jumlahHalaman = 3;
    strcpy(daftarBuku[jumlahBuku].halaman[0], "Bab 1: Python adalah bahasa pemrograman tingkat tinggi yang dibuat oleh Guido van Rossum dan dirilis pertama kali pada tahun 1991");
    strcpy(daftarBuku[jumlahBuku].halaman[1], "Bab 2: Bahasa ini dirancang dengan sintaks yang sederhana dan mudah dibaca, serta mendukung berbagai paradigma pemrograman seperti prosedural, fungsional, dan objek.");
    strcpy(daftarBuku[jumlahBuku].halaman[2], "Bab 3: Python biasa dipakai dalam pengembangan situs dan perangkat lunak, membuat analisis data, visualisasi data dan otomatisasi tugas");
    jumlahBuku++;

    simpanDataBuku();
}

void tampilkanDaftarBuku() {
    if (jumlahBuku == 0) {
        cout << "Belum ada buku tersedia.\n";
        return;
    }
    selectionSortBukuByJudul(); 

    cout << "\nDaftar Buku:\n";
    for (int i = 0; i < jumlahBuku; i++) {
        cout << i+1 << ". " << daftarBuku[i].judul
             << " (" << daftarBuku[i].jumlahHalaman << " halaman)\n";
    }
}

void beriUlasan(int indexBuku) {
    int indexUlasan = -1;
    for (int i = 0; i < jumlahUlasan; i++) {
        if (strcmp(daftarUlasan[i].username, pengguna[indexpengguna].nama) == 0 &&
            strcmp(daftarUlasan[i].judulBuku, daftarBuku[indexBuku].judul) == 0) {
            indexUlasan = i;
            break;
        }
    }

    if (indexUlasan != -1) {
        cout << "Anda sudah memberikan ulasan untuk buku ini.\n";
        cout << "Rating sebelumnya: " << daftarUlasan[indexUlasan].rating << endl;
        cout << "Ulasan sebelumnya: " << daftarUlasan[indexUlasan].komentar << endl;
        cout << "Apakah Anda ingin mengubahnya? (y/n): ";
        char pilihan;
        cin >> pilihan;
        cin.ignore();
        if (pilihan != 'y' && pilihan != 'Y') {
            return;
        }
    } else {
        if(jumlahUlasan >= BatasUlasan) {
            cout << "Kapasitas ulasan penuh." << endl;
            return;
        }
        indexUlasan = jumlahUlasan;
        jumlahUlasan++;
        strcpy(daftarUlasan[indexUlasan].username, pengguna[indexpengguna].nama);
        strcpy(daftarUlasan[indexUlasan].judulBuku, daftarBuku[indexBuku].judul);
    }

    cout << "Berikan rating (1-5): ";
    cin >> daftarUlasan[indexUlasan].rating;
    while (cin.fail() || daftarUlasan[indexUlasan].rating < 1 || daftarUlasan[indexUlasan].rating > 5) {
        cout << "Input tidak valid. Rating harus antara 1 sampai 5. Ulangi: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> daftarUlasan[indexUlasan].rating;
    }

    cin.ignore();
    cout << "Berikan ulasan (maksimal 500 karakter): ";
    cin.getline(daftarUlasan[indexUlasan].komentar, 500);
    
    char tanggal[20];
    dapatkanTanggalSekarang(tanggal);
    strcpy(daftarUlasan[indexUlasan].tanggal, tanggal);

    simpanDataUlasan();
    cout << "Terima kasih atas ulasannya!\n";
}

void tampilkanUlasan(int indexBuku) {
    double totalRating = 0;
    int count = 0;
    for (int i = 0; i < jumlahUlasan; i++) {
        if (strcmp(daftarUlasan[i].judulBuku, daftarBuku[indexBuku].judul) == 0) {
            totalRating += daftarUlasan[i].rating;
            count++;
        }
    }

    if (count > 0) {
        cout << fixed << setprecision(1);
        cout << "\nRata-rata rating: " << (totalRating / count) 
             << " dari 5 bintang (" << count << " ulasan)\n";
    } else {
        cout << "\nBelum ada rating untuk buku ini.\n";
    }

    cout << "\n=== ULASAN ===";
    bool adaUlasan = false;
    for (int i = 0; i < jumlahUlasan; i++) {
        if (strcmp(daftarUlasan[i].judulBuku, daftarBuku[indexBuku].judul) == 0) {
            adaUlasan = true;
            cout << "\nPengguna: " << daftarUlasan[i].username;
            cout << "\nRating: " << daftarUlasan[i].rating << "/5";
            cout << "\nUlasan: " << daftarUlasan[i].komentar;
            cout << "\nTanggal: " << daftarUlasan[i].tanggal;
            cout << "\n-------------------\n";
        }
    }
    if(!adaUlasan){
        cout << "\nBelum ada ulasan untuk buku ini.\n";
    }
}

void bacaBukuHandler(int indexBuku) {
    if (indexBuku < 0 || indexBuku >= jumlahBuku) {
        cout << "Indeks buku tidak valid.\n";
        return;
    }

    tampilkanUlasan(indexBuku);

    int halamanTerakhir = getBookmark(pengguna[indexpengguna].nama, daftarBuku[indexBuku].judul);
    int halamanSekarang = 0;

    if (halamanTerakhir != -1) {
        char pilihanLanjut;
        cout << "Anda terakhir membaca sampai halaman " << halamanTerakhir+1 
             << ". Lanjutkan dari sana? (y/n): ";
        cin >> pilihanLanjut;
        if (pilihanLanjut == 'y' || pilihanLanjut == 'Y') {
            halamanSekarang = halamanTerakhir;
        }
    }

    char perintah;
    do {
        system("cls||clear");
        cout << "Judul: " << daftarBuku[indexBuku].judul << endl;
        cout << "Halaman " << halamanSekarang+1 << " dari " 
             << daftarBuku[indexBuku].jumlahHalaman << endl;
        cout << "----------------------------------------\n";
        cout << daftarBuku[indexBuku].halaman[halamanSekarang] << endl;
        cout << "----------------------------------------\n";
        cout << "l: lanjut | k: kembali | p: Pilih Halaman | s: selesai\n";
        cout << "Perintah: ";
        cin >> perintah;
        perintah = tolower(perintah);

        if (perintah == 'l' && halamanSekarang < daftarBuku[indexBuku].jumlahHalaman-1) {
            halamanSekarang++;
        } else if (perintah == 'k' && halamanSekarang > 0) {
            halamanSekarang--;
        } else if (perintah == 'p') {
            int target;
            cout << "Masukkan nomor halaman (1-" << daftarBuku[indexBuku].jumlahHalaman << "): ";
            cin >> target;
            if (!cin.fail() && target >= 1 && target <= daftarBuku[indexBuku].jumlahHalaman) {
                halamanSekarang = target - 1;
            } else {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Halaman tidak valid!\n";
                system("pause");
            }
        }
    } while (perintah != 's');

    updateBookmark(pengguna[indexpengguna].nama, daftarBuku[indexBuku].judul, halamanSekarang);

    cout << "Apakah Anda ingin memberikan ulasan untuk buku ini? (y/n): ";
    char ulasan;
    cin >> ulasan;
    if (ulasan == 'y' || ulasan == 'Y') {
        beriUlasan(indexBuku);
    }
}

void inputMultiLine(char* buffer) {
    buffer[0] = '\0';
    char line[PanjangHal];
    
    cout << "Masukkan isi halaman (akhiri dengan '::end::' di baris baru):\n";
    while (true) {
        cin.getline(line, PanjangHal);
        if (strcmp(line, "::end::") == 0) break;
        
        if (strlen(buffer) + strlen(line) + 2 < PanjangHal) {
            if (buffer[0] != '\0') {
                strcat(buffer, "\n");
            }
            strcat(buffer, line);
        } else {
            cout << "Batas karakter halaman terlampaui! Sebagian input mungkin terpotong.\n";
            while(strcmp(line, "::end::") != 0 && cin.getline(line, PanjangHal)) {}
            break;
        }
    }
}

void tambahBuku() {
    if (jumlahBuku >= BatasBuku) {
        cout << "Kapasitas buku penuh!\n";
        return;
    }

    Buku b;
    cin.ignore();

    cout << "Masukkan judul buku: ";
    cin.getline(b.judul, BatasJudul);

    cout << "Masukkan jumlah halaman: ";
    cin >> b.jumlahHalaman;
    
    if (cin.fail() || b.jumlahHalaman <= 0 || b.jumlahHalaman > BatasHal) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Jumlah halaman tidak valid (1-" << BatasHal << ")\n";
        return;
    }
    cin.ignore();

    for (int i = 0; i < b.jumlahHalaman; i++) {
        cout << "Isi halaman " << i+1 << ":\n";
        inputMultiLine(b.halaman[i]);
    }

    daftarBuku[jumlahBuku++] = b;
    simpanDataBuku();
    cout << "Buku berhasil ditambahkan!\n";
}

void hapusBuku() { // linear short = gevan
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku(); 

    char judulHapus[BatasJudul];
    cin.ignore();
    cout << "\nMasukkan judul buku yang akan dihapus (kosongkan untuk batal): ";
    cin.getline(judulHapus, BatasJudul);

    if (strlen(judulHapus) == 0) {
        cout << "Penghapusan dibatalkan.\n";
        return;
    }

    // --- Searching Binary: untuk menghapus buku ---
    int index = binarySearchBukuByJudul(judulHapus); // Binary Short = wahyu

    if (index == -1) {
        cout << "Buku dengan judul '" << judulHapus << "' tidak ditemukan.\n";
        return;
    }
    
    cout << "Apakah Anda yakin ingin menghapus buku '" 
         << daftarBuku[index].judul << "'? (y/n): ";
    
    char konfirmasi;
    cin >> konfirmasi;
    cin.ignore();

    if (tolower(konfirmasi) == 'y') {
        for (int i = index; i < jumlahBuku - 1; i++) {
            daftarBuku[i] = daftarBuku[i+1];
        }
        jumlahBuku--;
        simpanDataBuku();
        cout << "Buku berhasil dihapus!\n";
    } else {
        cout << "Penghapusan dibatalkan.\n";
    }
}

void editBuku() { // linear short = gevan
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku();
    
    char judulEdit[BatasJudul];
    cin.ignore();
    cout << "\nMasukkan judul buku yang akan diedit (kosongkan untuk batal): ";
    cin.getline(judulEdit, BatasJudul);

    if (strlen(judulEdit) == 0) {
        cout << "Pengeditan dibatalkan.\n";
        return;
    }

    // --- Searching Binary: untuk mengedit buku ---
    int index = binarySearchBukuByJudul(judulEdit); // Binary Short = wahyu

    if (index == -1) {
        cout << "Buku dengan judul '" << judulEdit << "' tidak ditemukan.\n";
        return;
    }

    int pilihanEdit;

    do {
        system("cls||clear");
        cout << "\nMenu Edit Buku: " << daftarBuku[index].judul << endl;
        cout << "1. Edit Judul\n";
        cout << "2. Edit Halaman\n";
        cout << "3. Tambah Halaman\n";
        cout << "4. Hapus Halaman Terakhir\n";
        cout << "5. Selesai\n";
        cout << "Pilihan: ";
        cin >> pilihanEdit;

        if(cin.fail()){
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Pilihan tidak valid.\n";
            pilihanEdit = 0;
            system("pause");
            continue;
        }
        cin.ignore();

        switch (pilihanEdit) {
            case 1: {
                cout << "Judul baru: ";
                cin.getline(daftarBuku[index].judul, BatasJudul);
                simpanDataBuku();
                cout << "Judul berhasil diubah!\n";
                break;
            }
            case 2: {
                cout << "Nomor halaman yang diedit (1-" << daftarBuku[index].jumlahHalaman << "): ";
                int halaman;
                cin >> halaman;

                if (cin.fail() || halaman < 1 || halaman > daftarBuku[index].jumlahHalaman) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Halaman tidak valid.\n";
                    break;
                }
                cin.ignore();

                cout << "Isi halaman baru:\n";
                inputMultiLine(daftarBuku[index].halaman[halaman-1]);
                simpanDataBuku();
                cout << "Halaman berhasil diubah!\n";
                break;
            }
            case 3: {
                if (daftarBuku[index].jumlahHalaman >= BatasHal) {
                    cout << "Jumlah halaman sudah mencapai batas maksimal.\n";
                    break;
                }

                cout << "Isi halaman baru:\n";
                inputMultiLine(daftarBuku[index].halaman[daftarBuku[index].jumlahHalaman]);
                daftarBuku[index].jumlahHalaman++;
                simpanDataBuku();
                cout << "Halaman baru berhasil ditambahkan!\n";
                break;
            }
            case 4: {
                if (daftarBuku[index].jumlahHalaman <= 1) {
                    cout << "Tidak bisa menghapus halaman, buku minimal memiliki 1 halaman!\n";
                    break;
                }
                
                char conf;
                cout << "Yakin ingin menghapus halaman terakhir? (y/n): ";
                cin >> conf;
                cin.ignore();
                if(tolower(conf) == 'y'){
                    daftarBuku[index].jumlahHalaman--;
                    simpanDataBuku();
                    cout << "Halaman terakhir berhasil dihapus!\n";
                }
                break;
            }
            case 5:
                cout << "Selesai mengedit buku.\n";
                break;
            default:
                cout << "Pilihan tidak valid.\n";
        }
        if(pilihanEdit != 5) system("pause");
    } while (pilihanEdit != 5);
}

void hapusPengguna() {
    if (jumlahPengguna <= 1) {
        cout << "Tidak ada pengguna yang bisa dihapus.\n";
        return;
    }
    
    lihatPengguna();
    cout << "Pilih nomor pengguna yang akan dihapus (0 untuk batal): ";
    int pilihan;
    cin >> pilihan;
    
    if (cin.fail() || pilihan < 0 || pilihan > jumlahPengguna) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Nomor tidak valid.\n";
        return;
    }
     if (pilihan == 0) return;
    
    int index = pilihan - 1;
    
    if (index == indexpengguna) {
        cout << "Tidak dapat menghapus akun sendiri!\n";
        return;
    }

    if (strcmp(pengguna[index].role, "admin") == 0){
        cout << "Tidak dapat menghapus akun admin!" << endl;
        return;
    }
    
    for (int i = index; i < jumlahPengguna-1; i++) {
        pengguna[i] = pengguna[i+1];
    }
    jumlahPengguna--;
    simpanDataPengguna();
    cout << "Pengguna berhasil dihapus.\n";
}

void bannedPengguna() {
    if (jumlahPengguna <= 1) {
        cout << "Tidak ada pengguna untuk dibanned.\n";
        return;
    }
    lihatPengguna();
    cout << "Pilih nomor pengguna yang akan dibanned (0 untuk batal): ";
    int pilihan;
    cin >> pilihan;
    
    if (cin.fail() || pilihan < 0 || pilihan > jumlahPengguna) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Nomor tidak valid.\n";
        return;
    }
    if (pilihan == 0) return;

    int index = pilihan - 1;
    
    if (index == indexpengguna) {
        cout << "Tidak dapat membanned akun sendiri!\n";
        return;
    }

    if (strcmp(pengguna[index].role, "admin") == 0){
        cout << "Tidak dapat membanned akun admin!" << endl;
        return;
    }
    
    pengguna[index].banned = true;
    simpanDataPengguna();
    cout << "Pengguna berhasil dibanned.\n";
}

void unbannedPengguna() {
    lihatPengguna();
    cout << "Pilih nomor pengguna yang akan di-unbanned (0 untuk batal): ";
    int pilihan;
    cin >> pilihan;
    
    if (cin.fail() || pilihan < 0 || pilihan > jumlahPengguna) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Nomor tidak valid.\n";
        return;
    }
    if (pilihan == 0) return;
    
    pengguna[pilihan-1].banned = false;
    simpanDataPengguna();
    cout << "Pengguna berhasil di-unbanned.\n";
}

void lihatSemuaUlasan() {
    if (jumlahUlasan == 0) {
        cout << "Belum ada ulasan.\n";
        return;
    }

    cout << "\nDaftar Semua Ulasan:\n";
    for (int i = 0; i < jumlahUlasan; i++) {
        cout << "ID: " << i+1 << endl;
        cout << "Buku: " << daftarUlasan[i].judulBuku << endl;
        cout << "Pengguna: " << daftarUlasan[i].username << endl;
        cout << "Rating: " << daftarUlasan[i].rating << "/5" << endl;
        cout << "Ulasan: " << daftarUlasan[i].komentar << endl;
        cout << "Tanggal: " << daftarUlasan[i].tanggal << endl;
        cout << "------------------------\n";
    }

    cout << "Hapus ulasan? (y/n): ";
    char hapus;
    cin >> hapus;
    if (hapus == 'y' || hapus == 'Y') {
        cout << "Masukkan ID ulasan yang akan dihapus (0 untuk batal): ";
        int id;
        cin >> id;
        if (!cin.fail() && id >= 1 && id <= jumlahUlasan) {
            for (int i = id-1; i < jumlahUlasan-1; i++) {
                daftarUlasan[i] = daftarUlasan[i+1];
            }
            jumlahUlasan--;
            simpanDataUlasan();
            cout << "Ulasan berhasil dihapus.\n";
        } else if (id != 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "ID tidak valid.\n";
        }
    }
}

void tampilkanBukuSaya() { // linear short = dzakwan
    cout << "\nBuku yang Sedang Anda Baca:\n";
    bool ditemukan = false;
    for (int i = 0; i < jumlahBookmark; i++) { // Pencarian Linear (Dzakwan)
        if (strcmp(daftarBookmark[i].username, pengguna[indexpengguna].nama) == 0) {
            for (int j = 0; j < jumlahBuku; j++) { // Pencarian Linear (Dzakwan)
                if (strcmp(daftarBookmark[i].judulBuku, daftarBuku[j].judul) == 0) {
                    cout << " - " << daftarBuku[j].judul 
                         << " (Terakhir di Halaman " << daftarBookmark[i].halamanTerakhir+1 
                         << " dari " << daftarBuku[j].jumlahHalaman << ")\n";
                    ditemukan = true;
                    break; 
                }
            }
        }
    }

    if (!ditemukan) {
        cout << "Anda belum memiliki riwayat membaca buku.\n";
    }
}

void kelolaBacaBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    selectionSortBukuByJudul(); 

    int pilihanAksi;
    do {
        system("cls||clear");
        cout << "\n===== DAFTAR BUKU & BACA =====";
        cout << "\n1. Tampilkan Semua Buku";
        cout << "\n2. Cari Buku"; // Ini adalah pencarian linear (Gevan)
        cout << "\n3. Kembali ke Menu Utama";
        cout << "\nPilihan: ";
        cin >> pilihanAksi;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Pilihan tidak valid.\n";
            pilihanAksi = 0;
            system("pause");
            continue;
        }
        cin.ignore();

        switch (pilihanAksi) {
            case 1: {
                if (jumlahBuku == 0) {
                    cout << "Tidak ada buku tersedia.\n";
                    break;
                }
                
                cout << "\nDaftar Buku:\n";
                for (int i = 0; i < jumlahBuku; i++) {
                    cout << i + 1 << ". " << daftarBuku[i].judul
                         << " (" << daftarBuku[i].jumlahHalaman << " halaman)\n";
                }

                cout << "\nPilih nomor buku untuk dibaca (0 untuk batal): ";
                int pilihanBuku;
                cin >> pilihanBuku;

                if (cin.fail() || pilihanBuku < 0 || pilihanBuku > jumlahBuku) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Pilihan tidak valid.\n";
                } else if (pilihanBuku != 0) {
                    bacaBukuHandler(pilihanBuku - 1); 
                }
                break;
            }
            case 2: { // Linear Search: Mencari buku berdasarkan substring judul (Gevan)
                char judulCari[BatasJudul];
                cout << "Masukkan sebagian atau seluruh judul buku untuk dicari: ";
                cin.getline(judulCari, BatasJudul);

                for (int i = 0; judulCari[i]; i++) {
                    judulCari[i] = tolower(judulCari[i]);
                }

                cout << "\nHasil Pencarian:\n";
                bool ditemukan = false;
                int indexDitemukan[BatasBuku];
                int hitungDitemukan = 0;

                for (int i = 0; i < jumlahBuku; i++) { // Pencarian Linear (Gevan)
                    char judulBukuLower[BatasJudul];
                    strcpy(judulBukuLower, daftarBuku[i].judul);
                    for (int j = 0; judulBukuLower[j]; j++) {
                        judulBukuLower[j] = tolower(judulBukuLower[j]);
                    }

                    if (strstr(judulBukuLower, judulCari) != NULL) {
                        indexDitemukan[hitungDitemukan++] = i;
                        ditemukan = true;
                    }
                }

                if (!ditemukan) {
                    cout << "Tidak ada buku yang sesuai dengan pencarian.\n";
                } else {
                    for (int i = 0; i < hitungDitemukan; i++) {
                        int idx = indexDitemukan[i];
                        cout << (i + 1) << ". " << daftarBuku[idx].judul
                             << " (" << daftarBuku[idx].jumlahHalaman << " halaman)\n";
                    }

                    cout << "\nPilih nomor buku untuk dibaca (0 untuk batal): ";
                    int pilihanBuku;
                    cin >> pilihanBuku;

                    if (cin.fail() || pilihanBuku < 0 || pilihanBuku > hitungDitemukan) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Pilihan tidak valid.\n";
                    } else if (pilihanBuku != 0) {
                        bacaBukuHandler(indexDitemukan[pilihanBuku - 1]);
                    }
                }
                break;
            }
            case 3:
                cout << "Kembali ke menu utama.\n";
                break;
            default:
                cout << "Pilihan tidak valid.\n";
        }
        if (pilihanAksi != 3) system("pause");
    } while (pilihanAksi != 3);
}


void kelolaPengguna() {
    int pilihan;
    do {
        system("cls||clear");
        cout << "\n===== KELOLA PENGGUNA =====";
        cout << "\n1. Lihat Daftar Pengguna";
        cout << "\n2. Hapus Pengguna";
        cout << "\n3. Banned Pengguna";
        cout << "\n4. Unbanned Pengguna";
        cout << "\n5. Kembali";
        cout << "\nPilihan: ";
        cin >> pilihan;

        if(cin.fail()){
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Pilihan tidak valid.\n";
            pilihan = 0;
            system("pause");
            continue;
        }

        switch (pilihan) {
            case 1: lihatPengguna(); break;
            case 2: hapusPengguna(); break;
            case 3: bannedPengguna(); break;
            case 4: unbannedPengguna(); break;
            case 5: break;
            default: cout << "Pilihan tidak valid.\n";
        }
        if(pilihan != 5) system("pause");
    } while (pilihan != 5);
}

void menuAdmin() {
    int pilihan;
    do {
        system("cls||clear");
        cout << "\n===== MENU ADMINISTRATOR =====";
        cout << "\nSelamat datang, " << pengguna[indexpengguna].nama << "!";
        cout << "\n1. Tambah Buku";
        cout << "\n2. Edit Buku";
        cout << "\n3. Hapus Buku";
        cout << "\n4. Daftar Buku & Baca";
        cout << "\n5. Kelola Pengguna";
        cout << "\n6. Manajemen Ulasan";
        cout << "\n7. Logout";
        cout << "\nPilihan: ";
        cin >> pilihan;

        if(cin.fail()){
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Pilihan tidak valid.\n";
            pilihan = 0;
            system("pause");
            continue;
        }

        switch (pilihan) {
            case 1: tambahBuku(); break;
            case 2: editBuku(); break; 
            case 3: hapusBuku(); break; 
            case 4: kelolaBacaBuku(); break;
            case 5: kelolaPengguna(); break;
            case 6: lihatSemuaUlasan(); break;
            case 7: cout << "Logout berhasil.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
        if (pilihan != 7) system("pause");
    } while (pilihan != 7);
}

void menuUser() {
    int pilihan;
    do {
        system("cls||clear");
        cout << "\n===== MENU PENGGUNA =====";
        cout << "\nSelamat datang, " << pengguna[indexpengguna].nama << "!";
        cout << "\n1. Daftar Buku & Baca";
        cout << "\n2. Riwayat Buku Saya";
        cout << "\n3. Logout";
        cout << "\nPilihan: ";
        cin >> pilihan;

        if(cin.fail()){
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Pilihan tidak valid.\n";
            pilihan = 0;
            system("pause");
            continue;
        }

        switch (pilihan) {
            case 1: kelolaBacaBuku(); break;
            case 2: tampilkanBukuSaya(); break;
            case 3: cout << "Logout berhasil.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
        if (pilihan != 3) system("pause"); 
    } while (pilihan != 3); 
}

int main() {
    bacaDataPengguna();
    bacaDataBuku();
    bacaDataUlasan();
    bacaDataBookmark();
    
    if (jumlahPengguna == 0) {
        strcpy(pengguna[jumlahPengguna].nama, "admin");
        strcpy(pengguna[jumlahPengguna].id, "admin123");
        strcpy(pengguna[jumlahPengguna].role, "admin");
        strcpy(pengguna[jumlahPengguna].tipe, "admin");
        pengguna[jumlahPengguna].banned = false;
        jumlahPengguna++;
        simpanDataPengguna();
    }

    if (jumlahBuku == 0) {
        tambahBukuDefault();
    }

    int pilihan;
    bool login = false;

    while (true) {
        if (!login) {
            system("cls||clear");
            cout << "\n===== PERPUSTAKAAN DIGITAL =====";
            cout << "\n1. Registrasi";
            cout << "\n2. Login";
            cout << "\n3. Keluar";
            cout << "\nPilihan: ";
            cin >> pilihan;

            if(cin.fail()){
                cin.clear();
                cin.ignore(10000, '\n');
                pilihan = 0;
                system("pause");
                continue;
            }

            switch (pilihan) {
                case 1: 
                    registrasiPengguna(); 
                    system("pause");
                    break;
                case 2:
                    login = loginPengguna();
                    system("pause");
                    break;
                case 3: 
                    cout << "Terima kasih telah menggunakan program ini.\n";
                    return 0;
                default: 
                    cout << "Pilihan tidak valid.\n";
                    system("pause");
            }
        } else {
            if (strcmp(pengguna[indexpengguna].role, "admin") == 0) {
                menuAdmin();
            } else {
                menuUser();
            }
            login = false;
            indexpengguna = -1;
        }
    }
}
