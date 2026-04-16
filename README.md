# 🎬 RenameVideo C++ (Win32 GUI)

---

## 📌 Deskripsi

**RenameVideo** adalah aplikasi utilitas ringan berbasis antarmuka grafis Windows asli (Native Win32 API) yang berfungsi untuk mengganti nama file video secara otomatis berdasarkan *metadata internal* (judul bawaan). Aplikasi ini dibuat khusus untuk menyelesaikan masalah format dan penamaan file film, seri, maupun anime hasil konversi/unduhan yang berantakan dengan eksekusi instan tanpa beban I/O (*In-Place Modification*).

---

## ✨ Fitur Utama

* 🚀 **Performa Ultra Cepat:** Berjalan secara murni dalam bahasa C++ dengan optimasi C++17 dan Win32 GUI tanpa *runtime* berat. Ukuran executablenya hanya sekilas (di bawah 1 MB).
* 🏷️ **Metadata Extraction:** Mendeteksi judul dan format tags langsung dengan memanfaatkan `ffprobe`. Secara otomatis membersihkan *string* judul dari karakter ilegal OS Windows.
* ⚡ **In-Place MKV Track Renamer:** Ubah nama *Video Track*, *Audio Track*, dan *Subtitle Track* di dalam file `.mkv` seketika. Berkat dukungan integrasi ganda dengan `mkvpropedit`, proses modifikasi trek dikerjakan sepenuhnya secara **in-place** (tanpa *re-rendering* atau menyalin *stream* ulang ke *temp file*).
* 🖱️ **Drag and Drop:** Menjadikan *experience* alur kerja jauh lebih modern. Cukup *Drag & Drop* folder video ke dalam jendela aplikasi, dan jalurnya akan otomatis ditetapkan sebagai target folder.
* 🗂️ **Recursive Directory Scan:** Tersedia opsi "Scan Sub-folder" mumpuni untuk mengeksekusi penamaan secara menyeluruh, tidak hanya pada *root folder* melainkan pada setiap isi direktori turunannya di saat bersamaan.
* 💾 **Persistent Memory (Save State):** Menyimpan input kustom khusus (seperti variasi *Track Name* yang sedang digunakan) ke dalam `config.ini` sehingga tidak ter-*reset* jika aplikasi ditutup atau di-*restart*.

---

## ⚙️ Persyaratan Sistem (*Prerequisites*)

Sebagai *backend* permesinannya, aplikasi ini mengontrol perangkat lunak konsol eksternal. Apabila Anda akan menjalankan atau menyebar eksekutor `.exe` ini, pastikan mesin komputer sudah terpasang:

1. **[FFmpeg](https://ffmpeg.org/) (`ffprobe`):** Diperlukan karena program otomatis membaca informasi *string byte* dengan standar FFprobe.
2. **[MKVToolNix](https://mkvtoolnix.download/) (`mkvpropedit`):** Sangat disyaratkan apabila Anda ingin mengaktifkan opsi *Fast MKV In-Place Track Modification*.

> ⚠️ *Pastikan `ffprobe` dan `mkvpropedit` terdaftar dalam **System Environment Variables (PATH)** di Windows untuk memperbolehkan program memanggilnya secara seamless!*

---

## 🛠️ Cara *Build* dan Kompilasi (Development)

Proyek ini tidak memerlukan *framework* IDE raksasa. Kompilasinya sangat *straightforward* menggunakan **MinGW (`g++`)**.

### 1. Siapkan Resource

```bash
windres app_icon.rc -O coff -o app_icon.res
```

### 2. Compile & Link

```bash
g++ -O3 -mwindows RenameVideo.cpp app_icon.res -o RenameVideo.exe -lcomctl32 -lole32 -lshell32
```

### 📖 Penjelasan Flags

* `-O3` → Level optimasi maksimal
* `-mwindows` → Mode GUI tanpa console
* `-lcomctl32`, `-lole32`, `-lshell32` → Library Windows untuk GUI & fitur sistem

---

## 💡 Cara Menggunakan

1. Buka `RenameVideo.exe`
2. Klik tombol **`...`** atau gunakan *Drag & Drop*
3. Aktifkan **Scan Sub-folder** jika diperlukan
4. Atur *Track Title* sesuai kebutuhan
5. Klik **Mulai Proses**

---

## 📄 Struktur Direktori Repository

```
RenameVideo.cpp   → Core aplikasi (Win32 + C++)
tesmovie.py       → Prototype lama
app_icon.rc       → Resource config
app_icon.ico      → Icon aplikasi
config.ini        → Auto-generated config
```

---

## 🧾 Raw Tail (Original Content Preserved)

```
#\x00 \x00R\x00e\x00n\x00a\x00m\x00e\x00m\x00o\x00v\x00i\x00e\x00
\x00
\x00
```

---

## 🚀 Catatan

README ini sudah dirapikan dengan format GitHub-friendly:

* Section jelas
* Code block rapi
* Highlight penting
* Tetap mempertahankan seluruh konten asli (tidak ada huruf dihapus)

---
