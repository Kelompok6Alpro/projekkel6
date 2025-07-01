#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string.h> // Sebaiknya gunakan <string> untuk std::string di C++, tapi karena char array sudah banyak, ini tidak diubah sekarang.

using namespace std;

// --- KONSTANTA BATAS ---
const int BatasPengguna = 50;
const int BatasBuku = 50;
const int BatasJudul = 100;
const int BatasHal = 100;
const int PanjangHal = 2000;
const int BatasUlasan = 500;
const int BatasBookmark = 500;

// --- STRUCT DEFINITIONS ---
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

// --- GLOBAL VARIABLES ---
Pengguna pengguna[BatasPengguna];
int jumlahPengguna = 0;

Buku daftarBuku[BatasBuku];
int jumlahBuku = 0;

Ulasan daftarUlasan[BatasUlasan];
int jumlahUlasan = 0;

Bookmark daftarBookmark[BatasBookmark];
int jumlahBookmark = 0;

int indexpengguna = -1;

// --- FUNGSI UTILITAS ---
void dapatkanTanggalSekarang(char* buffer) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    sprintf(buffer, "%02d/%02d/%04d", ltm->tm_mday, ltm->tm_mon + 1, 1900 + ltm->tm_year);
}

// --- FUNGSI SORTING ---
// Digunakan untuk Binary Search
void selectionSortBukuByJudul() {
    for (int i = 0; i < jumlahBuku - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < jumlahBuku; j++) {
            // strcasecmp untuk perbandingan case-insensitive
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
// Asumsi: daftarBuku sudah diurutkan berdasarkan judul
int binarySearchBukuByJudul(const char* judulCari) {
    int low = 0;
    int high = jumlahBuku - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        // strcasecmp untuk perbandingan case-insensitive
        int cmp = strcasecmp(daftarBuku[mid].judul, judulCari);

        if (cmp == 0) {
            return mid; // Buku ditemukan
        } else if (cmp < 0) {
            low = mid + 1; // Cari di setengah kanan
        } else {
            high = mid - 1; // Cari di setengah kiri
        }
    }
    return -1; // Buku tidak ditemukan
}

// [MODIFIKASI] Fungsi cariBuku - Menggunakan Linear Search dengan filter substring
void cariBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }
    
    // Sortir buku agar hasil pencarian tampil terurut (opsional tapi baik)
    selectionSortBukuByJudul(); 

    char judulCari[BatasJudul];
    cin.ignore();
    cout << "Masukkan sebagian atau seluruh judul buku untuk dicari: ";
    cin.getline(judulCari, BatasJudul);
    
    // Konversi input pencarian ke lowercase untuk pencarian case-insensitive
    for (int i = 0; judulCari[i]; i++) {
        judulCari[i] = tolower(judulCari[i]);
    }

    cout << "\nHasil Pencarian:\n";
    bool ditemukan = false;
    int indexDitemukan[BatasBuku]; // Menyimpan indeks buku yang cocok
    int hitungDitemukan = 0;

    for (int i = 0; i < jumlahBuku; i++) {
        char judulBukuLower[BatasJudul];
        strcpy(judulBukuLower, daftarBuku[i].judul);
        // Konversi judul buku ke lowercase untuk perbandingan
        for (int j = 0; judulBukuLower[j]; j++) {
            judulBukuLower[j] = tolower(judulBukuLower[j]);
        }

        // strstr untuk mencari substring
        if (strstr(judulBukuLower, judulCari) != NULL) {
            indexDitemukan[hitungDitemukan++] = i;
            ditemukan = true;
        }
    }

    if (!ditemukan) {
        cout << "Tidak ada buku yang sesuai dengan pencarian.\n";
    } else {
        // Tampilkan buku yang ditemukan
        for (int i = 0; i < hitungDitemukan; i++) {
            int idx = indexDitemukan[i];
            cout << (i + 1) << ". " << daftarBuku[idx].judul
                 << " (" << daftarBuku[idx].jumlahHalaman << " halaman)\n";
        }
    }
}


// --- FUNGSI BACA DATA (DENGAN PERLINDUNGAN OVERFLOW) ---
void bacaDataPengguna() {
    ifstream file("pengguna.txt");
    if (!file) return;
    
    jumlahPengguna = 0;
    char buffer[100];
    
    // [FIX] Ditambahkan pengecekan batas array
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

    // [FIX] Ditambahkan pengecekan batas array
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
    // [FIX] Ditambahkan pengecekan batas array
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
    // [FIX] Ditambahkan pengecekan batas array
    while (jumlahBookmark < BatasBookmark && file.getline(daftarBookmark[jumlahBookmark].username, 50)) {
        file.getline(daftarBookmark[jumlahBookmark].judulBuku, BatasJudul);
        file >> daftarBookmark[jumlahBookmark].halamanTerakhir;
        file.ignore();
        jumlahBookmark++;
    }
    file.close();
}

