#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>

using namespace std;

const int BatasPengguna = 50;
const int BatasBuku = 50;
const int BatasJudul = 100;
const int BatasHal = 100;
const int PanjangHal = 500;

struct Pengguna {
    char nama[50];
    char nim[20];
};

struct Buku {
    char judul[BatasJudul];
    int jumlahHalaman;
    char halaman[BatasHal][PanjangHal];
};

Pengguna pengguna[BatasPengguna];
int jumlahPengguna = 0;

Buku daftarBuku[BatasBuku];
int jumlahBuku = 0;

void bacaDataPengguna() {
    ifstream file("pengguna.txt");
    if (!file) return;
    jumlahPengguna = 0;
    while (file.getline(pengguna[jumlahPengguna].nama, 50) && file.getline(pengguna[jumlahPengguna].nim, 20)) {
    	jumlahPengguna++;
    }
    file.close();
}

void simpanDataPengguna() {
    ofstream file("pengguna.txt");
    for (int i = 0; i < jumlahPengguna; i++) {
        file << pengguna[i].nama << '\n'
             << pengguna[i].nim << '\n';
    }
    file.close();
}

void bacaDataBuku() {
    ifstream file("buku.txt");
    if (!file) return;

    jumlahBuku = 0;
    char buffer[PanjangHal];

    while (file.getline(daftarBuku[jumlahBuku].judul, BatasJudul)) {
        file >> daftarBuku[jumlahBuku].jumlahHalaman;
        file.ignore();

        for (int i = 0; i < daftarBuku[jumlahBuku].jumlahHalaman; i++) {
            file.getline(daftarBuku[jumlahBuku].halaman[i], PanjangHal);
        }
        jumlahBuku++;
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
        }
    }
    file.close();
}

void registrasiPengguna() {
    if (jumlahPengguna >= BatasPengguna) {
        cout << "Kapasitas pengguna penuh!\n";
        return;
    }

    Pengguna p;
    cin.ignore();

    cout << "Masukkan Nama: ";
    cin.getline(p.nama, 50);
    cout << "Masukkan NIM: ";
    cin.getline(p.nim, 20);

    pengguna[jumlahPengguna++] = p;
    simpanDataPengguna();
    cout << "Registrasi berhasil!\n";
}

bool loginPengguna() {
    char nama[50];
    char nim[20];
    cin.ignore();

    cout << "Masukkan Nama: ";
    cin.getline(nama, 50);
    cout << "Masukkan NIM: ";
    cin.getline(nim, 20);

    for (int i = 0; i < jumlahPengguna; i++) {
        if (strcmp(nama, pengguna[i].nama) == 0 && strcmp(nim, pengguna[i].nim) == 0) {
            cout << "Login berhasil!\n";
            return true;
        }
    }

    cout << "Login gagal! Nama atau NIM tidak ditemukan.\n";
    return false;
}

void lihatPengguna() {
    if (jumlahPengguna == 0) {
        cout << "Belum ada pengguna yang terdaftar.\n";
        return;
    }

    cout << "Daftar Pengguna:\n";
    for (int i = 0; i < jumlahPengguna; i++) {
        cout << i+1 << ". Nama: " << pengguna[i].nama 
             << ", NIM: " << pengguna[i].nim << endl;
    }
}

void tampilkanDaftarBuku() {
    if (jumlahBuku == 0) {
        cout << "Belum ada buku tersedia.\n";
        return;
    }

    cout << "\nDaftar Buku:\n";
    for (int i = 0; i < jumlahBuku; i++) {
        cout << i+1 << ". " << daftarBuku[i].judul
             << " (" << daftarBuku[i].jumlahHalaman << " halaman)\n";
    }
}

void bacaBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku();
    cout << "\nPilih nomor buku (0 untuk batal): ";
    int pilihan;
    cin >> pilihan;

    if (pilihan < 1 || pilihan > jumlahBuku) {
        cout << "Pilihan tidak valid.\n";
        return;
    }

    int index = pilihan - 1;
    int halamanSekarang = 0;
    char perintah;

    do {
        system("cls||clear");
        cout << "Judul: " << daftarBuku[index].judul << endl;
        cout << "Halaman " << halamanSekarang+1 << " dari " 
             << daftarBuku[index].jumlahHalaman << endl;
        cout << "----------------------------------------\n";
        cout << daftarBuku[index].halaman[halamanSekarang] << endl;
        cout << "----------------------------------------\n";
        cout << "l: lanjut | k: kembali | s: selesai\n";
        cout << "Perintah: ";
        cin >> perintah;
        perintah = tolower(perintah);

        if (perintah == 'l' && halamanSekarang < daftarBuku[index].jumlahHalaman-1) {
            halamanSekarang++;
        } else if (perintah == 'k' && halamanSekarang > 0) {
            halamanSekarang--;
        } else if (perintah != 's') {
            cout << "Perintah tidak valid atau tidak bisa diproses.\n";
        }
    } while (perintah != 's');
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
    cin.ignore();

    if (b.jumlahHalaman <= 0 || b.jumlahHalaman > BatasHal) {
        cout << "Jumlah halaman tidak valid (1-" << BatasHal << ")\n";
        return;
    }

    for (int i = 0; i < b.jumlahHalaman; i++) {
        cout << "Isi halaman " << i+1 << ": ";
        cin.getline(b.halaman[i], PanjangHal);
    }

    daftarBuku[jumlahBuku++] = b;
    simpanDataBuku();
    cout << "Buku berhasil ditambahkan!\n";
}

void hapusBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku();
    cout << "\nPilih nomor buku yang akan dihapus (0 untuk batal): ";
    int pilihan;
    cin >> pilihan;

    if (pilihan < 1 || pilihan > jumlahBuku) {
        cout << "Pilihan tidak valid.\n";
        return;
    }

    int index = pilihan - 1;
    cout << "Apakah Anda yakin ingin menghapus buku '" 
         << daftarBuku[index].judul << "'? (y/n): ";
    
    char konfirmasi;
    cin >> konfirmasi;

    if (tolower(konfirmasi) == 'y') {
        for (int i = index; i < jumlahBuku-1; i++) {
            daftarBuku[i] = daftarBuku[i+1];
        }
        jumlahBuku--;
        simpanDataBuku();
        cout << "Buku berhasil dihapus!\n";
    } else {
        cout << "Penghapusan dibatalkan.\n";
    }
}

void editBuku() {
    if (jumlahBuku == 0) {
        cout << "Tidak ada buku tersedia.\n";
        return;
    }

    tampilkanDaftarBuku();
    cout << "\nPilih nomor buku yang akan diedit (0 untuk batal): ";
    int pilihan;
    cin >> pilihan;

    if (pilihan < 1 || pilihan > jumlahBuku) {
        cout << "Pilihan tidak valid.\n";
        return;
    }

    int index = pilihan - 1;
    int pilihanEdit;

    do {
        cout << "\nMenu Edit Buku: " << daftarBuku[index].judul << endl;
        cout << "1. Edit Judul\n";
        cout << "2. Edit Halaman\n";
        cout << "3. Tambah Halaman\n";
        cout << "4. Hapus Halaman Terakhir\n";
        cout << "5. Selesai\n";
        cout << "Pilihan: ";
        cin >> pilihanEdit;
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
                cout << "Nomor halaman yang diedit (1-" 
                     << daftarBuku[index].jumlahHalaman << "): ";
                int halaman;
                cin >> halaman;
                cin.ignore();

                if (halaman < 1 || halaman > daftarBuku[index].jumlahHalaman) {
                    cout << "Halaman tidak valid.\n";
                    break;
                }

                cout << "Isi halaman baru: ";
                cin.getline(daftarBuku[index].halaman[halaman-1], PanjangHal);
                simpanDataBuku();
                cout << "Halaman berhasil diubah!\n";
                break;
            }
            case 3: {
                if (daftarBuku[index].jumlahHalaman >= BatasHal) {
                    cout << "Jumlah halaman sudah mencapai batas maksimal.\n";
                    break;
                }

                cout << "Isi halaman baru: ";
                cin.getline(daftarBuku[index].halaman[daftarBuku[index].jumlahHalaman], PanjangHal);
                daftarBuku[index].jumlahHalaman++;
                simpanDataBuku();
                cout << "Halaman baru berhasil ditambahkan!\n";
                break;
            }
            case 4: {
                if (daftarBuku[index].jumlahHalaman <= 1) {
                    cout << "Tidak bisa menghapus halaman terakhir!\n";
                    break;
                }

                daftarBuku[index].jumlahHalaman--;
                simpanDataBuku();
                cout << "Halaman terakhir berhasil dihapus!\n";
                break;
            }
            case 5:
                cout << "Keluar dari menu edit.\n";
                break;
            default:
                cout << "Pilihan tidak valid.\n";
        }
    } while (pilihanEdit != 5);
}

void menuPerpustakaan() {
    int pilihan;
    
    do {
        cout << "\n===== PERPUSTAKAAN DIGITAL =====\n";
        cout << "1. Baca Buku\n";
        cout << "2. Tambah Buku\n";
        cout << "3. Edit Buku\n";
        cout << "4. Hapus Buku\n";
        cout << "5. Daftar Buku\n";
        cout << "6. Logout\n";
        cout << "Pilihan: ";
        cin >> pilihan;

        switch (pilihan) {
            case 1: 
				bacaBuku();
				break;
            case 2: 
				tambahBuku();
				break;
            case 3: 
				editBuku();
				break;
            case 4:
				hapusBuku(); 
				break;
            case 5: 
				tampilkanDaftarBuku(); 
				break;
            case 6: 
				cout << "Logout berhasil.\n"; 
				break;
            default: 
				cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 6);
}

int main() {
    bacaDataPengguna();
    bacaDataBuku();
    
    int pilihan;
    bool login = false;

    while (true) {
        if (!login) {
            cout << "\n===== MENU UTAMA =====\n";
            cout << "1. Registrasi" << endl;
            cout << "2. Login" << endl;
            cout << "3. Lihat Pengguna" << endl;
            cout << "4. Keluar" << endl;
            cout << "Pilihan: ";
            cin >> pilihan;

            switch (pilihan) {
                case 1: registrasiPengguna(); break;
                case 2: login = loginPengguna(); break;
                case 3: lihatPengguna(); break;
                case 4: 
                    cout << "Keluar dari program."<< endl;
                    return 0;
                default: cout << "Pilihan tidak valid." << endl;
            }
        } else {
            menuPerpustakaan();
            login = false;
        }
    }
}
