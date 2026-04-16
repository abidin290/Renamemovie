# 🎬 RenameVideo (C++ Win32 GUI)

![Platform](https://img.shields.io/badge/Platform-Windows-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B17-00599C)
![License](https://img.shields.io/badge/License-MIT-green)

---

## 📌 Deskripsi

**RenameVideo** adalah aplikasi utilitas ringan berbasis antarmuka grafis Windows asli (Native Win32 API) yang berfungsi untuk mengganti nama file video secara otomatis berdasarkan *internal metadata* (judul bawaan). Aplikasi ini dibuat khusus untuk menyelesaikan masalah format dan penamaan file film, seri, maupun anime hasil konversi atau unduhan yang berantakan.

Proses modifikasi dieksekusi secara instan tanpa beban komputasi besar (*In-Place Modification*), menjadikan aplikasi ini sangat cepat dan efisien.

---

## ✨ Fitur Utama

- 🚀 **Performa Ultra Cepat:** Berjalan secara murni dalam bahasa C++ dengan optimasi C++17 dan Win32 GUI tanpa *runtime* berat. Ukuran executable sangat kecil (di bawah 1 MB).
- 🏷️ **Metadata Extraction:** Mendeteksi judul dan format tags langsung dengan memanfaatkan `ffprobe`. Secara otomatis membersihkan string judul dari karakter ilegal OS Windows.
- ⚡ **In-Place MKV Track Renamer:** Ubah nama *Video Track*, *Audio Track*, dan *Subtitle Track* di dalam file `.mkv` seketika. Terintegrasi dengan `mkvpropedit`, proses modifikasi trek dikerjakan sepenuhnya secara **in-place** (tanpa memproses *re-rendering* ulang).
- 🖱️ **Drag and Drop:** Mendukung *Drag & Drop* folder langsung ke dalam jendela aplikasi, dan jalur folder akan otomatis ditetapkan sebagai target.
- 🗂️ **Recursive Directory Scan:** Tersedia opsi "Scan Sub-folder" mumpuni untuk mengeksekusi penggantian nama secara menyeluruh hingga ke akar direktori secara bersamaan.
- 💾 **Persistent Memory (Save State):** Menyimpan pengaturan dan input kustom (seperti *Track Name*) ke dalam `config.ini` sehingga preferensi pengguna tidak hilang saat aplikasi ditutup.

---

## ⚙️ Persyaratan Sistem (*Prerequisites*)

Sebagai *backend*, aplikasi ini mengontrol perangkat lunak *command-line* eksternal. Pastikan perangkat Anda telah terpasang *tools* berikut:

1. **[FFmpeg](https://ffmpeg.org/) (`ffprobe`):** Diperlukan untuk membaca informasi metadata video.
2. **[MKVToolNix](https://mkvtoolnix.download/) (`mkvpropedit`):** Disyaratkan apabila Anda ingin menggunakan fitur pengeditan *Fast MKV In-Place Track Modification*.

> ⚠️ **Penting:** Pastikan path folder instalasi `ffprobe` dan `mkvpropedit` terdaftar di dalam **System Environment Variables (PATH)** di Windows agar aplikasi berjalan tanpa kendala.

---

## 🛠️ Panduan Build (Kompilasi)

Aplikasi ini tidak memerlukan *framework* IDE yang berat. Proses kompilasi sangat simpel menggunakan kompilator bawaan **MinGW (`g++`)**.

### 1. Compile Resource Icon
Jalankan perintah ini untuk mengompilasi file icon aplikasi `.rc` menjadi resource `.res`:
```bash
windres app_icon.rc -O coff -o app_icon.res
```

### 2. Compile & Link Program
Jalankan perintah `g++` untuk mem-build program *executable* GUI:
```bash
g++ -O3 -mwindows RenameVideo.cpp app_icon.res -o RenameVideo.exe -lcomctl32 -lole32 -lshell32
```

**Penjelasan Flags Pembantu:**
- `-O3`: Level optimasi performa maksimal dari kompilator.
- `-mwindows`: Mode sub-sistem Windows berbasis GUI (Menyembunyikan jendela *console* belakang).
- `-lcomctl32`, `-lole32`, `-lshell32`: Pustaka *library* Windows (*Dynamic Link Library*) yang esensial untuk merender UI Windows dan menjembatani *drag and drop* sistem.

---

## 💡 Cara Menggunakan

1. Buka aplikasi **`RenameVideo.exe`**
2. Klik tombol **`...`** (Browse) atau **Drag & Drop** folder video kalian ke kolom text box input path.
3. (Opsional) Centang **Scan Sub-folder** jika ingin memproses tumpukan video di dalam urutan folder yang berlapis.
4. Sesuaikan **Track Title** yang akan disematkan ke dalam metadata internal (khusus jika Anda memodifikasi MKV tracks).
5. Klik **Mulai Proses** dan biarkan aplikasi mengeksekusi file video seketika.

---

## 📄 Struktur Direktori Repositori

```text
├── RenameVideo.cpp   # Source code utama (Core Win32 + C++)
├── tesmovie.py       # Script lama berbasis python
├── app_icon.rc       # Resource config windows icon
├── app_icon.ico      # Gambar ikon GUI
└── config.ini        # File memori konfigurasi auto-generated
```

---

<div align="center">
  <i>Diciptakan secara efisien untuk perapian perpustakaan media Anda.</i>
</div>