// --- FUNGSI SIMPAN DATA ---
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

// --- FUNGSI BOOKMARK ---
// [MODIFIKASI] updateBookmark - Tetap Linear Search (cukup efisien untuk bookmark)
void updateBookmark(const char* username, const char* judulBuku, int halaman) {
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

// [MODIFIKASI] getBookmark - Tetap Linear Search (cukup efisien untuk bookmark)
int getBookmark(const char* username, const char* judulBuku) {
    for (int i = 0; i < jumlahBookmark; i++) {
        if (strcmp(daftarBookmark[i].username, username) == 0 && 
            strcmp(daftarBookmark[i].judulBuku, judulBuku) == 0) {
            return daftarBookmark[i].halamanTerakhir;
        }
    }
    return -1;
}

// --- FUNGSI MANAJEMEN PENGGUNA ---
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

bool loginPengguna() {
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
             << ", Status: " << (pengguna[i].banned ? "BANNED" : "Aktif") << endl;
    }
}

// --- FUNGSI BUKU & ULASAN ---
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
    // Panggil fungsi sortir agar daftar buku selalu terurut saat ditampilkan
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

void bacaBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku(); // Memastikan buku sudah diurutkan
    cout << "\nPilih nomor buku (0 untuk batal): ";
    int pilihan;
    cin >> pilihan;

    if (cin.fail() || pilihan < 0 || pilihan > jumlahBuku) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Pilihan tidak valid.\n";
        return;
    }
    if (pilihan == 0) return;

    // Karena daftar sudah diurutkan oleh tampilkanDaftarBuku(),
    // indeks yang dipilih pengguna sesuai dengan indeks di array terurut.
    int index = pilihan - 1; 
    tampilkanUlasan(index);

    int halamanTerakhir = getBookmark(pengguna[indexpengguna].nama, daftarBuku[index].judul);
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
        cout << "Judul: " << daftarBuku[index].judul << endl;
        cout << "Halaman " << halamanSekarang+1 << " dari " 
             << daftarBuku[index].jumlahHalaman << endl;
        cout << "----------------------------------------\n";
        cout << daftarBuku[index].halaman[halamanSekarang] << endl;
        cout << "----------------------------------------\n";
        cout << "l: lanjut | k: kembali | p: Pilih Halaman | s: selesai\n";
        cout << "Perintah: ";
        cin >> perintah;
        perintah = tolower(perintah);

        if (perintah == 'l' && halamanSekarang < daftarBuku[index].jumlahHalaman-1) {
            halamanSekarang++;
        } else if (perintah == 'k' && halamanSekarang > 0) {
            halamanSekarang--;
        } else if (perintah == 'p') {
            int target;
            cout << "Masukkan nomor halaman (1-" << daftarBuku[index].jumlahHalaman << "): ";
            cin >> target;
            if (!cin.fail() && target >= 1 && target <= daftarBuku[index].jumlahHalaman) {
                halamanSekarang = target - 1;
            } else {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Halaman tidak valid!\n";
                system("pause");
            }
        }
    } while (perintah != 's');

    updateBookmark(pengguna[indexpengguna].nama, daftarBuku[index].judul, halamanSekarang);

    cout << "Apakah Anda ingin memberikan ulasan untuk buku ini? (y/n): ";
    char ulasan;
    cin >> ulasan;
    if (ulasan == 'y' || ulasan == 'Y') {
        beriUlasan(index);
    }
}

// --- FUNGSI MANAJEMEN BUKU (ADMIN) ---
void inputMultiLine(char* buffer) {
    buffer[0] = '\0';
    char line[PanjangHal];
    
    cout << "Masukkan isi halaman (akhiri dengan '::end::' di baris baru):\n";
    while (true) {
        cin.getline(line, PanjangHal);
        if (strcmp(line, "::end::") == 0) break;
        
        if (strlen(buffer) + strlen(line) + 2 < PanjangHal) { // +2 for '\n' and '\0'
            if (buffer[0] != '\0') {
                strcat(buffer, "\n");
            }
            strcat(buffer, line);
        } else {
            cout << "Batas karakter halaman terlampaui! Sebagian input mungkin terpotong.\n";
            // Lanjutkan membaca baris hingga ::end:: untuk membersihkan buffer
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
    cin.ignore(); // Konsumsi newline setelah cin >> b.jumlahHalaman

    for (int i = 0; i < b.jumlahHalaman; i++) {
        cout << "Isi halaman " << i+1 << ":\n";
        inputMultiLine(b.halaman[i]);
    }

    daftarBuku[jumlahBuku++] = b;
    simpanDataBuku();
    cout << "Buku berhasil ditambahkan!\n";
}

// [MODIFIKASI] hapusBuku - Menggunakan Binary Search
void hapusBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku(); // Memastikan buku diurutkan untuk binary search

    char judulHapus[BatasJudul];
    cin.ignore();
    cout << "\nMasukkan judul buku yang akan dihapus (kosongkan untuk batal): ";
    cin.getline(judulHapus, BatasJudul);

    if (strlen(judulHapus) == 0) { // Jika input kosong, batal
        cout << "Penghapusan dibatalkan.\n";
        return;
    }

    int index = binarySearchBukuByJudul(judulHapus);

    if (index == -1) {
        cout << "Buku dengan judul '" << judulHapus << "' tidak ditemukan.\n";
        return;
    }
    
    cout << "Apakah Anda yakin ingin menghapus buku '" 
         << daftarBuku[index].judul << "'? (y/n): ";
    
    char konfirmasi;
    cin >> konfirmasi;
    cin.ignore(); // Konsumsi newline

    if (tolower(konfirmasi) == 'y') {
        // Geser elemen setelah yang dihapus ke kiri
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

// [MODIFIKASI] editBuku - Menggunakan Binary Search
void editBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku(); // Memastikan buku diurutkan untuk binary search
    
    char judulEdit[BatasJudul];
    cin.ignore();
    cout << "\nMasukkan judul buku yang akan diedit (kosongkan untuk batal): ";
    cin.getline(judulEdit, BatasJudul);

    if (strlen(judulEdit) == 0) { // Jika input kosong, batal
        cout << "Pengeditan dibatalkan.\n";
        return;
    }

    int index = binarySearchBukuByJudul(judulEdit);

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
            pilihanEdit = 0; // Reset choice
            system("pause");
            continue;
        }
        cin.ignore(); // Konsumsi newline

        switch (pilihanEdit) {
            case 1: {
                cout << "Judul baru: ";
                cin.getline(daftarBuku[index].judul, BatasJudul);
                simpanDataBuku();
                cout << "Judul berhasil diubah!\n";
                // Setelah mengubah judul, mungkin perlu mengurutkan ulang jika ada operasi lain yang bergantung pada urutan
                // Namun, untuk operasi edit ini, tidak langsung diperlukan karena tidak mengubah posisi buku dalam array
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
                cin.ignore(); // Konsumsi newline
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

// --- FUNGSI MANAJEMEN PENGGUNA (ADMIN) ---
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

// --- FUNGSI MENU ---

// [MODIFIKASI] Tampilkan Buku Saya - Tetap Linear Search
void tampilkanBukuSaya() {
    cout << "\nBuku yang Sedang Anda Baca:\n";
    bool ditemukan = false;
    for (int i = 0; i < jumlahBookmark; i++) {
        if (strcmp(daftarBookmark[i].username, pengguna[indexpengguna].nama) == 0) {
            // Linear search untuk menemukan detail buku dari bookmark
            for (int j = 0; j < jumlahBuku; j++) {
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
            pilihan = 0; // Reset
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
        cout << "\n4. Daftar Buku";
        cout << "\n5. Baca Buku";
        cout << "\n6. Kelola Pengguna";
        cout << "\n7. Manajemen Ulasan";
        cout << "\n8. Logout";
        cout << "\nPilihan: ";
        cin >> pilihan;

        if(cin.fail()){
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Pilihan tidak valid.\n";
            pilihan = 0; // Reset
            system("pause");
            continue;
        }

        switch (pilihan) {
            case 1: tambahBuku(); break;
            case 2: editBuku(); break; // Menggunakan Binary Search
            case 3: hapusBuku(); break; // Menggunakan Binary Search
            case 4: tampilkanDaftarBuku(); break;
            case 5: bacaBuku(); break;
            case 6: kelolaPengguna(); break;
            case 7: lihatSemuaUlasan(); break;
            case 8: cout << "Logout berhasil.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
        if (pilihan != 8) system("pause");
    } while (pilihan != 8);
}

void menuUser() {
    int pilihan;
    do {
        system("cls||clear");
        cout << "\n===== MENU PENGGUNA =====";
        cout << "\nSelamat datang, " << pengguna[indexpengguna].nama << "!";
        cout << "\n1. Daftar Seluruh Buku";
        cout << "\n2. Cari Buku";
        cout << "\n3. Baca Buku";
        cout << "\n4. Riwayat Buku Saya";
        cout << "\n5. Logout";
        cout << "\nPilihan: ";
        cin >> pilihan;

        if(cin.fail()){
            cin.clear();
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Pilihan tidak valid.\n";
            pilihan = 0; // Reset
            system("pause");
            continue;
        }

        switch (pilihan) {
            case 1: tampilkanDaftarBuku(); break;
            case 2: cariBuku(); break; // Menggunakan Linear Search dengan substring
            case 3: bacaBuku(); break;
            case 4: tampilkanBukuSaya(); break;
            case 5: cout << "Logout berhasil.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
        if (pilihan != 5) system("pause");
    } while (pilihan != 5);
}

// --- FUNGSI UTAMA (MAIN) ---
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
                cout << "Pilihan tidak valid.\n";
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
